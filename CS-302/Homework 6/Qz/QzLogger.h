/////////////////////////////////////////////////////////////////////////////
//
//	File: QzLogger.h
//
//	$Header: /Projects/Qz/QzLogger.h  21  2010/5/15 7:37:25p  Lee $
//
//
//	Utility class for writing out messages to a log, stored either as a file
//	on disk or to a shared buffer in memory (allowing a separate app to view
//	the shared memory, yielding better run-time performance and the ability
//	to preserve the memory in case of a crash, whereas a log file will be
//	truncated and lose the last few messages if the app crashes).
//
/////////////////////////////////////////////////////////////////////////////


#pragma once


#include "QzStopWatch.h"


class QzLogger
{
private:
	bool		m_EnableFileLogging;
	bool		m_EnableMemoryLogging;
	FILE*		m_pFile;
	bool		m_RequiresBOM;

	bool		m_DetectedLogger;

	QzSharedMemory	m_SharedMemory;
	char*			m_pMappedAddress;
	U32				m_MappedSize;

	QzStopWatch		m_Timer;

public:
	QzCriticalSection m_Section;

	QzLogger(const char shareName[]);
	~QzLogger(void);

	void Open(const Utf08_t filename[]);
	void Close(void);

	void WriteHeader(void);
	void WriteBuffer(const Utf08_t buffer[], S32 byteCount = -1);

	bool IsLoggingEnabled(void)				{ return (m_EnableMemoryLogging || m_EnableFileLogging); }

	bool MemoryLoggingAvailable(void)		{ return m_DetectedLogger; }
};


extern QzLogger *g_pLog;



