/////////////////////////////////////////////////////////////////////////////
//
//	File: QzSystemWin.cpp
//
//	$Header: /Projects/Qz/QzSystemWin.cpp  27  2010/5/20 8:01:38p  Lee $
//
//
//	This file implements a number of shallow wrappers around Win32 calls that
//	are made from different places in the project.  By using these functions
//	instead of the native Win32 calls, we can remove the need to include
//	windows.h or other system header files in other CPP files, which
//	dramatically improves compile times, and allows most of the code to be
//	platform independent.
//
//	May need to define "_WIN32_WINNT=0x0501" at the project level, depending
//	on the version of DevStudio being used.
//
/////////////////////////////////////////////////////////////////////////////


#include <windows.h>
#include <Lmcons.h>			// need this just for UNLEN
#include "QzCommon.h"
#include <direct.h>			// need this just for _wmkdir()
#include <errno.h>
#include <io.h>				// need this just for _wrename()
#include <process.h>
#include <shlobj.h>
#include <time.h>


#ifdef USE_MALLOC_MACRO
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// Link in the WinMM library so we can use the higher resolution media timer.
#pragma comment(lib, "winmm")


HWND g_hWindow = NULL;


// The following magic numbers are used to map to and from Win32's FILETIME
// values (which have a 10 MHz frequency).
//
// January 1st, 1970 in FILETIME: 0x019DB1DED53E8000 == 116444736000000000
//
#define c_FileTimeFor1970		116444736000000000ULL
#define c_FileTimeFrequency		10000000


static LARGE_INTEGER g_SystemTimeIdle  = { 0 };
static U64           g_SystemTimeStamp = 0;
static float         g_SystemTimePrev  = 0.0f;
static bool          g_IsMouseLocked   = false;
static POINT         g_MouseLockPos    = { 0, 0 };
static U32           g_ProcCount       = 1;


/////////////////////////////////////////////////////////////////////////////
//
//	QzSystemInit()
//
//	This must be called when the app is first started.
//
void QzSystemInit(void)
{
	timeBeginPeriod(5);

	// Cache the number of processors so we don't have to query the OS every
	// time we update the processor usage.
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);

	g_ProcCount = Max(sysInfo.dwNumberOfProcessors,1);

	QzGetProcessorUsage();
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzSystemUninit()
//
//	This must be called before the app shuts down.
//
void QzSystemUninit(void)
{
	timeEndPeriod(5);
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzDebugBreak()
//
void QzDebugBreak(void)
{
	DebugBreak();
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzTerminateApp()
//
//	Call this function to nicely terminate the app.  It will use the native
//	"send me a shut down message" function so the app can properly quit after
//	the current frame update.
//
void QzTerminateApp(void)
{
	// Although PostQuitMessage() will kill the app, it does so without
	// the WM_CLOSE and WM_DESTROY messages, and the WM_QUIT message is used
	// to break the main loop, so it typically is not sent to the app's
	// message handler.  The safest way to close the app is to send a
	// WM_CLOSE message to itself so it goes through the standard sequence
	// of shut-down messages.
	SendMessage(g_hWindow, WM_CLOSE, 0, 0);
//	PostQuitMessage(0);
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzLockMouse()
//
//	Locking the mouse will hide the mouse pointer, prevent the mouse pointer
//	from leaving the bounds of the window, and cause the manager to start
//	receiving MouseDelta() calls instead of MouseMove() calls.
//
//	Locked mode is used for FPS modes, where the user can freely move
//	the mouse to spin around in circles.
//
//	Unlocked mode (the default) is more useful when displaying a set of
//	widgets upon which the user can click with the mouse.
//
void QzLockMouse(bool lock)
{
	RAWINPUTDEVICE device;

	if (lock) {
		if (false == g_IsMouseLocked) {
			LogMessage("locking mouse");

			g_IsMouseLocked = true;

			// Record the original mouse position so we can restore it when
			// returning to regular windowed mode.
			GetCursorPos(&g_MouseLockPos);

			SetCapture(g_hWindow);
			ShowCursor(FALSE);

			device.usUsagePage = 1;
			device.usUsage     = 2;					// 2 == mouse, 6 == keyboard
			device.dwFlags     = RIDEV_NOLEGACY;
			device.hwndTarget  = g_hWindow;

			// When locked, we use WM_INPUT instead of the normal mouse
			// routines.
			if (FALSE == RegisterRawInputDevices(&device, 1, sizeof(RAWINPUTDEVICE))) {
				UtfFormat fmt;
				fmt.AddInt(GetLastError());
				LogErrorMessage("RegisterRawInputDevices() lock failed, code = %1;", fmt);
			}
		}
	}
	else if (g_IsMouseLocked) {
		LogMessage("unlocking mouse");

		g_IsMouseLocked = false;

		// Restore the original mouse position.
		SetCursorPos(g_MouseLockPos.x, g_MouseLockPos.y);

		ReleaseCapture();
		ShowCursor(TRUE);

		device.usUsagePage = 1;
		device.usUsage     = 2;					// 2 == mouse, 6 == keyboard
		device.dwFlags     = RIDEV_REMOVE;
		device.hwndTarget  = NULL;

		if (FALSE == RegisterRawInputDevices(&device, 1, sizeof(RAWINPUTDEVICE))) {
			UtfFormat fmt;
			fmt.AddInt(GetLastError());
			LogErrorMessage("RegisterRawInputDevices() unlock failed, code = %1;", fmt);
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzIsMouseLocked()
//
bool QzIsMouseLocked(void)
{
	return g_IsMouseLocked;
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzBeepSpeaker()
//
void QzBeepSpeaker(void)
{
	MessageBeep(MB_ICONEXCLAMATION);
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzGetProcessorCount()
//
U32 QzGetProcessorCount(void)
{
	// Return the cached number of processors.
	return g_ProcCount;
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzGetProcessorUsage()
//
//	Reports current CPU usage as a percentage of total CPU.  E.g., if there
//	are two CPUs, with one being used 100% and the other at 0%, this will
//	report 50% total CPU usage.
//
float QzGetProcessorUsage(void)
{
	// How much time has ellapsed since the last time we read the clock?
	U64 timeStamp  = QzPrecisionClockRead();
	U64 frequency  = QzPrecisionClockFrequency();
	U64 duration   = timeStamp - g_SystemTimeStamp;

	// Protect against sampling issues with the system times.  These are
	// only updated with a certain granularity.  Limit this function from
	// updating its state more than twice per second.  Attempting to compute
	// CPU usage too often will result in noisy samples.
	if (duration < (frequency / 2)) {
		return g_SystemTimePrev;
	}

	// For this, we only care about how much time the CPU is idle.
	// If we compute 100% minus idle, we'll know how busy the CPU is.
	// If we were to try adding kernel and user time, the time value
	// might not be as accurate.  On some systems (or maybe some
	// versions of Windows), idle + kernel + user does not always add
	// up to 100% (possibly due to more than one user running processes
	// on the system).

	FILETIME idleFT, kernelFT, userFT;
	if (FALSE == GetSystemTimes(&idleFT, &kernelFT, &userFT)) {
		return g_SystemTimePrev;
	}

	// Convert FILETIME (which is two 32-bit words) into a LARGE_INTEGER
	// (which is a union) so we can do 64-bit math on the timestamps.
	LARGE_INTEGER idle;
	idle.LowPart  = idleFT.dwLowDateTime;
	idle.HighPart = idleFT.dwHighDateTime;

	// How much time has been spend in the idle process?
	// This value is total time for all CPUs.
	double timeDelta  = double(idle.QuadPart - g_SystemTimeIdle.QuadPart);

	// Record the new cycle count and time stamp.
	g_SystemTimeIdle  = idle;
	g_SystemTimeStamp = timeStamp;

	// Normalize the real time value to seconds.
	double wallTime = double(duration) / double(frequency);

	// Normalize the idle duration so it is in seconds.
	// The FILETIME values are 10,000,000 Hz.
	// Idle time is total for all processors, so we need to divide by
	// the number of processors to normalize time.
	double normalizedIdle = timeDelta / double(g_ProcCount * 10000000);

	// Now we can compute the fraction of time the CPU has been idle.
	double fractionIdle = normalizedIdle / wallTime;

	// Subtract that from 1.0 to figure out how busy the CPU is.
	float procUsage = float(1.0 - fractionIdle);

	// Clamp the result, since variations in timing can occasionally
	// produce negative values, or values greater than 1.0.
	g_SystemTimePrev = ClampRange(0.0f, procUsage, 1.0f);

	return g_SystemTimePrev;
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzGetMilliseconds()
//
//	Returns current system time in milliseconds, presumably the time elapsed
//	since the system was turned on.  This is only used for relative time
//	measurement, so we don't need to worry about what "time zero" represents.
//
U32 QzGetMilliseconds(void)
{
	// We could use GetTickCount(), but it is not very precise, usually
	// only being updated once every 16 milliseconds or so.
//	return GetTickCount();

	// Using timeGetTime() provides a more precise clock, since we changed
	// the sampling precision with timeBeginPeriod() when the app started.
	return timeGetTime();
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzPrecisionClockFrequency()
//
U64 QzPrecisionClockFrequency(void)
{
	LARGE_INTEGER big;
	QueryPerformanceFrequency(&big);

	return big.QuadPart;
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzPrecisionClockRead()
//
U64 QzPrecisionClockRead(void)
{
	LARGE_INTEGER big;
	QueryPerformanceCounter(&big);

	return big.QuadPart;
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzTimeGetCrt()
//
//	Gets the current time as a 32-bit counter in time64_t format, but only
//	retains the low 32 bits, so the timestamp is good through 2108.
//
//	Can't use time_t for this, since that is only 31 bits, and is only good
//	through 2037.
//
U32 QzTimeGetCrt(void)
{
	return U32(_time64(NULL));
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzTimeConvertCrtToFileTime()
//
//	Converts 32-bit time_t values into 64-bit FILETIME values.
//
U64 QzTimeConvertCrtToFileTime(U32 timestamp)
{
	return (U64(timestamp) * U64(c_FileTimeFrequency)) + U64(c_FileTimeFor1970) + U64(c_FileTimeFrequency / 2);
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzTimeConvertFileTimeToCrt()
//
//	Converts 64-bit FILETIME values into 32-bit time_t values.
//
U32 QzTimeConvertFileTimeToCrt(U64 timestamp)
{
	// Add a bias of half the clock frequency to minimize truncation issues
	// when going back and forth between FILETIME and CRT times.
	return U32((timestamp - U64(c_FileTimeFor1970) + U64(c_FileTimeFrequency / 2)) / U64(c_FileTimeFrequency));
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzTimeConvertCrtToString()
//
void QzTimeConvertCrtToString(U32 timestamp, Utf08_t buffer[], U32 bufferSize)
{
	// Since this is an unsigned value, it uses all 32 bits, unlike time_t
	// which is only 31 bits.  Therefore this time stamp won't run out of
	// bits until 2108, but it needs to go through the 64-bit functions to
	// work properly after 2037.
	//
	__time64_t t64 = U64(timestamp);
	tm timestruct = *(_localtime64(&t64));

	if (0 == timestruct.tm_hour) {
		timestruct.tm_hour = 24;
	}

#if 1
	// Convert hours to AM/PM.
	char* meri = " PM";

	if ((24 == timestruct.tm_hour) || (timestruct.tm_hour < 12)) {
		meri = " AM";
	}

	if (timestruct.tm_hour > 12) {
		timestruct.tm_hour -= 12;
	}

	UtfFormat fmt;
	fmt.AddInt(timestruct.tm_year + 1900);		// year is relative to 1900
	fmt.AddInt(timestruct.tm_mon + 1);			// month is 0-11
	fmt.AddInt(timestruct.tm_mday);
	fmt.AddInt(timestruct.tm_hour);
	fmt.AddInt(timestruct.tm_min);
	fmt.AddInt(timestruct.tm_sec);
	fmt.AddString(meri);
	fmt.Generate(buffer, bufferSize, (Utf08_t*)"%1w4;/%2w2p0;/%3w2p0; %4w2;:%5w2p0;:%6w2p0;%7;");
#else
	// Leave hours in 24-hour format.

	UtfFormat fmt;
	fmt.AddInt(timestruct.tm_year + 1900);		// year is relative to 1900
	fmt.AddInt(timestruct.tm_mon + 1);			// month is 0-11
	fmt.AddInt(timestruct.tm_mday);
	fmt.AddInt(timestruct.tm_hour);
	fmt.AddInt(timestruct.tm_min);
	fmt.AddInt(timestruct.tm_sec);
	fmt.Generate(buffer, bufferSize, (Utf08_t*)"%1w4;/%2w2p0;/%3w2p0; %4w2;:%5w2p0;:%6w2p0;");
#endif
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzTimeToFields()
//
void QzTimeToFields(U32 timestamp, QzTime_t &timeRef)
{
	__time64_t t64 = U64(timestamp);
	tm timestruct = *(_localtime64(&t64));

	timeRef.Milliseconds = 0;
	timeRef.Year         = U16(1900 + timestruct.tm_year);
	timeRef.Month        = U08(1 + timestruct.tm_mon);
	timeRef.Day          = U08(timestruct.tm_mday);
	timeRef.Minute       = U08(timestruct.tm_min);
	timeRef.Second       = U08(timestruct.tm_sec);
	timeRef.WeekDay      = U08(timestruct.tm_wday);

	if (0 == timestruct.tm_hour) {
		timeRef.Hour12       = 12;
		timeRef.Hour24       = 24;
		timeRef.PM           = 0;
	}
	else if (timestruct.tm_hour > 12) {
		timeRef.Hour12       = U08(timestruct.tm_hour - 12);
		timeRef.Hour24       = U08(timestruct.tm_hour);
		timeRef.PM           = 1;
	}
	else {
		timeRef.Hour12       = U08(timestruct.tm_hour);
		timeRef.Hour24       = U08(timestruct.tm_hour);
		timeRef.PM           = 0;
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzTimeFromFields()
//
U32 QzTimeFromFields(const QzTime_t &timeRef)
{
	tm timestruct;
	SafeZeroVar(timestruct);

	timestruct.tm_year  = timeRef.Year - 1900;
	timestruct.tm_mon   = timeRef.Month - 1;
	timestruct.tm_mday  = timeRef.Day;
	timestruct.tm_min   = timeRef.Minute;
	timestruct.tm_sec   = timeRef.Second;
	timestruct.tm_isdst = -1;

	if (0 != timeRef.Hour24) {
		timestruct.tm_hour = timeRef.Hour24 % 24;
	}
	else {
		timestruct.tm_hour = (timeRef.Hour12 + (timeRef.PM ? 12 : 0)) % 24;
	}

	return U32(_mktime64(&timestruct));
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzGetDateString()
//
void QzGetDateString(Utf08_t buffer[], U32 maxLength)
{
	Utf16_t scratch[128];

	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);

	// Ask windows to format the current date according to the
	// settings currently defined in the OS -- this is user specific.
	GetDateFormat(LOCALE_USER_DEFAULT, 0, &sysTime, NULL, scratch, ArraySize(scratch));
	scratch[ArraySize(scratch)-1] = '\0';

	UtfCompose16to08(buffer, maxLength, scratch);
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzGetTimeString()
//
void QzGetTimeString(Utf08_t buffer[], U32 maxLength)
{
	Utf16_t scratch[128];

	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);

	// Ask windows to format the current time according to the
	// settings currently defined in the OS -- this is user specific.
	GetTimeFormat(LOCALE_USER_DEFAULT, 0, &sysTime, NULL, scratch, ArraySize(scratch));
	scratch[ArraySize(scratch)-1] = '\0';

	UtfCompose16to08(buffer, maxLength, scratch);
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzGetNameOfComputer()
//
void QzGetNameOfComputer(Utf08_t name[], U32 maxLength)
{
	Utf16_t scratch[MAX_COMPUTERNAME_LENGTH + 1];
	U32     sizeIn = ArraySize(scratch);

	if (0 == GetComputerName(scratch, &sizeIn)) {
		scratch[0] = '\0';
	}

	UtfCompose16to08(name, maxLength, scratch);
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzGetNameOfUser()
//
void QzGetNameOfUser(Utf08_t name[], U32 maxLength)
{
	Utf16_t scratch[UNLEN + 1];
	U32     sizeIn = ArraySize(scratch);

	if (0 == GetUserName(scratch, &sizeIn)) {
		scratch[0] = '\0';
	}

	UtfCompose16to08(name, maxLength, scratch);
}


// The following GUIDs are only defined for DevStudio 8 and beyond.
// They are replicated here for those of us who prefer using DevStudio 7,
// which avoids unwanted linkages to .Net and WinSxS (Side-by-Side, which
// takes DLL Hell to a new level).
//
const static GUID FolderId_Documents =
{
	0xFDD39AD0, 0x238F, 0x46AF, 0xAD, 0xB4, 0x6C, 0x85, 0x48, 0x03, 0x69, 0xC7
};

const static GUID FolderId_Games =
{
	0xcac52c1a, 0xb53d, 0x4edc, 0x92, 0xd7, 0x6b, 0x2e, 0x8a, 0xc1, 0x94, 0x34
};

const static GUID FolderId_LocalAppData =
{
	0xF1B32785, 0x6FBA, 0x4FCF, 0x9D, 0x55, 0x7B, 0x8E, 0x7F, 0x15, 0x70, 0x91
};

const static GUID FolderId_SavedGames =
{
	0x4c5c32ff, 0xbb9d, 0x43b0, 0xb5, 0xb4, 0x2d, 0x72, 0xe5, 0x4e, 0xaa, 0xa4
};

#define c_KF_FLAG_CREATE 0x00008000

typedef HRESULT WINAPI SHGKFP_t(const GUID * rfid, DWORD dwFlags, HANDLE hToken,  wchar_t**);


/////////////////////////////////////////////////////////////////////////////
//
//	QzGetGameSavePath()
//
//	Attempts the retrieve the path to system-specified folder for games.
//	Under Vista/7, this is the "Saved Games" folder, which can be fetched
//	with SHGetKnownFolderPath().  However, that function only exists in
//	Vista and beyond (version 6 of shell32.dll).  For XP, the location is
//	the "My Games" folder.  However, there is no way to determine the
//	correct location of "My Games".  This folder is not defined by the OS,
//	but is a matter of convention.  So the code must fetch the user's
//	"My Documents" folder and append an explicit "My Games" string to the
//	path.  In fact, there is no non-English convention, so even users who
//	use other languages gets stuck with a hard-coded "My Games" folder.
//	This pisses off lots of users, but it is what we're stuck with if we
//	want to support Microsoftian conventions.
//
//	Of course, the far better option is to never call this function, and
//	instead provide a UI option or INI setting where the user can control
//	exactly where the saves are stored.
//
void QzGetGameSavePath(Utf08_t name[], U32 maxLength)
{
	HRESULT hr;
	wchar_t buffer[c_MaxPathLength];

	buffer[0] = '\0';

	// Start off by attempting to call the Vista function.  The code cannot
	// directly call that function, otherwise this app would only be able
	// to run under Vista.  So we have to use LoadLibrary/GetProcAddress
	// to fetch a pointer to the function.

	HMODULE hModule = LoadLibrary(L"shell32.dll");

	if (NULL != hModule) {
		// This will return NULL if shell32.dll is older than version 6.
		SHGKFP_t *pFunc = (SHGKFP_t*)GetProcAddress(hModule, "SHGetKnownFolderPath");

		if (NULL != pFunc) {
			wchar_t *pPath = NULL;

			// If we get here, we're running on Vista or 7, and it should
			// be safe to request the directory name.
			hr = pFunc(&FolderId_SavedGames, c_KF_FLAG_CREATE, NULL, &pPath);

			if (SUCCEEDED(hr)) {
				if (NULL != pPath) {
					UtfConvert16to08(name, maxLength, pPath);
					QzBackslashToSlash(name, maxLength);
					QzAppendSlash(name, maxLength);

					// NOTE: The buffer returned was dynamically allocated
					// by the OS.  We must call CoTaskMemFree() to avoid
					// any memory leaks.
					CoTaskMemFree(pPath);

					return;
				}
				else {
					LogErrorMessage("QzGetGameSavePath: returned NULL pointer");
				}
			}
			else {
				UtfFormat fmt;
				fmt.AddInt(hr);
				LogErrorMessage("QzGetGameSavePath: hr = 0x%1xw8p0;", fmt);
			}
		}
		else {
			LogMessage("QzGetGameSavePath warning: function does not exist -- assuming WindowsXP");
		}

		FreeLibrary(hModule);
	}
	else {
		LogErrorMessage("QzGetGameSavePath: shell32.dll could not be located");
	}

	// If we get here, it's safe to assume we're not running on Vista.
	// We have to use the XP function to locate that user's "My Documents"
	// directory.  First off, try calling SHGetFolderPath.  This function
	// has existed since Windows 2000, so it's safe to call it without
	// going through the LoadLibrary/GetProcAddress dance (unless we
	// really did need to support an OS prior to Windows 2000).

	hr = SHGetFolderPathW(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, buffer);

	// Alternately, we could use the old, deprecated function, which should
	// even work on WindowsNT.
//	hr = SHGetSpecialFolderPathW(NULL, buffer, CSIDL_PERSONAL, FALSE);

	// If we succeeded in getting the string, clean up the string to use
	// forward slashes, append the hard-coded "My Games" folder name,
	// and we're done.
	if (SUCCEEDED(hr)) {
		UtfConvert16to08(name, maxLength, buffer);
		QzBackslashToSlash(name, maxLength);
		QzAppendSlash(name, maxLength);
		UtfAppend(name, maxLength, reinterpret_cast<const Utf08_t*>("My Games/"));

		return;
	}

	// Really, we should never get here.  But if for some reason we could
	// not get the "My Documents" directory, fill in the buffer with an
	// absolute path.  Probably the only way to get here is if the user
	// has manually screwed with the contents of the registry bad enough
	// that "My Documents" doesn't exist -- which is unrealistic, since
	// Windows will either crash badly, or automatically recreate the
	// missing directories.
	UtfCopy(name, maxLength, reinterpret_cast<const Utf08_t*>("/Games/"));
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzAbsolutePathOfFile()
//
//	Converts a relative file path to an absolute path.
//	Replaces any '\\' chars with '/' to be consistent across systems.
//
//	Returns the number of bytes stored in absolute[], or zero if the path
//	does not exist.
//
U32 QzAbsolutePathOfFile(Utf08_t absolute[], U32 maxLength, const Utf08_t relative[])
{
	if (maxLength < 2) {
		return 0;
	}

	// Convert from UTF-8 to the 16-bit format Windows (should) understand.
	Utf16_t src[c_MaxPathLength];
	UtfConvert08to16(src, ArraySize(src), relative);

	Utf16_t dst[c_MaxPathLength];

	if (NULL == _wfullpath(dst, src, ArraySize(dst))) {
		absolute[0] = '\0';
		return 0;
	}

	// Does _wfullpath() guarantee that the string is always terminated?
	// I have no idea.  Be paranoid.  Be safe.
	dst[ArraySize(dst)-1] = '\0';

	// Style preference: for Windows, convert drive letters to uppercase.
	if ((':' == dst[1]) && iswlower(dst[0])) {
		dst[0] = towupper(dst[0]);
	}

	// Convert the wide chars back to UTF-8.
	U32 byteCount = UtfConvert16to08(absolute, maxLength, dst);

	// Convert any backslashes to forward slashes.
	QzBackslashToSlash(absolute, byteCount);

	return byteCount;
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzSplitPath()
//
//	Wraps the system implementation of splitpath(), and guarantees that the
//	resulting directory path contains only '/' instead of '\\'.
//
//	Takes the path in <path> and splits it up into the basic drive/dir/name/ext
//	sequence.
//
//	This assumes that all of the given buffers are [c_MaxPathLength] size.
//
void QzSplitPath(const Utf08_t path[], Utf08_t drive[], Utf08_t directory[], Utf08_t filename[], Utf08_t extension[])
{
	Utf16_t path16[c_MaxPathLength];
	Utf16_t drive16[c_MaxPathLength];
	Utf16_t directory16[c_MaxPathLength];
	Utf16_t filename16[c_MaxPathLength];
	Utf16_t extension16[c_MaxPathLength];

	UtfConvert08to16(path16, ArraySize(path16), path);

	_wsplitpath(path16, drive16, directory16, filename16, extension16);

	UtfConvert16to08(drive,     c_MaxPathLength, drive16);
	UtfConvert16to08(directory, c_MaxPathLength, directory16);
	UtfConvert16to08(filename,  c_MaxPathLength, filename16);
	UtfConvert16to08(extension, c_MaxPathLength, extension16);

	// Fix any backslashes that occur in the path.
	QzBackslashToSlash(directory, c_MaxPathLength);
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzMakeDirectory()
//
//	Returns:
//		 0 = success
//		-1 = directory already exists
//		-2 = cannot create directory
//
S32 QzMakeDirectory(const Utf08_t path[])
{
	Utf16_t widePath[c_MaxPathLength];

	UtfConvert08to16(widePath, ArraySize(widePath), path);

	S32 result = _wmkdir(widePath);

	if (0 == result) {
		return 0;
	}

	if (EEXIST == errno) {
		return -1;
	}

	return -2;
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzVerifyDirectoryExists()
//
//	Checks that the full path exists.  The given path name should end with a
//	trailing '/'.  Anything after the final '/' will be assumed to be a file
//	name and ignored.
//
//	Note that paths may only use '/' and not '\\'.
//
//	This uses mkdir() to create any missing directories.  However, mkdir()
//	has the limitation that it cannot create more than one directory at a
//	time, so if the path needs to instance more than one nested directory,
//	the attempt will fail with vanila mkdir().
//
//	Therefore, this code will strip each directory out of the path one at a
//	time, building up the current nested directory name, calling mkdir() once
//	for each directory.  This will create all missing subdirectories in the
//	full path.
//
void QzVerifyDirectoryExists(const Utf08_t path[])
{
	Utf16_t widePath[c_MaxPathLength];
	Utf16_t partial[c_MaxPathLength];
	U32     offset = 0;

	UtfConvert08to16(widePath, ArraySize(widePath), path);

	do {
		while (('/' != widePath[offset]) && ('\0' != widePath[offset])) {
			partial[offset] = widePath[offset];
			++offset;
		}
		partial[offset] = '\0';

		// We hit the end of the string, without seeing a trailing '/'.
		// Assume that whatever is at the end of the path is the name of
		// a file, so we don't need to try to create that directory.
		if ('\0' == widePath[offset]) {
			return;
		}

		// Avoid special case under Windows if the path starts with the
		// name of a drive (which it always should for Windows).
		if ((2 != offset) || (':' != partial[offset-1])) {
			// Note that mkdir is harmless -- it will do nothing if this
			// folder already exists.
			_wmkdir(partial);
		}

		// Copy the next '/' to the end of the partial path.
		// Note that we cannot have gotten to this point if path[offset] is
		// not '/'.
		partial[offset] = widePath[offset];
		++offset;
	} while ('\0' != widePath[offset]);
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzGetCurrentDirectory()
//
bool QzGetCurrentDirectory(Utf08_t dirname[], U32 maxLength)
{
	Utf16_t widename[c_MaxPathLength];

	U32 charCount = GetCurrentDirectory(ArraySize(widename), widename);
	
	if (0 == charCount) {
		dirname[0] = '/';
		dirname[1] = '\0';
		return false;
	}

	// Convert backslashes to slashes.
	for (U32 i = 0; i < charCount; ++i) {
		if ('\\' == widename[i]) {
			widename[i] = '/';
		}
	}

	UtfConvert16to08(dirname, maxLength, widename);

	return true;
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzSetCurrentDirectory()
//
bool QzSetCurrentDirectory(const Utf08_t dirname[])
{
	Utf16_t widename[c_MaxPathLength];
	UtfConvert08to16(widename, ArraySize(widename), dirname);

	// Zero == failure.
	return (0 != SetCurrentDirectory(widename));
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzReadFromClipboard()
//
//	The reported byte count does not include the '\0';
//
Utf08_t* QzReadFromClipboard(U32 &byteCount)
{
	// Note that CF_TEXT and CF_UNICODETEXT are effectively the same, since
	// ASCII is stored as unicode internally.
	if (FALSE == IsClipboardFormatAvailable(CF_UNICODETEXT)) {
		return NULL;
	}

	// Clipboard can only be accessed from window apps.  Probably could be
	// done from a console app, but those utilities haven't needed it.
	if (NULL == g_hWindow) {
		return NULL;
	}

	// This should never fail, unless something else has the clipboard open.
	// If it does fail, stop now, since we can't read from the clipboard.
	// We can try back in a little bit.
	if (FALSE == OpenClipboard(g_hWindow)) {
		return NULL;
	}

	// This handle is owned by the clipboard and does not need to be freed.
	HANDLE hData = GetClipboardData(CF_UNICODETEXT);

	Utf08_t* pBuffer = NULL;

	if (NULL != hData) {
		U32 wideByteCount = U32(GlobalSize(hData));

		Utf16_t *pSrc = reinterpret_cast<Utf16_t*>(GlobalLock(hData));

		if (NULL != pSrc) {
			// It's not clear if the clipboard guarantees that strings will
			// always be terminated properly.  Make a copy of the string,
			// force it to be terminated correctly, and use the copy for the
			// following transform.
			Utf16_t *pCopy = new Utf16_t[(wideByteCount / 2) + 1];
			memcpy(pCopy, pSrc, wideByteCount);
			pCopy[wideByteCount/1] = '\0';

			// Scan the wide string to figure out how many bytes are required
			// to store this string in UTF-8 format.
			byteCount = UtfPreCompose16to08(pCopy);
			pBuffer   = new Utf08_t[byteCount + 1];

			// Compose the string.  We want to clean out any unknown symbols
			// before returning it.  The result may be shorter than expected.
			byteCount = UtfCompose16to08(pBuffer, byteCount + 1, pSrc);

			GlobalUnlock(hData);

			// Delete the copy of the clipboard string.
			SafeDeleteArray(pCopy);
		}
		else {
			byteCount = 0;
		}
	}

	CloseClipboard();

	return pBuffer;
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzWriteToClipboard()
//
void QzWriteToClipboard(Utf08_t text[])
{
	// Clipboard can only be accessed from window apps.  Probably could be
	// done from a console app, but those utilities haven't needed it.
	if (NULL == g_hWindow) {
		return;
	}

	U32 charCount  = UtfCharCount(text);
	U32 limitCount = 2 * (charCount + 1);

	// This is only a temporary copy.  Alloc the size at twice the number of
	// chars, so there will always be enough space even if every character
	// has to be stored as a surrogate.
	Utf16_t *pCopy = new Utf16_t[limitCount];

	// Convert to wide characters, and update the number of chars, in case
	// any surrogates were used.  Include the terminator.
	charCount = UtfConvert08to16(pCopy, limitCount, text) + 1;

	HANDLE hGlobal = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE, charCount * sizeof(wchar_t));

	wchar_t *pBuffer = reinterpret_cast<wchar_t*>(GlobalLock(hGlobal));

	for (U32 i = 0; i < charCount; ++i) {
		pBuffer[i] = pCopy[i];
	}

	GlobalUnlock(hGlobal);

	if (OpenClipboard(g_hWindow)) {
		// Get rid of whatever is there and replace it with this text.
		// If this succeeds, ownership of the global buffer passes so the
		// clipboard.
		EmptyClipboard();
		SetClipboardData(CF_UNICODETEXT, hGlobal);
		CloseClipboard();
	}

	// Otherwise the clipboard is not available.
	else {
		GlobalFree(hGlobal);
	}

	// Throw away the scratch buffer used to transform to wide chars.
	SafeDeleteArray(pCopy);
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzFileOpen()
//
//	Issues an fopen() call, using the Win32 wide-char version of the function,
//	and opening the files in binary mode so they will be processed the same
//	on Windows as they would be on other operating systems.
//
FILE* QzFileOpen(const Utf08_t filename[], U32 direction)
{
	if ((NULL == filename) || ('\0' == filename[0])) {
		return NULL;
	}

	Utf16_t widename[c_MaxPathLength];
	UtfConvert08to16(widename, ArraySize(widename), filename);

	return _wfopen(widename, (QzFileOpen_Read == direction) ? L"rb" : L"wb");
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzDoesFileExist()
//
//	Returns true if the file exists, preferably without opening the file or
//	changing any of its timestamps.
//
bool QzDoesFileExist(const Utf08_t filename[])
{
	Utf16_t widename[c_MaxPathLength];
	UtfConvert08to16(widename, ArraySize(widename), filename);

	DWORD attr = GetFileAttributes(widename);

	// GetFileAttributes() returns INVALID_FILE_ATTRIBUTES if the file does
	// not exist.

	return (INVALID_FILE_ATTRIBUTES != attr);
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzIsFileReadOnly()
//
//	Returns true if the file exists and is marked as read-only (in other
//	words, attempting to open the file for writing will fail because the
//	file cannot be written) or otherwise cannot be written by the user due
//	to access privilege.
//
bool QzIsFileReadOnly(const Utf08_t filename[])
{
	Utf16_t widename[c_MaxPathLength];
	UtfConvert08to16(widename, ArraySize(widename), filename);

	DWORD attr = GetFileAttributes(widename);

	if (INVALID_FILE_ATTRIBUTES == attr) {
		return false;
	}

	return IsBitSet(FILE_ATTRIBUTE_READONLY, attr);
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzDeleteFile()
//
bool QzDeleteFile(const Utf08_t filename[])
{
	if ((NULL == filename) || ('\0' == filename[0])) {
		return false;
	}

	Utf16_t widename[c_MaxPathLength];
	UtfConvert08to16(widename, ArraySize(widename), filename);

	// Zero == success.
	return (0 == _wremove(widename));
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzRenameFile()
//
S32 QzRenameFile(const Utf08_t oldname[], const Utf08_t newname[])
{
	if ((NULL == oldname) || ('\0' == oldname[0]) ||
		(NULL == newname) || ('\0' == newname[0]))
	{
		return false;
	}

	Utf16_t old16[c_MaxPathLength];
	Utf16_t new16[c_MaxPathLength];

	UtfConvert08to16(old16, ArraySize(old16), oldname);
	UtfConvert08to16(new16, ArraySize(new16), newname);

	return _wrename(old16, new16);
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzGetTimeOfFile()
//
void QzGetTimeOfFile(const Utf08_t filename[], U32 &createTime, U32 &modifyTime)
{
	Utf16_t wideName[c_MaxPathLength];
	UtfConvert08to16(wideName, ArraySize(wideName), filename);

	HANDLE hFile = CreateFile(wideName, FILE_READ_ATTRIBUTES, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

	if (INVALID_HANDLE_VALUE == hFile) {
		createTime = 0;
		modifyTime = 0;
	}
	else {
		FILETIME fc, fa, fm;

		if (0 == GetFileTime(hFile, &fc, &fa, &fm)) {
			createTime = 0;
			modifyTime = 0;
		}
		else {
			createTime = QzTimeConvertFileTimeToCrt((U64(fc.dwHighDateTime) << 32) | U64(fc.dwLowDateTime));
			modifyTime = QzTimeConvertFileTimeToCrt((U64(fm.dwHighDateTime) << 32) | U64(fm.dwLowDateTime));
		}

		CloseHandle(hFile);
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzSetTimeOfFile()
//
void QzSetTimeOfFile(const Utf08_t filename[], U32 createTime, U32 modifyTime)
{
	Utf16_t wideName[c_MaxPathLength];
	UtfConvert08to16(wideName, ArraySize(wideName), filename);

	HANDLE hFile = CreateFile(wideName, FILE_WRITE_ATTRIBUTES, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

	if (INVALID_HANDLE_VALUE != hFile) {
		FILETIME fc, fm;
		U64 big;

		big = QzTimeConvertCrtToFileTime(createTime);
		fc.dwHighDateTime = U32(big >> 32);
		fc.dwLowDateTime  = U32(big);

		big = QzTimeConvertCrtToFileTime(modifyTime);
		fm.dwHighDateTime = U32(big >> 32);
		fm.dwLowDateTime  = U32(big);

		// Don't bother trying to change the access time.  Trying to change
		// the access time causes it to change to the current time, so it's
		// not possible to restore the access time.
		SetFileTime(hFile, &fc, NULL, &fm);

		CloseHandle(hFile);
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzShowSystemDialogBox()
//
//	Displays a message dialog using the operating system's native dialogs.
//
void QzShowSystemDialogBox(const Utf08_t message[], const Utf08_t title[], bool error)
{
	U32 flags = MB_OK;

	if (error) {
		flags |= MB_ICONEXCLAMATION;
	}

	Utf16_t message16[256];
	Utf16_t title16[256];

	// Since Windows uses 16-bit chars for all API calls, we need to convert
	// the given UTF-8 strings into UTF-16.  Presumably this should also work
	// for CJKV symbols and any other symbols that require the full 21-bit
	// representation of Unicode, but I have no way of verifying it.
	UtfConvert08to16(message16, ArraySize(message16), message);
	UtfConvert08to16(title16, ArraySize(title16), title);

	MessageBox(NULL, message16, title16, flags);
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzSleep()
//
void QzSleep(U32 milliseconds)
{
	Sleep(milliseconds);
}


struct ThreadName_t
{
	U32   Type;			// must be 0x1000
	char* Name;			// pointer to name (in user addr space)
	U32   ThreadID;		// thread ID (-1=caller thread)
	U32   Flags;		// reserved for future use, must be zero
};


/////////////////////////////////////////////////////////////////////////////
//
//	QzSetThreadName()
//
//	This should be called from within the thread that is being named.
//	To name a thread from within a separate thread, the other thread would
//	need to explicitly set the thread ID.  It's easier for a thread to name
//	itself when it starts up.
//
void QzSetThreadName(char threadName[])
{
	ThreadName_t info;
	info.Type		= 0x1000;
	info.Name		= threadName;
	info.ThreadID	= U32(-1);
	info.Flags		= 0;

	__try {
		RaiseException(0x406D1388, 0, sizeof(info)/sizeof(U32), (U32*)&info);
	}
	__except (EXCEPTION_CONTINUE_EXECUTION) {
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzThreadSafeIncrement()
//
//	Returns the post-incremented value.
//
S32 QzThreadSafeIncrement(S32 *pValue)
{
	return InterlockedIncrement(pValue);
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzThreadSafeDecrement()
//
//	Returns the post-decrement value.
//
S32 QzThreadSafeDecrement(S32 *pValue)
{
	return InterlockedDecrement(pValue);
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzThreadSafeAdd()
//
//	Returns the sum of the two values, e.g., the post-add result in *pValue.
//
S32 QzThreadSafeAdd(S32 *pValue, S32 delta)
{
	return InterlockedExchangeAdd(pValue, delta);
}



/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////
//
//	constructor
//
QzSyncEvent::QzSyncEvent(void)
	:	m_hEvent(NULL)
{
}


/////////////////////////////////////////////////////////////////////////////
//
//	destructor
//
QzSyncEvent::~QzSyncEvent(void)
{
	if (NULL != m_hEvent) {
		CloseHandle(m_hEvent);
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	Create()
//
void QzSyncEvent::Create(void)
{
	if (NULL != m_hEvent) {
		CloseHandle(m_hEvent);
	}

	m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
}


/////////////////////////////////////////////////////////////////////////////
//
//	Signal()
//
void QzSyncEvent::Signal(void)
{
	if (NULL != m_hEvent) {
		SetEvent(m_hEvent);
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	Wait()
//
U32 QzSyncEvent::Wait(U32 milliseconds)
{
	if (NULL != m_hEvent) {
		U32 result = WaitForSingleObject(m_hEvent, milliseconds);

		if (WAIT_OBJECT_0 == result) {
			return QzSyncWait_Signalled;
		}

		if (WAIT_TIMEOUT == result) {
			return QzSyncWait_Timeout;
		}
	}

	return QzSyncWait_Error;
}


/////////////////////////////////////////////////////////////////////////////
//
//	WaitForever()
//
U32 QzSyncEvent::WaitForever(void)
{
	return Wait(INFINITE);
}



/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////



struct QzThreadContext_t
{
	char          ThreadName[32];
	U32           ThreadID;
	HANDLE        hThread;
	volatile bool StopRequested;
	volatile bool StopCompleted;
	void*         pParams;
	U32           (*Func)(void*);
	QzSyncEvent   SignalEvent;
};


/////////////////////////////////////////////////////////////////////////////
//
//	constructor
//
QzThreadControl::QzThreadControl(void)
	:	m_pContext(NULL)
{
	QzThreadContext_t *pTC = new QzThreadContext_t;
	pTC->ThreadName[0] = '\0';
	pTC->ThreadID      = 0;
	pTC->hThread       = NULL;
	pTC->StopRequested = false;
	pTC->StopCompleted = false;
	pTC->pParams       = NULL;
	pTC->Func          = NULL;
	pTC->SignalEvent.Create();

	m_pContext = pTC;
}


/////////////////////////////////////////////////////////////////////////////
//
//	destructor
//
QzThreadControl::~QzThreadControl(void)
{
	QzThreadContext_t *pTC = reinterpret_cast<QzThreadContext_t*>(m_pContext);

	if (NULL != pTC->hThread) {
		StopThread();
		WaitForClosure();
	}

	delete pTC;
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzBaseThreadFunc()
//
//	The exact prototype of the function pointer required for spawning a
//	thread is platform-specific.  So we define a hidden function that will
//	be used for spawning, which in turn calls the function pointer that
//	was provided to StartThread().
//
static unsigned __stdcall QzBaseThreadFunc(void *pContext)
{
	QzThreadContext_t *pTC = reinterpret_cast<QzThreadContext_t*>(pContext);

	// Apply the name to the worker thread.  This name will be displayed in
	// some Win32 debugging messages, and can be accessed in the debugger
	// to help figure out which thread is currenting being viewed.
	QzSetThreadName(pTC->ThreadName);

	UtfFormat fmt;
	fmt.AddString(pTC->ThreadName);
	LogMessage("thread enter: %1;", fmt);

	// Invoke the static thread function defined in higher-level code.
	// This function call will not return until the thread has finished.
	//
	unsigned result = pTC->Func(pTC->pParams);

	fmt.AddInt(result);
	LogMessage("thread exit: %1;, code %2;", fmt);

	// The last thing we do is set this volatile flag, indicating that the
	// thread has terminated normally.
	pTC->StopCompleted = true;

	// Note that after this point, the various flags and thread resource
	// pointers in *pTC are still initialized.  Some of these values may be
	// needed to access OS-level information about the status of the closed
	// thread.  The most important one is the thread handle, which is needed
	// to the state of the thread in WaitForClosure();

	// Return the result code from pTC->Func().  This gets passed through to
	// Win32, which will write a termination message to debug output.
	return result;
}


/////////////////////////////////////////////////////////////////////////////
//
//	StartThread()
//
//	Note: This assumes that threads will not be subjected to repeated start
//	and stop operations.  If you're needing to restart a thread, this code
//	would also need to verify that the thread is not already alive,
//
bool QzThreadControl::StartThread(U32 (*func)(void*), void *pParams, const char threadName[])
{
	QzThreadContext_t *pTC = reinterpret_cast<QzThreadContext_t*>(m_pContext);

	// Do not attempt to start the thread if one is already running.
	if (NULL != pTC->hThread) {
		return false;
	}

	// The thread name needs to be ASCII, so we need to use standard string
	// copy functions instead of UTF-8 routines.  And since strncpy() is not
	// safe, we must explicitly make certain the copied string is terminated.
	strncpy(pTC->ThreadName, threadName, ArraySize(pTC->ThreadName));
	pTC->ThreadName[ArraySize(pTC->ThreadName)-1] = '\0';

	pTC->StopRequested = false;
	pTC->StopCompleted = false;
	pTC->pParams       = pParams;
	pTC->Func          = func;

	// Use a temporary variable to hold the thread ID, since strict compiling
	// will complain about pointer types if we attempt to pass in a pointer
	// to any other integer type.
	unsigned int id = 0;
	pTC->hThread  = reinterpret_cast<HANDLE>(_beginthreadex(NULL, 0, QzBaseThreadFunc, m_pContext, 0, &id));
	pTC->ThreadID = id;

	// NOTE: _beginthreadex returns NULL if it cannot create the thread
	// (as opposed to _beginthread, which returns the inconsistently used
	// INVALID_HANDLE_VALUE value if there is an error)

	return (NULL != pTC->hThread);
}


/////////////////////////////////////////////////////////////////////////////
//
//	StopThread()
//
void QzThreadControl::StopThread(void)
{
	QzThreadContext_t *pTC = reinterpret_cast<QzThreadContext_t*>(m_pContext);

    // First, set this volatile flag.  WaitForClosure() will test this
    // flag to determine whether the thread should terminate.
	pTC->StopRequested = true;

    // Then we signal this event.  This will wake up the worker thread
    // so it can check the status of the StopRequested flag.
	pTC->SignalEvent.Signal();
}


/////////////////////////////////////////////////////////////////////////////
//
//	WaitForClosure()
//
//	This is called by the parent thread.  It causes that thread to halt
//	until the worker thread has terminated.  This presumes that the parent
//	has called StopThread() right before this, but still allows the stop and
//	wait calls to be made from different places in the code.
//
U32 QzThreadControl::WaitForClosure(void)
{
	QzThreadContext_t *pTC = reinterpret_cast<QzThreadContext_t*>(m_pContext);

	U32 result = QzSyncWait_Signalled;

	// WaitForClosure() may be called more than once to assure that a thread
	// is really stopped, as part of fall-back error handling when shutting
	// down.  So this pointer is NULL after the thread has been stopped.
	//
	if (NULL != pTC->hThread) {
		//
		// WARNING: You may be tempted to change the time-out period to
		// something small if the thread appears to deadlock when terminating
		// (i.e., WaitForSingleObject never returns).  This can happen when
		// dealing with many threads doing many things.  Do not do this!
		// Never change the time-out period.  It will lead to infrequent,
		// subtle errors cropping up in other places because code ends up
		// running after its resources have been deleted.  These bugs are
		// extremely difficult to reproduce and isolate since the symptoms
		// manifest in places unrelated to the cause.  If this code is
		// hanging, the problem is a deadlock in some other place.  Find
		// and fix that bug.  I cannot stress this strongly enough:
		//
		//              Always use INFINITE for the timeout!
		//
		U32 flag = WaitForSingleObject(pTC->hThread, INFINITE);

		// Once the thread is done executing, we need to close the handle to
		// release the associated resources, and so the rest of the logic in
		// this class can detect that the thread is has finished.
		CloseHandle(pTC->hThread);
		pTC->hThread = NULL;

		if (WAIT_OBJECT_0 == flag) {
			result = QzSyncWait_Signalled;
		}
		else if (WAIT_TIMEOUT == result) {
			result = QzSyncWait_Timeout;
		}
		else {
			result = QzSyncWait_Error;
		}
	}

	return result;
}


/////////////////////////////////////////////////////////////////////////////
//
//	WaitForEvent()
//
U32 QzThreadControl::WaitForEvent(U32 milliseconds)
{
	QzThreadContext_t *pTC = reinterpret_cast<QzThreadContext_t*>(m_pContext);

	// Test the termination flag before waiting.
	if (pTC->StopRequested) {
		return QzSyncWait_StopRequest;
	}

	U32 result = pTC->SignalEvent.Wait(milliseconds);

	// Test the termination flag once more after the wait completes.
	// We have no idea how long the wait required, so the flag may have
	// been set in the interrum.  This requires that the flag be volatile
	// to avoid memory caching problems.
	if (pTC->StopRequested) {
		return QzSyncWait_StopRequest;
	}

	return result;
}


/////////////////////////////////////////////////////////////////////////////
//
//	SignalThread()
//
void QzThreadControl::SignalThread(void)
{
	QzThreadContext_t *pTC = reinterpret_cast<QzThreadContext_t*>(m_pContext);

	pTC->SignalEvent.Signal();
}


/////////////////////////////////////////////////////////////////////////////
//
//	BoostPriority()
//
//	Just set the priority a little higher than normal.  This amount should be
//	safe as long as it is used with moderation -- only some threads (like
//	audio callbacks) really need to deserve to have their priority elevated.
//	This should only be used with threads that need to run reliably, but only
//	take up a small amount of CPU time.
//
void QzThreadControl::BoostPriority(void)
{
	QzThreadContext_t *pTC = reinterpret_cast<QzThreadContext_t*>(m_pContext);

	if (NULL != pTC->hThread) {
		SetThreadPriority(pTC->hThread, THREAD_PRIORITY_ABOVE_NORMAL);
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	GetID()
//
U32 QzThreadControl::GetID(void)
{
	QzThreadContext_t *pTC = reinterpret_cast<QzThreadContext_t*>(m_pContext);

	return pTC->ThreadID;
}


/////////////////////////////////////////////////////////////////////////////
//
//	IsThreadAlive()
//
bool QzThreadControl::IsThreadAlive(void)
{
	QzThreadContext_t *pTC = reinterpret_cast<QzThreadContext_t*>(m_pContext);

	return (false == pTC->StopCompleted);
}


/////////////////////////////////////////////////////////////////////////////
//
//	TestStopFlag()
//
bool QzThreadControl::TestStopFlag(void)
{
	QzThreadContext_t *pTC = reinterpret_cast<QzThreadContext_t*>(m_pContext);

	return pTC->StopRequested;
}



/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////
//
//	constructor
//
QzCriticalSection::QzCriticalSection(void)
{
	CRITICAL_SECTION *pCrit = new CRITICAL_SECTION;
	m_pCritSection = pCrit;

	InitializeCriticalSection(pCrit);
}


/////////////////////////////////////////////////////////////////////////////
//
//	destructor
//
QzCriticalSection::~QzCriticalSection(void)
{
	CRITICAL_SECTION *pCrit = reinterpret_cast<CRITICAL_SECTION*>(m_pCritSection);
	DeleteCriticalSection(pCrit);
	delete pCrit;
}


/////////////////////////////////////////////////////////////////////////////
//
//	Lock()
//
void QzCriticalSection::Lock(void)
{
	EnterCriticalSection(reinterpret_cast<CRITICAL_SECTION*>(m_pCritSection));
}


/////////////////////////////////////////////////////////////////////////////
//
//	Unlock()
//
void QzCriticalSection::Unlock(void)
{
	LeaveCriticalSection(reinterpret_cast<CRITICAL_SECTION*>(m_pCritSection));
}



/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////
//
//	constructor
//
QzSharedMemory::QzSharedMemory(void)
	:	m_hFile(NULL),
		m_pAddress(NULL),
		m_ByteCount(0),
		m_AlreadyExisted(false)
{
}


/////////////////////////////////////////////////////////////////////////////
//
//	destructor
//
QzSharedMemory::~QzSharedMemory(void)
{
	Free();
}


/////////////////////////////////////////////////////////////////////////////
//
//	InitShare()
//
//	Create a shared address range that can be used by multiple processes to
//	exchange data.  This requires that all processes use the same name for
//	the shared memory, and reserve the exact same amount of memory.
//
//	Parameters:
//		name         - name of memory file that backs the shared virtual memory
//		byteCount    - maximum size of shared memory
//		allowCaching - flag to control page caching
//
bool QzSharedMemory::InitShare(const char name[], U32 byteCount, bool allowCaching)
{
	Free();

	// This is the default permission flag required to read and write to
	// shared memory.
	U32 protection = PAGE_READWRITE;

	// Add the following flags to prevent page caching.  Disabling caching
	// will deliver a slightly better performance, but prevents data changes
	// from immediately being visible to other processes that are accessing
	// this memory (meaning that the last range of data written to shared
	// memory before a crash may end up being lost -- exercise caution
	// when commenting out these flags).
	//
	if (false == allowCaching) {
		protection |= SEC_COMMIT | SEC_NOCACHE;
	}

	m_ByteCount = byteCount;

	// Attempt to open a shared, memory-mapped file that will be used for
	// the shared buffer.
	//
	// The two important values here are the name and byte count.  All
	// apps that are mapping the same memory space must use the exact
	// same "file name" to create the memory file.  This file exists in
	// memory only, without any disk space backing it (since we're passing
	// in INVALID_HANDLE_VALUE as the handle to the file), but it is still
	// a file as far as the OS is concerned.
	//
	// The other important value is m_ByteCount.  This specifies the
	// maximum size of the buffer.  This defines how much memory space is
	// reserved for the virtual file.  The memory will not be committed
	// until it is mapped with MapViewOfFile(), but the value provided
	// here will limit the maximum amount of memory that can be accessed
	// by MapViewOfFile().
	//
	// Note that we're calling the ASCII version of the function, since
	// we were given a char[] containing the file name.
	//
	m_hFile = ::CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, protection, 0, m_ByteCount, name);

	// Immediately test GetLastError().  This will indicate whether the
	// buffer was created successfully, or if it "failed" because the
	// buffer already exists.  Either way, a valid handle to the memory
	// file was returned, so we can use that handle for shared memory.
	// The only distinction drawn here is that if the memory file already
	// exists, we know that some other app has already created the memory
	// file.  Since shared memory is only being used by the logging code,
	// this indicates that a separate logging utility has reserved the
	// memory and is waiting to catch any log info in the event that this
	// process crashes.
	//
	m_AlreadyExisted = (ERROR_ALREADY_EXISTS == GetLastError());

	// Now check the return value.  If it returned the error code we gave
	// to the function (which was INVALID_HANDLE_VALUE), then the buffer
	// could not be created for some reason.
	//
	// However, we're going to be overly paranoid here.  Although testing
	// of the code shows it returns INVALID_HANDLE_VALUE, the docs say it
	// will return NULL if it fails.  Test both possible values to avoid
	// any chance of being screwed over by the inconsistent use of the
	// INVALID_HANDLE_VALUE value.
	//
	if ((INVALID_HANDLE_VALUE == m_hFile) || (NULL == m_hFile)) {
		return false;
	}

	// Now map the shared file into virtual memory so we can read/write
	// it.  Note that we are mapping the full range of memory created by
	// the call to CreateFileMapping().
	//
	m_pAddress = reinterpret_cast<U08*>(MapViewOfFile(m_hFile, FILE_MAP_ALL_ACCESS, 0, 0, m_ByteCount));

	// If for some reason the file could not be mapped, we need to close
	// the handle so the file will be freed, then return failure so the
	// caller does not attempt to access the file.
	//
	if (NULL == m_pAddress) {
		CloseHandle(m_hFile);
		m_hFile = NULL;

		return false;
	}

	return true;
}


/////////////////////////////////////////////////////////////////////////////
//
//	Free()
//
void QzSharedMemory::Free(void)
{
	if (m_pAddress != NULL) {
		UnmapViewOfFile(reinterpret_cast<void*>(m_pAddress));
		m_pAddress = NULL;
	}

	if (m_hFile != NULL) {
		CloseHandle(m_hFile);
		m_hFile = NULL;
	}

	m_ByteCount      = 0;
	m_AlreadyExisted = false;
}



