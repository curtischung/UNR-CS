/////////////////////////////////////////////////////////////////////////////
//
//	File: QzLogger.cpp
//
//	$Header: /Projects/Qz/QzLogger.cpp  30  2010/5/15 7:37:28p  Lee $
//
//
//	Utility class for writing out messages to a log, stored either as a file
//	on disk or to a shared buffer in memory (allowing a separate app to view
//	the shared memory, yielding better run-time performance and the ability
//	to preserve the memory in case of a crash, whereas a log file will be
//	truncated and lose the last few messages if the app crashes).
//
/////////////////////////////////////////////////////////////////////////////


#include "QzCommon.h"
#include "QzLogger.h"


#ifdef USE_MALLOC_MACRO
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// This header is stored at the beginning of shared memory.  Since shared
// memory is used as a ring buffer, these fields indicate the position of
// the data within the ring buffer.
//
struct QzLogInfo_t
{
	U32 Enable;        // flag used to disable the writing of log messages
	U32 ErrorCount;    // how many error messages have been logged
	U32 MaxByteCount;  // total size of shared memory buffer
	U32 ByteCount;     // total number of bytes in the ring buffer
	U32 ByteOffset;    // offset to the start of the ring
};


QzLogger* g_pLog       = NULL;
U32       g_ErrorCount = 0;


/////////////////////////////////////////////////////////////////////////////
//
//	LogMessage()
//
//	Wraps access to the logger, hiding the g_pLog pointer.  Code may still
//	access logging through g_pLog, but this function simplifies the calling,
//	and protects against NULL pointers.
//
void LogMessage(const char message[], UtfFormat &fmt)
{
	if ((NULL != g_pLog) && g_pLog->IsLoggingEnabled()) {
		Utf08_t buffer[512];

		U32 byteCount = fmt.Generate(buffer, ArraySize(buffer), reinterpret_cast<const Utf08_t*>(message));

		g_pLog->m_Section.Lock();
		g_pLog->WriteHeader();
		g_pLog->WriteBuffer(buffer, byteCount);
		g_pLog->m_Section.Unlock();

#ifdef QZ_LOG_PRINTF
		printf("%s\n", buffer);
#endif // QZ_LOG_PRINTF
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	LogMessage()
//
void LogMessage(const char message[])
{
	if ((NULL != g_pLog) && g_pLog->IsLoggingEnabled()) {
		g_pLog->m_Section.Lock();
		g_pLog->WriteHeader();
		g_pLog->WriteBuffer(reinterpret_cast<const Utf08_t*>(message));
		g_pLog->m_Section.Unlock();

#ifdef QZ_LOG_PRINTF
		printf("%s\n", message);
#endif // QZ_LOG_PRINTF
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	LogErrorMessage()
//
//	Wraps access to the logger, hiding the g_pLog pointer.  Code may still
//	access logging through g_pLog, but this function simplifies the calling,
//	and protects against NULL pointers.
//
void LogErrorMessage(const char message[], UtfFormat &fmt)
{
	++g_ErrorCount;

	if ((NULL != g_pLog) && g_pLog->IsLoggingEnabled()) {
		Utf08_t buffer[512];

		memcpy(buffer, "ERROR: ", 7);

		U32 byteCount = fmt.Generate(buffer + 7, ArraySize(buffer) - 7, reinterpret_cast<const Utf08_t*>(message));

		g_pLog->m_Section.Lock();
		g_pLog->WriteHeader();
		g_pLog->WriteBuffer(buffer, byteCount + 7);
		g_pLog->m_Section.Unlock();

#ifdef QZ_LOG_PRINTF
		printf("%s\n", buffer);
#endif // QZ_LOG_PRINTF
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	LogErrorMessage()
//
void LogErrorMessage(const char message[])
{
	++g_ErrorCount;

	if ((NULL != g_pLog) && g_pLog->IsLoggingEnabled()) {
		Utf08_t buffer[512];

		memcpy(buffer, "ERROR: ", 7);

		U32 byteCount = UtfCopy(buffer + 7, ArraySize(buffer) - 7, reinterpret_cast<const Utf08_t*>(message));

		g_pLog->m_Section.Lock();
		g_pLog->WriteHeader();
		g_pLog->WriteBuffer(buffer, byteCount + 7);
		g_pLog->m_Section.Unlock();

#ifdef QZ_LOG_PRINTF
		printf("%s\n", buffer);
#endif // QZ_LOG_PRINTF
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	constructor
//
QzLogger::QzLogger(const char shareName[])
	:	m_EnableFileLogging(false),
		m_EnableMemoryLogging(true),
		m_pFile(NULL),
		m_RequiresBOM(false),
		m_DetectedLogger(false),
		m_pMappedAddress(NULL),
		m_MappedSize(4 * 1024 * 1024)
{
	// Attempt to open a shared, memory-mapped file that will be used for
	// the shared buffer.
	if (m_SharedMemory.InitShare(shareName, m_MappedSize, false)) {
		m_DetectedLogger = m_SharedMemory.AlreadyExisted();

		m_pMappedAddress = reinterpret_cast<char*>(m_SharedMemory.Address());

		// Always reset back to the start of the buffer so it does not
		// accumulate logging info across multiple runs of the app.
		QzLogInfo_t *pInfo	= reinterpret_cast<QzLogInfo_t*>(m_pMappedAddress);
		pInfo->ErrorCount	= 0;
		pInfo->ByteOffset	= 0;
		pInfo->ByteCount	= 0;

		if (m_DetectedLogger) {
			m_EnableMemoryLogging = (pInfo->Enable != 0);
		}
		else {
			pInfo->Enable       = m_EnableMemoryLogging ? TRUE : FALSE;
			pInfo->MaxByteCount = m_MappedSize - sizeof(QzLogInfo_t);
		}

		// This version of the start message is only written if we're
		// using mapped memory for the log.
		Utf08_t message[256];
		Utf08_t timestrg[128];
		Utf08_t datestrg[128];
		QzGetTimeString(timestrg, ArraySize(timestrg));
		QzGetDateString(datestrg, ArraySize(datestrg));

		UtfFormat fmt;
		fmt.AddString(datestrg);
		fmt.AddString(timestrg);
		U32 byteCount = fmt.Generate(message, ArraySize(message), reinterpret_cast<const Utf08_t*>("Start of Log: %1;, %2;\n"));

		WriteBuffer(message, byteCount);
	}

	// Start up the stopwatch used to timestamp logged messages.
	m_Timer.Reset();
	m_Timer.Start();
}


/////////////////////////////////////////////////////////////////////////////
//
//	destructor
//
QzLogger::~QzLogger(void)
{
	// Write out a final message to indicate the logging was succesfully
	// completed (as opposed to truncated/empty file resulting from a crash).
	//
	Utf08_t message[256];
	Utf08_t timestrg[128];
	Utf08_t datestrg[128];
	QzGetTimeString(timestrg, ArraySize(timestrg));
	QzGetDateString(datestrg, ArraySize(datestrg));

	UtfFormat fmt;
	fmt.AddInt(g_ErrorCount);
	fmt.AddString(datestrg);
	fmt.AddString(timestrg);
	U32 byteCount = fmt.Generate(message, ArraySize(message), reinterpret_cast<const Utf08_t*>("\n\nErrors: %1;\n\nEnd of Log: %2;, %3;\n"));

	m_Section.Lock();
	WriteBuffer(message, byteCount);
	m_Section.Unlock();

	Close();

	m_SharedMemory.Free();
}


/////////////////////////////////////////////////////////////////////////////
//
//	Open()
//
//	This function is only called if mapped memory logging is not available.
//
void QzLogger::Open(const Utf08_t *filename)
{
	m_Section.Lock();

	if (m_pFile != NULL) {
		fclose(m_pFile);
	}

	m_pFile = QzFileOpen(filename, QzFileOpen_Write);

	m_EnableFileLogging = (m_pFile != NULL);

	m_RequiresBOM = false;

	// Write out the Unicode byte order marker.  In most cases, UTF-8 is not
	// needed, so we can overwrite this when closing the file.
	//
	if (NULL != m_pFile) {
		Utf08_t marker[3];
		marker[0] = 0xEF;
		marker[1] = 0xBB;
		marker[2] = 0xBF;
		fwrite(&marker, 1, 3, m_pFile);
	}

	Utf08_t message[256];
	Utf08_t timestrg[128];
	Utf08_t datestrg[128];
	QzGetTimeString(timestrg, ArraySize(timestrg));
	QzGetDateString(datestrg, ArraySize(datestrg));

	UtfFormat fmt;
	fmt.AddString(datestrg);
	fmt.AddString(timestrg);
	U32 byteCount = fmt.Generate(message, ArraySize(message), reinterpret_cast<const Utf08_t*>("Start of Log: %1;, %2;\n"));

	WriteBuffer(message, byteCount);

	m_Section.Unlock();
}


/////////////////////////////////////////////////////////////////////////////
//
//	Close()
//
void QzLogger::Close(void)
{
	m_Section.Lock();

	if (m_pFile != NULL) {

		// If it turns out we did not need the UTF-8 byte order marker, we
		// can seek back to the start of the file and overwrite it with
		// whitespace.  This avoids problems in reading UTF-8 files from
		// WordPad (though one would hope this bug has since been fixed).
		if (false == m_RequiresBOM) {
			U32 oldPos = ftell(m_pFile);
			fseek(m_pFile, 0, SEEK_SET);
			fwrite("  \n", 1, 3, m_pFile);
			fseek(m_pFile, oldPos, SEEK_SET);
		}

		fclose(m_pFile);
	}

	m_Section.Unlock();
}


/////////////////////////////////////////////////////////////////////////////
//
//	WriteHeader()
//
//	Produces the header at the start of each line, indicating the timestamp
//	of the log entry.
//
void QzLogger::WriteHeader(void)
{
	if (m_EnableMemoryLogging || m_EnableFileLogging) {
		U32 milliseconds = m_Timer.iDeltaMilliseconds();
		U32 seconds      = milliseconds / 1000;
		U32 minutes      = seconds / 60;
		U32 hours        = minutes / 60;

		minutes      %= 60;
		seconds      %= 60;
		milliseconds %= 1000;

		UtfFormat fmt;
		fmt.AddInt(hours);
		fmt.AddInt(minutes);
		fmt.AddInt(seconds);
		fmt.AddInt(milliseconds);

		Utf08_t timestamp[128];
		U32 byteCount = fmt.Generate(timestamp, ArraySize(timestamp), reinterpret_cast<const Utf08_t*>("\n%1;:%2w2p0;:%3w2p0;.%4w3p0;  "));

		WriteBuffer(timestamp, byteCount);
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	WriteBuffer()
//
//	This will write a block of text to the log file and mapped memory.
//	Generally, only one of those two should be open, but we allow for both
//	of them to be enabled for some debugging support.
//
void QzLogger::WriteBuffer(const Utf08_t buffer[], S32 byteCount)
{
	if (m_pFile != NULL) {
		// If the size is unknown, call a string function that will
		// determine the length of the string to write.  Normally, the
		// string formatting code will report the byte count, so this
		// extra step will be skipped.
		if (byteCount < 0) {
			byteCount = UtfByteCount(buffer);
		}

		if (byteCount > 0) {
			// If we have not yet seen any 8-bit values, scan this string
			// for them.  Once we've seen any 8-bit value, we know that
			// the file must stay in UTF-8 format, and that the BOM at
			// the start of the file is required.
			if (false == m_RequiresBOM) {
				for (U32 i = 0; i < U32(byteCount); ++i) {
					if (0 != (0x80 & buffer[i])) {
						m_RequiresBOM = true;
						break;
					}
				}
			}

			fwrite(buffer, 1, byteCount, m_pFile);
		}
	}

	if (m_pMappedAddress != NULL) {
		// If the size is unknown, call a string function that will
		// determine the length of the string to write.  Normally, the
		// string formatting code will report the byte count, so this
		// extra step will be skipped.
		if (byteCount < 0) {
			byteCount = UtfByteCount(buffer);
		}

		// Get a pointer to the header at the start of shared memory.
		QzLogInfo_t *pInfo = reinterpret_cast<QzLogInfo_t*>(m_pMappedAddress);

		// Sync error count up with shared memory.
		pInfo->ErrorCount = g_ErrorCount;

		// Check to see if the logger utility has disabled logging.
		if (!pInfo->Enable) {
			return;
		}

		// The logging data starts immediately after the header.
		Utf08_t *pStart = reinterpret_cast<Utf08_t*>(m_pMappedAddress + sizeof(QzLogInfo_t));

		// Once enough data has been written into the buffer to fill it
		// up, we're now dealing with ring buffer logic.  The block of
		// data we're about to write may need to wrap around the end of
		// the buffer.
		//
		if ((byteCount + pInfo->ByteOffset) >= pInfo->MaxByteCount) {
			U32 count2 = byteCount + pInfo->ByteOffset - pInfo->MaxByteCount;
			U32 count1 = byteCount - count2;

			// Copy first part of message to end of ring buffer.
			if (count1 > 0) {
				memcpy(pStart + pInfo->ByteOffset, buffer, count1);
			}

			// Remaining portion of message (if any) wraps to start of buffer.
			if (count2 > 0) {
				memcpy(pStart, buffer + count1, count2);
			}

			pInfo->ByteOffset = count2;
			pInfo->ByteCount  = pInfo->MaxByteCount;
		}

		// Otherwise we have not yet filled up the ring buffer.  No wrap
		// condition can yet exist, so we can memcpy the data in one call.
		//
		else {
			memcpy(pStart + pInfo->ByteOffset, buffer, byteCount);

			pInfo->ByteOffset += byteCount;

			// Check the resulting size.  Initially, the byte count will grow
			// until the buffer fills, after that it overwrites the oldest
			// data and remains a fixed size.
			if ((pInfo->ByteCount + byteCount) < pInfo->MaxByteCount) {
				pInfo->ByteCount += byteCount;
			}
			else {
				pInfo->ByteCount = pInfo->MaxByteCount;
			}
		}
	}
}


