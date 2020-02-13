/////////////////////////////////////////////////////////////////////////////
//
//	File: QzSystem.h
//
//	$Header: /Projects/Qz/QzSystem.h  17  2010/5/20 8:01:32p  Lee $
//
//
//	This header defines a number of standardized functions around OS-specific
//	functionality.  Most source files do everything they need with just these
//	functions.  The front-end (mouse, keyboard, and windowing events) and the
//	back-end (sound and rendering) will need to include Windows.h or the OS X
//	Carbon headers.
//
//	By using these routines and utility classes, 95% of the code can be kept
//	platform independent, making porting much easier.  And it makes compiling
//	much faster, since the bloated OS headers aren't needed (and we won't
//	run into problems with pre-compiled headers, which work differently for
//	Xcode and DevStudio, and which have a long history of introducing strange
//	heisenbugs that come and go each time the project is compiled).
//
/////////////////////////////////////////////////////////////////////////////


#pragma once


struct QzTime_t
{
	U16 Milliseconds;	// usually zero
	U16 Year;
	U08 Month;			// 1-12
	U08 Day;			// 1-31
	U08 Hour12;			// 1-12, hours on 12-hour clock
	U08 Hour24;			// 1-24, hours on 24-hour clock
	U08 PM;				// 0 == AM, 1 == PM
	U08 Minute;			// 0-59
	U08 Second;			// 0-59
	U08 WeekDay;		// 0 == Sunday
};


void  QzSystemInit(void);
void  QzSystemUninit(void);

void  QzDebugBreak(void);
void  QzTerminateApp(void);

void  QzLockMouse(bool locked);
bool  QzIsMouseLocked(void);

void  QzBeepSpeaker(void);
U32   QzGetProcessorCount(void);
float QzGetProcessorUsage(void);

U32   QzGetMilliseconds(void);
U64   QzPrecisionClockFrequency(void);
U64   QzPrecisionClockRead(void);

U32   QzTimeGetCrt(void);
U64   QzTimeConvertCrtToFileTime(U32 timestamp);
U32   QzTimeConvertFileTimeToCrt(U64 timestamp);
void  QzTimeConvertCrtToString(U32 timestamp, Utf08_t buffer[], U32 bufferSize);
void  QzTimeToFields(U32 timestamp, QzTime_t &timeRef);
U32   QzTimeFromFields(const QzTime_t &timeRef);

void  QzGetDateString(Utf08_t buffer[], U32 maxLength);
void  QzGetTimeString(Utf08_t buffer[], U32 maxLength);

void  QzGetNameOfComputer(Utf08_t name[], U32 maxLength);
void  QzGetNameOfUser(Utf08_t name[], U32 maxLength);
void  QzGetGameSavePath(Utf08_t name[], U32 maxLength);

U32   QzAbsolutePathOfFile(Utf08_t absolute[], U32 maxLength, const Utf08_t relative[]);
void  QzSplitPath(const Utf08_t path[], Utf08_t drive[], Utf08_t directory[], Utf08_t filename[], Utf08_t extension[]);
S32   QzMakeDirectory(const Utf08_t path[]);
void  QzVerifyDirectoryExists(const Utf08_t path[]);
bool  QzGetCurrentDirectory(Utf08_t dirname[], U32 maxLength);
bool  QzSetCurrentDirectory(const Utf08_t dirname[]);

Utf08_t* QzReadFromClipboard(U32 &byteCount);
void     QzWriteToClipboard(Utf08_t text[]);

#define  QzFileOpen_Read	0
#define  QzFileOpen_Write	1

FILE* QzFileOpen(const Utf08_t filename[], U32 direction);

bool  QzDoesFileExist(const Utf08_t filename[]);
bool  QzIsFileReadOnly(const Utf08_t filename[]);
bool  QzDeleteFile(const Utf08_t filename[]);
S32   QzRenameFile(const Utf08_t oldname[], const Utf08_t newname[]);
void  QzGetTimeOfFile(const Utf08_t filename[], U32 &createTime, U32 &modifyTime);
void  QzSetTimeOfFile(const Utf08_t filename[], U32 createTime, U32 modifyTime);

void  QzShowSystemDialogBox(const Utf08_t message[], const Utf08_t title[], bool error);

void  QzSleep(U32 milliseconds);

S32   QzThreadSafeIncrement(S32 *pValue);
S32   QzThreadSafeDecrement(S32 *pValue);
S32   QzThreadSafeAdd(S32 *pValue, S32 delta);


#define QzSyncWait_Signalled	0
#define QzSyncWait_Timeout		1
#define QzSyncWait_Error		2
#define QzSyncWait_StopRequest	3	// This is only used for threads, to signal request for thread termination.


/////////////////////////////////////////////////////////////////////////////
//
class QzSyncEvent
{
private:
	Handle_t	m_hEvent;

public:
	QzSyncEvent(void);
	~QzSyncEvent(void);

	void Create(void);
	void Signal(void);
	U32  Wait(U32 milliseconds);
	U32  WaitForever(void);
};


/////////////////////////////////////////////////////////////////////////////
//
class QzThreadControl
{
private:
	void* m_pContext;

public:
	QzThreadControl(void);
	~QzThreadControl(void);

	bool StartThread(U32 (*func)(void*), void *pParams, const char threadName[]);
	void StopThread(void);
	U32  WaitForClosure(void);
	U32  WaitForEvent(U32 milliseconds = 0xFFFFFFFF);
	void SignalThread(void);
	void BoostPriority(void);

	U32  GetID(void);
	bool IsThreadAlive(void);
	bool TestStopFlag(void);
};


/////////////////////////////////////////////////////////////////////////////
//
//	Wraps the platform-specific critical section,
//
class QzCriticalSection
{
private:
	void *m_pCritSection;

public:
	QzCriticalSection(void);
	~QzCriticalSection(void);

	void Lock(void);
	void Unlock(void);
};


/////////////////////////////////////////////////////////////////////////////
//
//	Automatically locks/unlocks a critical section using the constructor
//	and destructor, guaranteeing that the critical section will always get
//	unlocked, no matter how the stack is unwound.
//
class QzCriticalLock
{
private:
	QzCriticalSection *m_pSection;

public:
	QzCriticalLock(QzCriticalSection *pSection)
		:	m_pSection(pSection)
	{
		m_pSection->Lock();
	}

	~QzCriticalLock(void)
	{
		m_pSection->Unlock();
	}
};


/////////////////////////////////////////////////////////////////////////////
//
//	Simple utility class for creating memory buffers shared between
//	processes.
//
class QzSharedMemory
{
private:
	Handle_t	m_hFile;
	U08*		m_pAddress;
	U32			m_ByteCount;
	bool		m_AlreadyExisted;

public:
	QzSharedMemory(void);
	~QzSharedMemory(void);

	bool InitShare(const char name[], U32 byteCount, bool allowCaching);
	void Free(void);
	U08* Address(void)			{ return m_pAddress; }
	U32  ByteCount(void)		{ return m_ByteCount; }
	bool AlreadyExisted(void)	{ return m_AlreadyExisted; }
};



