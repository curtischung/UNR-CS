/////////////////////////////////////////////////////////////////////////////
//
//	File: QzByteAccum.h
//
//	$Header: /Projects/Qz/QzByteAccum.h  18  2010/4/3 1:58:47p  Lee $
//
//
//	Utility class for packing data into a byte stream.  Uses an internal
//	buffer that will grow as needed.  Use Alloc() to pre-allocate a buffer
//	for more efficient packing of large amounts of data.
//
/////////////////////////////////////////////////////////////////////////////


#pragma once


class QzByteAccum
{
private:
	U32  m_MaxSize;
	U32  m_Offset;
	U08* m_pData;

	void Grow(void);

public:
	QzByteAccum(void);
	~QzByteAccum(void);

	void Reset(void)					{ m_Offset = 0; }

	void Alloc(U32 byteCount);

	U08* GetAddress(void)				{ return m_pData; }
	U08* GetAddress(U32 offset)			{ return m_pData + offset; }
	U32  GetOffset(void)				{ return m_Offset; }

	U32  Reserve(U32 byteCount);

	U32  StartChunk(U32 headerSize);
	U32  FinishChunk(U32 baseOffset, U32 cookie);
	U32  FinishChunk(U32 baseOffset, U32 cookie, U32 contentFlags);
	U32  FinishChunkCRC(U32 baseOffset, U32 cookie);

	void WriteArrayU08(U08 *pData, U32 valueCount);
	void WriteArrayU16(U16 *pData, U32 valueCount);
	void WriteArrayU32(U32 *pData, U32 valueCount);
	void WritePadding(U32 byteCount);

	void WriteOffsetU08(U32 offset, U08 value);
	void WriteOffsetU16(U32 offset, U16 value);
	void WriteOffsetU32(U32 offset, U32 value);
	void WriteVarS32(S32 value);
	void WriteVarU32(U32 value);

	void WriteU08(U08 value);
	void WriteU16(U16 value);
	void WriteU32(U32 value);
	void WriteFourCC(U32 value);
	void WriteS08(S08 value);
	void WriteS16(S16 value);
	void WriteS32(S32 value);
	void WriteFloat(float value);
	void WriteString(const Utf08_t *pBuffer, S32 byteCount = -1);
	void WriteString8(const Utf08_t *pBuffer);
	void WriteString16(const Utf08_t *pBuffer);

	U08* MakeResourceBuffer(void);

	bool SaveToFile(const Utf08_t filename[]);
};



