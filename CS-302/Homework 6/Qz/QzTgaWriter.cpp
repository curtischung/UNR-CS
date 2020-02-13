//////////////////////////////////////////////////////////////////////////////
//
//	File: QzTgaWriter.cpp
//
//	$Header: /Projects/Qz/QzTgaWriter.cpp  7  2010/4/23 10:16:08a  Lee $
//
//
//////////////////////////////////////////////////////////////////////////////


#include "QzCommon.h"
#include "QzTgaWriter.h"


#ifdef USE_MALLOC_MACRO
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//////////////////////////////////////////////////////////////////////////////
//
//	constructor
//
QzTgaWriter::QzTgaWriter(void)
	:	m_Format(TgaFormat_Unknown),
		m_Width(0),
		m_Height(0),
		m_BitDepth(0),
		m_IsCompressed(false),
		m_IsBottomUp(false),
		m_ColorCount(0),
		m_IdStringLength(0)
{
}


//////////////////////////////////////////////////////////////////////////////
//
//	destructor
//
QzTgaWriter::~QzTgaWriter(void)
{
}


//////////////////////////////////////////////////////////////////////////////
//
//	Initialize()
//
void QzTgaWriter::Initialize(U32 width, U32 height, U32 bitDepth, QzTgaFormat_t format)
{
	m_Width  = width;
	m_Height = height;
	m_Format = format;

	if ((TgaFormat_ColorMapped == format) ||
		(TgaFormat_Greyscale == format))
	{
		m_BitDepth = 8;
	}
	else {
		m_BitDepth = ((bitDepth + 7) / 8) * 8;
	}
}


//////////////////////////////////////////////////////////////////////////////
//
//	SetColorTable()
//
void QzTgaWriter::SetColorTable(U32 colorCount, U32 colorTable[])
{
	m_ColorCount = Min(256, colorCount);

	memcpy(m_ColorTable, colorTable, m_ColorCount * sizeof(U32));
}


//////////////////////////////////////////////////////////////////////////////
//
//	SetCompressed()
//
//	Enable RLE compression of the image data.  While this can provide very
//	good compression for solid colors, it can increase the size of the file
//	for photographic or other complex, noisy image data.
//
void QzTgaWriter::SetCompressed(bool enable)
{
	m_IsCompressed = enable;
}


//////////////////////////////////////////////////////////////////////////////
//
//	SetBottomUp()
//
//	This will put the bottom-to-top marker in the image descriptor.  But it is
//	still left to the caller to provide the lines of image data in the correct
//	order.
//
void QzTgaWriter::SetBottomUp(bool enable)
{
	m_IsBottomUp = enable;
}


//////////////////////////////////////////////////////////////////////////////
//
//	SetIdString()
//
//	Write an ID string near the start of the file.  This will be placed after
//	the file header, and right before the color table (or the start of image
//	data for non-color-mapped images).
//
void QzTgaWriter::SetIdString(char strg[])
{
	m_IdStringLength = U32(strlen(strg));

	if (m_IdStringLength > 254) {
		m_IdStringLength = 254;
	}

	memcpy(m_IdString, strg, m_IdStringLength);
	m_IdString[m_IdStringLength] = '\0';

	m_IdStringLength += 1;
}


//////////////////////////////////////////////////////////////////////////////
//
//	StartEncoding()
//
//	This should not be called until after Initialize(), and any of the set
//	functions.  This will allocate the buffer that will be used by the byte
//	accumlator, and write the file header (along with the ID string and the
//	color table, if they are needed).
//
bool QzTgaWriter::StartEncoding(void)
{
	if (TgaFormat_Unknown == m_Format) {
		return false;
	}

	U32 estimatedSize = (m_Width * m_Height * m_BitDepth) / 8;

	estimatedSize += (estimatedSize / 2) + 256;

	m_Accum.Alloc(estimatedSize);

	m_Accum.WriteU08(U08(m_IdStringLength));
	m_Accum.WriteU08((TgaFormat_ColorMapped == m_Format) ? 1 : 0);

	U08 formatBits = U08(m_Format);

	if (m_IsCompressed) {
		formatBits |= 8;
	}

	m_Accum.WriteU08(formatBits);

	if (TgaFormat_ColorMapped == m_Format) {
		m_Accum.WriteU16(0);
		m_Accum.WriteU16(U16(m_ColorCount));
		m_Accum.WriteU08(32);
	}
	else {
		m_Accum.WriteU16(0);
		m_Accum.WriteU16(0);
		m_Accum.WriteU08(0);
	}

	// X-origin and Y-origin.  Leave them hardwired to (0,0).
	m_Accum.WriteU16(0);
	m_Accum.WriteU16(0);

	m_Accum.WriteU16(U16(m_Width));
	m_Accum.WriteU16(U16(m_Height));

	m_Accum.WriteU08(U08(m_BitDepth));

	// Image descriptor: bit 0x20 indicates top-down image data, and bit
	// 0x10 indicates right-to-left pixel ordering.  While upside-down image
	// data is allowed, this code will not support right-to-left ordering.
	// Possibly no one outside of Truevision (may she rest in peace) has
	// ever used the right-to-left flag.
	if (m_IsBottomUp) {
		m_Accum.WriteU08(0);
	}
	else {
		m_Accum.WriteU08(0x20);
	}

	// Write the optional ID string immediately following the file header.
	if (0 != m_IdStringLength) {
		m_Accum.WriteArrayU08(m_IdString, m_IdStringLength);
	}

	// The color table is the normally the last thing that occurs before the
	// start of image data.
	if (TgaFormat_ColorMapped == m_Format) {
		m_Accum.WriteArrayU32(m_ColorTable, m_ColorCount);
	}

	// The first pixel will be appended right after this point.

	return true;
}


//////////////////////////////////////////////////////////////////////////////
//
//	WriteFrame()
//
void QzTgaWriter::WriteFrame(U08 *pStart, S32 bytePitch)
{
	for (U32 lineNum = 0; lineNum < m_Height; ++lineNum) {
		WriteLine(pStart);

		pStart += bytePitch;
	}
}


//////////////////////////////////////////////////////////////////////////////
//
//	WriteLine()
//
//	Write pixels into the accumulator one line at a time.  This is primarily
//	for the sake of RLE-compressed pixels, where the RLE must break at the
//	end of each line to conform with the TGA file spec.
//
void QzTgaWriter::WriteLine(U08 *pStart)
{
	U08 *pLine = pStart;

	U32 componentCount = m_BitDepth / 8;

	if (2 == componentCount) {
		// Internal 16-bit format is 5:6:5, but most TGA readers
		// expect 5:5:5 when given 16-bit files.
		U16 *pCopy = reinterpret_cast<U16*>(alloca(2 * m_Width));
		for (U32 i = 0; i < m_Width; ++i) {
			pCopy[i] = ((reinterpret_cast<U16*>(pStart)[i]     ) & 0x001F)
						| ((reinterpret_cast<U16*>(pStart)[i] >> 1) & 0x7FE0);
		}

		pLine = reinterpret_cast<U08*>(pCopy);
	}

	// Default mode is to write the raw pixel data straight to the file.
	if (false == m_IsCompressed) {
		U32 lineLength = (m_Width * m_BitDepth) / 8;

		m_Accum.WriteArrayU08(pLine, lineLength);
	}

	// Alternative is to use RLE (run-length encoding) to (attempt to)
	// compress the data into the output buffer.  This can significantly
	// reduce the memory requirement by a factor as large as 128, or it can
	// increase the size of the data if all of the pixels are of different
	// value.
	//
	else {
		switch (componentCount) {
			case 4:
				Encode32(pLine);
				break;

			case 3:
				Encode24(pLine);
				break;

			case 2:
				Encode16(pLine);
				break;

			default:
				Encode08(pLine);
				break;
		}
	}
}


//////////////////////////////////////////////////////////////////////////////
//
//	Encode08()
//
void QzTgaWriter::Encode08(U08 *pPixel)
{
	U32  pixelCount  = m_Width;
	U32  offset      = 0;
	U08* pLineBuffer = reinterpret_cast<U08*>(alloca(m_Width * 8));

	// Loop until all pixels in the line are encoded.
	while (pixelCount > 0) {
		S32 runLength = 1;

		// Get the number of remaining pixels on the current line,
		S32 remaining = Min(pixelCount, 128) - 1;

		// Scan ahead until a different color is found.
		U08 *pLookahead = pPixel + 1;
		while (remaining > 0) {
			if (pLookahead[0] != pPixel[0]) {
				break;
			}

			++runLength;
			--remaining;
			pLookahead += 1;
		}

		// If the run length of same-color pixels is greater than
		// one, save those pixels as a run-length encoded packet.
		if (runLength > 1) {
			if (runLength > 128) {
				runLength = 128;
			}

			pLineBuffer[offset++] = U08(0x80 | (runLength - 1));
			pLineBuffer[offset++] = pPixel[0];

			pPixel += runLength;
		}
		//
		// Otherwise, create a raw packet.
		// This requires finding out how many successively different pixels
		// there are, starting from the current one and stopping at the end
		// of the line.
		//
		else {
			runLength = 0;

			// Get the number of pixels remaining to the end of
			// this scan line.
			remaining = Min(pixelCount, 128);

			// Count the number of sequential pixels which are of different
			// colors, until two same-color pixels are found, or the end of
			// the line is reached.
			pLookahead = pPixel;
			while (remaining > 1) {
				if (pLookahead[0] == pLookahead[1]) {
					break;
				}

				++runLength;
				--remaining;
				pLookahead += 1;
			}

			// If we fall out of the loop with one pixel remaining on this
			// line, then we have the case where there is only one pixel for
			// the last packet, or the last two pixels are the same.  In
			// either case, put everything remaining into a single packet,
			// rather than leaving one or two pixels to go into a packet by
			// themselves.
			if (remaining == 1) {
				++runLength;
			}

			// If the run-length of different-colored pixels exceeds zero (it
			// is zero if the next two pixels are the same color), write out
			// a raw packet
			if (runLength > 0) {
				if (runLength > 128) {
					runLength = 128;
				}

				pLineBuffer[offset++] = U08(runLength - 1);

				for (S32 i = runLength; i > 0; --i) {
					pLineBuffer[offset++] = *(pPixel++);
				}
			}
		}

		// Decrement the count of unwritten pixels by the number which were
		// handled by the packet just written.
		pixelCount -= runLength;
	}

	m_Accum.WriteArrayU08(pLineBuffer, offset);
}


//////////////////////////////////////////////////////////////////////////////
//
//	Encode16()
//
void QzTgaWriter::Encode16(U08 *pPixel)
{
	U32  pixelCount  = m_Width;
	U32  offset      = 0;
	U08* pLineBuffer = reinterpret_cast<U08*>(alloca(m_Width * 8));

	// Loop until all pixels in the line are encoded.
	while (pixelCount > 0) {
		S32 runLength = 1;

		// Get the number of remaining pixels on the current line,
		S32 remaining = Min(pixelCount, 128) - 1;

		// Scan ahead until a different color is found.
		U08 *pLookahead = pPixel + 2;
		while (remaining > 0) {
			if ((pLookahead[0] != pPixel[0]) ||
				(pLookahead[1] != pPixel[1]))
			{
				break;
			}

			++runLength;
			--remaining;
			pLookahead += 2;
		}

		// If the run length of same-color pixels is greater than
		// one, save those pixels as a run-length encoded packet.
		if (runLength > 1) {
			if (runLength > 128) {
				runLength = 128;
			}

			pLineBuffer[offset++] = U08(0x80 | (runLength - 1));
			pLineBuffer[offset++] = pPixel[0];
			pLineBuffer[offset++] = pPixel[1];

			pPixel += 2 * runLength;
		}
		//
		// Otherwise, create a raw packet.
		// This requires finding out how many successively different pixels
		// there are, starting from the current one and stopping at the end
		// of the line.
		//
		else {
			runLength = 0;

			// Get the number of pixels remaining to the end of
			// this scan line.
			remaining = Min(pixelCount, 128);

			// Count the number of sequential pixels which are of different
			// colors, until two same-color pixels are found, or the end of
			// the line is reached.
			pLookahead = pPixel;
			while (remaining > 1) {
				if ((pLookahead[0] == pLookahead[2]) &&
					(pLookahead[1] == pLookahead[3]))
				{
					break;
				}

				++runLength;
				--remaining;
				pLookahead += 2;
			}

			// If we fall out of the loop with one pixel remaining on this
			// line, then we have the case where there is only one pixel for
			// the last packet, or the last two pixels are the same.  In
			// either case, put everything remaining into a single packet,
			// rather than leaving one or two pixels to go into a packet by
			// themselves.
			if (remaining == 1) {
				++runLength;
			}

			// If the run-length of different-colored pixels exceeds zero (it
			// is zero if the next two pixels are the same color), write out
			// a raw packet
			if (runLength > 0) {
				if (runLength > 128) {
					runLength = 128;
				}

				pLineBuffer[offset++] = U08(runLength - 1);

				for (S32 i = runLength; i > 0; --i) {
					pLineBuffer[offset++] = *(pPixel++);
					pLineBuffer[offset++] = *(pPixel++);
				}
			}
		}

		// Decrement the count of unwritten pixels by the number which were
		// handled by the packet just written.
		pixelCount -= runLength;
	}

	m_Accum.WriteArrayU08(pLineBuffer, offset);
}


//////////////////////////////////////////////////////////////////////////////
//
//	Encode24()
//
void QzTgaWriter::Encode24(U08 *pPixel)
{
	U32  pixelCount  = m_Width;
	U32  offset      = 0;
	U08* pLineBuffer = reinterpret_cast<U08*>(alloca(m_Width * 8));

	// Loop until all pixels in the line are encoded.
	while (pixelCount > 0) {
		S32 runLength = 1;

		// Get the number of remaining pixels on the current line,
		S32 remaining = Min(pixelCount, 128) - 1;

		// Scan ahead until a different color is found.
		U08 *pLookahead = pPixel + 3;
		while (remaining > 0) {
			if ((pLookahead[0] != pPixel[0]) ||
				(pLookahead[1] != pPixel[1]) ||
				(pLookahead[2] != pPixel[2]))
			{
				break;
			}

			++runLength;
			--remaining;
			pLookahead += 3;
		}

		// If the run length of same-color pixels is greater than
		// one, save those pixels as a run-length encoded packet.
		if (runLength > 1) {
			if (runLength > 128) {
				runLength = 128;
			}

			pLineBuffer[offset++] = U08(0x80 | (runLength - 1));
			pLineBuffer[offset++] = pPixel[0];
			pLineBuffer[offset++] = pPixel[1];
			pLineBuffer[offset++] = pPixel[2];

			pPixel += 3 * runLength;
		}
		//
		// Otherwise, create a raw packet.
		// This requires finding out how many successively different pixels
		// there are, starting from the current one and stopping at the end
		// of the line.
		//
		else {
			runLength = 0;

			// Get the number of pixels remaining to the end of
			// this scan line.
			remaining = Min(pixelCount, 128);

			// Count the number of sequential pixels which are of different
			// colors, until two same-color pixels are found, or the end of
			// the line is reached.
			pLookahead = pPixel;
			while (remaining > 1) {
				if ((pLookahead[0] == pLookahead[3]) &&
					(pLookahead[1] == pLookahead[4]) &&
					(pLookahead[2] == pLookahead[5]))
				{
					break;
				}

				++runLength;
				--remaining;
				pLookahead += 3;
			}

			// If we fall out of the loop with one pixel remaining on this
			// line, then we have the case where there is only one pixel for
			// the last packet, or the last two pixels are the same.  In
			// either case, put everything remaining into a single packet,
			// rather than leaving one or two pixels to go into a packet by
			// themselves.
			if (remaining == 1) {
				++runLength;
			}

			// If the run-length of different-colored pixels exceeds zero (it
			// is zero if the next two pixels are the same color), write out
			// a raw packet
			if (runLength > 0) {
				if (runLength > 128) {
					runLength = 128;
				}

				pLineBuffer[offset++] = U08(runLength - 1);

				for (S32 i = runLength; i > 0; --i) {
					pLineBuffer[offset++] = *(pPixel++);
					pLineBuffer[offset++] = *(pPixel++);
					pLineBuffer[offset++] = *(pPixel++);
				}
			}
		}

		// Decrement the count of unwritten pixels by the number which were
		// handled by the packet just written.
		pixelCount -= runLength;
	}

	m_Accum.WriteArrayU08(pLineBuffer, offset);
}


//////////////////////////////////////////////////////////////////////////////
//
//	Encode32()
//
void QzTgaWriter::Encode32(U08 *pPixel)
{
	U32  pixelCount  = m_Width;
	U32  offset      = 0;
	U08* pLineBuffer = reinterpret_cast<U08*>(alloca(m_Width * 8));

	// Loop until all pixels in the line are encoded.
	while (pixelCount > 0) {
		S32 runLength = 1;

		// Get the number of remaining pixels on the current line,
		S32 remaining = Min(pixelCount, 128) - 1;

		// Scan ahead until a different color is found.
		U08 *pLookahead = pPixel + 4;
		while (remaining > 0) {
			if ((pLookahead[0] != pPixel[0]) ||
				(pLookahead[1] != pPixel[1]) ||
				(pLookahead[2] != pPixel[2]) ||
				(pLookahead[3] != pPixel[3]))
			{
				break;
			}

			++runLength;
			--remaining;
			pLookahead += 4;
		}

		// If the run length of same-color pixels is greater than
		// one, save those pixels as a run-length encoded packet.
		if (runLength > 1) {
			if (runLength > 128) {
				runLength = 128;
			}

			U08 packet = U08(0x80 | (runLength - 1));
			pLineBuffer[offset++] = packet;
			pLineBuffer[offset++] = pPixel[0];
			pLineBuffer[offset++] = pPixel[1];
			pLineBuffer[offset++] = pPixel[2];
			pLineBuffer[offset++] = pPixel[3];

			pPixel += 4 * runLength;
		}
		//
		// Otherwise, create a raw packet.
		// This requires finding out how many successively different pixels
		// there are, starting from the current one and stopping at the end
		// of the line.
		//
		else {
			runLength = 0;

			// Get the number of pixels remaining to the end of
			// this scan line.
			remaining = Min(pixelCount, 128);

			// Count the number of sequential pixels which are of different
			// colors, until two same-color pixels are found, or the end of
			// the line is reached.
			pLookahead = pPixel;
			while (remaining > 1) {
				if ((pLookahead[0] == pLookahead[4]) &&
					(pLookahead[1] == pLookahead[5]) &&
					(pLookahead[2] == pLookahead[6]) &&
					(pLookahead[3] == pLookahead[7]))
				{
					break;
				}

				++runLength;
				--remaining;
				pLookahead += 4;
			}

			// If we fall out of the loop with one pixel remaining on this
			// line, then we have the case where there is only one pixel for
			// the last packet, or the last two pixels are the same.  In
			// either case, put everything remaining into a single packet,
			// rather than leaving one or two pixels to go into a packet by
			// themselves.
			if (remaining == 1) {
				++runLength;
			}

			// If the run-length of different-colored pixels exceeds zero (it
			// is zero if the next two pixels are the same color), write out
			// a raw packet
			if (runLength > 0) {
				if (runLength > 128) {
					runLength = 128;
				}

				pLineBuffer[offset++] = U08(runLength - 1);

				for (S32 i = runLength; i > 0; --i) {
					pLineBuffer[offset++] = *(pPixel++);
					pLineBuffer[offset++] = *(pPixel++);
					pLineBuffer[offset++] = *(pPixel++);
					pLineBuffer[offset++] = *(pPixel++);
				}
			}
		}

		// Decrement the count of unwritten pixels by the number which were
		// handled by the packet just written.
		pixelCount -= runLength;
	}

	m_Accum.WriteArrayU08(pLineBuffer, offset);
}


//////////////////////////////////////////////////////////////////////////////
//
//	SaveFile()
//
bool QzTgaWriter::SaveFile(const Utf08_t filename[])
{
	if (TgaFormat_Unknown == m_Format) {
		return false;
	}

	// Before closing the file, write the footer at the end.  It is required
	// by the 2.0 TGA file spec, but virtually no one bothers with it.  Since
	// it is the only wait to positively identify a TGA file, given the lack
	// of identifying markers at the start of the file, we'll always write
	// the footer.

	// These two words are the offsets of the extension area and the
	// developer directory.  No one ever seems to use these, or look for
	// them in files, so leave them blank.
	m_Accum.WriteU32(0);
	m_Accum.WriteU32(0);

	m_Accum.WriteArrayU08((U08*)"TRUEVISION-XFILE.", 18);

	if (false == m_Accum.SaveToFile(filename)) {
		return false;
	}

	m_Format = TgaFormat_Unknown;

	return true;
}


