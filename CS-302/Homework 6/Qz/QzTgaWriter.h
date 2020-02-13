//////////////////////////////////////////////////////////////////////////////
//
//	File: QzTgaWriter.h
//
//	$Header: /TS/TsImage/QzTgaWriter.h  5  2009/9/13 2:07:55p  Lee $
//
//
//////////////////////////////////////////////////////////////////////////////


#pragma once


#include "QzByteAccum.h"



enum QzTgaFormat_t
{
	TgaFormat_Unknown		= 0,
	TgaFormat_ColorMapped	= 1,
	TgaFormat_TrueColor		= 2,
	TgaFormat_Greyscale		= 3
};


class QzTgaWriter
{
private:
	QzByteAccum	m_Accum;
	QzTgaFormat_t		m_Format;
	U32					m_Width;
	U32					m_Height;
	U32					m_BitDepth;
	bool				m_IsCompressed;
	bool				m_IsBottomUp;
	U32					m_ColorCount;
	U32					m_ColorTable[256];
	U32					m_IdStringLength;
	U08					m_IdString[256];

public:
	QzTgaWriter(void);
	~QzTgaWriter(void);

	void Initialize(U32 width, U32 height, U32 bitDepth, QzTgaFormat_t format);

	void SetColorTable(U32 colorCount, U32 colorTable[]);
	void SetCompressed(bool enable);
	void SetBottomUp(bool enable);
	void SetIdString(char strg[]);

	bool StartEncoding(void);

	void WriteFrame(U08 *pStart, S32 bytePitch);
	void WriteLine(U08 *pStart);

	void Encode08(U08 *pPixel);
	void Encode16(U08 *pPixel);
	void Encode24(U08 *pPixel);
	void Encode32(U08 *pPixel);

	bool SaveFile(const Utf08_t filename[]);
};


