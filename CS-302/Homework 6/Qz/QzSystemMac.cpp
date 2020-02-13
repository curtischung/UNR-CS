/////////////////////////////////////////////////////////////////////////////
//
//	File: QzSystemMac.cpp
//
//	$Header: /Projects/Qz/QzSystemMac.cpp  11  2010/4/3 1:58:14p  Lee $
//
//
//	This file implements a number of shallow wrappers around MacOS calls that
//	are made from different places in the project.  By using these functions
//	instead of the native MacOS calls, we can remove the need to include any
//	Carbon or other system header files in other CPP files, which dramatically
//	improves compile times, and allows most of the code to be platform
//	independent.
//
/////////////////////////////////////////////////////////////////////////////


#include "QzCommon.h"
#include <Carbon/Carbon.h>
#include <errno.h>
#include <libgen.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/sysctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>

// #include <unistd.h>


#ifdef USE_MALLOC_MACRO
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// The following magic numbers are used to map to and from Win32's FILETIME
// values (which have a 10 MHz frequency).
//
// January 1st, 1970 in FILETIME: 0x019DB1DED53E8000 == 116444736000000000
//
#define c_FileTimeFor1970		116444736000000000ULL
#define c_FileTimeFrequency		10000000


// Critical section needed to protect against opening files, since some of
// the hacks below require changing directories, which can temporarily screw
// up the expected path.
//
static QzCriticalSection* g_pFileSection = NULL;

static bool g_IsMouseLocked = false;

//static PasteboardRef g_Clipboard = NULL;


/////////////////////////////////////////////////////////////////////////////
//
//	QzSystemInit()
//
void QzSystemInit(void)
{
	g_pFileSection = new QzCriticalSection;

//	PasteboardCreate(kPasteboardClipboard, &g_Clipboard);
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzSystemUninit()
//
void QzSystemUninit(void)
{
	SafeDelete(g_pFileSection);

//	if (NULL != g_Clipboard) {
//		CFRelease(g_Clipboard);
//		g_Clipboard = NULL;
//	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzDebugBreak()
//
//	Trigger a breakpoint in the debugger.
//
void QzDebugBreak(void)
{
// FIXME: is there an xcode/gcc equivalent for DebugBreak() ?
// Debugger/DebugStr are kernel-level breakpoints for remote debugging.
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
	QuitApplicationEventLoop();
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
void QzLockMouse(bool locked)
{
	if (locked) {
		if (false == g_IsMouseLocked) {
			g_IsMouseLocked = true;
			HideCursor();
		}
	}
	else {
		if (g_IsMouseLocked) {
			g_IsMouseLocked = false;
			ShowCursor();
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
// FIXME: play some kind of alert sound
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzGetProcessorCount()
//
U32 QzGetProcessorCount(void)
{
	int    mib[2];
	size_t len;
	S32    cpuCount = 1;

	mib[0] = CTL_HW;
	mib[1] = HW_NCPU;
	sysctl(mib, 2, NULL, &len, NULL, 0);
	if (4 == len) {
		sysctl(mib, 2, &cpuCount, &len, NULL, 0);

		cpuCount = ClampRange(1, cpuCount, 64);
	}

	return cpuCount;
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzGetProcessorUsage()
//
float QzGetProcessorUsage(void)
{
	// Find out the average CPU usage.  This will return the average usage:
	//   [0] = previous 1 minute
	//   [1] = previous 5 minutes
	//   [2] = previous 15 minutes
	//
	// For this code, we only care about the previous 60 seconds (actually,
	// a shorter window would be more responsive and informative when
	// working with graphical apps, but that's not an option on this
	// platform.)
	//
	double avg[3] = { 0.0, 0.0, 0.0 };
	getloadavg(avg, 1);
	return float(avg[0]);
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
	UnsignedWide wide;
	Microseconds(&wide);

	// Mux the two portions of the timestamp into a single 64-bit value,
	// then divide the result to convert microseconds to milliseconds.
	return U32(((U64(wide.hi) << 32) | U64(wide.lo)) / 1000);
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzPrecisionClockFrequency()
//
//	Reports the precision of the clock.  This code uses the Microsecond()
//	function, so the frequency is 1 MHz.
//
U64 QzPrecisionClockFrequency(void)
{
	return 1000000;
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzPrecisionClockRead()
//
//	Report the current time with the highest precision clock on the system.
//
U64 QzPrecisionClockRead(void)
{
	UnsignedWide wide;
	Microseconds(&wide);

	// Mux the two portions of the timestamp into a single 64-bit value.
	return (U64(wide.hi) << 32) | U64(wide.lo);
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzTimeGetCrt()
//
//	Returns the current time as a time_t value, but casts away that specific
//	type to hide the low-level implementation.  This uses the Linux 31-bit
//	time values, which wrap around in 2037.
//
U32 QzTimeGetCrt(void)
{
	return U32(time(NULL));
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzTimeConvertCrtToFileTime()
//
//	Converts 32-bit time_t values into 64-bit FILETIME values.
//
U64 QzTimeConvertCrtToFileTime(U32 timestamp)
{
	// Add half of the frequency to round off to the nearest second.
	// This helps maintain symmetry when going back and forth between
	// CRT's time_t and FILETIME values.
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
//	Given a 31-bit time_t value, this will convert the time into a formatted
//	string.
//
void QzTimeConvertCrtToString(U32 timestamp, Utf08_t buffer[], U32 bufferSize)
{
	time_t t32 = U32(timestamp);
	tm timestruct = *(localtime(&t32));

	if (0 == timestruct.tm_hour) {
		timestruct.tm_hour = 24;
	}

	UtfFormat fmt;
	fmt.AddInt(timestruct.tm_year + 1900);
	fmt.AddInt(timestruct.tm_mon + 1);
	fmt.AddInt(timestruct.tm_mday);
	fmt.AddInt(timestruct.tm_hour);
	fmt.AddInt(timestruct.tm_min);
	fmt.AddInt(timestruct.tm_sec);
	fmt.Generate(buffer, bufferSize, reinterpret_cast<const Utf08_t*>("%1w4;/%2w2p0;/%3w2p0; %4w2;:%5w2p0;:%6w2p0;"));
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzTimeToFields()
//
void QzTimeToFields(U32 timestamp, QzTime_t &timeRef)
{
	time_t t32 = time_t(timestamp);
	tm timestruct = *(localtime(&t32));

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

	return U32(mktime(&timestruct));
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzGetDateString()
//
//	Generates a string with the current year/month/day.
//
void QzGetDateString(Utf08_t buffer[], U32 maxLength)
{
	time_t t32 = time(NULL);
	tm timestruct = *(localtime(&t32));

	UtfFormat fmt;
	fmt.AddInt(timestruct.tm_year + 1900);
	fmt.AddInt(timestruct.tm_mon + 1);
	fmt.AddInt(timestruct.tm_mday);
	fmt.Generate(buffer, maxLength, reinterpret_cast<const Utf08_t*>("%1w4;/%2w2p0;/%3w2p0;"));
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzGetTimeString()
//
//	Generates a string with the current hour:minutes:seconds.
//
void QzGetTimeString(Utf08_t buffer[], U32 maxLength)
{
	time_t t32 = time(NULL);
	tm timestruct = *(localtime(&t32));

	UtfFormat fmt;
	fmt.AddInt(timestruct.tm_hour);
	fmt.AddInt(timestruct.tm_min);
	fmt.AddInt(timestruct.tm_sec);
	fmt.Generate(buffer, maxLength, reinterpret_cast<const Utf08_t*>("%1w2;:%2w2p0;:%3w2p0;"));
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzGetNameOfComputer()
//
void QzGetNameOfComputer(Utf08_t name[], U32 maxLength)
{
	char scratch[256];

	if (0 != gethostname(scratch, ArraySize(scratch))) {
		UtfCopy(name, maxLength, CharToUtf("MacBook"));
	}

	scratch[ArraySize(scratch)-1] = '\0';

	UtfComposeASCII(name, maxLength, scratch);
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzAbsolutePathOfFile()
//
//	Converts a relative file path to an absolute path.
//
//	Returns the number of bytes stored in absolute[], or zero if the path
//	does not exist.
//
U32 QzAbsolutePathOfFile(Utf08_t absolute[], U32 maxLength, const Utf08_t relative[])
{
	if (maxLength < 2) {
		return 0;
	}

	// Protect the file path.  Since we have to screw with the current path
	// to figure out the path of the specified file, this will temporarily
	// break any calls to FileOpen().
	QzCriticalLock lock(g_pFileSection);

	// There is no fullpath() function on Mac.  So jump through some hoops.

	char oldPath[c_MaxPathLength];
	char partial[c_MaxPathLength];
	char newPath[c_MaxPathLength];

	// Record the name of the current working directory, so we can revert
	// back to it when we are done.
	getcwd(oldPath, ArraySize(oldPath));

	// Strip off the relative part of the path name, ignoring the file name.
	UtfCopy((Utf08_t*)partial, ArraySize(partial), (Utf08_t*)dirname((char*)relative));

	// Using only the relative path, switch to the directory containing the
	// file we're looking for.
	chdir(partial);

	// Now when we get the current working directory, we have absolute path
	// to the file.
	getcwd(newPath, ArraySize(newPath));

	// Finally, restory the original working directory so we don't screw up
	// any other threads that are trying to access a specific directory.
	chdir(oldPath);

	// Special case for when we're looking up the current working path.
	// Avoid concatenating "./" to the end of the path.  It looks goofy,
	// and it confuses QzDirList::UpOneLevel().
	if (('.' == relative[0]) && ('\0' == relative[1])) {
		return UtfCopy(absolute, maxLength, reinterpret_cast<Utf08_t*>(newPath));
	}

	// Concatenate the relative path and the file name to generate the full
	// absolute path to the file.
	UtfFormat fmt;
	fmt.AddString(newPath);
	fmt.AddString(basename((char*)relative));
	return fmt.Generate(absolute, maxLength, reinterpret_cast<const Utf08_t*>("%1;/%2;"));
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzSplitPath()
//
//	Wraps the system implementation of splitpath(), and guarantees that the
//	resulting directory path contains only '/' instead of '\\'.
//
//	Takes the path in <path> and splits it up into the basic drive/dir/name/ext
//	sequence.  Since there are no drive names on Macs, that string will always
//	be empty.
//
//	This assumes that all of the given buffers are [c_MaxPathLength] size.
//
void QzSplitPath(const Utf08_t path[], Utf08_t drive[], Utf08_t directory[], Utf08_t filename[], Utf08_t extension[])
{
	// The drive name is never valid on a Mac.
	drive[0] = '\0';

	// Copy the full filename to the directory buffer.  That's where we'll
	// start screwing with it.
	UtfCopy(directory, c_MaxPathLength, path);

	// Fix any backslashes that occur in the path.
	QzBackslashToSlash(directory, c_MaxPathLength);

	// Try to find the last '/' in the filename.
	Utf08_t *pFilename = UtfContainsLastSymbol(directory, '/');

	// If there is one, then the file name has to come after the '/'.
	if (NULL != pFilename) {
		// Copy the next char after the '/', since that should be the first
		// char of the filename (or '\0' if there is no name).
		UtfCopy(filename, c_MaxPathLength, pFilename + 1);

		// Truncate the string in directory[] so it no longer contains the
		// filename.
		pFilename[1] = '\0';

		// Then look to see if the filename has an extension.
		Utf08_t *pExtension = UtfContainsLastSymbol(filename, '.');

		if (NULL != pExtension) {
			UtfCopy(extension, c_MaxPathLength, pExtension);

			// Truncate filename[] so it does not include the extension.
			pExtension[0];
		}
		else {
			extension[0] = '\0';
		}
	}
	else {
		filename[0]  = '\0';
		extension[0] = '\0';
	}
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
	QzCriticalLock lock(g_pFileSection);

	S32 result = mkdir(reinterpret_cast<const char*>(path), 0777);

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
	// Apple uses UTF-8 for file names, but it requires that the file names
	// be in their canonical decomposition form.  It's not clear if Apple
	// will reformat the string, so we'll explicitly decompose it here to
	// make certain that it is (or at least should be) properly formatted.
	Utf08_t decomposed[1024];
	UtfCanonicalDecompose08to08(decomposed, ArraySize(decomposed), path);

	Utf08_t partial[c_MaxPathLength];
	U32     offset = 0;

	do {
		while (('/' != decomposed[offset]) && ('\0' != decomposed[offset])) {
			partial[offset] = decomposed[offset];
			++offset;
		}
		partial[offset] = '\0';

		// We hit the end of the string, without seeing a trailing '/'.
		// Assume that whatever is at the end of the path is the name of
		// a file, so we don't need to try to create that directory.
		if ('\0' == decomposed[offset]) {
			return;
		}

		// Avoid special case under Windows if the path starts with the
		// name of a drive (which it always should for Windows).
		if ((2 != offset) || (':' != partial[offset-1])) {
			// Note that mkdir is harmless -- it will do nothing if this
			// folder already exists.
			mkdir(reinterpret_cast<char*>(partial), 0777);
		}

		// Copy the next '/' to the end of the partial path.
		// Note that we cannot have gotten to this point if path[offset] is
		// not '/'.
		partial[offset] = decomposed[offset];
		++offset;
	} while ('\0' != decomposed[offset]);
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzGetCurrentDirectory()
//
bool QzGetCurrentDirectory(Utf08_t dirname[], U32 maxLength)
{
	Utf08_t tempname[1024];
	if (NULL == getcwd(reinterpret_cast<char*>(tempname), ArraySize(tempname))) {
		return false;
	}

	UtfCompose08to08(dirname, maxLength, tempname);

	return true;
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzSetCurrentDirectory()
//
bool QzSetCurrentDirectory(const Utf08_t dirname[])
{
	// Apple uses UTF-8 for file names, but it requires that the file names
	// be in their canonical decomposition form.  It's not clear if Apple
	// will reformat the string, so we'll explicitly decompose it here to
	// make certain that it is (or at least should be) properly formatted.
	Utf08_t decomposed[1024];
	UtfCanonicalDecompose08to08(decomposed, ArraySize(decomposed), dirname);

	return (0 == chdir(reinterpret_cast<char*>(decomposed)));
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzReadFromClipboard()
//
//	The reported byte count does not include the '\0';
//
Utf08_t* QzReadFromClipboard(U32 &byteCount)
{
/*
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
*/
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzWriteToClipboard()
//
void QzWriteToClipboard(Utf08_t text[])
{
/*
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
*/
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzFileOpen()
//
FILE* QzFileOpen(const Utf08_t filename[], U32 direction)
{
	if ((NULL == filename) || ('\0' == filename[0])) {
		return NULL;
	}

	// Apple uses UTF-8 for file names, but it requires that the file names
	// be in their canonical decomposition form.  It's not clear if Apple
	// will reformat the string, so we'll explicitly decompose it here to
	// make certain that it is (or at least should be) properly formatted.
	Utf08_t decomposed[1024];
	UtfCanonicalDecompose08to08(decomposed, ArraySize(decomposed), filename);

	QzCriticalLock lock(g_pFileSection);

	return fopen(reinterpret_cast<const char*>(decomposed), (QzFileOpen_Read == direction) ? "r" : "w");
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

	// Apple uses UTF-8 for file names, but it requires that the file names
	// be in their canonical decomposition form.  It's not clear if Apple
	// will reformat the string, so we'll explicitly decompose it here to
	// make certain that it is (or at least should be) properly formatted.
	Utf08_t decomposed[1024];
	UtfCanonicalDecompose08to08(decomposed, ArraySize(decomposed), filename);

	// Zero == success.
	return (0 == remove(reinterpret_cast<const char*>(decomposed)));
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

	QzCriticalLock lock(g_pFileSection);

	return rename(reinterpret_cast<const char*>(oldname), reinterpret_cast<const char*>(newname));
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzGetTimeOfFile()
//
void QzGetTimeOfFile(const Utf08_t filename[], U32 &createTime, U32 &modifyTime)
{
	// Apple uses UTF-8 for file names, but it requires that the file names
	// be in their canonical decomposition form.  It's not clear if Apple
	// will reformat the string, so we'll explicitly decompose it here to
	// make certain that it is (or at least should be) properly formatted.
	Utf08_t decomposed[1024];
	UtfCanonicalDecompose08to08(decomposed, ArraySize(decomposed), filename);

	struct stat64 statInfo;
	if (0 == stat64(reinterpret_cast<char*>(decomposed), &statInfo)) {
		createTime = statInfo.st_birthtimespec.tv_sec;
		modifyTime = statInfo.st_mtimespec.tv_sec;
	}
	else {
		createTime = 0;
		modifyTime = 0;
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzSetTimeOfFile()
//
void QzSetTimeOfFile(const Utf08_t filename[], U32 createTime, U32 modifyTime)
{
	// Apple uses UTF-8 for file names, but it requires that the file names
	// be in their canonical decomposition form.  It's not clear if Apple
	// will reformat the string, so we'll explicitly decompose it here to
	// make certain that it is (or at least should be) properly formatted.
	Utf08_t decomposed[1024];
	UtfCanonicalDecompose08to08(decomposed, ArraySize(decomposed), filename);
// FIXME:
	QzCriticalLock lock(g_pFileSection);

	struct timeval t[2];
	t[0].tv_sec  = modifyTime;
	t[0].tv_usec = 0;
	t[1].tv_sec  = modifyTime;
	t[1].tv_usec = 0;

	utimes(reinterpret_cast<char*>(decomposed), t);
//
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzShowSystemDialogBox()
//
void QzShowSystemDialogBox(const Utf08_t message[], const Utf08_t title[], bool error)
{
/*
	U32 flags = MB_OK;

	if (error) {
		flags |= MB_ICONEXCLAMATION;
	}

	MessageBox(NULL, message, title, flags);
*/
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzSleep()
//
//	Puts the current thread to sleep for a specified number of milliseconds.
//	This does not guarantee that the full duration of the sleep occurs,
//	since task switching may result in the thread waking up early.
//
void QzSleep(U32 milliseconds)
{
	timespec delay;
	delay.tv_sec  = milliseconds / 1000;
	delay.tv_nsec = (milliseconds % 1000) * 1000000;

	// The second parameter takes another timespec, which will return how
	// much time is remaining to be slept if the sleep was interrupted.
	nanosleep(&delay, NULL);
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzThreadSafeIncrement()
//
//	Returns the post-incremented value.
//
S32 QzThreadSafeIncrement(S32 *pValue)
{
	// Add a +1 since IncrementAtomic() returns the pre-increment value.
	return IncrementAtomic(pValue) + 1;
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzThreadSafeDecrement()
//
//	Returns the post-decrement value.
//
S32 QzThreadSafeDecrement(S32 *pValue)
{
	// Subtract a -1 since DecrementAtomic() returns the pre-decrement value.
	return DecrementAtomic(pValue) - 1;
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzThreadSafeAdd()
//
//	Returns the sum of the two values, e.g., the post-add result in *pValue.
//
S32 QzThreadSafeAdd(S32 *pValue, S32 delta)
{
	return AddAtomic(pValue, delta);
}



/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


struct QzSyncEvent_t
{
	pthread_cond_t  Cond;
	pthread_mutex_t Mutex;
};


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
	QzSyncEvent_t *pEvent = reinterpret_cast<QzSyncEvent_t*>(m_hEvent);

	if (NULL != pEvent) {
		pthread_cond_destroy(&(pEvent->Cond));
		pthread_mutex_destroy(&(pEvent->Mutex));
		delete pEvent;
		m_hEvent = NULL;
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	Create()
//
void QzSyncEvent::Create(void)
{
	QzSyncEvent_t *pEvent = reinterpret_cast<QzSyncEvent_t*>(m_hEvent);

	if (NULL != pEvent) {
		pthread_cond_destroy(&(pEvent->Cond));
		pthread_mutex_destroy(&(pEvent->Mutex));
	}

	if (NULL == pEvent) {
		pEvent   = new QzSyncEvent_t;
		m_hEvent = pEvent;
	}

	pthread_cond_init(&(pEvent->Cond), NULL);
	pthread_mutex_init(&(pEvent->Mutex), NULL);
}


/////////////////////////////////////////////////////////////////////////////
//
//	Signal()
//
void QzSyncEvent::Signal(void)
{
	if (NULL != m_hEvent) {
		QzSyncEvent_t *pEvent = reinterpret_cast<QzSyncEvent_t*>(m_hEvent);

		pthread_cond_signal(&(pEvent->Cond));
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	Wait()
//
U32 QzSyncEvent::Wait(U32 milliseconds)
{
	if (NULL != m_hEvent) {
		QzSyncEvent_t *pEvent = reinterpret_cast<QzSyncEvent_t*>(m_hEvent);

		// pthread_cond_timedwait uses an absolute time to denote when to
		// stop waiting, instead of a relative value.  So we must query the
		// current time (which is in seconds + microseconds), convert to a
		// different struct (seconds + nanoseconds), and add the millisecond
		// timeout wait duration.
		timeval curr;
		gettimeofday(&curr, NULL);

		timespec abstime;
		abstime.tv_sec  = curr.tv_sec + milliseconds / 1000;
		abstime.tv_nsec = (curr.tv_usec * 1000) + ((milliseconds % 1000) * 1000000);

		// Protect against rollover of nanoseconds.
		abstime.tv_sec += abstime.tv_nsec / 1000000000;
		abstime.tv_nsec = abstime.tv_nsec % 1000000000;

		S32 result = pthread_cond_timedwait(&(pEvent->Cond), &(pEvent->Mutex), &abstime);

		if (0 == result) {
			return QzSyncWait_Signalled;
		}

		if (ETIMEDOUT == result) {
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
	if (NULL != m_hEvent) {
		QzSyncEvent_t *pEvent = reinterpret_cast<QzSyncEvent_t*>(m_hEvent);

		S32 result = pthread_cond_wait(&(pEvent->Cond), &(pEvent->Mutex));

		if (0 == result) {
			return QzSyncWait_Signalled;
		}
	}

	return QzSyncWait_Error;
}



/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////



struct QzThreadContext_t
{
	char          ThreadName[32];
	U32           ThreadID;
	pthread_t     hThread;
	volatile bool StopRequested;
	volatile bool StopCompleted;
	void*         pParams;
	U32           (*Func)(void*);
	QzSyncEvent   SignalEvent;
};


// Thread IDs are only used for compatibility with Win32, since they can be
// useful in Win32 for debugging purposes.
static S32 g_NextThreadID = 0;


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
//	BaseThreadFunc()
//
//	The exact prototype of the function pointer required for spawning a
//	thread is platform-specific.  So we define a hidden function that will
//	be used for spawning, which in turn calls the function pointer that
//	was provided to StartThread().
//
static void* QzBaseThreadFunc(void *pContext)
{
	QzThreadContext_t *pTC = reinterpret_cast<QzThreadContext_t*>(pContext);

	// Write out a message to the log file (if one is open).  We will record
	// the thread name in the log file.  However, since MacOS does not have
	// a mechanism to name threads, we cannot do anything else useful with
	// the name.
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

	// Return NULL, since the return value is not important to pthreads.
	// It is possible for the app to retrieve the return pointer, but that
	// logic is specific to pthreads.  Since it does not generalize to Win32,
	// we cannot do anything useful with this pointer.
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
//
//	StartThread()
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
	pTC->ThreadID      = QzThreadSafeIncrement(&g_NextThreadID);

	pthread_create(&(pTC->hThread), NULL, QzBaseThreadFunc, m_pContext);

	return true;
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
	if (NULL != pTC->hThread) {
		// This gets assigned the void* pointer that is returned by the
		// function pointer that was passed to StartThread.
		void *pReturnedVoid = NULL;

		// Wait for the thread to terminate.
		S32 result = pthread_join(pTC->hThread, &pReturnedVoid);

		pTC->hThread = NULL;

		if (0 == result) {
			result = QzSyncWait_Signalled;
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
	pthread_mutex_t *pMutex = new pthread_mutex_t;

	pthread_mutex_init(pMutex, NULL);

	m_pCritSection = reinterpret_cast<void*>(pMutex);
}


/////////////////////////////////////////////////////////////////////////////
//
//	destructor
//
QzCriticalSection::~QzCriticalSection(void)
{
	pthread_mutex_t *pMutex = reinterpret_cast<pthread_mutex_t*>(m_pCritSection);

	pthread_mutex_destroy(pMutex);
	delete pMutex;
}


/////////////////////////////////////////////////////////////////////////////
//
//	Lock()
//
void QzCriticalSection::Lock(void)
{
	pthread_mutex_lock(reinterpret_cast<pthread_mutex_t*>(m_pCritSection));
}


/////////////////////////////////////////////////////////////////////////////
//
//	Unlock()
//
void QzCriticalSection::Unlock(void)
{
	pthread_mutex_unlock(reinterpret_cast<pthread_mutex_t*>(m_pCritSection));
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
//		allowCaching - this is not used on the OSX version of the code
//
bool QzSharedMemory::InitShare(const char name[], U32 byteCount, bool allowCaching)
{
	Free();

	// For generality, we are given the name of a virtual file that will
	// be used to share memory.  However, for shm_open() to work, the name
	// needs to start with '/', so we will make a copy of the name that
	// starts with the required symbol.
	//
	char sharedName[64];
	strncpy(sharedName + 1, name, ArraySize(sharedName) - 2);
	sharedName[0] = '/';
	sharedName[ArraySize(sharedName)-1] = '\0';

	// Call shm_open() to create a virtual file that will be used to back
	// the shared memory.  This file is created without any physical
	// backing, since we don't want to persist the data to disk, nor do we
	// want to run the performance risk of writing to a physical device
	// when logging large amounts of data.
	//
	// Note that for cross-platform compatibility, the return result is
	// being stored in a void* pointer instead of an int.
	//
	m_hFile = (Handle_t)shm_open(sharedName, O_RDWR);

	// Failure returns -1 instead of NULL.
	if (-1 == (int)m_hFile) {
		m_hFile = NULL;
		return false;
	}

	// Next we need to map the system memory into the virtual address
	// space of this process.  Again we need to specify read and
	// write permission to avoid page faults when writing to the
	// address space.
	//
	m_pAddress = (U08*)mmap(NULL, byteCount, PROT_READ | PROT_WRITE, MAP_SHARED, (int)m_hFile, 0);

	if (NULL == m_pAddress) {
		close((int)m_hFile);
		m_hFile = NULL;
		return false;
	}

	m_ByteCount = byteCount;

	return true;
}


/////////////////////////////////////////////////////////////////////////////
//
//	Free()
//
void QzSharedMemory::Free(void)
{
	if (NULL != m_pAddress) {
		munmap(m_pAddress, m_ByteCount);
		m_pAddress = NULL;
	}

	if (NULL != m_hFile) {
		close((int)m_hFile);
		m_hFile = NULL;
	}

	m_ByteCount = 0;
}



