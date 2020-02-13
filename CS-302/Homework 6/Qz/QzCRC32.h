/////////////////////////////////////////////////////////////////////////////
//
//	File: QzCRC32.h
//
//	$Header: /TS/TsFile/QzCRC32.h  7  2009/9/14 2:15:45p  Lee $
//
//
//	This file contains several 32-bit CRC routines.
//
//	The QzCRC32 class and QzComputeCrc32() function are equivalent, with the
//	difference QzCRC32 is intended to be called multiple times, each time
//	processing a portion of the data to be CRC'd, while QzComputeCrc32() does
//	all of the data in one pass.  (This code is based upon sample code from
//	Tomi Mikkonen (tomitm@remedy.fi) many years ago.)
//
//	QzComputeAdlerCrc32() is an alternate 32-bit CRC algorithm used in the
//	PNG file format.
//
//	VssCrc16() and VssCrc32() are non-standard CRC implementations that are
//	used for decoding Visual Source Safe database files.
//
/////////////////////////////////////////////////////////////////////////////


#pragma once


class QzCRC32
{
private:
	U32 m_CRC;

public:
	QzCRC32(void);
//	~QzCRC32(void);

	void  Reset(void);
	U32   RetrieveCRC(void);
	void  AccumulateBuffer(const U08 buffer[], U32 bufferSize);
};


U32 QzComputeCrc32(const U08 *pSrc, U32 byteCount);
U32 QzComputeAdlerCrc32(U32 adler, U08 *pBuffer, U32 byteCount);
U32 VssCrc32(const U08 buffer[], U32 bufferSize);
U16 VssCrc16(const U08 buffer[], U32 bufferSize);



