/////////////////////////////////////////////////////////////////////////////
//
//	File: UtfString.cpp
//
//	$Header: /Projects/Qz/UtfString.cpp  25  2010/4/18 7:56:36p  Lee $
//
//
//	This file contains a collection of support routines for performing string
//	operations on UTF strings.  Most transforms are for data in UTF-8 format,
//	but it also supports converting to and from UTF-16 and UTF-32.
//
/////////////////////////////////////////////////////////////////////////////


#include "QzCommon.h"
#include "UtfData.h"


#ifdef USE_MALLOC_MACRO
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
//
//	UtfSymbolSize08()
//
//	Determines the size of the UTF-08 byte sequence, based on the high bits
//	of the first byte.
//
//	1-byte code: 0??? ????
//	2-byte code: 110? ????
//	3-byte code: 1110 ????
//	4-byte code: 1111 0???
//	middle code: 10?? ????
//
//	Returns either the expected number of bytes in the sequence, or zero if
//	the symbol is from the middle of a byte sequence.
//
U32 UtfSymbolSize08(Utf08_t symbol)
{
	if (symbol <= 0x7F) {
		return 1;
	}

	if (0xC0 == (0xE0 & symbol)) {
		return 2;
	}

	if (0xE0 == (0xF0 & symbol)) {
		return 3;
	}

	if (0xF0 == (0xF8 & symbol)) {
		return 4;
	}

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfSymbolSize32()
//
//	How many bytes will be packed into the buffer by calling UtfPackChar08()?
//	Since UtfPackChar08() will do nothing for out-of-range values, this must
//	return zero for that case.
//
U32 UtfSymbolSize32(Utf32_t value)
{
	if (value <= 0x007F) {
		return 1;
	}

	if (value <= 0x07FF) {
		return 2;
	}

	if (value <= 0xFFFF) {
		return 3;
	}

	if (value <= Unicode_MaxValidCode) {
		return 4;
	}

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
//
//	IsMiddleByte()
//
//	Middle bytes in a multi-byte sequence should have bit 7 set, and bit 6
//	clear.
//
static inline bool IsMiddleByte(Utf08_t symbol)
{
	return (0x80 == (0xC0 & symbol));
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfIsAlpha()
//
bool UtfIsAlpha(Utf32_t value)
{
	UtfSymbolTable_t *pSymbol = UtfLookUpSymbol(value);

	if (NULL != pSymbol) {
		return (0 != ((UtfFlag_Lowercase | UtfFlag_Uppercase) & pSymbol->Flags));
	}

	return false;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfIsDigit()
//
bool UtfIsDigit(Utf32_t value)
{
	return (('0' <= value) && (value <= '9'));
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfIsHexDigit()
//
bool UtfIsHexDigit(Utf32_t symbol)
{
	return ((('0' <= symbol) && (symbol <= '9')) ||
			(('a' <= symbol) && (symbol <= 'f')) ||
			(('A' <= symbol) && (symbol <= 'F')));
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfIsKnown()
//
//	Returns true if the symbol is a known, supported Unicode character.
//	If the symbol does not have an entry in g_UtfSymbolTable[], it is not
//	a known or supported character.
//
bool UtfIsKnown(Utf32_t symbol)
{
	return (NULL != UtfLookUpSymbol(symbol));
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfIsLower()
//
bool UtfIsLower(Utf32_t value)
{
	UtfSymbolTable_t *pSymbol = UtfLookUpSymbol(value);

	if (NULL != pSymbol) {
		return (0 != (UtfFlag_Lowercase & pSymbol->Flags));
	}

	return false;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfIsUpper()
//
bool UtfIsUpper(Utf32_t value)
{
	UtfSymbolTable_t *pSymbol = UtfLookUpSymbol(value);

	if (NULL != pSymbol) {
		return (0 != (UtfFlag_Uppercase & pSymbol->Flags));
	}

	return false;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfExtractHexDigit()
//
//	Converts a hex digit into its binary equivalent, or returns false if
//	this symbol is not a hex digit.
//
bool UtfExtractHexDigit(Utf32_t symbol, U32 &value)
{
	if (('0' <= symbol) && (symbol <= '9')) {
		value = symbol - '0';
		return true;
	}

	if (('a' <= symbol) && (symbol <= 'z')) {
		value = symbol - 'a' + 10;
		return true;
	}

	if (('A' <= symbol) && (symbol <= 'Z')) {
		value = symbol - 'A' + 10;
		return true;
	}

	return false;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfToLower()
//
//	Maps an upper-case character to lower-case.
//
//	Returns the given character unchanged if it is not an upper-case symbol.
//
Utf32_t UtfToLower(Utf32_t value)
{
	UtfSymbolTable_t *pSymbol = UtfLookUpSymbol(value);

	// Exercise caution: some characters are flagged as upper-case, but do
	// not have a lower-case value, which is indicated by a zero in AltCase.
	if ((NULL != pSymbol) &&
		(0    != (UtfFlag_Uppercase & pSymbol->Flags)) &&
		('\0' != pSymbol->AltCase))
	{
		return pSymbol->AltCase;
	}

	return value;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfToUpper()
//
//	Maps a lower-case character to upper-case.
//
//	Returns the given character unchanged if it is not a lower-case symbol.
//
Utf32_t UtfToUpper(Utf32_t value)
{
	UtfSymbolTable_t *pSymbol = UtfLookUpSymbol(value);

	// Exercise caution: some characters are flagged as lower-case, but do
	// not have an upper-case value, which is indicated by a zero in AltCase.
	if ((NULL != pSymbol) &&
		(0    != (UtfFlag_Lowercase & pSymbol->Flags)) &&
		('\0' != pSymbol->AltCase))
	{
		return pSymbol->AltCase;
	}

	return value;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfIsValid()
//
//	Symbols are limited to 21 bits, but cannot exceed 0x10FFFF.
//	Symbols cannot end with 0xFFFF or 0xFFFE.
//	Symbols between 0xD800 and 0xDFFF are reserved for surrogates.
//	Symbols between 0xFDD0 and 0xFDEF are illegal non-chars, for private use.
//
bool UtfIsValid(Utf32_t symbol)
{
	return ((symbol <= Unicode_MaxValidCode) &&
			(0xFFFE != (0xFFFE & symbol)) &&
			(0xFFFF != (0xFFFF & symbol)) &&
			((symbol < Unicode_FirstSurrogate) || (symbol > Unicode_LastSurrogate)));
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfPackChar08()
//
//	Packs a unicode value into a UTF-8 byte sequence.  Out-of-range symbols
//	are ignored.
//
void UtfPackChar08(Utf08_t *pDst, U32 &offset, Utf32_t value)
{
	if (value <= 0x007F) {
		pDst[offset++] = Utf08_t(value);
	}
	else if (value <= 0x07FF) {
		pDst[offset++] = Utf08_t(0xC0 | ((value >>  6)       ));
		pDst[offset++] = Utf08_t(0x80 | ((value      ) & 0x3F));
	}
	else if (value <= 0xFFFF) {
		pDst[offset++] = Utf08_t(0xE0 | ((value >> 12)       ));
		pDst[offset++] = Utf08_t(0x80 | ((value >>  6) & 0x3F));
		pDst[offset++] = Utf08_t(0x80 | ((value      ) & 0x3F));
	}
	else if (value <= Unicode_MaxValidCode) {
		pDst[offset++] = Utf08_t(0xF0 | ((value >> 18)       ));
		pDst[offset++] = Utf08_t(0x80 | ((value >> 12) & 0x3F));
		pDst[offset++] = Utf08_t(0x80 | ((value >>  6) & 0x3F));
		pDst[offset++] = Utf08_t(0x80 | ((value      ) & 0x3F));
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfNextChar()
//
//	Attempts to return the next symbol from the byte stream.  If the next
//	sequence of bytes does not form a valid Unicode value, this will return
//	zero.  It will advance the pointer in that case, so the caller can
//	continue to scan for more characters if it expects them
//
Utf32_t UtfNextChar(const Utf08_t *pStrg, U32 &offset)
{
	Utf32_t b0 = pStrg[offset];

	// Special case for end of string: do not scan past the sentinel.
	// For any other symbol, we need to advance at least one byte, even if
	// we don't know what that value is, so we can avoid infinite loops.
	if ('\0' == b0) {
		return 0;
	}
	else if (b0 <= 0x7F) {
		offset += 1;
		return b0;
	}
	else if (b0 <= 0xDF) {
		Utf08_t b1 = pStrg[offset+1];

		if (IsMiddleByte(b1)) {
			offset += 2;
			return ((b0 & 0x1F) << 6) | U32(b1 & 0x3F);
		}
	}
	else if (b0 <= 0xEF) {
		Utf08_t b1 = pStrg[offset+1];
		Utf08_t b2 = pStrg[offset+2];

		if (IsMiddleByte(b1) && IsMiddleByte(b2)) {
			offset += 3;
			return (U32(b0 & 0x0F) << 12) | (U32(b1 & 0x3F) << 6) | U32(b2 & 0x3F);
		}
	}
	else if (b0 <= 0xF7) {
		Utf08_t b1 = pStrg[offset+1];
		Utf08_t b2 = pStrg[offset+2];
		Utf08_t b3 = pStrg[offset+3];

		if (IsMiddleByte(b1) && IsMiddleByte(b2) && IsMiddleByte(b3)) {
			offset += 4;
			return (U32(b0 & 0x07) << 18) | (U32(b1 & 0x3F) << 12) | (U32(b2 & 0x3F) << 6) | U32(b3 & 0x3F);
		}
	}

	// If we get to this point, it does not appear to be a valid symbol.
	// Just increment the pointer one step and return zero.  The caller
	// will probably treat this like the end of the string and stop scanning
	// any further.
	offset += 1;
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfNextChar()
//
//	Attempts to return the next symbol from the byte stream.  If the next
//	sequence of bytes does not form a valid Unicode value, this will return
//	zero.  It will advance the pointer in that case, so the caller can
//	continue to scan for more characters if it expects them
//
Utf32_t UtfNextChar(const Utf16_t *pStrg, U32 &offset)
{
	Utf32_t b0 = pStrg[offset];

	// Do not increment the offset when we're looking at the sentinel.
	if ('\0' == b0) {
		return 0;
	}

	// The special case is for surrogates, which split up a Unicode
	// symbol across two UTF-16 symbols.  If the value is in this range,
	// it may be either the first or second symbol in a surrogate pair.
	if ((b0 >= Unicode_FirstSurrogate) && (b0 <= Unicode_LastSurrogate)) {
		Utf32_t b1 = pStrg[offset+1];

		if ((b0 < 0xDC00) && (b1 >= 0xDC00) && (b1 <= Unicode_LastSurrogate)) {
			// Note the trick with 0xD7C0: This really is 0xD800, but the
			// high word is stored with -1 to shift the 1-16 range down to
			// 0-15 so it will fit in 4 bits instead of 5.  Factoring that
			// extra adjustment in, we can use 0xD7C0 instead of 0xD800.
			offset += 2;
			return ((b0 - 0xD7C0) << 10) | Utf32_t(b1 & 0x03FF);
		}

		// Otherwise, we ignore stray second symbols.
		offset += 1;
		return 0;
	}

	offset += 1;
	return b0;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfPrevChar()
//
//	Scans backwards to locate the previous character.  The given offset value
//	is always honoured, so code will never scan backwards off the beginning
//	of the buffer.
//
//	To start scanning from the very end of the buffer, set <offset> to the
//	position of the '\0' at the end of the string.
//
Utf32_t UtfPrevChar(const Utf08_t *pStrg, U32 &offset)
{
	if (0 == offset) {
		return 0;
	}

	Utf32_t b0 = pStrg[offset-1];

	if (b0 <= 0x7F) {
		offset -= 1;
		return b0;
	}

	if (offset < 2) {
		offset = 0;
		return 0;
	}

	b0 = pStrg[offset-2];

	if (b0 <= 0xDF) {
		Utf08_t b1 = pStrg[offset-1];

		if (IsMiddleByte(b1)) {
			offset -= 2;
			return ((b0 & 0x1F) << 6) | U32(b1 & 0x3F);
		}
	}

	if (offset < 3) {
		offset = 0;
		return 0;
	}

	b0 = pStrg[offset-3];

	if (b0 <= 0xEF) {
		Utf08_t b1 = pStrg[offset-2];
		Utf08_t b2 = pStrg[offset-1];

		if (IsMiddleByte(b1) && IsMiddleByte(b2)) {
			offset -= 3;
			return (U32(b0 & 0x0F) << 12) | (U32(b1 & 0x3F) << 6) | U32(b2 & 0x3F);
		}
	}

	if (offset < 4) {
		offset = 0;
		return 0;
	}

	b0 = pStrg[offset-4];

	if (b0 <= 0xF7) {
		Utf08_t b1 = pStrg[offset-3];
		Utf08_t b2 = pStrg[offset-2];
		Utf08_t b3 = pStrg[offset-1];

		if (IsMiddleByte(b1) && IsMiddleByte(b2) && IsMiddleByte(b3)) {
			offset -= 4;
			return (U32(b0 & 0x07) << 18) | (U32(b1 & 0x3F) << 12) | (U32(b2 & 0x3F) << 6) | U32(b3 & 0x3F);
		}
	}

	// If we get to this point, it does not appear to be a valid symbol.
	// Just increment the pointer one step and return zero.  The caller
	// will probably treat this like the end of the string and stop scanning
	// any further.
	offset -= 1;
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfToLowerASCII()
//
//	Takes a string, converting all upper-case ASCII characters to lower case.
//	Any Unicode symbols in the string are ignored.
//
void UtfToLowerASCII(Utf08_t pStrg[])
{
	if (NULL != pStrg) {
		Utf08_t *p = pStrg;

		while ('\0' != *p) {
			if (('A' <= *p) && (*p <= 'Z')) {
				*p += 'a' - 'A';
			}

			++p;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfCharLength()
//
//	Analyzes the UTF-8 string and reports how many bytes are required to
//	store the next char.
//
//	If the next byte sequence does not form a valid character, it will return
//	1 so the caller can do a one-byte-at-a-time scan until it finds the start
//	of the next valid UTF-8 character.
//
U32 UtfCharLength(const Utf08_t pStrg[])
{
	Utf32_t b0 = pStrg[0];

	if (b0 <= 0x7F) {
		return 1;
	}

	if (b0 <= 0xDF) {
		if (IsMiddleByte(pStrg[1])) {
			return 2;
		}

		return 1;
	}

	if (b0 <= 0xEF) {
		if (IsMiddleByte(pStrg[1]) && IsMiddleByte(pStrg[2])) {
			return 3;
		}

		return 1;
	}

	if (b0 <= 0xF7) {
		if (IsMiddleByte(pStrg[1]) && IsMiddleByte(pStrg[2]) && IsMiddleByte(pStrg[3])) {
			return 4;
		}

		return 1;
	}

	return 1;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfByteCount()
//
//	Counts the total number of bytes in a UTF-8 string.  This is effectively
//	the same as strlen(), since it only needs to seek to the first non-zero
//	byte in the string.
//
U32 UtfByteCount(const Utf08_t pStrg[])
{
	if (NULL == pStrg) {
		return 0;
	}

	U32 byteCount = 0;

	Utf08_t *p = const_cast<Utf08_t*>(pStrg);

	while ('\0' != *(p++)) {
		++byteCount;
	}

	return byteCount;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfCharCount()
//
//	Counts up the total number of chars in a UTF-8 string.  Since this is a
//	multi-byte format, each char may comprise one, two, three, or four bytes.
//
//	If the string ends with a truncated symbol, that symbol will not be
//	counted in the size.
//
U32 UtfCharCount(const Utf08_t pStrg[])
{
	U32 byteCount;
	U32 charCount;

	// Let UtfStringSize() do all of the heavy lifting.  We'll just return
	// the final tally of symbols.
	UtfStringSize(pStrg, byteCount, charCount);

	return charCount;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfCharCount()
//
//	The 32-bit version of UtfCharCount().  Since UTF-32 strings store exactly
//	one char in each word, we can determine the number of chars the old
//	fashioned way -- count words until we find '\0';
//
U32 UtfCharCount(const Utf32_t pStrg[])
{
	if (NULL == pStrg) {
		return 0;
	}

	U32 charCount = 0;

	while ('\0' != pStrg[charCount]) {
		++charCount;
	}

	return charCount;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfStringSize()
//
//	This is a combined version of UtfCharCount() and UtfByteCount().  It will
//	compute both count values in a single pass for better performance when
//	both values are needed for string processing.
//
void UtfStringSize(const Utf08_t pStrg[], U32 &byteCount, U32 &charCount)
{
	if (NULL == pStrg) {
		byteCount = 0;
		charCount = 0;
		return;
	}

	charCount  = 0;
	U32 offset = 0;

	for (;;) {
		U32 symbol = pStrg[offset];

		if ('\0' == symbol) {
			break;
		}

		if (symbol <= 0x7F) {
			++offset;
			++charCount;
		}
		else if (0xC0 == (0xE0 & symbol)) {
			if (IsMiddleByte(pStrg[offset+1])) {
				offset    += 2;
				charCount += 1;
			}
			else if ('\0' == pStrg[offset+1]) {
				break;
			}
			else {
				++offset;
			}
		}
		else if (0xE0 == (0xF0 & symbol)) {
			if (IsMiddleByte(pStrg[offset+1]) && IsMiddleByte(pStrg[offset+2])) {
				offset    += 3;
				charCount += 1;
			}
			else if (('\0' == pStrg[offset+1]) || ('\0' == pStrg[offset+2])) {
				break;
			}
			else {
				++offset;
			}
		}
		else if (0xF0 == (0xF8 & symbol)) {
			if (IsMiddleByte(pStrg[offset+1]) && IsMiddleByte(pStrg[offset+2]) && IsMiddleByte(pStrg[offset+3])) {
				offset    += 4;
				charCount += 1;
			}
			else if (('\0' == pStrg[offset+1]) || ('\0' == pStrg[offset+2]) || ('\0' == pStrg[offset+3])) {
				break;
			}
			else {
				++offset;
			}
		}
		else {
			++offset;
		}
	}

	byteCount = offset;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfPreCompose32to08()
//
//	This works the same as UtfCompose32to08(), only without writing anything
//	into a buffer.  Use this function to determine how much memory will be
//	required to store the entire composed string.
//
U32 UtfPreCompose32to08(const Utf32_t pSrc[])
{
	if (NULL == pSrc) {
		return 0;
	}

	U32 srcNum = 0;
	U32 dstNum = 0;

	Utf32_t a = pSrc[srcNum++];
	Utf32_t b = 0;
	Utf32_t replace;

	if ('\0' != a) {
		b = pSrc[srcNum++];
	}

	while ('\0' != a) {
		U32 replaceCount = UtfComposeChars(a, b, replace);

		// The two symbols were composed into a single symbol which was
		// return in <replace>.  This replaces <a>, and we need to load
		// the next value of <b>.  Do not output <a> yet, since we need to
		// make another compose attempt on the next iteration, since there
		// are a number of three-symbol sequences that can be composed into
		// one symbol.
		if (2 == replaceCount) {
			a = replace;

			if ('\0' != b) {
				b = pSrc[srcNum++];
			}
		}

		// The value <a> was replaced with <replace>, but <b> was left alone.
		// Swap in the new value for <a> and keep going, since it's possible
		// that the new value of <a> will compose with whatever symbol is
		// currently stored in <b>.
		else if (1 == replaceCount) {
			a = replace;
		}

		// Otherwise no composition is possible.  We can output <a> to the
		// destination buffer and advance to the next input symbol.
		else {
			if ('\r' != a) {
				dstNum += UtfSymbolSize32(a);
			}

			// Swap the current <b> into <a> for the next composition pass.
			a = b;

			// Special case: if we just wrote '\0' into <a>, we have nothing
			// more to process.
			if ('\0' == b) {
				break;
			}

			// Pre-load the next value into <b>.  Don't stop if we're at the
			// end of the source string, since the value now in <a> may still
			// need to have a substitution applied to it.
			b = pSrc[srcNum++];
		}
	}

	return dstNum;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfPreCompose16to08()
//
//	This works the same as UtfCompose16to08(), only without writing anything
//	into a buffer.  Use this function to determine how much memory will be
//	required to store the entire composed string.
//
U32 UtfPreCompose16to08(const Utf16_t pSrc[])
{
	if (NULL == pSrc) {
		return 0;
	}

	U32 srcNum = 0;
	U32 dstNum = 0;

	Utf32_t a, b, replace;

	a = UtfNextChar(pSrc, srcNum);
	b = UtfNextChar(pSrc, srcNum);

	while ('\0' != a) {
		U32 replaceCount = UtfComposeChars(a, b, replace);

		// The two symbols were composed into a single symbol which was
		// return in <replace>.  This replaces <a>, and we need to load
		// the next value of <b>.  Do not output <a> yet, since we need to
		// make another compose attempt on the next iteration, since there
		// are a number of three-symbol sequences that can be composed into
		// one symbol.
		if (2 == replaceCount) {
			a = replace;
			b = UtfNextChar(pSrc, srcNum);
		}

		// The value <a> was replaced with <replace>, but <b> was left alone.
		// Swap in the new value for <a> and keep going, since it's possible
		// that the new value of <a> will compose with whatever symbol is
		// currently stored in <b>.
		else if (1 == replaceCount) {
			a = replace;
		}

		// Otherwise no composition is possible.  We can output <a> to the
		// destination buffer and advance to the next input symbol.
		else {
			if ('\r' != a) {
				dstNum += UtfSymbolSize32(a);
			}

			// Swap the current <b> into <a> for the next composition pass.
			a = b;

			// Special case: if we just wrote '\0' into <a>, we have nothing
			// more to process.
			if ('\0' == b) {
				break;
			}

			// Pre-load the next value into <b>.  Don't stop if we're at the
			// end of the source string, since the value now in <a> may still
			// need to have a substitution applied to it.
			b = UtfNextChar(pSrc, srcNum);
		}
	}

	return dstNum;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfPreCompose08to08()
//
//	This works the same as UtfCompose08to08(), only without writing anything
//	into a buffer.  Use this function to determine how much memory will be
//	required to store the entire composed string.
//
U32 UtfPreCompose08to08(const Utf08_t pSrc[])
{
	if (NULL == pSrc) {
		return 0;
	}

	U32 srcNum = 0;
	U32 dstNum = 0;

	Utf32_t a, b, replace;

	a = UtfNextChar(pSrc, srcNum);
	b = UtfNextChar(pSrc, srcNum);

	while ('\0' != a) {
		U32 replaceCount = UtfComposeChars(a, b, replace);

		// The two symbols were composed into a single symbol which was
		// return in <replace>.  This replaces <a>, and we need to load
		// the next value of <b>.  Do not output <a> yet, since we need to
		// make another compose attempt on the next iteration, since there
		// are a number of three-symbol sequences that can be composed into
		// one symbol.
		if (2 == replaceCount) {
			a = replace;
			b = UtfNextChar(pSrc, srcNum);
		}

		// The value <a> was replaced with <replace>, but <b> was left alone.
		// Swap in the new value for <a> and keep going, since it's possible
		// that the new value of <a> will compose with whatever symbol is
		// currently stored in <b>.
		else if (1 == replaceCount) {
			a = replace;
		}

		// Otherwise no composition is possible.  We can output <a> to the
		// destination buffer and advance to the next input symbol.
		else {
			if ('\r' != a) {
				dstNum += UtfSymbolSize32(a);
			}

			// Swap the current <b> into <a> for the next composition pass.
			a = b;

			// Special case: if we just wrote '\0' into <a>, we have nothing
			// more to process.
			if ('\0' == b) {
				break;
			}

			// Pre-load the next value into <b>.  Don't stop if we're at the
			// end of the source string, since the value now in <a> may still
			// need to have a substitution applied to it.
			b = UtfNextChar(pSrc, srcNum);
		}
	}

	return dstNum;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfPreComposeASCII()
//
//	This works the same as UtfComposeASCII(), only without writing anything
//	into a buffer.  Use this function to determine how much memory will be
//	required to store the entire composed string.
//
U32 UtfPreComposeASCII(const char pSrc[])
{
	if (NULL == pSrc) {
		return 0;
	}

	U32 dstOffset = 0;
	U32 srcOffset = 0;

	while ('\0' != pSrc[srcOffset]) {
		Utf08_t symbol = pSrc[srcOffset++];

		if (symbol <= 0x7F) {
			// All 7-bit ASCII values map directly.  But we'll ignore
			// carriage returns, making it easier for higher-level code to
			// parse text.
			if ('\r' != symbol) {
				++dstOffset;
			}
		}
		else {
			// Map the 8-bit ASCII into its Unicode symbol.
			Utf32_t mapped = UtfMapASCII(symbol);

			// Some symbols are not supported.  Ignore any symbol that maps
			// to zero.
			if (0 != mapped) {
				dstOffset += UtfSymbolSize32(mapped);
			}
		}
	}

	return dstOffset;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfCompose32to08()
//
//	Takes a UTF-32 string, composes any normalized symbols into their
//	corresponding composit, and writes the result in the destination buffer.
//
//	This is intended for conditioning all input text to make sure it only
//	contains the sub-set of unicode values which we know how to process.
//
U32 UtfCompose32to08(Utf08_t pDst[], U32 dstLimit, const Utf32_t pSrc[])
{
	if (NULL == pSrc) {
		pDst[0] = '\0';
		return 0;
	}

	U32 srcNum = 0;
	U32 dstNum = 0;

	Utf32_t a = pSrc[srcNum++];
	Utf32_t b = 0;
	Utf32_t replace;

	if ('\0' != a) {
		b = pSrc[srcNum++];
	}

	while ('\0' != a) {
		U32 replaceCount = UtfComposeChars(a, b, replace);

		// The two symbols were composed into a single symbol which was
		// return in <replace>.  This replaces <a>, and we need to load
		// the next value of <b>.  Do not output <a> yet, since we need to
		// make another compose attempt on the next iteration, since there
		// are a number of three-symbol sequences that can be composed into
		// one symbol.
		if (2 == replaceCount) {
			a = replace;

			if ('\0' != b) {
				b = pSrc[srcNum++];
			}
		}

		// The value <a> was replaced with <replace>, but <b> was left alone.
		// Swap in the new value for <a> and keep going, since it's possible
		// that the new value of <a> will compose with whatever symbol is
		// currently stored in <b>.
		else if (1 == replaceCount) {
			a = replace;
		}

		// Otherwise no composition is possible.  We can output <a> to the
		// destination buffer and advance to the next input symbol.
		else {
			if ('\r' != a) {
				if ((dstNum + UtfSymbolSize32(a)) > dstLimit) {
					break;
				}

				UtfPackChar08(pDst, dstNum, a);
			}

			// Swap the current <b> into <a> for the next composition pass.
			a = b;

			// Special case: if we just wrote '\0' into <a>, we have nothing
			// more to process.
			if ('\0' == b) {
				break;
			}

			// Pre-load the next value into <b>.  Don't stop if we're at the
			// end of the source string, since the value now in <a> may still
			// need to have a substitution applied to it.
			b = pSrc[srcNum++];
		}
	}

	pDst[dstNum] = '\0';

	return dstNum;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfCompose16to08()
//
//	Takes a UTF-16 string, composes any normalized symbols into their
//	corresponding composit, and writes the result in the destination buffer.
//
//	This is intended for conditioning all input text to make sure it only
//	contains the sub-set of unicode values which we know how to process.
//
U32 UtfCompose16to08(Utf08_t pDst[], U32 dstLimit, const Utf16_t pSrc[])
{
	if (NULL == pSrc) {
		pDst[0] = '\0';
		return 0;
	}

	U32 srcNum = 0;
	U32 dstNum = 0;

	Utf32_t a, b, replace;

	a = UtfNextChar(pSrc, srcNum);
	b = UtfNextChar(pSrc, srcNum);

	while ('\0' != a) {
		U32 replaceCount = UtfComposeChars(a, b, replace);

		// The two symbols were composed into a single symbol which was
		// return in <replace>.  This replaces <a>, and we need to load
		// the next value of <b>.  Do not output <a> yet, since we need to
		// make another compose attempt on the next iteration, since there
		// are a number of three-symbol sequences that can be composed into
		// one symbol.
		if (2 == replaceCount) {
			a = replace;
			b = UtfNextChar(pSrc, srcNum);
		}

		// The value <a> was replaced with <replace>, but <b> was left alone.
		// Swap in the new value for <a> and keep going, since it's possible
		// that the new value of <a> will compose with whatever symbol is
		// currently stored in <b>.
		else if (1 == replaceCount) {
			a = replace;
		}

		// Otherwise no composition is possible.  We can output <a> to the
		// destination buffer and advance to the next input symbol.
		else {
			if ('\r' != a) {
				if ((dstNum + UtfSymbolSize32(a)) > dstLimit) {
					break;
				}

				UtfPackChar08(pDst, dstNum, a);
			}

			// Swap the current <b> into <a> for the next composition pass.
			a = b;

			// Special case: if we just wrote '\0' into <a>, we have nothing
			// more to process.
			if ('\0' == b) {
				break;
			}

			// Pre-load the next value into <b>.  Don't stop if we're at the
			// end of the source string, since the value now in <a> may still
			// need to have a substitution applied to it.
			b = UtfNextChar(pSrc, srcNum);
		}
	}

	pDst[dstNum] = '\0';

	return dstNum;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfCompose08to08()
//
//	Takes a UTF-08 string, composes any normalized symbols into their
//	corresponding composit, and writes the result in the destination buffer.
//
//	This is intended for conditioning all input text to make sure it only
//	contains the sub-set of unicode values which we know how to process.
//
U32 UtfCompose08to08(Utf08_t pDst[], U32 dstLimit, const Utf08_t pSrc[])
{
	if (NULL == pSrc) {
		pDst[0] = '\0';
		return 0;
	}

	U32 srcNum = 0;
	U32 dstNum = 0;

	Utf32_t a, b, replace;

	a = UtfNextChar(pSrc, srcNum);
	b = UtfNextChar(pSrc, srcNum);

	while ('\0' != a) {
		U32 replaceCount = UtfComposeChars(a, b, replace);

		// The two symbols were composed into a single symbol which was
		// return in <replace>.  This replaces <a>, and we need to load
		// the next value of <b>.  Do not output <a> yet, since we need to
		// make another compose attempt on the next iteration, since there
		// are a number of three-symbol sequences that can be composed into
		// one symbol.
		if (2 == replaceCount) {
			a = replace;
			b = UtfNextChar(pSrc, srcNum);
		}

		// The value <a> was replaced with <replace>, but <b> was left alone.
		// Swap in the new value for <a> and keep going, since it's possible
		// that the new value of <a> will compose with whatever symbol is
		// currently stored in <b>.
		else if (1 == replaceCount) {
			a = replace;
		}

		// Otherwise no composition is possible.  We can output <a> to the
		// destination buffer and advance to the next input symbol.
		else {
			if ('\r' != a) {
				if ((dstNum + UtfSymbolSize32(a)) > dstLimit) {
					break;
				}

				UtfPackChar08(pDst, dstNum, a);
			}

			// Swap the current <b> into <a> for the next composition pass.
			a = b;

			// Special case: if we just wrote '\0' into <a>, we have nothing
			// more to process.
			if ('\0' == b) {
				break;
			}

			// Pre-load the next value into <b>.  Don't stop if we're at the
			// end of the source string, since the value now in <a> may still
			// need to have a substitution applied to it.
			b = UtfNextChar(pSrc, srcNum);
		}
	}

	pDst[dstNum] = '\0';

	return dstNum;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfComposeASCII()
//
//	This doesn't really compose characters.  However, it does eliminate any
//	symbols above 0x7F to avoid producing corrupted UTF-8 sequences.
//
U32 UtfComposeASCII(Utf08_t pDst[], U32 dstLimit, const char pSrc[])
{
	if (NULL == pSrc) {
		pDst[0] = '\0';
		return 0;
	}

	--dstLimit;

	U32 srcNum = 0;
	U32 dstNum = 0;

	// This loop assumes that we're only dealing with 7-bit ASCII.
	// Any 8-bit ASCII values will map to multi-byte sequences, but that
	// case is handled explicitly within the loop.
	while (dstNum < dstLimit) {
		U08 a = U08(pSrc[srcNum++]);

		if ('\0' == a) {
			break;
		}

		// Assume that most values are 7-bit ASCII.  Those we can just poke
		// right into the destination buffer, since they don't require any
		// remapping or multi-byte packing.
		//
		// But ignore carriage returns.  Those are not needed.
		//
		if (a <= 0x7F) {
			if ('\r' != a) {
				pDst[dstNum++] = a;
			}
		}

		// Otherwise we have an 8-bit value, which needs to be mapped into
		// Unicode space and converted into a UTF-8 multi-byte sequence.
		else {
			Utf32_t mapped = UtfMapASCII(a);

			// Check for zero.  This indicates that the symbol is not
			// currently supported.
			if (0 != mapped) {

				// Make certain that appending this symbol won't overflow the
				// buffer.  An 8-bit ASCII value will map to values that can
				// require 1, 2, or sometimes 3 bytes.
				if ((dstNum + UtfSymbolSize32(mapped)) > dstLimit) {
					break;
				}

				UtfPackChar08(pDst, dstNum, mapped);
			}
		}
	}

	pDst[dstNum] = '\0';

	return dstNum;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfNormalize08to32()
//
//	Normalizes UTF-8 to UTF-32 format.  This should break all composed
//	symbols into a sequence of separate symbols for each composed element.
//	At least that's what the Unicode standard insists upon.  However, this
//	is optional.  For European languages, all of the composed symbols are
//	commonly used, so normalization is not necessary.
//
//	The only thing this code really needs to deal with are stand-alone
//	diacritics, which should follow spaces.  The spaces were removed when
//	composing (which is technically illegal, but makes internal processing
//	of strings easier), so we need to re-insert them when normalizing.
//
U32 UtfNormalize08to32(Utf32_t pDst[], U32 dstLimit, const Utf08_t pSrc[])
{
	U32 srcNum = 0;
	U32 dstNum = 0;

	// We're not going to waste the effort to fully normalize all symbols.
	// The only ones that could be a problem are stand-alone diacritics,
	// since they should follow a space.  Composing the input would have
	// stripped out the spaces to simplify font rendering.  We need to
	// reinsert the spaces before diacritics before writing the output.
	while ((dstNum + 1) < dstLimit) {
		Utf32_t symbol = UtfNextChar(pSrc, srcNum);

		if ('\0' == symbol) {
			break;
		}

		if ((0x0300 <= symbol) && (symbol <= 0x036F)) {
			pDst[dstNum++] = 0x20;
		}

		pDst[dstNum++] = Utf16_t(symbol);
	}

	pDst[dstNum] = '\0';

	return dstNum;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfNormalize08to16()
//
//	Normalizes UTF-8 to UTF-16 format.  This should break all composed
//	symbols into a sequence of separate symbols for each composed element.
//	At least that's what the Unicode standard insists upon.  However, this
//	is optional.  For European languages, all of the composed symbols are
//	commonly used, so normalization is not necessary.
//
//	The only thing this code really needs to deal with are stand-alone
//	diacritics, which should follow spaces.  The spaces were removed when
//	composing (which is technically illegal, but makes internal processing
//	of strings easier), so we need to re-insert them when normalizing.
//
U32 UtfNormalize08to16(Utf16_t pDst[], U32 dstLimit, const Utf08_t pSrc[])
{
	U32 srcOffset = 0;
	U32 dstOffset = 0;

	// We're not going to waste the effort to fully normalize all symbols.
	// The only ones that could be a problem are stand-alone diacritics,
	// since they should follow a space.  Composing the input would have
	// stripped out the spaces to simplify font rendering.  We need to
	// reinsert the spaces before diacritics before writing the output.
	while ((dstOffset + 1) < dstLimit) {
		Utf32_t symbol = UtfNextChar(pSrc, srcOffset);

		if ('\0' == symbol) {
			break;
		}

		// Prepend a space before each diacritic.
		if ((0x0300 <= symbol) && (symbol <= 0x036F)) {
			// Avoid overflow in the case where there is only enough room for
			// the ' ' and the diacritic, but not the final '\0'.
			if ((dstOffset + 2) < dstLimit) {
				pDst[dstOffset++] = 0x20;
				pDst[dstOffset++] = Utf16_t(symbol);
			}
			else {
				break;
			}
		}
		else if (symbol <= 0xFFFF) {
			pDst[dstOffset++] = Utf16_t(symbol);
		}
		else if ((dstOffset + 2) < dstLimit) {
			pDst[dstOffset++] = Utf16_t(symbol >> 10)    + 0xD800 - 0x0040;
			pDst[dstOffset++] = Utf16_t(symbol & 0x03FF) + 0xDC00;
		}
		else {
			break;
		}
	}

	pDst[dstOffset] = '\0';

	return dstOffset;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfNormalize08to08()
//
//	Normalizes a UTF-8 string.  This should break all composed symbols into a
//	sequence of separate symbols for each composed element.  At least that's
//	what the Unicode standard insists upon.  However, this is optional.  For
//	European languages, all of the composed symbols are commonly used, so
//	normalization is not necessary.
//
//	The only thing this code really needs to deal with are stand-alone
//	diacritics, which should follow spaces.  The spaces were removed when
//	composing (which is technically illegal, but makes internal processing
//	of strings easier), so we need to re-insert them when normalizing.
//
U32 UtfNormalize08to08(Utf08_t pDst[], U32 dstLimit, const Utf08_t pSrc[])
{
	U32 srcNum = 0;
	U32 dstNum = 0;

	// We're not going to waste the effort to fully normalize all symbols.
	// The only ones that could be a problem are stand-alone diacritics,
	// since they should follow a space.  Composing the input would have
	// stripped out the spaces to simplify font rendering.  We need to
	// reinsert the spaces before diacritics before writing the output.
	// The only diacritics we support are 0x0300 through 0x0331, all of
	// which start with the byte 0xCC.  And all we need to do is insert a
	// single byte denoting a space before each 0xCC we see.
	//
	// Note that this only covers diacritics betwen 0x0300 and 0x33F, since
	// those between 0x0340 and 0x36F will start with 0xCD.  But none of
	// those diacritics are used.  And besides, DevStudio's debugger anytime
	// it sees 0xCD being used for anything.
	//
	while (('\0' != pSrc[srcNum]) && ((dstNum + 1) < dstLimit)) {
		if (0xCC == pSrc[srcNum]) {
			pDst[dstNum++] = 0x20;
		}

		pDst[dstNum++] = pSrc[srcNum++];
	}

	pDst[dstNum] = '\0';

	return dstNum;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfCanonicalDecompose08to08()
//
//	This is a strict version of UtfNormalize08to08().  It should be fully
//	compliant with the rules of canonical decomposition, such that all
//	diacritics are decomposed, not just the stand-alone ones that are
//	handled by UtfNormalize08to08().
//
//	This code is not especially efficient.  However, canonical decomposition
//	is a rare operation, one that is not intended for normal internal use.
//	So we don't need to worry about being efficient.
//
U32 UtfCanonicalDecompose08to08(Utf08_t pDst[], U32 dstLimit, const Utf08_t pSrc[])
{
	U32 srcNum = 0;
	U32 dstNum = 0;

	--dstLimit;

	Utf32_t symbol[3];
	bool    keepLooping = true;

	while (keepLooping) {
		symbol[0] = UtfNextChar(pSrc, srcNum);
		symbol[1] = 0;
		symbol[2] = 0;

		// Only symbols requiring 8 or more bits need to be decomposed.
		if (symbol[0] >= 128) {
			// All 8-bit symbols need to be tested for decomposition, which
			// can result in splitting a symbol into up to three components.
			// Strictly speaking, this could produce more than three output
			// values for some symbols, but none of those symbols are
			// supported, since no symbol with more than two diacritics is
			// handled by UtfData.cpp.
			UtfDecomposeChar(symbol[0], symbol[0], symbol[1], symbol[2]);
		}
		else if (0 == symbol[0]) {
			break;
		}

		// Append all three of the new symbols to the output string.  For
		// the expected usage, there will seldom be more than one symbol, so
		// we can break out of the loop when we see the first zero.
		for (U32 i = 0; i < 3; ++i) {
			if (0 == symbol[i]) {
				break;
			}

			if ((dstNum + UtfSymbolSize32(symbol[i])) > dstLimit) {
				keepLooping = false;
				break;
			}

			UtfPackChar08(pDst, dstNum, symbol[i]);
		}
	}

	pDst[dstNum] = '\0';

	return dstNum;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfConvert32to08()
//
//	This assumes that the UTF-32 string has already been scanned for illegal
//	unicode values, so it will not do any extra sanity checking of codes
//	beyond ignoring values above 0x10FFFF.
//
//	Guarantees that the string will always be '\0' terminated.
//
//	Returns the number of bytes written to the destination buffer, not
//	counting the '\0'.
//
U32 UtfConvert32to08(Utf08_t pDst[], U32 dstLimit, const Utf32_t pSrc[])
{
	if ((NULL == pDst) || (dstLimit < 1)) {
		return 0;
	}

	if (NULL == pSrc) {
		pDst[0] = '\0';
		return 0;
	}

	U32 srcNum = 0;
	U32 dstNum = 0;

	while ((dstNum + 4) < dstLimit) {
		Utf32_t value = pSrc[srcNum++];
		if ('\0' == value) {
			break;
		}

		UtfPackChar08(pDst, dstNum, value);
	}

	pDst[dstNum] = 0;

	return dstNum;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfConvert08to32()
//
//	Guarantees that the string will always be '\0' terminated.
//
//	Returns the number of bytes written to the destination buffer, not
//	counting the '\0'.
//
U32 UtfConvert08to32(Utf32_t pDst[], U32 dstLimit, const Utf08_t pSrc[])
{
	if ((NULL == pDst) || (dstLimit < 1)) {
		return 0;
	}

	if (NULL == pSrc) {
		pDst[0] = '\0';
		return 0;
	}

	U32 srcNum = 0;
	U32 dstNum = 0;

	while ((dstNum + 1) < dstLimit) {
		Utf32_t b0     = pSrc[srcNum++];
		Utf32_t result = Unicode_UnknownSymbol;

		if (b0 <= 0x7F) {
			result = b0;
		}
		else if (b0 <= 0xDF) {
			Utf08_t b1 = pSrc[srcNum++];

			if (IsMiddleByte(b1)) {
				result = ((b0 & 0x1F) << 6) | U32(b1 & 0x3F);
			}

			// Stop if we encountered the end-of-string in the middle of the
			// symbol.  Discard the partial symbol.
			else if ('\0' == b1) {
				break;
			}

			// Any other value falls through with Unicode_UnknownSymbol as
			// the value that gets written for this symbol.
		}
		else if (b0 <= 0xEF) {
			Utf08_t b1 = pSrc[srcNum++];
			Utf08_t b2 = pSrc[srcNum++];

			if (IsMiddleByte(b1) && IsMiddleByte(b2)) {
				result = (U32(b0 & 0x0F) << 12) | (U32(b1 & 0x3F) << 6) | U32(b2 & 0x3F);
			}

			// Stop if we encountered the end-of-string in the middle of the
			// symbol.  Discard the partial symbol.
			else if (('\0' == b1) || ('\0' == b2)) {
				break;
			}

			// Any other value falls through with Unicode_UnknownSymbol as
			// the value that gets written for this symbol.
		}
		else if (b0 <= 0xF7) {
			Utf08_t b1 = pSrc[srcNum++];
			Utf08_t b2 = pSrc[srcNum++];
			Utf08_t b3 = pSrc[srcNum++];

			if (IsMiddleByte(b1) && IsMiddleByte(b2) && IsMiddleByte(b3)) {
				result = (U32(b0 & 0x07) << 18) | (U32(b1 & 0x3F) << 12) | (U32(b2 & 0x3F) << 6) | U32(b3 & 0x3F);
			}

			// Stop if we encountered the end-of-string in the middle of the
			// symbol.  Discard the partial symbol.
			else if (('\0' == b1) || ('\0' == b2) || ('\0' == b3)) {
				break;
			}

			// Any other value falls through with Unicode_UnknownSymbol as
			// the value that gets written for this symbol.
		}

		if ('\0' == result) {
			break;
		}
		else if (UtfIsValid(result)) {
			pDst[dstNum++] = result;
		}
		else {
			pDst[dstNum++] = Unicode_UnknownSymbol;
		}
	}

	pDst[dstNum] = 0;

	return dstNum;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfConvert32to16()
//
//	Guarantees that the string will always be '\0' terminated.
//
//	Returns the number of bytes written to the destination buffer, not
//	counting the '\0'.
//
U32 UtfConvert32to16(Utf16_t pDst[], U32 dstLimit, const Utf32_t pSrc[])
{
	if ((NULL == pDst) || (dstLimit < 1)) {
		return 0;
	}

	if (NULL == pSrc) {
		pDst[0] = '\0';
		return 0;
	}

	U32 srcNum = 0;
	U32 dstNum = 0;

	while ((dstNum + 2) < dstLimit) {
		Utf32_t value = pSrc[srcNum++];

		if ('\0' == value) {
			break;
		}

		if (value <= 0xFFFF) {
			pDst[dstNum++] = Utf16_t(value);
		}
		else {
			pDst[dstNum++] = Utf16_t(value >> 10)    + 0xD800 - 0x0040;
			pDst[dstNum++] = Utf16_t(value & 0x03FF) + 0xDC00;
		}
	}

	pDst[dstNum] = 0;

	return dstNum;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfConvert16to32()
//
//	Guarantees that the string will always be '\0' terminated.
//
//	Returns the number of bytes written to the destination buffer, not
//	counting the '\0'.
//
U32 UtfConvert16to32(Utf32_t pDst[], U32 dstLimit, const Utf16_t pSrc[])
{
	if ((NULL == pDst) || (dstLimit < 1)) {
		return 0;
	}

	if (NULL == pSrc) {
		pDst[0] = '\0';
		return 0;
	}

	U32 srcNum = 0;
	U32 dstNum = 0;

	while ((dstNum + 1) < dstLimit) {
		Utf32_t value = pSrc[srcNum++];

		if ('\0' == value) {
			break;
		}

		// Most 16-bit values map directly.
		if ((value < Unicode_FirstSurrogate) || (value > Unicode_LastSurrogate)) {
			pDst[dstNum++] = value;
		}

		// But special handling is required for surrogates, which have the
		// 21-bit Unicode value split up across two UTF-16 values.
		//
		// But we need to verify that the current symbol is the first of a
		// surrogate pair, and that the next symbol is the second half.  Only
		// then is it safe to mux their bits together to produce the UTF-32
		// code value.
		//
		else if ((value < 0xDC00) && (pSrc[srcNum] >= 0xDC00) && (pSrc[srcNum] <= Unicode_LastSurrogate)) {
			// Note the trick with 0xD7C0: This really is 0xD800, but the
			// high word is stored with -1 to shift the 1-16 range down to
			// 0-15 so it will fit in 4 bits instead of 5.  Factoring that
			// extra adjustment in, we can use 0xD7C0 instead of 0xD800.
			pDst[dstNum++] = ((value - 0xD7C0) << 10) | Utf32_t(pSrc[srcNum++] & 0x03FF);
		}

		// Anything else that sort-of looks like a surrogate will be ignored.
		// We either have garbage input, or the buffer started with the
		// second half of a surrogate pair.
		//
		// Inject an invalid symbol marker into the string in place of
		// whatever garbage was in the source buffer.
		//
		else {
			pDst[dstNum++] = Unicode_UnknownSymbol;
		}
	}

	pDst[dstNum] = 0;

	return dstNum;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfConvert16to08()
//
//	There is no direct mapping between UTF-8 and UTF-16.  Converting between
//	them effectively requires converting to UTF-32, then from there to the
//	output format.
//
U32 UtfConvert16to08(Utf08_t pDst[], U32 dstLimit, const Utf16_t pSrc[])
{
	if ((NULL == pDst) || (dstLimit < 1)) {
		return 0;
	}

	if (NULL == pSrc) {
		pDst[0] = '\0';
		return 0;
	}

	U32 srcNum = 0;
	U32 dstNum = 0;

	while ((dstNum + 4) < dstLimit) {
		Utf32_t value = pSrc[srcNum++];

		if ('\0' == value) {
			break;
		}

		// The special case is for surrogates, which split up a Unicode
		// symbol across two UTF-16 symbols.  If the value is in this range,
		// it may be either the first or second symbol in a surrogate pair.
		if ((value >= Unicode_FirstSurrogate) && (value <= Unicode_LastSurrogate)) {
			if ((value < 0xDC00) && (pSrc[srcNum] >= 0xDC00) && (pSrc[srcNum] <= Unicode_LastSurrogate)) {
				// Note the trick with 0xD7C0: This really is 0xD800, but the
				// high word is stored with -1 to shift the 1-16 range down to
				// 0-15 so it will fit in 4 bits instead of 5.  Factoring that
				// extra adjustment in, we can use 0xD7C0 instead of 0xD800.
				value = ((value - 0xD7C0) << 10) | Utf32_t(pSrc[srcNum++] & 0x03FF);
			}

			// Otherwise, we ignore stray second symbols.
		}

		// Now we can convert the UTF-32 value to UTF-8 format.
		UtfPackChar08(pDst, dstNum, value);

		// No other value should possibly exist, since we're coming from
		// UTF-16 format, and the surrogate mechanism places a hard limit
		// on the max possible code value.
	}

	pDst[dstNum] = 0;

	return dstNum;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfConvert08to16()
//
//	There is no direct mapping between UTF-8 and UTF-16.  Converting between
//	them effectively requires converting to UTF-32, then from there to the
//	output format.
//
//	Returns the number of Utf16_t values stored in pDst[], not counting the
//	terminator.
//
U32 UtfConvert08to16(Utf16_t pDst[], U32 dstLimit, const Utf08_t pSrc[])
{
	if ((NULL == pDst) || (dstLimit < 1)) {
		return 0;
	}

	if (NULL == pSrc) {
		pDst[0] = '\0';
		return 0;
	}

	U32 srcNum = 0;
	U32 dstNum = 0;

	while ((dstNum + 1) < dstLimit) {
		Utf32_t b0     = pSrc[srcNum++];
		Utf32_t result = Unicode_UnknownSymbol;

		if (b0 <= 0x7F) {
			result = b0;
		}
		else if (b0 <= 0xDF) {
			Utf08_t b1 = pSrc[srcNum++];

			if (IsMiddleByte(b1)) {
				result = ((b0 & 0x1F) << 6) | U32(b1 & 0x3F);
			}

			// Stop if we encountered the end-of-string in the middle of the
			// symbol.  Discard the partial symbol.
			else if ('\0' == b1) {
				break;
			}

			// Any other value falls through with Unicode_UnknownSymbol as
			// the value that gets written for this symbol.
		}
		else if (b0 <= 0xEF) {
			Utf08_t b1 = pSrc[srcNum++];
			Utf08_t b2 = pSrc[srcNum++];

			if (IsMiddleByte(b1) && IsMiddleByte(b2)) {
				result = (U32(b0 & 0x0F) << 12) | (U32(b1 & 0x3F) << 6) | U32(b2 & 0x3F);
			}

			// Stop if we encountered the end-of-string in the middle of the
			// symbol.  Discard the partial symbol.
			else if (('\0' == b1) || ('\0' == b2)) {
				break;
			}

			// Any other value falls through with Unicode_UnknownSymbol as
			// the value that gets written for this symbol.
		}
		else if (b0 <= 0xF7) {
			Utf08_t b1 = pSrc[srcNum++];
			Utf08_t b2 = pSrc[srcNum++];
			Utf08_t b3 = pSrc[srcNum++];

			if (IsMiddleByte(b1) && IsMiddleByte(b2) && IsMiddleByte(b3)) {
				result = (U32(b0 & 0x07) << 18) | (U32(b1 & 0x3F) << 12) | (U32(b2 & 0x3F) << 6) | U32(b3 & 0x3F);
			}

			// Stop if we encountered the end-of-string in the middle of the
			// symbol.  Discard the partial symbol.
			else if (('\0' == b1) || ('\0' == b2) || ('\0' == b3)) {
				break;
			}

			// Any other value falls through with Unicode_UnknownSymbol as
			// the value that gets written for this symbol.
		}

		if ('\0' == result) {
			break;
		}
		else if (UtfIsValid(result)) {
			if (result <= 0xFFFF) {
				pDst[dstNum++] = Utf16_t(result);
			}
			else {
				pDst[dstNum++] = Utf16_t(result >> 10)    + 0xD800 - 0x0040;
				pDst[dstNum++] = Utf16_t(result & 0x03FF) + 0xDC00;
			}
		}
	}

	pDst[dstNum] = 0;

	return dstNum;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfCopy()
//
U32 UtfCopy(Utf32_t pDst[], U32 dstLimit, const Utf32_t pSrc[])
{
	if ((NULL == pDst) || (dstLimit < 1)) {
		return 0;
	}

	if (NULL == pSrc) {
		pDst[0] = 0;
		return 0;
	}

	U32 offset = 0;
	--dstLimit;

	while ((offset < dstLimit) && (0 != pSrc[offset])) {
		pDst[offset] = pSrc[offset];
		++offset;
	}

	pDst[offset] = '\0';

	return offset;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfCopy()
//
//	Copies a UTF-8 string from one buffer to another.  This will guarantee
//	that the destination buffer does not overflow, it always '\0' terminated,
//	and that the string ends with a whole UTF-8 byte sequence.
//
//	No additional symbol validation is performed, so bad or illegal symbols
//	in the middle of the string will not be detected.
//
//	Returns the total number of bytes written to the destination buffer,
//	not counting the '\0'.
//
U32 UtfCopy(Utf08_t pDst[], U32 dstLimit, const Utf08_t pSrc[])
{
	if (NULL == pDst) {
		return 0;
	}

	if (NULL == pSrc) {
		pDst[0] = '\0';
		return 0;
	}

	if (0 == dstLimit) {
		return 0;
	}

	if (1 == dstLimit) {
		pDst[0] = '\0';
		return 0;
	}

	// Knock one off of the size of the buffer so the loop will always stop
	// with space at the end for a terminator.
	--dstLimit;

	// Copy bytes until we fill the destination buffer, or we find the end
	// of the source string.
	U32 length = 0;
	while (length < dstLimit) {
		if ('\0' == pSrc[length]) {
			break;
		}

		pDst[length] = pSrc[length];
		++length;
	}

	// Protect against partial UTF-8 byte sequences.  Start scanning
	// backwards until we find the start of a byte sequence.  This should
	// need to look no more than 4 bytes into the past to find the final
	// valid UTF-8 code value.  If we manage to hit the start of the buffer,
	// the final length will default to an empty string.
	U32 finalLength = 0;
	for (U32 backscan = length; backscan > 0; --backscan) {
		U32 symbolSize = UtfSymbolSize08(pDst[backscan-1]);

		if (symbolSize > 0) {
			// If the last symbol in the buffer should run past the end of
			// the buffer, we need to discard the last symbol.
			if ((backscan - 1 + symbolSize) > length) {
				finalLength = backscan - 1;
			}

			// Otherwise reset the length to end just past the last symbol.
			// This will truncate the string if it contains stray bytes that
			// have the high bit set.
			else {
				finalLength = backscan - 1 + symbolSize;
			}

			break;
		}
	}

	// Guarantee that the string is always terminated.
	pDst[finalLength] = '\0';

	return finalLength;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfAppend()
//
//	Appends the source string to the end of the destination buffer.  The
//	result will be truncated if the destination buffer is not long enough.
//
//	Returns the total number of bytes in the destination buffer.
//
U32 UtfAppend(Utf08_t pDst[], U32 dstLimit, const Utf08_t pSrc[])
{
	if ((NULL == pDst) || (0 == dstLimit)) {
		return 0;
	}

	U32 offset = 0;

	// Scan for the terminator at the end of the string, being careful to not
	// scan past the end of the buffer.
	while (offset < dstLimit) {
		if ('\0' == pDst[offset]) {
			break;
		}

		++offset;
	}

	// Destination buffer is already full, and we cannot append any more data
	// into it.
	if ('\0' != pDst[offset]) {
		return offset;
	}

	// Use UtfCopy() to perform the actual copy operation, adjusting the
	// buffer settings to skip past the string already contained in the
	// buffer.
	return offset + UtfCopy(pDst + offset, dstLimit - offset, pSrc);
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfAppendSymbol()
//
U32 UtfAppendSymbol(Utf08_t pDst[], U32 dstLimit, Utf32_t symbol)
{
	U32 byteCount = UtfSymbolSize32(symbol);

	if (byteCount <= dstLimit) {
		U32 offset = 0;
		UtfPackChar08(pDst, offset, symbol);

		return byteCount;
	}

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfCompareBytewise()
//
S32 UtfCompareBytewise(const Utf32_t pCA[], const Utf32_t pCB[])
{
	if ((NULL == pCA) || (NULL == pCB)) {
		return -1;
	}

	Utf32_t *pA = const_cast<Utf32_t*>(pCA);
	Utf32_t *pB = const_cast<Utf32_t*>(pCB);

	while ((*pA == *pB) && ('\0' != *pB)) {
		++pA;
		++pB;
	}

	if (*pA < *pB) {
		return -1;
	}

	if (*pA > *pB) {
		return 1;
	}

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfCompareBytewise()
//
//	Does a byte-wise comparison of two UTF-8 strings.  This should be the
//	same as calling memcmp() or strcmp().  It does not honour any encoded
//	values above 0x7F, treating them as individual bytes.
//
//	This is used in sorting routines and comparisons that are never expected
//	to be viewed by human eyes.
//
S32 UtfCompareBytewise(const Utf08_t pCA[], const Utf08_t pCB[])
{
	if ((NULL == pCA) || (NULL == pCB)) {
		return -1;
	}

	Utf08_t *pA = const_cast<Utf08_t*>(pCA);
	Utf08_t *pB = const_cast<Utf08_t*>(pCB);

	while ((*pA == *pB) && ('\0' != *pB)) {
		++pA;
		++pB;
	}

	if (*pA < *pB) {
		return -1;
	}

	if (*pA > *pB) {
		return 1;
	}

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfCompareExact()
//
//	Performs an exact numerical comparison between the two strings.  Really,
//	this should provide the exact same results as UtfCompareBytewise, and
//	is a little slower about doing it.  The only difference between them
//	should occur if one of the strings contains an invalid Unicode symbol,
//	which would result in UtfNextChar() returning a '\0'.
//
S32 UtfCompareExact(const Utf08_t pA[], const Utf08_t pB[])
{
	if ((NULL == pA) || (NULL == pB)) {
		return -1;
	}

	Utf32_t a;
	Utf32_t b;
	U32 offsetA = 0;
	U32 offsetB = 0;

	do {
		a = UtfNextChar(pA, offsetA);
		b = UtfNextChar(pB, offsetB);
	} while ((a == b) && ('\0' != a));

	if (a < b) {
		return -1;
	}

	if (a > b) {
		return 1;
	}

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfCompareNocase()
//
//	This performs a case in-sensitive comparison of the two strings.
//	All characters are mapped to lower-case before comparing.
//
S32 UtfCompareNocase(const Utf08_t pA[], const Utf08_t pB[])
{
	if ((NULL == pA) || (NULL == pB)) {
		return -1;
	}

	Utf32_t a;
	Utf32_t b;
	U32 offsetA = 0;
	U32 offsetB = 0;

	do {
		a = UtfNextChar(pA, offsetA);
		b = UtfNextChar(pB, offsetB);

		UtfSymbolTable_t *pSymA = UtfLookUpSymbol(a);
		UtfSymbolTable_t *pSymB = UtfLookUpSymbol(b);

		if ((NULL != pSymA) &&
			(0 != (UtfFlag_Uppercase & pSymA->Flags)) &&
			(0 != pSymA->AltCase))
		{
			a = pSymA->AltCase;
		}

		if ((NULL != pSymB) &&
			(0 != (UtfFlag_Uppercase & pSymB->Flags)) &&
			(0 != pSymB->AltCase))
		{
			b = pSymB->AltCase;
		}
	} while ((a == b) && ('\0' != a));

	if (a < b) {
		return -1;
	}

	if (a > b) {
		return 1;
	}

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfCompareNocaseLength()
//
//	This performs a case in-sensitive comparison of the two strings.
//	All characters are mapped to lower-case before comparing.
//
S32 UtfCompareNocaseLength(const Utf08_t pA[], const Utf08_t pB[], U32 charCount)
{
	if ((NULL == pA) || (NULL == pB)) {
		return -1;
	}

	Utf32_t a;
	Utf32_t b;
	U32 offsetA = 0;
	U32 offsetB = 0;
	U32 charNum = 0;

	do {
		a = UtfNextChar(pA, offsetA);
		b = UtfNextChar(pB, offsetB);

		UtfSymbolTable_t *pSymA = UtfLookUpSymbol(a);
		UtfSymbolTable_t *pSymB = UtfLookUpSymbol(b);

		if ((NULL != pSymA) &&
			(0 != (UtfFlag_Uppercase & pSymA->Flags)) &&
			(0 != pSymA->AltCase))
		{
			a = pSymA->AltCase;
		}

		if ((NULL != pSymB) &&
			(0 != (UtfFlag_Uppercase & pSymB->Flags)) &&
			(0 != pSymB->AltCase))
		{
			b = pSymB->AltCase;
		}

		++charNum;
	} while ((a == b) && ('\0' != a) && (charNum < charCount));

	if (a < b) {
		return -1;
	}

	if (a > b) {
		return 1;
	}

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfCompareNocaseASCII()
//
//	This performace a case in-sensitive comparison of the two strings.
//	The difference from UtfCompareNocase() is that one string is a 7-bit
//	ASCII character.  If we see any 8-bit, multi-byte UTF-8 symbols, then
//	the string cannot be a match, so the compare will fail.
//	All characters are mapped to lower-case before comparing.
//
S32 UtfCompareNocaseASCII(const char pA[], const Utf08_t pB[])
{
	if ((NULL == pA) || (NULL == pB)) {
		return -1;
	}

	char a;
	char b;
	U32 offsetA = 0;
	U32 offsetB = 0;

	do {
		a = pA[offsetA++];
		b = pB[offsetB++];

		// Fail if we see any multi-byte UTF-8 symbols.
		if (0 != (0x80 & b)) {
			return -1;
		}

		if (('A' <= a) && (a <= 'Z')) {
			a += 'a' - 'A';
		}

		if (('A' <= b) && (b <= 'Z')) {
			b += 'a' - 'A';
		}
	} while ((a == b) && ('\0' != a));

	if (a < b) {
		return -1;
	}

	if (a > b) {
		return 1;
	}

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfCompareLexical()
//
//	This performs a locale-sensitive
//
S32 UtfCompareLexical(const Utf08_t pA[], const Utf08_t pB[])
{
	if ((NULL == pA) || (NULL == pB)) {
		return -1;
	}

	UtfSortEntry_t *pSortA;
	UtfSortEntry_t *pSortB;

	U32 offsetA = 0;
	U32 offsetB = 0;

	U32 langID = UtfGetLanguage();

	for (U32 passNum = 0; passNum < 4; ++passNum) {
		Utf32_t a;
		Utf32_t b;

		// For most languages, we need to scan forwards through the string.
		if ((passNum != 1) || (LangID_French != langID)) {
			// Note: We have to reset the counts here to avoid screwing up
			// the special French-only version of the search.
			offsetA = 0;
			offsetB = 0;

			for (;;) {
				a = UtfNextChar(pA, offsetA);
				b = UtfNextChar(pB, offsetB);

				if (a != b) {
					if ('\0' == a) {
						return -1;
					}
					else if ('\0' == b) {
						return 1;
					}
				}

				// Otherwise if both symbols are zero, we've reached the end of
				// the string.  The strings are the same according to this level
				// of lexical comparison.  Either the strings are an exact match,
				// or we need to test one more level.
				else if ('\0' == a) {
					break;
				}

				pSortA = UtfLookUpSort(a);
				pSortB = UtfLookUpSort(b);

				if (NULL == pSortA) {
					return -1;
				}

				if (NULL == pSortB) {
					return 1;
				}

				if (pSortA->Sort[passNum] < pSortB->Sort[passNum]) {
					return -1;
				}

				if (pSortA->Sort[passNum] > pSortB->Sort[passNum]) {
					return 1;
				}
			}
		}

		// Otherwise we're dealing with French sorting, on the second pass.
		// This should be comparing the weights of equivalent diacritics.
		// For French, we need to sort diacritics right-to-left.
		// So we have to decrement the two offsets to undo the '\0' that
		// was consumed at the end of the previous loop.
		else {
			// We should enter this loop with offsetA and offsetB pointing to
			// the '\0' sentinel where we stopped scanning on the previous
			// iteration.  UtfPrevChar() will then be able to scan backwards
			// to locate previous characters.

			for (;;) {
				a = UtfPrevChar(pA, offsetA);
				b = UtfPrevChar(pB, offsetB);

				if (a != b) {
					if ('\0' == a) {
						return -1;
					}
					else if ('\0' == b) {
						return 1;
					}
				}

				// Otherwise if both symbols are zero, we've reached the end of
				// the string.  The strings are the same according to this level
				// of lexical comparison.  Either the strings are an exact match,
				// or we need to test one more level.
				else if ('\0' == a) {
					break;
				}

				pSortA = UtfLookUpSort(a);
				pSortB = UtfLookUpSort(b);

				if (NULL == pSortA) {
					return -1;
				}

				if (NULL == pSortB) {
					return 1;
				}

				if (pSortA->Sort[passNum] < pSortB->Sort[passNum]) {
					return -1;
				}

				if (pSortA->Sort[passNum] > pSortB->Sort[passNum]) {
					return 1;
				}
			}

			offsetA = 0;
			offsetB = 0;
		}
	}

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfCompareWildcard()
//
//	Tests whether two strings are exactly identical (including case), with
//	the allowance that the second string may contain '?' or '*'.  '?' will
//	map to exactly one symbol being ignored, while '*' matches zero or more
//	symbols.
//
//	NOTE: This implementation only works reliably for at most two '*' in the
//	wildcard string.  If there are more two '*', more elaborate logic is
//	needed to handle cases of duplicate substrings.
//
bool UtfCompareWildcard(const Utf08_t pA[], const Utf08_t pWild[])
{
	if ((NULL == pA) || (NULL == pWild)) {
		return false;
	}

	Utf32_t a = '\0';
	Utf32_t b = '\0';
	U32 limitA  = UtfCharCount(pA);
	U32 limitB  = UtfCharCount(pWild);
	U32 frontA  = 0;
	U32 frontB  = 0;
	U32 backA   = limitA;
	U32 backB   = limitB;

	// Note that in the loop below, we sometimes add +1 or -1 to the offset
	// instead of using UtfNextChar or UtfPrevChar to scan.  This is safe
	// since the code only does this when looking at '*' chars, which are
	// single-byte ASCII values, allowing us to ignore the variable-length
	// UTF-8 codes in this special case.

	for (;;) {
		// Start at the beginning of the string and match symbols until we
		// reach the first '*'.
		while (frontB < backB) {
			b = UtfNextChar(pWild, frontB);

			if ('*' == b) {
				// If there are multiple '*' in a row, consume them.
				while ((frontB < backB) && ('*' == pWild[frontB])) {
					++frontB;
				}
				--frontB;
				break;
			}

			if (frontA >= backA) {
				return false;
			}

			a = UtfNextChar(pA, frontA);

			if (('?' != b) && (a != b)) {
				return false;
			}
		}

		// Start at the end of the string and scan backwards, matching
		// symbols until we reach the last '*'.
		while (frontB < backB) {
			b = UtfPrevChar(pWild, backB);

			if ('*' == b) {
				// If there are multiple '*' in a row, consume them.
				while ((frontB < backB) && ('*' == pWild[backB-1])) {
					--backB;
				}
				++backB;
				break;
			}

			if (frontA >= backA) {
				return false;
			}

			a = UtfPrevChar(pA, backA);

			if (('?' != b) && (a != b)) {
				return false;
			}
		}

		// No '*' were found, and both strings were consumed.  They match
		// without any wildcards.
		if ((frontA == backA) && (frontB == backB)) {
			return true;
		}

		// Both head and tail substrings were consumed, and only a single
		// '*' remains in the wildcard string.  We can ignore whatever
		// remains in string A, which maps to the '*'.
		if ((frontB + 1) == backB) {
			return true;
		}

		// There are two '*' with one or more chars between them.

		// Skip the leading '*'.
		++frontB;

		// Count the number of non-'*' characters in the wildcard pattern
		// that need to be matched.  Since the code above consumed all
		// adjacent '*', there must be one non-'*' between them.
		U32 subLength = 0;
		U32 subOffset = frontB;
		for (;;) {
			b = UtfNextChar(pWild, subOffset);
			if (('\0' == b) || ('*' == b)) {
				--subOffset;
				break;
			}
			++subLength;
		}

		// The number of bytes in the substring.
		U32 byteCountB = subOffset - frontB;

		// Attempt to locate the substring in A.
		while ((frontA + byteCountB) < backA) {
			U32 baseA    = frontA;
			U32 baseB    = frontB;
			U32 mismatch = false;

			for (U32 i = 0; i < subLength; ++i) {
				a = UtfNextChar(pA, baseA);
				b = UtfNextChar(pWild, baseB);

				if (('?' != b) && (a != b)) {
					mismatch = true;
					break;
				}
			}

			// We've located the substring.  Advance the bottom of the search
			// range to the end of the two strings.
			if (false == mismatch) {
				frontA = baseA;
				frontB = baseB;
				break;
			}

			UtfNextChar(pA, frontA);
		}

		// NOTE: There is a logical flaw in the loop above.  If the middle
		// substring occurs more than once, the correct match may be to the
		// second instance of the substring instead of the first.  The loop
		// above only locates the first instance of the substring, so it may
		// fail to match correctly when given complex wildcards with three or
		// more '*' in them.
		//
		// Support for more complex patterns has not been needed, so this
		// version of the code is good enough.

		// There is not enough space in A to contain the substring, so no
		// match is possible.
		if ((frontA + byteCountB) > backA) {
			return false;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfCompareWildcardNocase()
//
//	This is identical to UtfCompareWildcardNocase(), except that all string
//	comparisons are done in lower-case by calling UtfToLower().
//
bool UtfCompareWildcardNocase(const Utf08_t pA[], const Utf08_t pWild[])
{
	if ((NULL == pA) || (NULL == pWild)) {
		return false;
	}

	Utf32_t a = '\0';
	Utf32_t b = '\0';
	U32 limitA  = UtfCharCount(pA);
	U32 limitB  = UtfCharCount(pWild);
	U32 frontA  = 0;
	U32 frontB  = 0;
	U32 backA   = limitA;
	U32 backB   = limitB;

	// Note that in the loop below, we sometimes add +1 or -1 to the offset
	// instead of using UtfNextChar or UtfPrevChar to scan.  This is safe
	// since the code only does this when looking at '*' chars, which are
	// single-byte ASCII values, allowing us to ignore the variable-length
	// UTF-8 codes in this special case.

	for (;;) {
		// Start at the beginning of the string and match symbols until we
		// reach the first '*'.
		while (frontB < backB) {
			b = UtfToLower(UtfNextChar(pWild, frontB));

			if ('*' == b) {
				// If there are multiple '*' in a row, consume them.
				while ((frontB < backB) && ('*' == pWild[frontB])) {
					++frontB;
				}
				--frontB;
				break;
			}

			if (frontA >= backA) {
				return false;
			}

			a = UtfToLower(UtfNextChar(pA, frontA));

			if (('?' != b) && (a != b)) {
				return false;
			}
		}

		// Start at the end of the string and scan backwards, matching
		// symbols until we reach the last '*'.
		while (frontB < backB) {
			b = UtfToLower(UtfPrevChar(pWild, backB));

			if ('*' == b) {
				// If there are multiple '*' in a row, consume them.
				while ((frontB < backB) && ('*' == pWild[backB-1])) {
					--backB;
				}
				++backB;
				break;
			}

			if (frontA >= backA) {
				return false;
			}

			a = UtfToLower(UtfPrevChar(pA, backA));

			if (('?' != b) && (a != b)) {
				return false;
			}
		}

		// No '*' were found, and both strings were consumed.  They match
		// without any wildcards.
		if ((frontA == backA) && (frontB == backB)) {
			return true;
		}

		// Both head and tail substrings were consumed, and only a single
		// '*' remains in the wildcard string.  We can ignore whatever
		// remains in string A, which maps to the '*'.
		if ((frontB + 1) == backB) {
			return true;
		}

		// There are two '*' with one or more chars between them.

		// Skip the leading '*'.
		++frontB;

		// Count the number of non-'*' characters in the wildcard pattern
		// that need to be matched.  Since the code above consumed all
		// adjacent '*', there must be one non-'*' between them.
		U32 subLength = 0;
		U32 subOffset = frontB;
		for (;;) {
			b = UtfToLower(UtfNextChar(pWild, subOffset));
			if (('\0' == b) || ('*' == b)) {
				--subOffset;
				break;
			}
			++subLength;
		}

		// The number of bytes in the substring.
		U32 byteCountB = subOffset - frontB;

		// Attempt to locate the substring in A.
		while ((frontA + byteCountB) < backA) {
			U32 baseA    = frontA;
			U32 baseB    = frontB;
			U32 mismatch = false;

			for (U32 i = 0; i < subLength; ++i) {
				a = UtfToLower(UtfNextChar(pA, baseA));
				b = UtfToLower(UtfNextChar(pWild, baseB));

				if (('?' != b) && (a != b)) {
					mismatch = true;
					break;
				}
			}

			// We've located the substring.  Advance the bottom of the search
			// range to the end of the two strings.
			if (false == mismatch) {
				frontA = baseA;
				frontB = baseB;
				break;
			}

			UtfNextChar(pA, frontA);
		}

		// NOTE: There is a logical flaw in the loop above.  If the middle
		// substring occurs more than once, the correct match may be to the
		// second instance of the substring instead of the first.  The loop
		// above only locates the first instance of the substring, so it may
		// fail to match correctly when given complex wildcards with three or
		// more '*' in them.
		//
		// Support for more complex patterns has not been needed, so this
		// version of the code is good enough.

		// There is not enough space in A to contain the substring, so no
		// match is possible.
		if ((frontA + byteCountB) > backA) {
			return false;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfContainsWildcards()
//
//	Returns true if the string contains any wildcard characters: '?' or '*'.
//
bool UtfContainsWildcards(const Utf08_t pWild[])
{
	if (NULL == pWild) {
		return false;
	}

	U32 offset = 0;

	while ('\0' != pWild[offset]) {
		U32 c = UtfNextChar(pWild, offset);

		if (('?' == c) || ('*' == c)) {
			return true;
		}
	}

	return false;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfPrefix()
//
//	Returns true if <prefix> is a prefix of <strg>.
//	This version is case sensitive.
//
bool UtfPrefix(const Utf08_t prefix[], const Utf08_t strg[])
{
	// Fail the test for emptry strings, otherwise an empty prefix could be
	// considered a prefix of any string.
	if ((NULL == prefix) || ('\0' == prefix[0]) || (NULL == strg)) {
		return false;
	}

	U32 offsetPrefix = 0;
	U32 offsetStrg   = 0;

	Utf32_t p;
	Utf32_t s;

	do {
		p = UtfNextChar(prefix, offsetPrefix);
		s = UtfNextChar(strg, offsetStrg);

		if ('\0' == p) {
			return true;
		}
	} while (s == p);

	return false;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfPrefixNocase()
//
//	Maps all uppercase symbols to lowercase, performing a case-insensitive
//	comparison.
//
bool UtfPrefixNocase(const Utf08_t prefix[], const Utf08_t strg[])
{
	// Fail the test for emptry strings, otherwise an empty prefix could be
	// considered a prefix of any string.
	if ((NULL == prefix) || ('\0' == prefix[0]) || (NULL == strg)) {
		return false;
	}

	U32 offsetPrefix = 0;
	U32 offsetStrg   = 0;

	Utf32_t p;
	Utf32_t s;

	do {
		p = UtfNextChar(prefix, offsetPrefix);
		s = UtfNextChar(strg, offsetStrg);

		if ('\0' == p) {
			return true;
		}

		UtfSymbolTable_t *pSymP = UtfLookUpSymbol(p);
		UtfSymbolTable_t *pSymS = UtfLookUpSymbol(s);

		if ((NULL != pSymP) &&
			(0 != (UtfFlag_Uppercase & pSymP->Flags)) &&
			(0 != pSymP->AltCase))
		{
			p = pSymP->AltCase;
		}

		if ((NULL != pSymS) &&
			(0 != (UtfFlag_Uppercase & pSymS->Flags)) &&
			(0 != pSymS->AltCase))
		{
			s = pSymS->AltCase;
		}
	} while (s == p);

	return false;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfSuffix()
//
bool UtfSuffix(const Utf08_t suffix[], const Utf08_t strg[])
{
	if ((NULL == suffix) || (NULL == strg)) {
		return false;
	}

	U32 suflen = UtfCharCount(suffix);
	U32 length = UtfCharCount(strg);

	if (0 == suflen) {
		return false;
	}

	if (suflen > length) {
		return false;
	}

	U32 offsetSuffix = 0;
	U32 offsetStrg   = 0;

	// Skip the leading symbols in the string.  We only need to compare the
	// last sequence of them.
	for (U32 i = 0; i < (length - suflen); ++i) {
		UtfNextChar(strg, offsetStrg);
	}

	for (U32 i = 0; i < suflen; ++i) {
		Utf32_t a = UtfNextChar(suffix, offsetSuffix);
		Utf32_t b = UtfNextChar(strg, offsetStrg);

		UtfSymbolTable_t *pSymA = UtfLookUpSymbol(a);
		UtfSymbolTable_t *pSymB = UtfLookUpSymbol(b);

		if ((NULL != pSymA) &&
			(0 != (UtfFlag_Uppercase & pSymA->Flags)) &&
			(0 != pSymA->AltCase))
		{
			a = pSymA->AltCase;
		}

		if ((NULL != pSymB) &&
			(0 != (UtfFlag_Uppercase & pSymB->Flags)) &&
			(0 != pSymB->AltCase))
		{
			b = pSymB->AltCase;
		}

		if (a != b) {
			return false;
		}
	}

	return true;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfSuffixNocase()
//
bool UtfSuffixNocase(const Utf08_t suffix[], const Utf08_t strg[])
{
	if ((NULL == suffix) || (NULL == strg)) {
		return false;
	}

	U32 suflen = UtfCharCount(suffix);
	U32 length = UtfCharCount(strg);

	if (0 == suflen) {
		return false;
	}

	if (suflen > length) {
		return false;
	}

	U32 offsetSuffix = 0;
	U32 offsetStrg   = 0;

	// Skip the leading symbols in the string.  We only need to compare the
	// last sequence of them.
	for (U32 i = 0; i < (length - suflen); ++i) {
		UtfNextChar(strg, offsetStrg);
	}

	for (U32 i = 0; i < suflen; ++i) {
		Utf32_t a = UtfNextChar(suffix, offsetSuffix);
		Utf32_t b = UtfNextChar(strg, offsetStrg);

		if (a != b) {
			return false;
		}
	}

	return true;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfLocalizedFromS32()
//
//	Converts an integer to string format, embedding commas to separate the
//	thousands.  This accesses the localization info to use the appropriate
//	separator for the current language.
//
//	This implementation ignores that some languages put commas (or decimals)
//	only two digits apart instead of three.  That requires more extensive
//	locale information.
//
U32 UtfLocalizedFromS32(Utf08_t buffer[], S32 value)
{
	// Fetch the locale-specific separator.
	Utf08_t thousands = UtfGetThousands();

	if (0 == value) {
		buffer[0] = '0';
		buffer[1] = '\0';
		return 1;
	}

	bool negative = false;
	if (value < 0) {
		negative = true;
		value    = -value;
	}

	Utf08_t digits[32];
	U32     tally    = 0;
	U32     digitNum = 0;

	// Strip off the digits and push them onto a stack.
	while (0 != value) {
		digits[tally++] = Utf08_t((value % 10) + '0');
		value /= 10;

		// Periodically insert a separator to split up the number into
		// thousands so it's easier to read.
		if (2 == (digitNum++ % 3)) {
			if (0 != value) {
				digits[tally++] = thousands;
			}
		}
	}

	if (negative) {
		digits[tally++] = '-';
	}

	// Pop the digits off the stack to append them onto the string in the
	// correct order.
	for (U32 i = 0; i < tally; ++i) {
		buffer[i] = digits[tally-i-1];
	}

	buffer[tally] = '\0';

	return tally;
}


/////////////////////////////////////////////////////////////////////////////
//
//	S32ToString()
//
//	Converts a signed integer into a string.  This will prepend a '-' to the
//	number if the value is negative.
//
//	Returns the number of bytes written into the buffer.
//
U32 UtfFromS32(Utf08_t buffer[], S32 value)
{
	if (0 == value) {
		buffer[0] = '0';
		buffer[1] = '\0';
		return 1;
	}

	bool negative = false;
	if (value < 0) {
		negative = true;
		value    = -value;
	}

	Utf08_t digits[32];
	U32     tally = 0;

	// Strip off the digits and push them onto a stack.
	while (0 != value) {
		digits[tally++] = Utf08_t((value % 10) + '0');
		value /= 10;
	}

	if (negative) {
		digits[tally++] = '-';
	}

	// Pop the digits off the stack to append them onto the string in the
	// correct order.
	for (U32 i = 0; i < tally; ++i) {
		buffer[i] = digits[tally-i-1];
	}

	buffer[tally] = '\0';

	return tally;
}


/////////////////////////////////////////////////////////////////////////////
//
//	U32ToString()
//
//	Converts an unsigned integer to a string.
//
//	Returns the number of bytes written into the buffer.
//
U32 UtfFromU32(Utf08_t buffer[], U32 value)
{
	if (0 == value) {
		buffer[0] = '0';
		buffer[1] = '\0';
		return 1;
	}

	Utf08_t digits[32];
	U32     tally = 0;

	// Strip off the digits and push them onto a stack.
	while (0 != value) {
		digits[tally++] = Utf08_t((value % 10) + '0');
		value /= 10;
	}

	// Pop the digits off the stack to append them onto the string in the
	// correct order.
	for (U32 i = 0; i < tally; ++i) {
		buffer[i] = digits[tally-i-1];
	}

	buffer[tally] = '\0';

	return tally;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfFromHex()
//
//	Converts an unsigned integer to a string.
//
//	Returns the number of bytes written into the buffer.
//
U32 UtfFromHex(Utf08_t buffer[], U32 value)
{
	if (0 == value) {
		buffer[0] = '0';
		buffer[1] = '\0';
		return 1;
	}

	Utf08_t digits[32];
	U32     tally = 0;

	// Strip off the digits and push them onto a stack.
	while (0 != value) {
		U32 digit = value & 0xF;

		if (digit < 10) {
			digits[tally++] = Utf08_t('0' + digit);
		}
		else {
			digits[tally++] = Utf08_t(digit - 10 + 'A');
		}

		value = value >> 4;
	}

	// Pop the digits off the stack to append them onto the string in the
	// correct order.
	for (U32 i = 0; i < tally; ++i) {
		buffer[i] = digits[tally-i-1];
	}

	buffer[tally] = '\0';

	return tally;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfFromFloat()
//
//	This converts the float to a string using two floating-point conversions.
//	To keep integer numerology within 32 bits, the fractional precision is
//	limited to 8 digits.  This could be extended to 9, but this leaves enough
//	bits for one extra decimal in overflow.  Most probably not needed, but
//	play it safe until there's a need for more precision.
//
//	If precision is set to zero, the string will contain only the whole value
//	of the string, without even a decimal point.
//
//	Returns the number of bytes written into the buffer.
//
U32 UtfFromFloat(Utf08_t buffer[], float value, U32 precision)
{
	Utf08_t digits[32];
	U32     tally = 0;

	if (precision > 8) {
		precision = 8;
	}

#if 1

	// This part is ugly, but we need a scaler that shifts the desired part
	// of the fraction into an integer, so we can properly bias the rounding.
	// Note that the scaler is 1 if the precision is zero, since we won't be
	// needing any digits after the decimal.
	U32 scaler = 1;
	for (U32 i = 0; i < precision; ++i) {
		scaler *= 10;
	}

	// Convert the float into a double-precision number, which is scaled so
	// the desired fractional portion is part of the whole number.
	double d = double(value) * double(scaler);

	// Convert the double to a 64-bit integer, using the correct bias value
	// to round away from zero.  Withouth the bias, all values from -0.999...
	// to +0.999... would be truncated to zero, introducing error into the
	// resulting value.
	S64 whole = (d >= 0.0) ? S64(d + 0.5) : S64(d - 0.5);

	// Make certain the value is positive for the next chunk of logic.  We
	// will push the negative sign onto the stack when we're done.
	bool negative = false;
	if (whole < 0) {
		whole    = -whole;
		negative = true;
	}

	// If any digits after the decimal are required, push them onto the stack
	// first, then push a '.' onto the stack.
	//
	// NOTE: This code does not use the language-specific decimal character,
	// since the string produced by this function will be used in files with
	// standardized formats, and must be parsed back by the engine.  The
	// language-specific decimal and thousands characters should only used
	// for strings intended to be displayed in the user interface.
	//
	if (precision > 0) {
		for (U32 i = 0; i < precision; ++i) {
			digits[tally++] = Utf08_t((whole % 10) + '0');
			whole /= 10;
		}

		digits[tally++] = '.';
	}

	// Now push the whole digits onto the stack.  Special logic is required
	// to make certain a '0' is pushed when dealing with a value < 1.0,
	// since the while() loop won't push anything in that case.
	if (0 != whole) {
		while (whole > 0) {
			digits[tally++] = Utf08_t((whole % 10) + '0');
			whole /= 10;
		}
	}
	else {
		digits[tally++] = '0';
	}

#else

	double d = value;

	// All of the remaining logic assumes that it is dealing with a positive
	// value.  If not, discard the sign bit and set a flag so we can push a
	// negative sign on the stack when we're done converting to text.
	bool negative = false;
	if (d < 0.0f) {
		d        = -d;
		negative = true;
	}

	// This part is ugly, but we need a scaler that shifts the desired part
	// of the fraction into an integer, so we can properly bias the rounding.
	// Note that the scaler is wrong if the precision is zero, but in that
	// case we won't be using it.
	U32 scaler = 10;
	for (U32 i = 1; i < precision; ++i) {
		scaler *= 10;
	}

	// Convert the float into two integers, one for the whole part and one
	// for the fraction.  Since float-to-int conversion is done by using
	// truncation, we truncate the value, subtract that off, and add a bias
	// to round up.
	U32 whole = (precision > 0) ? U32(d) : U32(d + 0.5);
	S32 frac  = S32(((d - double(whole)) * scaler) + 0.5);

	// Overflow test.  If the float is so large that the integer value has
	// overflowed, we can detect this by subtracting off the integer from the
	// original float.  If the result is more than a bit larger than 1.0 (due
	// to precision slop), we've got overflow and need to reset the value to
	// safe limits.
	//
	// Note: We could probably test this against double(0x7FFFFFFF), but
	// we don't know what kind of conditioning the float-to-int conversion
	// has under the hood.  It's probably defined in IEEE, but that spec is
	// not at hand, and C/C++ typically pick non-default options for various
	// settings.
	//
	if ((d - double(whole)) > 1.001) {
		whole = 999999999;
		frac  = scaler - 1;
	}

	// This only needs to be processed if we want digits after the decimal
	// point.  It will push the fraction digits onto the stack first (since
	// we have to process digits right-to-left), followed by the decimal
	// point.
	if (precision > 0) {
		// Clamp the range.  It's possible there were a few bits of overflow
		// or underflow when subtracting off the whole part and scaling.
		frac = ClampRange(0, frac, S32(scaler - 1));

		// Do not display the negative sign if the value is zero (at the
		// precision at which it is being stringified).  This can happen
		// because either the number really is just a bit below zero, or
		// because floats really do allow -0.0 to be a legit number.
		if ((0 == whole) && (0 == frac)) {
			negative = false;
		}

		for (U32 i = 0; i < precision; ++i) {
			digits[tally++] = Utf08_t((frac % 10) + '0');
			frac /= 10;
		}

		digits[tally++] = '.';
	}

	// Otherwise we are only printing the whole part of the number.
	// Here we need to avoid printing "-0", which doesn't make sense.
	else if (0 == whole) {
		negative = false;
	}

	// Strip off the digits and push them onto a stack.
	// Make certain that we have a leading zero if the value is less than 1.
	if (0 == whole) {
		digits[tally++] = '0';
	}
	else {
		while (0 != whole) {
			digits[tally++] = Utf08_t((whole % 10) + '0');
			whole /= 10;
		}
	}

#endif

	// Finally push the negative sign onto the stack.
	if (negative) {
		digits[tally++] = '-';
	}

	// Pop the digits off the stack to append them onto the string in the
	// correct order.
	for (U32 i = 0; i < tally; ++i) {
		buffer[i] = digits[tally-i-1];
	}

	buffer[tally] = '\0';

	return tally;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfFromFourCC()
//
//	Formats a string of the format "0x20534444=DDS ".
//
U32 UtfFromFourCC(Utf08_t buffer[], U32 fourCC)
{
	for (U32 i = 0; i < 8; ++i) {
		U32 bits = (fourCC >> (28 - (i * 4))) & 0xF;

		if (bits < 10) {
			buffer[2+i] = Utf08_t(bits + '0');
		}
		else {
			buffer[2+i] = Utf08_t(bits + 'A' - 10);
		}
	}
	buffer[0]  = '0';
	buffer[1]  = 'x';
	buffer[10] = '=';

	for (U32 i = 0; i < 4; ++i) {
		Utf08_t bits = Utf08_t(fourCC & 0xFF);
		fourCC = fourCC >> 8;

		if ((bits < 32) || (bits >= 127)) {
			bits = '?';
		}

		buffer[11+i] = bits;
	}

	buffer[15] = '\0';

	return 15;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfToS32()
//
//	Converts text values to integers, supporting negative signs, hex, octal,
//	and binary.
//
bool UtfToS32(const Utf08_t buffer[], S32 &result)
{
	U32  length = UtfByteCount(buffer);
	U32  offset = 0;
	bool negate = false;

	if ('-' == buffer[0]) {
		negate = true;
		++offset;
	}
	else if ('+' == buffer[0]) {
		++offset;
	}

	if ('0' == buffer[offset]) {
		if (('b' == buffer[offset+1]) || ('B' == buffer[offset+1])) {
			S32 accum = 0;
			offset += 2;

			while (offset < length) {
				if (('0' <= buffer[offset]) && (buffer[offset] <= '1')) {
					accum = (accum << 1) | S32(U32(buffer[offset] - '0'));
					++offset;
				}
				else {
					return false;
				}
			}

			result = negate ? (-accum) : accum;

			return true;
		}
		else if (('o' == buffer[offset+1]) || ('O' == buffer[offset+1])) {
			S32 accum = 0;
			offset += 2;

			while (offset < length) {
				if (('0' <= buffer[offset]) && (buffer[offset] <= '7')) {
					accum = (accum << 3) | S32(U32(buffer[offset] - '0'));
					++offset;
				}
				else {
					return false;
				}
			}

			result = negate ? (-accum) : accum;

			return true;
		}
		else if (('x' == buffer[offset+1]) || ('X' == buffer[offset+1])) {
			S32 accum = 0;
			offset += 2;

			while (offset < length) {
				if (('0' <= buffer[offset]) && (buffer[offset] <= '9')) {
					accum = (accum << 4) + S32(U32(buffer[offset] - '0'));
					++offset;
				}
				else if (('a' <= buffer[offset]) && (buffer[offset] <= 'f')) {
					accum = (accum << 4) + S32(U32(buffer[offset] - 'a')) + 10;
					++offset;
				}
				else if (('A' <= buffer[offset]) && (buffer[offset] <= 'F')) {
					accum = (accum << 4) + S32(U32(buffer[offset] - 'A')) + 10;
					++offset;
				}
				else {
					return false;
				}
			}

			result = negate ? (-accum) : accum;

			return true;
		}
	}

	if (('0' <= buffer[offset]) && (buffer[offset] <= '9')) {
		S32 accum = 0;

		while (offset < length) {
			if (('0' <= buffer[offset]) && (buffer[offset] <= '9')) {
				accum = (accum * 10) + U32(buffer[offset] - '0');
				++offset;
			}
			else {
				return false;
			}
		}

		result = negate ? (-accum) : accum;

		return true;
	}

	return false;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfToS32()
//
S32 UtfToS32(const Utf08_t buffer[])
{
	S32 i;
	if (UtfToS32(buffer, i)) {
		return i;
	}

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfHexToDigit()
//
//	Converts a hexadecimal character into a 4-bit number.
//
U32 UtfHexToDigit(Utf08_t symbol)
{
	if (('0' <= symbol) && (symbol <= '9')) {
		return U32(symbol - '0');
	}

	if (('a' <= symbol) && (symbol <= 'f')) {
		return U32(symbol - 'a') + 10;
	}

	if (('A' <= symbol) && (symbol <= 'F')) {
		return U32(symbol - 'A') + 10;
	}

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfToHex()
//
//	Converts a sequence of hex digits to an integer value.  This assumes that
//	there is no "0x" marker at the start of the string, and only processes
//	the digits.
//
U32 UtfToHex(const Utf08_t buffer[])
{
	U32 accum  = 0;
	U32 offset = 0;

	while ('\0' != buffer[offset]) {
		Utf08_t c = buffer[offset++];

		if (('0' <= c) && (c <= '9')) {
			accum = (accum << 4) + U32(c - '0');
		}
		else if (('a' <= c) && (c <= 'f')) {
			accum = (accum << 4) + U32(c - 'a') + 10;
		}
		else if (('A' <= c) && (c <= 'F')) {
			accum = (accum << 4) + U32(c - 'A') + 10;
		}
		else {
			break;
		}
	}

	return accum;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfToFloat()
//
//	Converts text values to float, supporting negative signs.
//
bool UtfToFloat(Utf08_t buffer[], float &result)
{
	U32    length = UtfByteCount(buffer);
	U32    offset = 0;
	bool   negate = false;
	double temp   = 0.0;

	if ('-' == buffer[0]) {
		negate = true;
		++offset;
	}

	// Should never be any white space, but skip it if there is any.
	while ((offset < length) && ((' ' == buffer[offset]) || ('\t' == buffer[offset]))) {
		++offset;
	}

	if (offset >= length) {
		return false;
	}

	while (('0' <= buffer[offset]) && (buffer[offset] <= '9')) {
		temp = (temp * 10.0) + double(U32(buffer[offset] - '0'));
		++offset;
	}

	// There may not be a decimal point.
	if (offset >= length) {
		result = float(temp);
		return true;
	}

	// Any char other than a decimal point is invalid.
	if ('.' != buffer[offset]) {
		return false;
	}

	++offset;

	// There may not be any digits after the decimal point.
	if (offset >= length) {
		result = float(temp);
		return true;
	}

	double divi = 0.1;

	while (('0' <= buffer[offset]) && (buffer[offset] <= '9')) {
		temp += divi * double(U32(buffer[offset] - '0'));
		divi /= 10.0;
		++offset;
	}

	result = float(temp);

	return (offset >= length);
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfToFloat()
//
float UtfToFloat(Utf08_t buffer[])
{
	float f;
	if (UtfToFloat(buffer, f)) {
		return f;
	}

	return 0.0f;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfDetectEncoding()
//
//	Given the first few bytes of a file, this will determine what type of
//	UTF encoding is being used.  If it does not recognize any of the common
//	Byte Order Mark variant, is assumes that the file is plain ASCII.
//
U32 UtfDetectEncoding(Utf08_t buffer[], U32 byteCount)
{
	// Need at least two bytes for a UTF-16 marker.
	if (byteCount < 2) {
		return UtfEncode_ASCII;
	}

	// Intel byte ordering.
	if ((0xFF == buffer[0]) && (0xFE == buffer[1])) {

		// Avoid prefix problem: UTF-32 LE looks like UTF-16 LE if you only
		// look at the first two bytes.  If the next two bytes are zero,
		// then it's UTF-32 not UTF-16.
		if ((byteCount > 3) && (0x00 == buffer[2]) && (0x00 == buffer[3])) {
			return UtfEncode_UTF_32_LE;
		}

		return UtfEncode_UTF_16_LE;
	}

	// Motorola byte ordering.
	if ((0xFE == buffer[0]) && (0xFF == buffer[1])) {
		return UtfEncode_UTF_16_BE;
	}

	// Need at least three bytes for a UTF-8 marker.
	if (byteCount < 3) {
		return UtfEncode_ASCII;
	}

	if ((0xEF == buffer[0]) && (0xBB == buffer[1]) && (0xBF == buffer[2])) {
		return UtfEncode_UTF_8;
	}

	// Need at least four bytes for a UTF-32 marker.
	if (byteCount < 4) {
		return UtfEncode_ASCII;
	}

	// Intel byte ordering.
	if ((0xFF == buffer[0]) && (0xFE == buffer[1]) && (0x00 == buffer[2]) && (0x00 == buffer[3])) {
		return UtfEncode_UTF_32_LE;
	}

	// Motorola byte ordering.
	if ((0x00 == buffer[0]) && (0x00 == buffer[1]) && (0xFE == buffer[2]) && (0xFF == buffer[3])) {
		return UtfEncode_UTF_32_BE;
	}

	// Assume that anything else is ASCII.  If it is Unicode, we cannot
	// reliably determine without a marker at the start of the file.
	return UtfEncode_ASCII;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfWriteByteOrderMark()
//
//	Writes the appropriate byte-order-mark sequence for the different UTF
//	encodings.  Depending on the format of UTF, this will take either 2, 3,
//	or 4 bytes to hold the marker.
//
U32 UtfWriteByteOrderMark(Utf08_t buffer[], U32 utfEncoding)
{
	switch (utfEncoding) {
		case UtfEncode_UTF_8:
			buffer[0] = 0xEF;
			buffer[1] = 0xBB;
			buffer[2] = 0xBF;

			return 3;

		case UtfEncode_UTF_16_LE:
			buffer[0] = 0xFF;
			buffer[1] = 0xFE;

			return 2;

		case UtfEncode_UTF_16_BE:
			buffer[0] = 0xFE;
			buffer[1] = 0xFF;

			return 2;

		case UtfEncode_UTF_32_LE:
			buffer[0] = 0xFF;
			buffer[1] = 0xFE;
			buffer[2] = 0x00;
			buffer[3] = 0x00;

			return 4;

		case UtfEncode_UTF_32_BE:
			buffer[0] = 0x00;
			buffer[1] = 0x00;
			buffer[2] = 0xFE;
			buffer[3] = 0xFF;

			return 4;
	}

	// Otherwise assume ASCII, which does not have a byte order mark.
	// Better assumption is that this function won't be called when
	// trying to write ASCII text files.
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfContainsSymbol()
//
//	Search for a specific symbol within a string.  This will return a pointer
//	to the position of this symbol, or NULL if the symbol is not found within
//	the string.
//
Utf08_t* UtfContainsSymbol(Utf08_t buffer[], Utf32_t symbol)
{
	if (NULL == buffer) {
		return NULL;
	}

	U32 offset = 0;

	Utf32_t c;

	do {
		// Record the offset before reading the next symbol.  If this is the
		// symbol we're looking for, this is the position we need to return.
		U32 prevOffset = offset;

		c = UtfNextChar(buffer, offset);

		if (c == symbol) {
			return buffer + prevOffset;
		}
	} while ('\0' != c);

	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfContainsLastSymbol()
//
//	Similar to UtfContainsSymbol(), except that this will return the last
//	occurance of the symbol instead of the first.
//
Utf08_t* UtfContainsLastSymbol(Utf08_t buffer[], Utf32_t symbol)
{
	S32 lastOffset = -1;
	U32 offset     = 0;

	Utf32_t c;

	do {
		// Record the offset before reading the next symbol.  If this is the
		// symbol we're looking for, this is the position we need to return.
		U32 prevOffset = offset;

		c = UtfNextChar(buffer, offset);

		if (c == symbol) {
			lastOffset = prevOffset;
		}
	} while ('\0' != c);

	return (lastOffset < 0) ? NULL : (buffer + lastOffset);
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfContainsString()
//
Utf08_t* UtfContainsString(Utf08_t buffer[], U32 bufLength, Utf08_t symbol[], U32 symLength)
{
	if (symLength > bufLength) {
		return NULL;
	}

	U32 compCount = bufLength - symLength + 1;

	for (U32 i = 0; i < compCount; ++i) {
		if (0 == memcmp(buffer + i, symbol, symLength)) {
			return buffer + i;
		}
	}

	return NULL;
}





