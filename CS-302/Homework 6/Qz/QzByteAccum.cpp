/////////////////////////////////////////////////////////////////////////////
//
//	File: QzByteAccum.cpp
//
//	$Header: /Projects/Qz/QzByteAccum.cpp  24  2010/4/3 1:58:50p  Lee $
//
//
//	Utility class for accumulating a byte stream, typically while assembling
//	a packed data object like a module while in resource format.
//
/////////////////////////////////////////////////////////////////////////////


#include "QzCommon.h"
#include "QzByteAccum.h"
#include "QzCRC32.h"


#ifdef USE_MALLOC_MACRO
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
//
//	constructor
//
QzByteAccum::QzByteAccum(void)
	:	m_MaxSize(0),
		m_Offset(0),
		m_pData(NULL)
{
}


/////////////////////////////////////////////////////////////////////////////
//
//	destructor
//
QzByteAccum::~QzByteAccum(void)
{
	SafeDeleteArray(m_pData);
}


/////////////////////////////////////////////////////////////////////////////
//
//	Grow()
//
void QzByteAccum::Grow(void)
{
	if (NULL != m_pData) {
		U32 newSize = m_MaxSize * 2;

		U08 *pNew = new U08[newSize];
		memcpy(pNew, m_pData, m_Offset);

		SafeDeleteArray(m_pData);

		m_pData   = pNew;
		m_MaxSize = newSize;
	}
	else {
		m_MaxSize = 256 * 1024;
		m_pData   = new U08[m_MaxSize];
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	Alloc()
//
//	Pre-allocates a buffer of the specified size.  Useful when preparing to
//	pack a very large file, since this can minimize or eliminate the need to
//	grow the buffer.
//
void QzByteAccum::Alloc(U32 byteCount)
{
	// Only alloc a new buffer if the requested size is larger than the
	// existing buffer.
	if ((NULL == m_pData) || (byteCount > m_MaxSize)) {
		SafeDeleteArray(m_pData);

		m_MaxSize = byteCount;
		m_pData   = new U08[m_MaxSize];
	}

	m_Offset  = 0;
}


/////////////////////////////////////////////////////////////////////////////
//
//	Reserve()
//
//	Reserves an empty chunk of memory, assuming that the caller will write
//	something to this location later on.
//
//	Returns the offset to the start of the reserved memory.
//
U32 QzByteAccum::Reserve(U32 byteCount)
{
	U32 oldOffset = m_Offset;

	// Do this in a loop, since we might be given a really large chunk of
	// data via WriteArrayU08().
	while ((m_Offset + byteCount) > m_MaxSize) {
		Grow();
	}

	m_Offset += byteCount;

	return oldOffset;
}


/////////////////////////////////////////////////////////////////////////////
//
//	StartChunk()
//
U32 QzByteAccum::StartChunk(U32 headerSize)
{
	U32 offset = m_Offset;

	for (U32 i = 0; i < headerSize; ++i) {
		WriteU32(headerSize);
	}

	return offset;
}


/////////////////////////////////////////////////////////////////////////////
//
//	FinishChunk()
//
U32 QzByteAccum::FinishChunk(U32 baseOffset, U32 cookie)
{
	QzAssert((baseOffset + 2 * sizeof(U32)) <= m_Offset);

	U32 marker   = (U32(m_pData[baseOffset+0])      )
				 | (U32(m_pData[baseOffset+1]) <<  8)
				 | (U32(m_pData[baseOffset+2]) << 16)
				 | (U32(m_pData[baseOffset+3]) << 24);

	// Verify that chunk was started with space for 2 U32s, and that the
	// offset points to the space that was reserved.
	QzAssert(marker == 2);

	U32 totalSize = m_Offset - baseOffset;

	WriteOffsetU32(baseOffset + 0, cookie);
	WriteOffsetU32(baseOffset + 4, totalSize - (2 * sizeof(U32)));

	return totalSize;
}


/////////////////////////////////////////////////////////////////////////////
//
//	FinishChunk()
//
U32 QzByteAccum::FinishChunk(U32 baseOffset, U32 cookie, U32 contentFlags)
{
	QzAssert((baseOffset + 3 * sizeof(U32)) <= m_Offset);

	U32 marker   = (U32(m_pData[baseOffset+0])      )
				 | (U32(m_pData[baseOffset+1]) <<  8)
				 | (U32(m_pData[baseOffset+2]) << 16)
				 | (U32(m_pData[baseOffset+3]) << 24);

	// Verify that chunk was started with space for 3 U32s, and that the
	// offset points to the space that was reserved.
	QzAssert(marker == 3);

	U32 totalSize = m_Offset - baseOffset;

	WriteOffsetU32(baseOffset + 0, cookie);
	WriteOffsetU32(baseOffset + 4, totalSize - (2 * sizeof(U32)));
	WriteOffsetU32(baseOffset + 8, contentFlags);

	return totalSize;
}


/////////////////////////////////////////////////////////////////////////////
//
//	FinishChunkCRC()
//
//	This creates a chunk header that is 12 bytes long instead of the usual
//	8 bytes.  The extra 4 bytes contain a CRC for the chunk's payload.
//
U32 QzByteAccum::FinishChunkCRC(U32 baseOffset, U32 cookie)
{
	QzAssert((baseOffset + 3 * sizeof(U32)) <= m_Offset);

	U32 marker   = (U32(m_pData[baseOffset+0])      )
				 | (U32(m_pData[baseOffset+1]) <<  8)
				 | (U32(m_pData[baseOffset+2]) << 16)
				 | (U32(m_pData[baseOffset+3]) << 24);

	// Verify that chunk was started with space for 3 U32s, and that the
	// offset points to the space that was reserved.
	QzAssert(marker == 3);

	U32 totalSize = m_Offset - baseOffset;
	U32 crc       = QzComputeCrc32(m_pData + baseOffset + 12, totalSize - 12);

	WriteOffsetU32(baseOffset + 0, cookie);
	WriteOffsetU32(baseOffset + 4, totalSize - 12);
	WriteOffsetU32(baseOffset + 8, crc);

	return totalSize;
}


/////////////////////////////////////////////////////////////////////////////
//
//	WriteArrayU08()
//
void QzByteAccum::WriteArrayU08(U08 *pData, U32 valueCount)
{
	while ((m_Offset + valueCount) > m_MaxSize) {
		Grow();
	}

	memcpy(m_pData + m_Offset, pData, valueCount);

	m_Offset += valueCount;
}


/////////////////////////////////////////////////////////////////////////////
//
//	WriteArrayU16()
//
void QzByteAccum::WriteArrayU16(U16 *pData, U32 valueCount)
{
	while ((m_Offset + (valueCount * sizeof(U16))) > m_MaxSize) {
		Grow();
	}

#ifdef IS_BIG_ENDIAN
	U32 offset = m_Offset;
	for (U32 i = 0; i < valueCount; ++i) {
		m_pData[offset++] = U08(pData[i]     );
		m_pData[offset++] = U08(pData[i] >> 8);
	}
#else
	memcpy(m_pData + m_Offset, pData, valueCount * sizeof(U16));
#endif

	m_Offset += valueCount * sizeof(U16);
}


/////////////////////////////////////////////////////////////////////////////
//
//	WriteArrayU32()
//
void QzByteAccum::WriteArrayU32(U32 *pData, U32 valueCount)
{
	while ((m_Offset + (valueCount * sizeof(U32))) > m_MaxSize) {
		Grow();
	}

#ifdef IS_BIG_ENDIAN
	U32 offset = m_Offset;
	for (U32 i = 0; i < valueCount; ++i) {
		m_pData[offset++] = U08(pData[i]      );
		m_pData[offset++] = U08(pData[i] >>  8);
		m_pData[offset++] = U08(pData[i] >> 16);
		m_pData[offset++] = U08(pData[i] >> 24);
	}
#else
	memcpy(m_pData + m_Offset, pData, valueCount * sizeof(U32));
#endif

	m_Offset += valueCount * sizeof(U32);
}


/////////////////////////////////////////////////////////////////////////////
//
//	WritePadding()
//
void QzByteAccum::WritePadding(U32 byteCount)
{
	while ((m_Offset + byteCount) > m_MaxSize) {
		Grow();
	}

	memset(m_pData + m_Offset, 0, byteCount);

	m_Offset += byteCount;
}


/////////////////////////////////////////////////////////////////////////////
//
//	WriteOffsetU08()
//
void QzByteAccum::WriteOffsetU08(U32 offset, U08 value)
{
	m_pData[offset] = value;
}


/////////////////////////////////////////////////////////////////////////////
//
//	WriteOffsetU16()
//
void QzByteAccum::WriteOffsetU16(U32 offset, U16 value)
{
	m_pData[offset++] = U08(value      );
	m_pData[offset  ] = U08(value >>  8);
}


/////////////////////////////////////////////////////////////////////////////
//
//	WriteOffsetU32()
//
void QzByteAccum::WriteOffsetU32(U32 offset, U32 value)
{
	m_pData[offset++] = U08(value      );
	m_pData[offset++] = U08(value >>  8);
	m_pData[offset++] = U08(value >> 16);
	m_pData[offset  ] = U08(value >> 24);
}


/////////////////////////////////////////////////////////////////////////////
//
//	WriteVarS32()
//
void QzByteAccum::WriteVarS32(S32 value)
{
	if (value >= 0) {
		WriteVarU32(U32(value << 1));
	}
	else {
		WriteVarU32(U32(((-value) << 1) | 1));
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	WriteVarU32()
//
//	Write out a number as a variable-length sequence.  This strips off 7 bits
//	at a time.  As long as the result is non-zero, the high bit is set and
//	it keeps using more bytes to store the result.  At worst, a 32-bit value
//	will require 5 bytes to store, so this is really only efficient for
//	storing values that are normally small.
//
void QzByteAccum::WriteVarU32(U32 value)
{
	U08 bytes[5];
	U32 total = 0;

	do {
		bytes[total] = U08(value & 0x7F);
		value = value >> 7;
		if (0 != value) {
			bytes[total++] |= 0x80;
		}
	} while (0 != value);

	WriteArrayU08(bytes, total + 1);
}


/////////////////////////////////////////////////////////////////////////////
//
//	WriteU08()
//
void QzByteAccum::WriteU08(U08 value)
{
	if ((m_Offset + sizeof(U08)) > m_MaxSize) {
		Grow();
	}

	m_pData[m_Offset++] = value;
}


/////////////////////////////////////////////////////////////////////////////
//
//	WriteU16()
//
void QzByteAccum::WriteU16(U16 value)
{
	if ((m_Offset + sizeof(U16)) > m_MaxSize) {
		Grow();
	}

	m_pData[m_Offset++] = U08(value);
	m_pData[m_Offset++] = U08(value >> 8);
}


/////////////////////////////////////////////////////////////////////////////
//
//	WriteU32()
//
void QzByteAccum::WriteU32(U32 value)
{
	if ((m_Offset + sizeof(U32)) > m_MaxSize) {
		Grow();
	}

	m_pData[m_Offset++] = U08(value      );
	m_pData[m_Offset++] = U08(value >>  8);
	m_pData[m_Offset++] = U08(value >> 16);
	m_pData[m_Offset++] = U08(value >> 24);
}


/////////////////////////////////////////////////////////////////////////////
//
//	WriteFourCC()
//
//	This is identical to WriteU32() on an Intel little-endian platform.
//	For big-endian, this would need to leave the byte order alone, while
//	WriteU32() would need to reverse the byte-order.
//
void QzByteAccum::WriteFourCC(U32 value)
{
	if ((m_Offset + sizeof(U32)) > m_MaxSize) {
		Grow();
	}

	m_pData[m_Offset++] = U08(value      );
	m_pData[m_Offset++] = U08(value >>  8);
	m_pData[m_Offset++] = U08(value >> 16);
	m_pData[m_Offset++] = U08(value >> 24);
}


/////////////////////////////////////////////////////////////////////////////
//
//	WriteS08()
//
void QzByteAccum::WriteS08(S08 value)
{
	if ((m_Offset + sizeof(S08)) > m_MaxSize) {
		Grow();
	}

	m_pData[m_Offset++] = value;
}


/////////////////////////////////////////////////////////////////////////////
//
//	WriteS16()
//
void QzByteAccum::WriteS16(S16 value)
{
	if ((m_Offset + sizeof(S16)) > m_MaxSize) {
		Grow();
	}

	reinterpret_cast<S16*>(m_pData + m_Offset)[0] = value;

	m_pData[m_Offset++] = U08(U16(value));
	m_pData[m_Offset++] = U08(U16(value) >> 8);
}


/////////////////////////////////////////////////////////////////////////////
//
//	WriteS32()
//
void QzByteAccum::WriteS32(S32 value)
{
	if ((m_Offset + sizeof(S32)) > m_MaxSize) {
		Grow();
	}

	m_pData[m_Offset++] = U08(U32(value)      );
	m_pData[m_Offset++] = U08(U32(value) >>  8);
	m_pData[m_Offset++] = U08(U32(value) >> 16);
	m_pData[m_Offset++] = U08(U32(value) >> 24);
}


/////////////////////////////////////////////////////////////////////////////
//
//	WriteFloat()
//
void QzByteAccum::WriteFloat(float value)
{
	if ((m_Offset + sizeof(float)) > m_MaxSize) {
		Grow();
	}

#ifdef NORMAL_ENDIAN_FLOATS
	reinterpret_cast<float*>(m_pData + m_Offset)[0] = value;

	m_Offset += sizeof(float);
#else
	QzFloatIntUnion_t fiu;
	fiu.f = value;
	m_pData[m_Offset++] = U08(fiu.u      );
	m_pData[m_Offset++] = U08(fiu.u >>  8);
	m_pData[m_Offset++] = U08(fiu.u >> 16);
	m_pData[m_Offset++] = U08(fiu.u >> 24);
#endif
}


/////////////////////////////////////////////////////////////////////////////
//
//	WriteString()
//
void QzByteAccum::WriteString(const Utf08_t *pBuffer, S32 byteCount)
{
	if (byteCount < 0) {
		byteCount = UtfByteCount(pBuffer) + 1;
	}

	while ((m_Offset + byteCount) > m_MaxSize) {
		Grow();
	}

	if (byteCount > 0) {
		memcpy(m_pData + m_Offset, pBuffer, byteCount);

		m_Offset += byteCount;
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	WriteString8()
//
//	Similar to WriteString(), this will write an 8-bit count, followed by
//	the string itself.
//
//	Since an explicit size value is stored, the string will not be terminated
//	with '\0'.
//
void QzByteAccum::WriteString8(const Utf08_t *pBuffer)
{
	U32 byteCount = 0;

	if (NULL != pBuffer) {
		byteCount = UtfByteCount(pBuffer);
	}

	if (byteCount > 0xFF) {
		byteCount = 0xFF;
	}

	WriteU08(U08(byteCount));

	if (byteCount > 0) {
		WriteString(pBuffer, byteCount);
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	WriteString16()
//
//	Similar to WriteString(), this will write an 16-bit count, followed by
//	the string itselft.
//
//	Since an explicit size value is stored, the string will not be terminated
//	with '\0'.
//
void QzByteAccum::WriteString16(const Utf08_t *pBuffer)
{
	U32 byteCount = 0;

	if (NULL != pBuffer) {
		byteCount = UtfByteCount(pBuffer);
	}

	if (byteCount > 0xFFFF) {
		byteCount = 0xFFFF;
	}

	WriteU16(U16(byteCount));

	if (byteCount > 0) {
		WriteString(pBuffer, byteCount);
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	MakeResourceBuffer()
//
//	Copies the data into a buffer that can be given to the resource manager.
//	This assures that the buffer is no larger than necessary (since m_pData
//	is over-allocated by up to a factor of 2).
//
//	A newly allocated buffer is returned.  Memory management responsibility
//	is passed off to the caller, under the assumption that the buffer will
//	be given to the resource manager.
//
U08* QzByteAccum::MakeResourceBuffer(void)
{
	// Allocate only as much memory as needed to store the data.
	U08* pBuffer = new U08[m_Offset];

	// Copy all of the data into the new buffer.
	memcpy(pBuffer, m_pData, m_Offset);

	return pBuffer;
}


/////////////////////////////////////////////////////////////////////////////
//
//	SaveToFile()
//
bool QzByteAccum::SaveToFile(const Utf08_t filename[])
{
	FILE *pFile = QzFileOpen(filename, QzFileOpen_Write);

	if (NULL == pFile) {
		return false;
	}

	bool success = (m_Offset == fwrite(m_pData, 1, m_Offset, pFile));

	fclose(pFile);

	return success;
}


