/////////////////////////////////////////////////////////////////////////////
//
//	File: UtfData.h
//
//	$Header: /TS/TsGui/UtfData.h  9  2009/9/8 11:15:06a  Lee $
//
//
//	Contains the data tables used by UtfString.cpp.  That is the only file
//	that is expected to reference the contents of this file.
//
//	The app must call UtfInitialize() at start-up to assure that all of the
//	look-up tables are constructed.  All of the tables are static, so there
//	is no corresponding shut-down function.
//
/////////////////////////////////////////////////////////////////////////////


#pragma once


/////////////////////////////////////////////////////////////////////////////
//
struct UtfComposeTable_t
{
	Utf16_t Composed;
	Utf16_t First;
	Utf16_t Second;
};


extern UtfComposeTable_t g_UtfComposeValues[];


/////////////////////////////////////////////////////////////////////////////
//
struct UtfSymbolTable_t
{
	// Only supported symbols are stored in the table.
	Utf16_t Symbol;

	// Support characters with up to two diacritic marks.  The first value
	// is the base character.  The other two values are diacritics, or zero
	// if the character has no diacritics.
	Utf16_t Render1;
	Utf16_t Render2;
	Utf16_t Render3;

	// For upper-case symbols, this is the lower-case mapping.
	// For lower-case symbols, this is the upper-case mapping.
	// For non-cased symbols, this is zero.
	//
	// NOTE: Some upper- and lower-case characters do not have AltCase
	// mappings.  For these characters, AltCase maps back to the letter
	// itself.
	//
	Utf16_t AltCase;

	U16		Flags;
};


extern UtfSymbolTable_t g_UtfSymbolTable[];


/////////////////////////////////////////////////////////////////////////////
//
struct UtfSortEntry_t
{
	Utf16_t Symbol;
	Utf16_t Sort[4];
};


extern UtfSortEntry_t g_UtfDefaultSortTable[];


/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////


U32  UtfComposeTableSize(void);
U32  UtfSymbolTableSize(void);
U32  UtfDefaultSortTableSize(void);

void UtfInitialize(void);
void UtfUninitialize(void);

Utf32_t UtfMapASCII(Utf08_t symbol);
Utf32_t UtfComposeChars(Utf32_t a, Utf32_t b, Utf32_t &result);
void    UtfDecomposeChar(Utf32_t symbol, Utf32_t &c1, Utf32_t &c2, Utf32_t &c3);
bool    UtfDecomposeValidate(void);
Utf08_t UtfGetDecimal(void);
Utf08_t UtfGetThousands(void);

void UtfSetLanguage(U32 langID);
U32  UtfGetLanguage(void);

UtfSortEntry_t*   UtfLookUpSort(Utf32_t symbol);
UtfSymbolTable_t* UtfLookUpSymbol(Utf32_t symbol);


