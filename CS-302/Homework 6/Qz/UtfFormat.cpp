/////////////////////////////////////////////////////////////////////////////
//
//	File: UtfFormat.cpp
//
//	$Header: /Projects/Qz/UtfFormat.cpp  10  2010/5/17 12:14:21a  Lee $
//
//
//	Utility class for performing sprintf-like string formatting.  Except that
//	this will take UTF-8 strings as input, produce UTF-8 strings as output,
//	expose different parameter access logic so that it won't need to use
//	var-args, and it will allow the parameters can be accessed in any order,
//	which is very important for supporting internationalized strings.
//
//	Up to 9 parameters may be provided.  These are referenced by the numbers
//	1-9 (so we don't have to explain zero-based counting to users).
//
//	The reference to a parameter is indicated by the '%', followed by the
//	parameter digit, and always ending with ';'.  So the most basic pattern
//	that could be provided would be "%1;", which formats the first parameter.
//
//	After the parameter digit, one or more parameter values may be provided.
//	Unknown values are ignored.
//
//		-		left align the field, the default is right alignment
//					ex: "%1-;"
//
//		w#		minimum width of the field, the default minimum is 1
//					ex: "%1w10;" -- forces a minimum width of 10
//
//		w#.#	second width after decimal indicates precision of floats
//				default precision is 3 digits after decimal
//					ex: "%1w8.2;" -- min width is 8, with 2 digits after decimal
//
//		m#		maximum width of field, longer values are truncated
//				numbers too long for the field are printed as asterisks
//				by default, there is no max width
//					ex: "%1m10;" -- truncate field to max of 10 chars
//
//		+		force numbers to be prefixed with a '+' sign
//
//		x		integers will be displayed as hexadecimal values
//				this is ignored for strings and floats
//					ex: "%1x;"
//
//		p?		use the next char as the padding value to fill in unused
//				space when a minimum width is specified
//				the default character is a space
//					ex: "%1xw10p0;" - print number in hex, min width 10, padded with '0'
//
//		%		output a '%' character
//				no ';' is required after this symbol
//					ex: "%%" -- output a single '%'
//
/////////////////////////////////////////////////////////////////////////////


#include "QzCommon.h"


#ifdef USE_MALLOC_MACRO
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
//
//	constructor
//
UtfFormat::UtfFormat(void)
	:	m_ParamCount(0)
{
}


/////////////////////////////////////////////////////////////////////////////
//
//	Reset()
//
void UtfFormat::Reset(void)
{
	m_ParamCount = 0;
}


/////////////////////////////////////////////////////////////////////////////
//
//	AddChar()
//
void UtfFormat::AddChar(Utf32_t value)
{
	if (m_ParamCount < ArraySize(m_ParamList)) {
		m_ParamList[m_ParamCount].Integer = value;
		m_ParamList[m_ParamCount].Type    = UtfParam_Char;
		++m_ParamCount;
	}
	else {
		LogErrorMessage("UtfFormat::AddChar() parameter overflow");
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	AddFloat()
//
void UtfFormat::AddFloat(float value)
{
	if (m_ParamCount < ArraySize(m_ParamList)) {
		m_ParamList[m_ParamCount].Float = value;
		m_ParamList[m_ParamCount].Type  = UtfParam_Float;
		++m_ParamCount;
	}
	else {
		LogErrorMessage("UtfFormat::AddFloat() parameter overflow");
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	AddFourCC()
//
void UtfFormat::AddFourCC(U32 value)
{
	if (m_ParamCount < ArraySize(m_ParamList)) {
		m_ParamList[m_ParamCount].Integer = value;
		m_ParamList[m_ParamCount].Type    = UtfParam_FourCC;
		++m_ParamCount;
	}
	else {
		LogErrorMessage("UtfFormat::AddFourCC() parameter overflow");
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	AddInt()
//
void UtfFormat::AddInt(S32 value)
{
	if (m_ParamCount < ArraySize(m_ParamList)) {
		m_ParamList[m_ParamCount].Integer = value;
		m_ParamList[m_ParamCount].Type    = UtfParam_Int;
		++m_ParamCount;
	}
	else {
		LogErrorMessage("UtfFormat::AddInt() parameter overflow");
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	AddString()
//
//	Arguably, ASCII strings should have a separate processing pass to turn
//	them into UTF-8 format, but the only time a char pointer will be used
//	for a string is when an internally-declared string is given as the
//	parameter.  Since these should never contain anything other than simple
//	7-bit ASCII, no special mapping is required.
//
void UtfFormat::AddString(const char *pValue)
{
	if (m_ParamCount < ArraySize(m_ParamList)) {
		m_ParamList[m_ParamCount].pString = CharToUtf((NULL != pValue) ? pValue : "<NULL>");
		m_ParamList[m_ParamCount].Type    = UtfParam_Utf8;
		++m_ParamCount;
	}
	else {
		LogErrorMessage("UtfFormat::AddString() parameter overflow");
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	AddString()
//
void UtfFormat::AddString(const Utf08_t *pValue)
{
	if (m_ParamCount < ArraySize(m_ParamList)) {
		m_ParamList[m_ParamCount].pString = (NULL != pValue) ? CharToUtf(pValue) : CharToUtf("<NULL>");
		m_ParamList[m_ParamCount].Type    = UtfParam_Utf8;
		++m_ParamCount;
	}
	else {
		LogErrorMessage("UtfFormat::AddString() parameter overflow");
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	Generate()
//
//	Returns the number of bytes written into the destination buffer.
//
U32 UtfFormat::Generate(Utf08_t dst[], U32 dstLimit, const Utf08_t pattern[])
{
	if ((NULL == dst) || (dstLimit < 1)) {
		return 0;
	}

	if (NULL == pattern) {
		dst[0] = '\0';
		return 0;
	}

	// Do not change dstLimit.  That value needs to be passed into GenParam()
	// unchanged so that the correct buffer limit is being honored.  If we
	// were to subtract one here, we could end up with a string that ends
	// with "\0\0" instead of "\0", and is missing one char that could have
	// and should have been printed.  So we use a different variable for
	// testing the end of the buffer for the loop below, and still can pass
	// dstLimit into GenParam().
	U32 dstLimSubOne = dstLimit - 1;

	U32 dstOffset = 0;
	U32 srcOffset = 0;

	// Just copy bytes to the destination buffer.  We don't care if there are
	// any multi-byte symbols in the stream, since all we're looking for is
	// the '%' that denotes a parameter field.
	while (dstOffset < dstLimSubOne) {
		if ('\0' == pattern[srcOffset]) {
			break;
		}

		if ('%' == pattern[srcOffset]) {
			++srcOffset;

			// Special case for "%%", which just needs to output a single
			// percent symbol.
			if ('%' == pattern[srcOffset]) {
				dst[dstOffset++] = pattern[srcOffset++];
			}

			// Otherwise this is a parameter reference.
			else {
				dstOffset = GenParam(dst, dstOffset, dstLimit, pattern, srcOffset);
			}
		}
		else {
			dst[dstOffset++] = pattern[srcOffset++];
		}
	}

	dst[dstOffset] = '\0';

	return dstOffset;
}


/////////////////////////////////////////////////////////////////////////////
//
//	GenParam()
//
//	This will handle processing a parameter reference.  It will read in any
//	options associated with the parameter, format the value accordingly, and
//	attempt to append it to the destination string.
//
//	Returns the updated value of dstOffset.
//
U32 UtfFormat::GenParam(Utf08_t dst[], U32 dstOffset, U32 dstLimit, const Utf08_t src[], U32 &srcOffset)
{
	U32     paramNum    = 0;
	bool    rightAlign  = true;
	bool    showPlus    = false;
	bool    useHex      = false;
	bool	useUnsigned = false;
	U32     minWidth    = 0;
	U32     precision   = 3;
	U32     maxWidth    = 0;
	Utf32_t padChar     = ' ';

	// The parameter must start with a digit 1-9 to indicate which parameter
	// should be printed.
	if ((src[srcOffset] < '1') || (src[srcOffset] > '9')) {
		UtfFormat fmt;
		fmt.AddChar(src[srcOffset]);
		fmt.AddString(src);
		LogErrorMessage("GenParam(): invalid parameter reference '%1;': \"%2;\"", fmt);

		return dstOffset;
	}

	paramNum = src[srcOffset] - '1';
	++srcOffset;
	bool cleanEnding = false;

	// Consume all formatting characters.  Stop when we hit the end of the
	// buffer (which should be an error) or when we see the ';'.  Everything
	// in between should be part of a formatting command.  We'll also break
	// out of the loop if we see an unknown character, so the jibberish will
	// get printed in hopes of signalling the formatting problem to the user.
	//
	while ('\0' != src[srcOffset]) {
		// Found the expected end-of-parameter marker.
		if (';' == src[srcOffset]) {
			++srcOffset;
			cleanEnding = true;
			break;
		}

		// Force integers to be printed in hex format.
		else if ('x' == src[srcOffset]) {
			++srcOffset;
			useHex = true;
		}

		// Force '+' sign to appear for positive numbers.
		else if ('+' == src[srcOffset]) {
			++srcOffset;
			showPlus = true;
		}

		// Flag indicating left alignment.
		else if ('-' == src[srcOffset]) {
			++srcOffset;
			rightAlign = false;
		}

		// Force integers to be printed unsigned.
		else if ('u' == src[srcOffset]) {
			++srcOffset;
			useUnsigned = true;
		}

		// Minimum width value.
		else if ('w' == src[srcOffset]) {
			++srcOffset;
			minWidth = 0;
			while (('0' <= src[srcOffset]) && (src[srcOffset] <= '9')) {
				minWidth = (minWidth * 10) + (src[srcOffset] - '0');
				++srcOffset;
			}

			// If there is a decimal point, there is a second width value
			// that controls the precision of floating-point numbers.
			if ('.' == src[srcOffset]) {
				++srcOffset;

				precision = 0;
				while (('0' <= src[srcOffset]) && (src[srcOffset] <= '9')) {
					precision = (precision * 10) + (src[srcOffset] - '0');
					++srcOffset;
				}
			}
		}

		// Maximum width of field.
		else if ('m' == src[srcOffset]) {
			++srcOffset;
			maxWidth = 0;
			while (('0' <= src[srcOffset]) && (src[srcOffset] <= '9')) {
				maxWidth = (maxWidth * 10) + (src[srcOffset] - '0');
				++srcOffset;
			}
		}

		// Specify what character to use for padding.  This may be a Unicode
		// symbol, so exercise caution when extracting it from the pattern.
		else if ('p' == src[srcOffset]) {
			++srcOffset;

			// Grab the next Unicode symbol from the UTF-8 stream, which
			// could be up to 4 bytes long.
			padChar = UtfNextChar(src, srcOffset);

			// The symbol may be unsupported, but any string that gets to
			// here should have been composed, so that is not likely.
			if ('\0' == padChar) {
				padChar = ' ';
			}
		}

		// If we see an unknown character, break out of the loop so that
		// character will be printed to the output buffer for the user to
		// hopefully see and fix.
		else {
			break;
		}
	}

	// Display a warning message if we did not see the final ';'.  Although
	// we could try to format the parameter, we'll inject a chunk of error
	// text into the buffer to make it obvious that the parameter is badly
	// formatted, otherwise the user won't know there is an error without
	// checking the log.
	if (false == cleanEnding) {
		UtfFormat fmt;
		fmt.AddInt(paramNum + 1);
		fmt.AddString(src);
		LogErrorMessage("GenParam(): param %1; does not end with ';': \"%2;\"", fmt);

		return dstOffset + UtfCopy(dst + dstOffset, dstLimit - dstOffset, reinterpret_cast<const Utf08_t*>("<BAD PARAM FORMAT>"));
	}

	// If the parameter number is out-of-range, insert an error message.
	if (paramNum >= m_ParamCount) {
		UtfFormat fmt;
		fmt.AddInt(m_ParamCount);
		fmt.AddInt(paramNum + 1);
		fmt.AddString(src);
		LogErrorMessage("GenParam(): there are %1; param(s), param %2; does not exist: \"%3;\"", fmt);

		return dstOffset + UtfCopy(dst + dstOffset, dstLimit - dstOffset, reinterpret_cast<const Utf08_t*>("[INVALID PARAM NUM]"));
	}

	// Limit minWidth to the given maxWidth.  But maxWidth is only valid
	// if both minWidth and maxWidth are non-zero.
	if ((maxWidth > 0) && (minWidth > maxWidth)) {
		minWidth = maxWidth;
	}

	UtfParam_t &ref = m_ParamList[paramNum];

	Utf08_t numerical[32];
	U32     byteCount = 0;

	//
	// Strings and FourCCs
	//

	if ((UtfParam_Utf8 == ref.Type) || (UtfParam_FourCC == ref.Type)) {
		U32      charCount = 0;
		Utf08_t* pStrg     = NULL;

		if (UtfParam_Utf8 == ref.Type) {
			// For a string, we just use the source string for processing.
			// Note that the string cannot be NULL.  Pre-conditioning will
			// change NULL strings to "<NULL>" to avoid problems.
			UtfStringSize(ref.pString, byteCount, charCount);
			pStrg = ref.pString;
		}
		else {
			// For a FourCC, we need to convert it into a string.
			byteCount = UtfFromFourCC(numerical, ref.Integer);
			charCount = byteCount;
			pStrg     = numerical;
		}

		// If there is a maximum width and this string is too long, we need
		// to truncate it to fit within the field.
		if ((maxWidth > 0) && (charCount > maxWidth)) {
			// Reserve one extra space for the '\0' that will be appended by
			// UtfCopy(), since it needs to guarantee the string is always
			// terminated properly.
			U32 copyLimit = Min(maxWidth + 1, dstLimit - dstOffset);
			dstOffset += UtfCopy(dst + dstOffset, copyLimit, pStrg);
		}

		// But if there is a minimum width, we need to insert padding either
		// to the left or right of the string to attain the desired width.
		else if (charCount < minWidth) {
			if (rightAlign) {
				for (U32 i = 0; i < (minWidth - charCount); ++i) {
					dstOffset += UtfAppendSymbol(dst + dstOffset, dstLimit - dstOffset, padChar);
				}

				dstOffset += UtfCopy(dst + dstOffset, dstLimit - dstOffset, pStrg);
			}
			else {
				dstOffset += UtfCopy(dst + dstOffset, dstLimit - dstOffset, pStrg);

				for (U32 i = 0; i < (minWidth - charCount); ++i) {
					dstOffset += UtfAppendSymbol(dst + dstOffset, dstLimit - dstOffset, padChar);
				}
			}
		}

		// Otherwise we fall through to the standard condition, where there
		// is no minimum or maximum field size, and we can simply append the
		// whole string into the destination buffer.
		else {
			dstOffset += UtfCopy(dst + dstOffset, dstLimit - dstOffset, pStrg);
		}
	}

	//
	// Floats and Integers
	//

	else if ((UtfParam_Float == ref.Type) || (UtfParam_Int == ref.Type)) {
		if (UtfParam_Float == ref.Type) {
			if (showPlus && (ref.Float >= 0.0f)) {
				numerical[byteCount++] = '+';
			}

			byteCount += UtfFromFloat(numerical + byteCount, ref.Float, precision);

			// The resulting float may be too long to fit within the given
			// max width.  Try stripping off digits after the decimal point
			// to make the string fit.
			if ((maxWidth > 0) && (byteCount > maxWidth) && (precision > 0)) {
				// Try stripping off all digits after the decimal.
				while ((byteCount > maxWidth) && (precision > 0)) {
					numerical[--byteCount] = '\0';
					--precision;
				}

				// If we're left with nothing after the decimal point,
				// that char should also be removed.
				if ((byteCount > 0) && ('.' == numerical[byteCount-1])) {
					numerical[--byteCount] = '\0';
				}
			}
		}
		else {
			if (false == useHex) {
				if (showPlus && (useUnsigned || (ref.Integer >= 0))) {
					numerical[byteCount++] = '+';
				}

				if (useUnsigned) {
					byteCount += UtfFromU32(numerical + byteCount, ref.Integer);
				}
				else {
					byteCount += UtfFromS32(numerical + byteCount, ref.Integer);
				}
			}
			else {
				byteCount = UtfFromHex(numerical, ref.Integer);
			}
		}

		// If there is a maximum width and the number is too long, we'll
		// pad out the field with with asterisks so the user knows the
		// number won't fit in the available space.  Otherwise we risk
		// truncating digits, displaying the wrong value.
		if ((maxWidth > 0) && (byteCount > maxWidth)) {
			for (U32 i = 0; i < maxWidth; ++i) {
				if (dstOffset < dstLimit) {
					dst[dstOffset++] = '*';
				}
			}
		}
		else if (byteCount >= minWidth) {
			for (U32 i = 0; i < byteCount; ++i) {
				if (dstOffset < dstLimit) {
					dst[dstOffset++] = numerical[i];
				}
			}
		}
		else if (rightAlign) {
			for (U32 i = 0; i < (minWidth - byteCount); ++i) {
				dstOffset += UtfAppendSymbol(dst + dstOffset, dstLimit - dstOffset, padChar);
			}
			for (U32 i = 0; i < byteCount; ++i) {
				if (dstOffset < dstLimit) {
					dst[dstOffset++] = numerical[i];
				}
			}
		}
		else {
			for (U32 i = 0; i < byteCount; ++i) {
				if (dstOffset < dstLimit) {
					dst[dstOffset++] = numerical[i];
				}
			}
			for (U32 i = 0; i < (minWidth - byteCount); ++i) {
				dstOffset += UtfAppendSymbol(dst + dstOffset, dstLimit - dstOffset, padChar);
			}
		}
	}

	else if (UtfParam_Char == ref.Type) {
		dstOffset += UtfAppendSymbol(dst + dstOffset, dstLimit - dstOffset, ref.Integer);
	}

	// No other types are currently supported.
	else {
		UtfFormat fmt;
		fmt.AddInt(paramNum + 1);
		fmt.AddInt(ref.Type);
		fmt.AddString(src);
		LogErrorMessage("GenParam(): internal error: param %1; has invalid type %2;: \"%3;\"", fmt);

		dstOffset += UtfCopy(dst + dstOffset, dstLimit - dstOffset, reinterpret_cast<const Utf08_t*>("[INVALID TYPE]"));
	}

	return dstOffset;
}

