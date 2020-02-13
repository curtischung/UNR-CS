/////////////////////////////////////////////////////////////////////////////
//
//	File: UtfFormat.h
//
//	$Header: /TS/TsGui/UtfFormat.h 4     1/09/09 3:19p Lee $
//
//
//	Utility class for performing sprintf-like string formatting.  Except that
//	this will take UTF-8 strings as input, produce UTF-8 strings as output,
//	expose different parameter access logic so that it won't need to use
//	var-args, and it will allow the parameters can be accessed in any order,
//	which is very important for supporting internationalized strings.
//
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
//		u		integers will be printed as unsigned numbers
//				this only effects numbers over 0x7FFFFFFF
//					ex: "%1u;"
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


#pragma once


enum UtfParamType_t
{
	UtfParam_Char,
	UtfParam_Float,
	UtfParam_FourCC,
	UtfParam_Int,
	UtfParam_Utf8
};


struct UtfParam_t
{
	float			Float;
	S32				Integer;
	Utf08_t*		pString;
	UtfParamType_t	Type;
};


class UtfFormat
{
private:
	U32			m_ParamCount;
	UtfParam_t	m_ParamList[9];

public:
	UtfFormat(void);

	void Reset(void);
	void AddChar(Utf32_t value);
	void AddFloat(float value);
	void AddFourCC(U32 value);
	void AddInt(S32 value);
	void AddString(const char *pValue);
	void AddString(const Utf08_t *pValue);

	U32  Generate(Utf08_t dst[], U32 dstLimit, const Utf08_t pattern[]);
	U32  GenParam(Utf08_t dst[], U32 dstOffset, U32 dstLimit, const Utf08_t src[], U32 &srcOffset);
};



