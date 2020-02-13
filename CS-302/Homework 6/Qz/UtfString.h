/////////////////////////////////////////////////////////////////////////////
//
//	File: UtfString.h
//
//	$Header: /Projects/Qz/UtfString.h  21  2010/4/18 7:56:32p  Lee $
//
//
//	This file contains a collection of support routines for performing string
//	operations on UTF strings.  Most transforms are for data in UTF-8 format,
//	but it also supports converting to and from UTF-16 and UTF-32.
//
//
//	UTF file markers:
//		UTF-8:    EF BB BF
//		UTF-16LE: FF FE
//		UTF-16BE: FE FF
//		UTF-32LE: FF FE 00 00
//		UTF-32BE: 00 00 FE FF
//
/////////////////////////////////////////////////////////////////////////////


#pragma once


// These values are used as IDs in data files, so never change them.
#define LangID_English			0
#define LangID_French			1
#define LangID_German			2
#define LangID_Irish			3
#define LangID_Italian			4
#define LangID_Polish			5
#define LangID_Russian			6
#define LangID_Spanish			7
#define LangID_ArraySize		8


#define UtfEncode_ASCII			0
#define UtfEncode_UTF_8			1
#define UtfEncode_UTF_16		2			// default to Intel Little-Endian formatting
#define UtfEncode_UTF_16_LE		2
#define UtfEncode_UTF_16_BE		3
#define UtfEncode_UTF_32		4			// default to Intel Little-Endian formatting
#define UtfEncode_UTF_32_LE		4
#define UtfEncode_UTF_32_BE		5


#define Unicode_NBSP				0x00A0		// non-breaking space
#define Unicode_MultiplySign		0x00D7
#define Unicode_LeftSingleQuote		0x2018		// left single-quote
#define Unicode_RightSingleQuote	0x2019		// right single-quote
#define Unicode_LeftDoubleQuote		0x201C		// left double-quote
#define Unicode_RightRoubleQuote	0x201D		// right double-quote
#define Unicode_Bullet				0x2022
#define Unicode_FirstSurrogate		0xD800		// 0xD800-0xDFFF is reserved for surrogates in UTF-16
#define Unicode_LastSurrogate		0xDFFF
#define Unicode_UnknownSymbol		0xFFFD		// typically rendered as a question mark in a diamond
#define Unicode_ByteOrderMark		0xFFFE
#define Unicode_MaxValidCode		0x10FFFF	// maximum possible code for 21-bit Unicode


#define	UtfFlag_Lowercase		(1 << 0)
#define UtfFlag_Uppercase		(1 << 1)
#define UtfFlag_CombineHiMid	(1 << 2)
#define UtfFlag_CombineHiRight	(1 << 3)
#define UtfFlag_CombineLoMid	(1 << 4)
#define UtfFlag_CombineLoRight	(1 << 5)

U32   UtfSymbolSize08(Utf08_t symbol);
U32   UtfSymbolSize32(Utf32_t value);

bool  UtfIsAlpha(Utf32_t symbol);
bool  UtfIsDigit(Utf32_t symbol);
bool  UtfIsHexDigit(Utf32_t symbol);
bool  UtfIsKnown(Utf32_t symbol);
bool  UtfIsLower(Utf32_t symbol);
bool  UtfIsUpper(Utf32_t symbol);
bool  UtfIsValid(Utf32_t symbol);

bool  UtfExtractHexDigit(Utf32_t symbol, U32 &value);

Utf32_t UtfToLower(Utf32_t symbol);
Utf32_t UtfToUpper(Utf32_t symbol);
void    UtfPackChar08(Utf08_t *pDst, U32 &offset, Utf32_t value);
Utf32_t UtfNextChar(const Utf08_t *pStrg, U32 &offset);
Utf32_t UtfNextChar(const Utf16_t *pStrg, U32 &offset);
Utf32_t UtfPrevChar(const Utf08_t *pStrg, U32 &offset);

void  UtfToLowerASCII(Utf08_t pStrg[]);

U32   UtfCharLength(const Utf08_t pStrg[]);
U32   UtfByteCount(const Utf08_t pStrg[]);
U32   UtfCharCount(const Utf08_t pStrg[]);
U32   UtfCharCount(const Utf32_t pStrg[]);
void  UtfStringSize(const Utf08_t pStrg[], U32 &byteCount, U32 &charCount);
U32   UtfPreCompose32to08(const Utf32_t pSrc[]);
U32   UtfPreCompose16to08(const Utf16_t pSrc[]);
U32   UtfPreCompose08to08(const Utf08_t pSrc[]);
U32   UtfPreComposeASCII(const char pSrc[]);
U32   UtfCompose32to08(Utf08_t pDst[], U32 dstLimit, const Utf32_t pSrc[]);
U32   UtfCompose16to08(Utf08_t pDst[], U32 dstLimit, const Utf16_t pSrc[]);
U32   UtfCompose08to08(Utf08_t pDst[], U32 dstLimit, const Utf08_t pSrc[]);
U32   UtfComposeASCII(Utf08_t pDst[], U32 dstLimit, const char pSrc[]);
U32   UtfNormalize08to32(Utf32_t pDst[], U32 dstLimit, const Utf08_t pSrc[]);
U32   UtfNormalize08to16(Utf16_t pDst[], U32 dstLimit, const Utf08_t pSrc[]);
U32   UtfNormalize08to08(Utf08_t pDst[], U32 dstLimit, const Utf08_t pSrc[]);
U32   UtfCanonicalDecompose08to08(Utf08_t pDst[], U32 dstLimit, const Utf08_t pSrc[]);
U32   UtfConvert32to08(Utf08_t pDst[], U32 dstLimit, const Utf32_t pSrc[]);
U32   UtfConvert08to32(Utf32_t pDst[], U32 dstLimit, const Utf08_t pSrc[]);
U32   UtfConvert32to16(Utf16_t pDst[], U32 dstLimit, const Utf32_t pSrc[]);
U32   UtfConvert16to32(Utf32_t pDst[], U32 dstLimit, const Utf16_t pSrc[]);
U32   UtfConvert16to08(Utf08_t pDst[], U32 dstLimit, const Utf16_t pSrc[]);
U32   UtfConvert08to16(Utf16_t pDst[], U32 dstLimit, const Utf08_t pSrc[]);
U32   UtfCopy(Utf32_t pDst[], U32 dstLimit, const Utf32_t pSrc[]);
U32   UtfCopy(Utf08_t pDst[], U32 dstLimit, const Utf08_t pSrc[]);
U32   UtfAppend(Utf08_t pDst[], U32 dstLimit, const Utf08_t pSrc[]);
U32   UtfAppendSymbol(Utf08_t pDst[], U32 dstLimit, Utf32_t symbol);
S32   UtfCompareBytewise(const Utf32_t pA[], const Utf32_t pB[]);
S32   UtfCompareBytewise(const Utf08_t pA[], const Utf08_t pB[]);
S32   UtfCompareExact(const Utf08_t pA[], const Utf08_t pB[]);
S32   UtfCompareNocase(const Utf08_t pA[], const Utf08_t pB[]);
S32   UtfCompareNocaseLength(const Utf08_t pA[], const Utf08_t pB[], U32 charCount);
S32   UtfCompareNocaseASCII(const char pA[], const Utf08_t pB[]);
S32   UtfCompareLexical(const Utf08_t pA[], const Utf08_t pB[]);
bool  UtfCompareWildcard(const Utf08_t pA[], const Utf08_t pWild[]);
bool  UtfCompareWildcardNocase(const Utf08_t pA[], const Utf08_t pWild[]);
bool  UtfContainsWildcards(const Utf08_t pWild[]);
bool  UtfPrefix(const Utf08_t prefix[], const Utf08_t strg[]);
bool  UtfPrefixNocase(const Utf08_t prefix[], const Utf08_t strg[]);
bool  UtfSuffix(const Utf08_t suffix[], const Utf08_t strg[]);
bool  UtfSuffixNocase(const Utf08_t suffix[], const Utf08_t strg[]);
U32   UtfLocalizedFromS32(Utf08_t buffer[], S32 value);
U32   UtfFromS32(Utf08_t buffer[], S32 value);
U32   UtfFromU32(Utf08_t buffer[], U32 value);
U32   UtfFromHex(Utf08_t buffer[], U32 value);
U32   UtfFromFloat(Utf08_t buffer[], float value, U32 precision = 3);
U32   UtfFromFourCC(Utf08_t buffer[], U32 fourCC);
bool  UtfToS32(const Utf08_t buffer[], S32 &result);
S32   UtfToS32(const Utf08_t buffer[]);
U32   UtfHexToDigit(Utf08_t symbol);
U32   UtfToHex(const Utf08_t buffer[]);
bool  UtfToFloat(Utf08_t buffer[], float &result);
float UtfToFloat(Utf08_t buffer[]);
U32   UtfDetectEncoding(Utf08_t buffer[], U32 byteCount);
U32   UtfWriteByteOrderMark(Utf08_t buffer[], U32 utfEncoding);

Utf08_t* UtfContainsSymbol(Utf08_t buffer[], Utf32_t symbol);
Utf08_t* UtfContainsLastSymbol(Utf08_t buffer[], Utf32_t symbol);
Utf08_t* UtfContainsString(Utf08_t buffer[], U32 bufLength, Utf08_t symbol[], U32 symLength);


/////////////////////////////////////////////////////////////////////////////
//
//	CharToUtf()
//
//	Lame hack to deal with const-to-non-const type casting when taking a
//	char string and assigning it to a Utf08_t* parameter.
//
inline Utf08_t* CharToUtf(const char *pChar)
{
	return (Utf08_t*)pChar;
}

inline Utf08_t* CharToUtf(const Utf08_t *pChar)
{
	return (Utf08_t*)pChar;
}




