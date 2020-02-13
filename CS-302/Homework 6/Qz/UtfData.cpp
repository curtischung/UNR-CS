/////////////////////////////////////////////////////////////////////////////
//
//	File: UtfData.cpp
//
//	$Header: /Projects/Qz/UtfData.cpp  12  2010/4/23 10:18:24a  Lee $
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


#include "QzCommon.h"
#include "UtfData.h"


#ifdef USE_MALLOC_MACRO
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//	Which language is currently being used for sorting algorithms.
//	This should only be changed with UtfSetLanguage(), since the sorting
//	tables need to be rebuilt when this is changed.
//
static U32     g_UtfLanguage  = LangID_English;
static Utf08_t g_UtfDecimal   = '.';
static Utf08_t g_UtfThousands = ',';


/////////////////////////////////////////////////////////////////////////////
//
//	This is for composing two characters at a time.  It should be performed
//	multiple times, allowing symbols with two diacritics to resolve to the
//	final symbol.
//
//	Note that some entries have a zero for the second value.  This is used
//	to remap some alternate symbols to equivalent symbols to avoid having
//	symbols that have different code values even though they look identical.
//
UtfComposeTable_t g_UtfComposeValues[] =
{
	// Lone diacritics may occur after a space, which simply indicates that
	// the diacritic should be displayed without a character.  We'll compose
	// these into stand-alone diacritics to make processing simpler.  But
	// that means that we'll need to normalize text before outputting to to
	// a text file.
	{ 0x0300, 0x0020, 0x0300 },		// space + grave
	{ 0x0300, 0x00A0, 0x0300 },		// space + grave
	{ 0x0301, 0x0020, 0x0301 },		// space + acute
	{ 0x0301, 0x00A0, 0x0301 },		// space + acute
	{ 0x0302, 0x0020, 0x0302 },		// space + circumflex
	{ 0x0302, 0x00A0, 0x0302 },		// space + circumflex
	{ 0x0303, 0x0020, 0x0303 },		// space + tilde
	{ 0x0303, 0x00A0, 0x0303 },		// space + tilde
	{ 0x0304, 0x0020, 0x0304 },		// space + macron
	{ 0x0304, 0x00A0, 0x0304 },		// space + macron
	{ 0x0306, 0x0020, 0x0306 },		// space + breve
	{ 0x0306, 0x00A0, 0x0306 },		// space + breve
	{ 0x0307, 0x0020, 0x0307 },		// space + dot above
	{ 0x0307, 0x00A0, 0x0307 },		// space + dot above
	{ 0x0308, 0x0020, 0x0308 },		// space + diaeresis
	{ 0x0308, 0x00A0, 0x0308 },		// space + diaeresis
	{ 0x0309, 0x0020, 0x0309 },		// space + hook above
	{ 0x0309, 0x00A0, 0x0309 },		// space + hook above
	{ 0x030A, 0x0020, 0x030A },		// space + ring above
	{ 0x030A, 0x00A0, 0x030A },		// space + ring above
	{ 0x030B, 0x0020, 0x030B },		// space + double acute
	{ 0x030B, 0x00A0, 0x030B },		// space + double acute
	{ 0x030C, 0x0020, 0x030C },		// space + caron
	{ 0x030C, 0x00A0, 0x030C },		// space + caron
	{ 0x0323, 0x0020, 0x0323 },		// space + dot below
	{ 0x0323, 0x00A0, 0x0323 },		// space + dot below
	{ 0x0324, 0x0020, 0x0324 },		// space + diaeresis below
	{ 0x0324, 0x00A0, 0x0324 },		// space + diaeresis below
	{ 0x0325, 0x0020, 0x0325 },		// space + ring below
	{ 0x0325, 0x00A0, 0x0325 },		// space + ring below
	{ 0x0327, 0x0020, 0x0327 },		// space + cedilla
	{ 0x0327, 0x00A0, 0x0327 },		// space + cedilla
	{ 0x0328, 0x0020, 0x0328 },		// space + ogonek
	{ 0x0328, 0x00A0, 0x0328 },		// space + ogonek
	{ 0x032D, 0x0020, 0x032D },		// space + circumflex below
	{ 0x032D, 0x00A0, 0x032D },		// space + circumflex below
	{ 0x032E, 0x0020, 0x032E },		// space + breve below
	{ 0x032E, 0x00A0, 0x032E },		// space + breve below
	{ 0x0330, 0x0020, 0x0330 },		// space + tilde below
	{ 0x0330, 0x00A0, 0x0330 },		// space + tilde below
	{ 0x0331, 0x0020, 0x0331 },		// space + macron below
	{ 0x0331, 0x00A0, 0x0331 },		// space + macron below

	{ 0x00C1, 0x0041, 0x0301 },		// A + acute
	{ 0x00E1, 0x0061, 0x0301 },		// a + acute
	{ 0x0102, 0x0041, 0x0306 },		// A + breve
	{ 0x0103, 0x0061, 0x0306 },		// a + breve
	{ 0x00C2, 0x0041, 0x0302 },		// A + circumflex
	{ 0x00E2, 0x0061, 0x0302 },		// a + circumflex
	{ 0x00C4, 0x0041, 0x0308 },		// A + diaeresis
	{ 0x00E4, 0x0061, 0x0308 },		// a + diaeresis
	{ 0x00C0, 0x0041, 0x0300 },		// A + grave
	{ 0x00E0, 0x0061, 0x0300 },		// a + grave
	{ 0x0100, 0x0041, 0x0304 },		// A + macron
	{ 0x0101, 0x0061, 0x0304 },		// a + macron
	{ 0x0104, 0x0041, 0x0328 },		// A + ogonek
	{ 0x0105, 0x0061, 0x0328 },		// a + ogonek
	{ 0x00C5, 0x0041, 0x030A },		// A + ring above
	{ 0x00E5, 0x0061, 0x030A },		// a + ring above
	{ 0x1E00, 0x0041, 0x0325 },		// A + ring below
	{ 0x1E01, 0x0061, 0x0325 },		// a + ring below
	{ 0x00C3, 0x0041, 0x0303 },		// A + tilde
	{ 0x00E3, 0x0061, 0x0303 },		// a + tilde

	{ 0x1E02, 0x0042, 0x0307 },		// B + dot above
	{ 0x1E03, 0x0062, 0x0307 },		// b + dot above
	{ 0x1E04, 0x0042, 0x0323 },		// B + dot below
	{ 0x1E05, 0x0062, 0x0323 },		// b + dot below
	{ 0x1E06, 0x0042, 0x0331 },		// B + macron below
	{ 0x1E07, 0x0062, 0x0331 },		// b + macron below

	{ 0x0106, 0x0043, 0x0301 },		// C + acute
	{ 0x0107, 0x0063, 0x0301 },		// c + acute
	{ 0x010C, 0x0043, 0x030C },		// C + caron
	{ 0x010D, 0x0063, 0x030C },		// c + caron
	{ 0x00C7, 0x0043, 0x0327 },		// C + cedilla
	{ 0x00E7, 0x0063, 0x0327 },		// c + cedilla
	{ 0x1E08, 0x00C7, 0x0301 },		// C + cedilla + acute
	{ 0x1E09, 0x00E7, 0x0301 },		// c + cedilla + acute
	{ 0x0108, 0x0043, 0x0302 },		// C + circumflex
	{ 0x0109, 0x0063, 0x0302 },		// c + circumflex
	{ 0x010A, 0x0043, 0x0307 },		// C + dot above
	{ 0x010B, 0x0063, 0x0307 },		// c + dot above

	{ 0x010E, 0x0044, 0x030C },		// D + caron
	{ 0x010F, 0x0064, 0x030C },		// d + caron
	{ 0x1E10, 0x0044, 0x0327 },		// D + cedilla
	{ 0x1E11, 0x0064, 0x0327 },		// d + cedilla
	{ 0x1E12, 0x0044, 0x032D },		// D + circumflex below
	{ 0x1E13, 0x0064, 0x032D },		// d + circumflex below
	{ 0x1E0A, 0x0044, 0x0307 },		// D + dot above
	{ 0x1E0B, 0x0064, 0x0307 },		// d + dot above
	{ 0x1E0C, 0x0044, 0x0323 },		// D + dot below
	{ 0x1E0D, 0x0064, 0x0323 },		// d + dot below
	{ 0x1E0E, 0x0044, 0x0331 },		// D + macron below
	{ 0x1E0F, 0x0064, 0x0331 },		// d + macron below

	{ 0x00C9, 0x0045, 0x0301 },		// E + acute
	{ 0x00E9, 0x0065, 0x0301 },		// e + acute
	{ 0x0114, 0x0045, 0x0306 },		// E + breve
	{ 0x0115, 0x0065, 0x0306 },		// e + breve
	{ 0x1E1C, 0x0114, 0x0327 },		// E + breve + cedilla
	{ 0x1E1D, 0x0115, 0x0327 },		// e + breve + cedilla
	{ 0x011A, 0x0045, 0x030C },		// E + caron
	{ 0x011B, 0x0065, 0x030C },		// e + caron
	{ 0x00CA, 0x0045, 0x0302 },		// E + circumflex
	{ 0x00EA, 0x0065, 0x0302 },		// e + circumflex
	{ 0x1E18, 0x0045, 0x032D },		// E + circumflex below
	{ 0x1E19, 0x0065, 0x032D },		// e + circumflex below
	{ 0x00CB, 0x0045, 0x0308 },		// E + diaeresis
	{ 0x00EB, 0x0065, 0x0308 },		// e + diaeresis
	{ 0x0116, 0x0045, 0x0307 },		// E + dot above
	{ 0x0117, 0x0065, 0x0307 },		// e + dot above
	{ 0x00C8, 0x0045, 0x0300 },		// E + grave
	{ 0x00E8, 0x0065, 0x0300 },		// e + grave
	{ 0x0112, 0x0045, 0x0304 },		// E + macron
	{ 0x0113, 0x0065, 0x0304 },		// e + macron
	{ 0x1E16, 0x0112, 0x0301 },		// E + macron + acute
	{ 0x1E17, 0x0113, 0x0301 },		// e + macron + acute
	{ 0x1E14, 0x0112, 0x0300 },		// E + macron + grave
	{ 0x1E15, 0x0113, 0x0300 },		// e + macron + grave
	{ 0x0118, 0x0045, 0x0328 },		// E + ogonek
	{ 0x0119, 0x0065, 0x0328 },		// e + ogonek
	{ 0x1E1A, 0x0045, 0x0330 },		// E + tilde below
	{ 0x1E1B, 0x0065, 0x0330 },		// e + tilde below

	{ 0x1E1E, 0x0046, 0x0307 },		// F + dot above
	{ 0x1E1F, 0x0066, 0x0307 },		// f + dot above

	{ 0x011E, 0x0047, 0x0306 },		// G + breve
	{ 0x011F, 0x0067, 0x0306 },		// g + breve
	{ 0x0122, 0x0047, 0x0327 },		// G + cedilla
	{ 0x0123, 0x0067, 0x0327 },		// g + cedilla
	{ 0x011C, 0x0047, 0x0302 },		// G + circumflex
	{ 0x011D, 0x0067, 0x0302 },		// g + circumflex
	{ 0x0120, 0x0047, 0x0307 },		// G + dot above
	{ 0x0121, 0x0067, 0x0307 },		// g + dot above
	{ 0x1E20, 0x0047, 0x0304 },		// G + macron
	{ 0x1E21, 0x0067, 0x0304 },		// g + macron

	{ 0x1E2A, 0x0048, 0x032E },		// H + breve below
	{ 0x1E2B, 0x0068, 0x032E },		// h + breve below
	{ 0x1E28, 0x0048, 0x0327 },		// H + cedilla
	{ 0x1E29, 0x0068, 0x0327 },		// h + cedilla
	{ 0x0124, 0x0048, 0x0302 },		// H + circumflex
	{ 0x0125, 0x0068, 0x0302 },		// h + circumflex
	{ 0x1E26, 0x0048, 0x0308 },		// H + diaeresis
	{ 0x1E27, 0x0068, 0x0308 },		// h + diaeresis
	{ 0x1E22, 0x0048, 0x0307 },		// H + dot above
	{ 0x1E23, 0x0068, 0x0307 },		// h + dot above
	{ 0x1E24, 0x0048, 0x0323 },		// H + dot below
	{ 0x1E25, 0x0068, 0x0323 },		// h + dot below
	{ 0x1E96, 0x0068, 0x0331 },		// h + macron below -- lower-case only

	{ 0x00CD, 0x0049, 0x0301 },		// I + acute
	{ 0x00ED, 0x0069, 0x0301 },		// i + acute
	{ 0x012C, 0x0049, 0x0306 },		// I + breve
	{ 0x012D, 0x0069, 0x0306 },		// i + breve
	{ 0x00CE, 0x0049, 0x0302 },		// I + circumflex
	{ 0x00EE, 0x0069, 0x0302 },		// i + circumflex
	{ 0x00CF, 0x0049, 0x0308 },		// I + diaeresis
	{ 0x00EF, 0x0069, 0x0308 },		// i + diaeresis
	{ 0x1E2E, 0x00CF, 0x0301 },		// I + diaeresis + acute
	{ 0x1E2F, 0x00EF, 0x0301 },		// i + diaeresis + acute
	{ 0x00CC, 0x0049, 0x0300 },		// I + grave
	{ 0x00EC, 0x0069, 0x0300 },		// i + grave
	{ 0x012A, 0x0049, 0x0304 },		// I + macron
	{ 0x012B, 0x0069, 0x0304 },		// i + macron
	{ 0x012E, 0x0049, 0x0328 },		// I + ogonek
	{ 0x012F, 0x0069, 0x0328 },		// i + ogonek
	{ 0x0128, 0x0049, 0x0303 },		// I + tilde
	{ 0x0129, 0x0069, 0x0303 },		// i + tilde
	{ 0x1E2C, 0x0049, 0x0330 },		// I + tilde below
	{ 0x1E2D, 0x0069, 0x0330 },		// i + tilde below

	{ 0x0134, 0x004A, 0x0302 },		// J + circumflex
	{ 0x0135, 0x006A, 0x0302 },		// j + circumflex

	{ 0x1E30, 0x004B, 0x0301 },		// K + acute
	{ 0x1E31, 0x006B, 0x0301 },		// k + acute
	{ 0x0136, 0x004B, 0x0327 },		// K + cedilla
	{ 0x0137, 0x006B, 0x0327 },		// k + cedilla
	{ 0x1E32, 0x004B, 0x0323 },		// K + dot below
	{ 0x1E33, 0x006B, 0x0323 },		// k + dot below
	{ 0x1E34, 0x004B, 0x0331 },		// K + macron below
	{ 0x1E35, 0x006B, 0x0331 },		// k + macron below

	{ 0x0139, 0x004C, 0x0301 },		// L + acute
	{ 0x013A, 0x006C, 0x0301 },		// l + acute
	{ 0x013D, 0x004C, 0x030C },		// L + caron
	{ 0x013E, 0x006C, 0x030C },		// l + caron
	{ 0x013B, 0x004C, 0x0327 },		// L + cedilla
	{ 0x013C, 0x006C, 0x0327 },		// l + cedilla
	{ 0x1E3C, 0x004C, 0x032D },		// L + circumflex below
	{ 0x1E3D, 0x006C, 0x032D },		// l + circumflex below
	{ 0x1E36, 0x004C, 0x0323 },		// L + dot below
	{ 0x1E37, 0x006C, 0x0323 },		// l + dot below
	{ 0x1E38, 0x1E36, 0x0304 },		// L + dot below + macron
	{ 0x1E39, 0x1E37, 0x0304 },		// l + dot below + macron
	{ 0x1E3A, 0x004C, 0x0331 },		// L + macron below
	{ 0x1E3B, 0x006C, 0x0331 },		// l + macron below
	{ 0x013F, 0x004C, 0x00B7 },		// L + middle dot
	{ 0x0140, 0x006C, 0x00B7 },		// l + middle dot

	{ 0x1E3E, 0x004D, 0x0301 },		// M + acute
	{ 0x1E3F, 0x006D, 0x0301 },		// m + acute
	{ 0x1E40, 0x004D, 0x0307 },		// M + dot above
	{ 0x1E41, 0x006D, 0x0307 },		// m + dot above
	{ 0x1E42, 0x004D, 0x0323 },		// M + dot below
	{ 0x1E43, 0x006D, 0x0323 },		// m + dot below

	{ 0x0143, 0x004E, 0x0301 },		// N + acute
	{ 0x0144, 0x006E, 0x0301 },		// n + acute
	{ 0x0147, 0x004E, 0x030C },		// N + caron
	{ 0x0148, 0x006E, 0x030C },		// n + caron
	{ 0x0145, 0x004E, 0x0327 },		// N + cedilla
	{ 0x0146, 0x006E, 0x0327 },		// n + cedilla
	{ 0x1E4A, 0x004E, 0x032D },		// N + circumflex below
	{ 0x1E4B, 0x006E, 0x032D },		// n + circumflex below
	{ 0x1E44, 0x004E, 0x0307 },		// N + dot above
	{ 0x1E45, 0x006E, 0x0307 },		// n + dot above
	{ 0x1E46, 0x004E, 0x0323 },		// N + dot below
	{ 0x1E47, 0x006E, 0x0323 },		// n + dot below
	{ 0x1E48, 0x004E, 0x0331 },		// N + macron below
	{ 0x1E49, 0x006E, 0x0331 },		// n + macron below
	{ 0x00D1, 0x004E, 0x0303 },		// N + tilde
	{ 0x00F1, 0x006E, 0x0303 },		// n + tilde

	{ 0x00D3, 0x004F, 0x0301 },		// O + acute
	{ 0x00F3, 0x006F, 0x0301 },		// o + acute
	{ 0x014E, 0x004F, 0x0306 },		// O + breve
	{ 0x014F, 0x006F, 0x0306 },		// o + breve
	{ 0x00D4, 0x004F, 0x0302 },		// O + circumflex
	{ 0x00F4, 0x006F, 0x0302 },		// o + circumflex
	{ 0x00D6, 0x004F, 0x0308 },		// O + diaeresis
	{ 0x00F6, 0x006F, 0x0308 },		// o + diaeresis
	{ 0x0150, 0x004F, 0x030B },		// O + double acute
	{ 0x0151, 0x006F, 0x030B },		// o + double acute
	{ 0x00D2, 0x004F, 0x0300 },		// O + grave
	{ 0x00F2, 0x006F, 0x0300 },		// o + grave
	{ 0x014C, 0x004F, 0x0304 },		// O + macron
	{ 0x014D, 0x006F, 0x0304 },		// o + macron
	{ 0x1E52, 0x014C, 0x0301 },		// O + macron + acute
	{ 0x1E53, 0x014D, 0x0301 },		// o + macron + acute
	{ 0x1E50, 0x014C, 0x0300 },		// O + macron + grave
	{ 0x1E51, 0x014D, 0x0300 },		// o + macron + grave
	{ 0x00D5, 0x004F, 0x0303 },		// O + tilde
	{ 0x00F5, 0x006F, 0x0303 },		// o + tilde
	{ 0x1E4C, 0x00D5, 0x0301 },		// O + tilde + acute
	{ 0x1E4D, 0x00F5, 0x0301 },		// o + tilde + acute
	{ 0x1E4E, 0x00D5, 0x0308 },		// O + tilde + diaeresis
	{ 0x1E4F, 0x00F5, 0x0308 },		// o + tilde + diaeresis

	{ 0x1E54, 0x0050, 0x0301 },		// P + acute
	{ 0x1E55, 0x0070, 0x0301 },		// p + acute
	{ 0x1E56, 0x0050, 0x0307 },		// P + dot above
	{ 0x1E57, 0x0070, 0x0307 },		// p + dot above

	{ 0x0154, 0x0052, 0x0301 },		// R + acute
	{ 0x0155, 0x0072, 0x0301 },		// r + acute
	{ 0x0156, 0x0052, 0x0327 },		// R + cedilla
	{ 0x0157, 0x0072, 0x0327 },		// r + cedilla
	{ 0x0158, 0x0052, 0x030C },		// R + caron
	{ 0x0159, 0x0072, 0x030C },		// r + caron
	{ 0x1E58, 0x0052, 0x0307 },		// R + dot above
	{ 0x1E59, 0x0072, 0x0307 },		// r + dot above
	{ 0x1E5A, 0x0052, 0x0323 },		// R + dot below
	{ 0x1E5B, 0x0072, 0x0323 },		// r + dot below
	{ 0x1E5C, 0x1E5A, 0x0304 },		// R + dot below + macron
	{ 0x1E5D, 0x1E5B, 0x0304 },		// r + dot below + macron
	{ 0x1E5E, 0x0052, 0x0331 },		// R + macron below
	{ 0x1E5F, 0x0072, 0x0331 },		// r + macron below

	{ 0x015A, 0x0053, 0x0301 },		// S + acute
	{ 0x015B, 0x0073, 0x0301 },		// s + acute
	{ 0x1E64, 0x015A, 0x0307 },		// S + acute + dot above
	{ 0x1E65, 0x015B, 0x0307 },		// s + acute + dot above
	{ 0x015C, 0x0053, 0x0302 },		// S + circumflex
	{ 0x015D, 0x0073, 0x0302 },		// s + circumflex
	{ 0x015E, 0x0053, 0x0327 },		// S + cedilla
	{ 0x015F, 0x0073, 0x0327 },		// s + cedilla
	{ 0x0160, 0x0053, 0x030C },		// S + caron
	{ 0x0161, 0x0073, 0x030C },		// s + caron
	{ 0x1E66, 0x0160, 0x0307 },		// S + caron + dot above
	{ 0x1E67, 0x0161, 0x0307 },		// s + caron + dot above
	{ 0x1E60, 0x0053, 0x0307 },		// S + dot above
	{ 0x1E61, 0x0073, 0x0307 },		// s + dot above
	{ 0x1E68, 0x1E60, 0x0323 },		// S + dot above + dot below
	{ 0x1E69, 0x1E61, 0x0323 },		// s + dot above + dot below
	{ 0x1E62, 0x0053, 0x0323 },		// S + dot below
	{ 0x1E63, 0x0073, 0x0323 },		// s + dot below
	{ 0x1E68, 0x1E62, 0x0307 },		// S + dot below + dot above
	{ 0x1E69, 0x1E63, 0x0307 },		// s + dot below + dot above

	{ 0x0162, 0x0054, 0x0327 },		// T + cedilla
	{ 0x0163, 0x0074, 0x0327 },		// t + cedilla
	{ 0x0164, 0x0054, 0x030C },		// T + caron
	{ 0x0165, 0x0074, 0x030C },		// t + caron
	{ 0x1E70, 0x0054, 0x032D },		// T + circumflex below
	{ 0x1E71, 0x0074, 0x032D },		// t + circumflex below
	{ 0x1E97, 0x0074, 0x0308 },		// t + diaeresis -- lower-case only
	{ 0x1E6A, 0x0054, 0x0307 },		// T + dot above
	{ 0x1E6B, 0x0074, 0x0307 },		// t + dot above
	{ 0x1E6C, 0x0054, 0x0323 },		// T + dot below
	{ 0x1E6D, 0x0074, 0x0323 },		// t + dot below
	{ 0x1E6E, 0x0054, 0x0331 },		// T + macron below
	{ 0x1E6F, 0x0074, 0x0331 },		// t + macron below

	{ 0x00DA, 0x0055, 0x0301 },		// U + acute
	{ 0x00FA, 0x0075, 0x0301 },		// u + acute
	{ 0x016C, 0x0055, 0x0306 },		// U + breve
	{ 0x016D, 0x0075, 0x0306 },		// u + breve
	{ 0x00DB, 0x0055, 0x0302 },		// U + circumflex
	{ 0x00FB, 0x0075, 0x0302 },		// u + circumflex
	{ 0x1E76, 0x0055, 0x032D },		// U + circumflex below
	{ 0x1E77, 0x0075, 0x032D },		// u + circumflex below
	{ 0x00DC, 0x0055, 0x0308 },		// U + diaeresis
	{ 0x00FC, 0x0075, 0x0308 },		// u + diaeresis
	{ 0x1E72, 0x0055, 0x0324 },		// U + diaeresis below
	{ 0x1E73, 0x0075, 0x0324 },		// u + diaeresis below
	{ 0x0170, 0x0055, 0x030B },		// U + double acute
	{ 0x0171, 0x0075, 0x030B },		// u + double acute
	{ 0x00D9, 0x0055, 0x0300 },		// U + grave
	{ 0x00F9, 0x0075, 0x0300 },		// u + grave
	{ 0x016A, 0x0055, 0x0304 },		// U + macron
	{ 0x016B, 0x0075, 0x0304 },		// u + macron
	{ 0x1E7A, 0x016A, 0x0308 },		// U + macron + diaeresis
	{ 0x1E7B, 0x016B, 0x0308 },		// u + macron + diaeresis
	{ 0x0172, 0x0055, 0x0328 },		// U + ogonek
	{ 0x0173, 0x0075, 0x0328 },		// u + ogonek
	{ 0x016E, 0x0055, 0x030A },		// U + ring above
	{ 0x016F, 0x0075, 0x030A },		// u + ring above
	{ 0x0168, 0x0055, 0x0303 },		// U + tilde
	{ 0x0169, 0x0075, 0x0303 },		// u + tilde
	{ 0x1E78, 0x0168, 0x0301 },		// U + tilde + acute
	{ 0x1E79, 0x0169, 0x0301 },		// u + tilde + acute
	{ 0x1E74, 0x0055, 0x0330 },		// U + tilde below
	{ 0x1E75, 0x0075, 0x0330 },		// u + tilde below

	{ 0x1E7E, 0x0056, 0x0323 },		// V + dot below
	{ 0x1E7F, 0x0076, 0x0323 },		// v + dot below
	{ 0x1E7C, 0x0056, 0x0303 },		// V + tilde
	{ 0x1E7D, 0x0076, 0x0303 },		// v + tilde

	{ 0x1E82, 0x0057, 0x0301 },		// W + acute
	{ 0x1E83, 0x0077, 0x0301 },		// w + acute
	{ 0x0174, 0x0057, 0x0302 },		// W + circumflex
	{ 0x0175, 0x0077, 0x0302 },		// w + circumflex
	{ 0x1E84, 0x0057, 0x0308 },		// W + diaeresis
	{ 0x1E85, 0x0077, 0x0308 },		// w + diaeresis
	{ 0x1E86, 0x0057, 0x0307 },		// W + dot above
	{ 0x1E87, 0x0077, 0x0307 },		// w + dot above
	{ 0x1E88, 0x0057, 0x0323 },		// W + dot below
	{ 0x1E89, 0x0077, 0x0323 },		// w + dot below
	{ 0x1E80, 0x0057, 0x0300 },		// W + grave
	{ 0x1E81, 0x0077, 0x0300 },		// w + grave
	{ 0x1E98, 0x0077, 0x030A },		// w + ring above -- lower-case only

	{ 0x1E8A, 0x0058, 0x0307 },		// X + dot above
	{ 0x1E8B, 0x0078, 0x0307 },		// x + dot above
	{ 0x1E8C, 0x0058, 0x0308 },		// X + diaeresis
	{ 0x1E8D, 0x0078, 0x0308 },		// x + diaeresis

	{ 0x00FD, 0x0079, 0x0301 },		// y + acute
	{ 0x00DD, 0x0059, 0x0301 },		// Y + acute
	{ 0x0178, 0x0059, 0x0308 },		// Y + diaeresis
	{ 0x00FF, 0x0079, 0x0308 },		// y + diaeresis
	{ 0x0176, 0x0059, 0x0302 },		// Y + circumflex
	{ 0x0177, 0x0079, 0x0302 },		// y + circumflex
	{ 0x1E8E, 0x0059, 0x0307 },		// Y + dot above
	{ 0x1E8F, 0x0079, 0x0307 },		// y + dot above
	{ 0x1EF4, 0x0059, 0x0323 },		// Y + dot below
	{ 0x1EF5, 0x0079, 0x0323 },		// y + dot below
	{ 0x1EF2, 0x0059, 0x0300 },		// Y + grave
	{ 0x1EF3, 0x0079, 0x0300 },		// y + grave
	{ 0x1EF6, 0x0059, 0x0309 },		// Y + hook above
	{ 0x1EF7, 0x0079, 0x0309 },		// y + hook above
	{ 0x1E99, 0x0079, 0x030A },		// y + ring above -- lower-case only
	{ 0x1EF8, 0x0059, 0x0303 },		// Y + tilde
	{ 0x1EF9, 0x0079, 0x0303 },		// y + tilde

	{ 0x0179, 0x005A, 0x0301 },		// Z + acute
	{ 0x017A, 0x007A, 0x0301 },		// z + acute
	{ 0x017D, 0x005A, 0x030C },		// Z + caron
	{ 0x017E, 0x007A, 0x030C },		// z + caron
	{ 0x1E90, 0x0060, 0x0302 },		// Z + circumflex
	{ 0x1E91, 0x0080, 0x0302 },		// z + circumflex
	{ 0x017B, 0x005A, 0x0307 },		// Z + dot above
	{ 0x017C, 0x007A, 0x0307 },		// z + dot above
	{ 0x1E92, 0x0060, 0x0323 },		// Z + dot below
	{ 0x1E93, 0x0080, 0x0323 },		// z + dot below
	{ 0x1E94, 0x0060, 0x0331 },		// Z + macron below
	{ 0x1E95, 0x0080, 0x0331 },		// z + macron below

	{ 0x1E9B, 0x017F, 0x0307 },		// Gaelic 'f' + dot above

	// The following old-style characters will be replaced with more standard
	// symbols used throughout the Unicode tables.

	{ 0x0308, 0x00A8,      0 },		// diaeresis
	{ 0x0304, 0x00AF,      0 },		// macron
	{ 0x0301, 0x00B4,      0 },		// acute
	{ 0x0327, 0x00B8,      0 },		// cedilla
	{ 0x002D, 0x2010,      0 },		// hyphen
	{ 0x002D, 0x2011,      0 },		// non-breaking hyphen
	{ 0x201C, 0x201F,      0 },		// reversed left double quote
	{ 0x002D, 0x00AD,      0 }		// convert soft-hypens into hypens
};


/////////////////////////////////////////////////////////////////////////////
//
UtfSymbolTable_t g_UtfSymbolTable[] =
{
	// space
	{ 0x0020, 0x0020,      0,      0,      0, 0 },

	// !
	{ 0x0021, 0x0021,      0,      0,      0, 0 },

	// "
	{ 0x0022, 0x0022,      0,      0,      0, 0 },

	// #
	{ 0x0023, 0x0023,      0,      0,      0, 0 },

	// $
	{ 0x0024, 0x0024,      0,      0,      0, 0 },

	// %
	{ 0x0025, 0x0025,      0,      0,      0, 0 },

	// &
	{ 0x0026, 0x0026,      0,      0,      0, 0 },

	// '
	{ 0x0027, 0x0027,      0,      0,      0, 0 },

	// (
	{ 0x0028, 0x0028,      0,      0,      0, 0 },

	// )
	{ 0x0029, 0x0029,      0,      0,      0, 0 },

	// *
	{ 0x002A, 0x002A,      0,      0,      0, 0 },

	// +
	{ 0x002B, 0x002B,      0,      0,      0, 0 },

	// ,
	{ 0x002C, 0x002C,      0,      0,      0, 0 },

	// -
	{ 0x002D, 0x002D,      0,      0,      0, 0 },

	// .
	{ 0x002E, 0x002E,      0,      0,      0, 0 },

	// /
	{ 0x002F, 0x002F,      0,      0,      0, 0 },

	// 0
	{ 0x0030, 0x0030,      0,      0,      0, 0 },

	// 1
	{ 0x0031, 0x0031,      0,      0,      0, 0 },

	// 2
	{ 0x0032, 0x0032,      0,      0,      0, 0 },

	// 3
	{ 0x0033, 0x0033,      0,      0,      0, 0 },

	// 4
	{ 0x0034, 0x0034,      0,      0,      0, 0 },

	// 5
	{ 0x0035, 0x0035,      0,      0,      0, 0 },

	// 6
	{ 0x0036, 0x0036,      0,      0,      0, 0 },

	// 7
	{ 0x0037, 0x0037,      0,      0,      0, 0 },

	// 8
	{ 0x0038, 0x0038,      0,      0,      0, 0 },

	// 9
	{ 0x0039, 0x0039,      0,      0,      0, 0 },

	// :
	{ 0x003A, 0x003A,      0,      0,      0, 0 },

	// ;
	{ 0x003B, 0x003B,      0,      0,      0, 0 },

	// <
	{ 0x003C, 0x003C,      0,      0,      0, 0 },

	// =
	{ 0x003D, 0x003D,      0,      0,      0, 0 },

	// >
	{ 0x003E, 0x003E,      0,      0,      0, 0 },

	// ?
	{ 0x003F, 0x003F,      0,      0,      0, 0 },

	// @
	{ 0x0040, 0x0040,      0,      0,      0, 0 },

	// A
	{ 0x0041, 0x0041,      0,      0, 0x0061, UtfFlag_Uppercase },

	// B
	{ 0x0042, 0x0042,      0,      0, 0x0062, UtfFlag_Uppercase },

	// C
	{ 0x0043, 0x0043,      0,      0, 0x0063, UtfFlag_Uppercase },

	// D
	{ 0x0044, 0x0044,      0,      0, 0x0064, UtfFlag_Uppercase },

	// E
	{ 0x0045, 0x0045,      0,      0, 0x0065, UtfFlag_Uppercase },

	// F
	{ 0x0046, 0x0046,      0,      0, 0x0066, UtfFlag_Uppercase },

	// G
	{ 0x0047, 0x0047,      0,      0, 0x0067, UtfFlag_Uppercase },

	// H
	{ 0x0048, 0x0048,      0,      0, 0x0068, UtfFlag_Uppercase },

	// I
	{ 0x0049, 0x0049,      0,      0, 0x0069, UtfFlag_Uppercase },

	// J
	{ 0x004A, 0x004A,      0,      0, 0x006A, UtfFlag_Uppercase },

	// K
	{ 0x004B, 0x004B,      0,      0, 0x006B, UtfFlag_Uppercase },

	// L
	{ 0x004C, 0x004C,      0,      0, 0x006C, UtfFlag_Uppercase },

	// M
	{ 0x004D, 0x004D,      0,      0, 0x006D, UtfFlag_Uppercase },

	// N
	{ 0x004E, 0x004E,      0,      0, 0x006E, UtfFlag_Uppercase },

	// O
	{ 0x004F, 0x004F,      0,      0, 0x006F, UtfFlag_Uppercase },

	// P
	{ 0x0050, 0x0050,      0,      0, 0x0070, UtfFlag_Uppercase },

	// Q
	{ 0x0051, 0x0051,      0,      0, 0x0071, UtfFlag_Uppercase },

	// R
	{ 0x0052, 0x0052,      0,      0, 0x0072, UtfFlag_Uppercase },

	// S
	{ 0x0053, 0x0053,      0,      0, 0x0073, UtfFlag_Uppercase },

	// T
	{ 0x0054, 0x0054,      0,      0, 0x0074, UtfFlag_Uppercase },

	// U
	{ 0x0055, 0x0055,      0,      0, 0x0075, UtfFlag_Uppercase },

	// V
	{ 0x0056, 0x0056,      0,      0, 0x0076, UtfFlag_Uppercase },

	// W
	{ 0x0057, 0x0057,      0,      0, 0x0077, UtfFlag_Uppercase },

	// X
	{ 0x0058, 0x0058,      0,      0, 0x0078, UtfFlag_Uppercase },

	// Y
	{ 0x0059, 0x0059,      0,      0, 0x0079, UtfFlag_Uppercase },

	// Z
	{ 0x005A, 0x005A,      0,      0, 0x007A, UtfFlag_Uppercase },

	// [
	{ 0x005B, 0x005B,      0,      0,      0, 0 },

	// '\'
	{ 0x005C, 0x005C,      0,      0,      0, 0 },

	// ]
	{ 0x005D, 0x005D,      0,      0,      0, 0 },

	// ^
	{ 0x005E, 0x005E,      0,      0,      0, 0 },

	// _
	{ 0x005F, 0x005F,      0,      0,      0, 0 },

	// `
	{ 0x0060, 0x0060,      0,      0,      0, 0 },

	// a
	{ 0x0061, 0x0061,      0,      0, 0x0041, UtfFlag_Lowercase },

	// b
	{ 0x0062, 0x0062,      0,      0, 0x0042, UtfFlag_Lowercase },

	// c
	{ 0x0063, 0x0063,      0,      0, 0x0043, UtfFlag_Lowercase },

	// d
	{ 0x0064, 0x0064,      0,      0, 0x0044, UtfFlag_Lowercase },

	// e
	{ 0x0065, 0x0065,      0,      0, 0x0045, UtfFlag_Lowercase },

	// f
	{ 0x0066, 0x0066,      0,      0, 0x0046, UtfFlag_Lowercase },

	// g
	{ 0x0067, 0x0067,      0,      0, 0x0047, UtfFlag_Lowercase },

	// h
	{ 0x0068, 0x0068,      0,      0, 0x0048, UtfFlag_Lowercase },

	// i
	{ 0x0069, 0x0069,      0,      0, 0x0049, UtfFlag_Lowercase },

	// j
	{ 0x006A, 0x006A,      0,      0, 0x004A, UtfFlag_Lowercase },

	// k
	{ 0x006B, 0x006B,      0,      0, 0x004B, UtfFlag_Lowercase },

	// l
	{ 0x006C, 0x006C,      0,      0, 0x004C, UtfFlag_Lowercase },

	// m
	{ 0x006D, 0x006D,      0,      0, 0x004D, UtfFlag_Lowercase },

	// n
	{ 0x006E, 0x006E,      0,      0, 0x004E, UtfFlag_Lowercase },

	// o
	{ 0x006F, 0x006F,      0,      0, 0x004F, UtfFlag_Lowercase },

	// p
	{ 0x0070, 0x0070,      0,      0, 0x0050, UtfFlag_Lowercase },

	// q
	{ 0x0071, 0x0071,      0,      0, 0x0051, UtfFlag_Lowercase },

	// r
	{ 0x0072, 0x0072,      0,      0, 0x0052, UtfFlag_Lowercase },

	// s
	{ 0x0073, 0x0073,      0,      0, 0x0053, UtfFlag_Lowercase },

	// t
	{ 0x0074, 0x0074,      0,      0, 0x0054, UtfFlag_Lowercase },

	// u
	{ 0x0075, 0x0075,      0,      0, 0x0055, UtfFlag_Lowercase },

	// v
	{ 0x0076, 0x0076,      0,      0, 0x0056, UtfFlag_Lowercase },

	// w
	{ 0x0077, 0x0077,      0,      0, 0x0057, UtfFlag_Lowercase },

	// x
	{ 0x0078, 0x0078,      0,      0, 0x0058, UtfFlag_Lowercase },

	// y
	{ 0x0079, 0x0079,      0,      0, 0x0059, UtfFlag_Lowercase },

	// z
	{ 0x007A, 0x007A,      0,      0, 0x005A, UtfFlag_Lowercase },

	// {
	{ 0x007B, 0x007B,      0,      0,      0, 0 },

	// |
	{ 0x007C, 0x007C,      0,      0,      0, 0 },

	// }
	{ 0x007D, 0x007D,      0,      0,      0, 0 },

	// ~
	{ 0x007E, 0x007E,      0,      0,      0, 0 },

	// non-breaking space
	{ 0x00A0, 0x0020,      0,      0,      0, 0 },

	// inverted exclamation mark
	{ 0x00A1, 0x00A1,      0,      0,      0, 0 },

	// cent sign
	{ 0x00A2, 0x00A2,      0,      0,      0, 0 },

	// pound sign
	{ 0x00A3, 0x00A3,      0,      0,      0, 0 },

	// currency sign
	{ 0x00A4, 0x00A4,      0,      0,      0, 0 },

	// yen sign
	{ 0x00A5, 0x00A5,      0,      0,      0, 0 },

	// broken bar, old version of '|'
	{ 0x00A6, 0x00A6,      0,      0,      0, 0 },

	// section sign
	{ 0x00A7, 0x00A7,      0,      0,      0, 0 },

	// diaeresis -- this should be replaced during composition
	{ 0x00A8, 0x0308,      0,      0,      0, 0 },

	// copyright sign (C)
	{ 0x00A9, 0x00A9,      0,      0,      0, 0 },

	// Spanish feminine ordinal
	{ 0x00AA, 0x00AA,      0,      0,      0, 0 },

	// left guillemet
	{ 0x00AB, 0x00AB,      0,      0,      0, 0 },

	// NOT sign
	{ 0x00AC, 0x00AC,      0,      0,      0, 0 },

	// soft hyphen -- not supported, should be stripped out when importing text
//	{ 0x00AD, 0x002D,      0,      0,      0, 0 },

	// registered sign (R)
	{ 0x00AE, 0x00AE,      0,      0,      0, 0 },

	// macron -- this should be replaced during composition
	{ 0x00AF, 0x0304,      0,      0,      0, 0 },

	// degree sign
	{ 0x00B0, 0x00B0,      0,      0,      0, 0 },

	// plus-minus sign
	{ 0x00B1, 0x00B1,      0,      0,      0, 0 },

	// super-script 2
	{ 0x00B2, 0x00B2,      0,      0,      0, 0 },

	// super-script 3
	{ 0x00B3, 0x00B3,      0,      0,      0, 0 },

	// acute accent
	{ 0x00B4, 0x0301,      0,      0,      0, 0 },

	// micro sign
	{ 0x00B5, 0x00B5,      0,      0,      0, 0 },

	// pilcrow/paragraph sign
	{ 0x00B6, 0x00B6,      0,      0,      0, 0 },

	// middle dot
	{ 0x00B7, 0x00B7,      0,      0,      0, 0 },

	// cedilla
	{ 0x00B8, 0x0327,      0,      0,      0, 0 },

	// super-script 1
	{ 0x00B9, 0x00B9,      0,      0,      0, 0 },

	// Spanish masculine ordinal
	{ 0x00BA, 0x00BA,      0,      0,      0, 0 },

	// right guillemet
	{ 0x00BB, 0x00BB,      0,      0,      0, 0 },

	// one-quarter fraction
	{ 0x00BC, 0x00BC,      0,      0,      0, 0 },

	// one-half fraction
	{ 0x00BD, 0x00BD,      0,      0,      0, 0 },

	// three-quarter fraction
	{ 0x00BE, 0x00BE,      0,      0,      0, 0 },

	// inverted question mark
	{ 0x00BF, 0x00BF,      0,      0,      0, 0 },

	// A + grave
	{ 0x00C0, 0x0041, 0x0300,      0, 0x00E0, UtfFlag_Uppercase },

	// A + acute
	{ 0x00C1, 0x0041, 0x0301,      0, 0x00E1, UtfFlag_Uppercase },

	// A + circumflex
	{ 0x00C2, 0x0041, 0x0302,      0, 0x00E2, UtfFlag_Uppercase },

	// A + tilde
	{ 0x00C3, 0x0041, 0x0303,      0, 0x00E3, UtfFlag_Uppercase },

	// A + diaeresis
	{ 0x00C4, 0x0041, 0x0308,      0, 0x00E4, UtfFlag_Uppercase },

	// A + ring above
	{ 0x00C5, 0x0041, 0x030A,      0, 0x00E5, UtfFlag_Uppercase },

	// 'AE'
	{ 0x00C6, 0x00C6,      0,      0, 0x00E6, UtfFlag_Uppercase },

	// C + cedilla
	{ 0x00C7, 0x0043, 0x0327,      0, 0x00E7, UtfFlag_Uppercase },

	// E + grave
	{ 0x00C8, 0x0045, 0x0300,      0, 0x00E8, UtfFlag_Uppercase },

	// E + acute
	{ 0x00C9, 0x0045, 0x0301,      0, 0x00E9, UtfFlag_Uppercase },

	// E + circumflex
	{ 0x00CA, 0x0045, 0x0302,      0, 0x00EA, UtfFlag_Uppercase },

	// E + diaeresis
	{ 0x00CB, 0x0045, 0x0308,      0, 0x00EB, UtfFlag_Uppercase },

	// I + grave
	{ 0x00CC, 0x0049, 0x0300,      0, 0x00EC, UtfFlag_Uppercase },

	// I + acute
	{ 0x00CD, 0x0049, 0x0301,      0, 0x00ED, UtfFlag_Uppercase },

	// I + circumflex
	{ 0x00CE, 0x0049, 0x0302,      0, 0x00EE, UtfFlag_Uppercase },

	// I + diaeresis
	{ 0x00CF, 0x0049, 0x0308,      0, 0x00EF, UtfFlag_Uppercase },

	// Latin letter 'ETH'
	{ 0x00D0, 0x00D0,      0,      0, 0x00F0, UtfFlag_Uppercase },

	// N + tilde
	{ 0x00D1, 0x004E, 0x0303,      0, 0x00F1, UtfFlag_Uppercase },

	// O + grave
	{ 0x00D2, 0x004F, 0x0300,      0, 0x00F2, UtfFlag_Uppercase },

	// O + acute
	{ 0x00D3, 0x004F, 0x0301,      0, 0x00F3, UtfFlag_Uppercase },

	// O + circumflex
	{ 0x00D4, 0x004F, 0x0302,      0, 0x00F4, UtfFlag_Uppercase },

	// O + tilde
	{ 0x00D5, 0x004F, 0x0303,      0, 0x00F5, UtfFlag_Uppercase },

	// O + diaeresis
	{ 0x00D6, 0x004F, 0x0308,      0, 0x00F6, UtfFlag_Uppercase },

	// multiplication sign
	{ 0x00D7, 0x00D7,      0,      0,      0, 0 },

	// O + stroke
	{ 0x00D8, 0x00D8,      0,      0, 0x00F8, UtfFlag_Uppercase },

	// U + grave
	{ 0x00D9, 0x0055, 0x0300,      0, 0x00F9, UtfFlag_Uppercase },

	// U + acute
	{ 0x00DA, 0x0055, 0x0301,      0, 0x00FA, UtfFlag_Uppercase },

	// U + circumflex
	{ 0x00DB, 0x0055, 0x0302,      0, 0x00FB, UtfFlag_Uppercase },

	// U + diaeresis
	{ 0x00DC, 0x0055, 0x0308,      0, 0x00FC, UtfFlag_Uppercase },

	// Y + acute
	{ 0x00DD, 0x0059, 0x0301,      0, 0x00FD, UtfFlag_Uppercase },

	// Latin letter 'THORN'
	{ 0x00DE, 0x00DE,      0,      0, 0x00FE, UtfFlag_Uppercase },

	// German 'ss' -- there is no upper-case mapping of this
	// so we set its case mapping to map back to itself
	{ 0x00DF, 0x00DF,      0,      0, 0x00DF, UtfFlag_Uppercase },

	// a + grave
	{ 0x00E0, 0x0061, 0x0300,      0, 0x00C0, UtfFlag_Lowercase },

	// a + acute
	{ 0x00E1, 0x0061, 0x0301,      0, 0x00C1, UtfFlag_Lowercase },

	// a + circumflex
	{ 0x00E2, 0x0061, 0x0302,      0, 0x00C2, UtfFlag_Lowercase },

	// a + tilde
	{ 0x00E3, 0x0061, 0x0303,      0, 0x00C3, UtfFlag_Lowercase },

	// a + diaeresis
	{ 0x00E4, 0x0061, 0x0308,      0, 0x00C4, UtfFlag_Lowercase },

	// a + ring above
	{ 0x00E5, 0x0061, 0x030A,      0, 0x00C5, UtfFlag_Lowercase },

	// 'ae'
	{ 0x00E6, 0x00E6,      0,      0, 0x00C6, UtfFlag_Lowercase },

	// c + cedilla
	{ 0x00E7, 0x0063, 0x0327,      0, 0x00C7, UtfFlag_Lowercase },

	// e + grave
	{ 0x00E8, 0x0065, 0x0300,      0, 0x00C8, UtfFlag_Lowercase },

	// e + acute
	{ 0x00E9, 0x0065, 0x0301,      0, 0x00C9, UtfFlag_Lowercase },

	// e + circumflex
	{ 0x00EA, 0x0065, 0x0302,      0, 0x00CA, UtfFlag_Lowercase },

	// e + diaeresis
	{ 0x00EB, 0x0065, 0x0308,      0, 0x00CB, UtfFlag_Lowercase },

	// i + grave
	{ 0x00EC, 0xFFD0, 0x0300,      0, 0x00CC, UtfFlag_Lowercase },

	// i + acute
	{ 0x00ED, 0xFFD0, 0x0301,      0, 0x00CD, UtfFlag_Lowercase },

	// i + circumflex
	{ 0x00EE, 0xFFD0, 0x0302,      0, 0x00CE, UtfFlag_Lowercase },

	// i + diaeresis
	{ 0x00EF, 0xFFD0, 0x0308,      0, 0x00CF, UtfFlag_Lowercase },

	// Latin letter 'eth'
	{ 0x00F0, 0x00F0,      0,      0, 0x00D0, UtfFlag_Lowercase },

	// n + tilde
	{ 0x00F1, 0x006E, 0x0303,      0, 0x00D1, UtfFlag_Lowercase },

	// o + grave
	{ 0x00F2, 0x006F, 0x0300,      0, 0x00D2, UtfFlag_Lowercase },

	// o + acute
	{ 0x00F3, 0x006F, 0x0301,      0, 0x00D3, UtfFlag_Lowercase },

	// o + circumflex
	{ 0x00F4, 0x006F, 0x0302,      0, 0x00D4, UtfFlag_Lowercase },

	// o + tilde
	{ 0x00F5, 0x006F, 0x0303,      0, 0x00D5, UtfFlag_Lowercase },

	// o + diaeresis
	{ 0x00F6, 0x006F, 0x0308,      0, 0x00D6, UtfFlag_Lowercase },

	// division sign
	{ 0x00F7, 0x00F7,      0,      0,      0, 0 },

	// o + stroke
	{ 0x00F8, 0x00F8,      0,      0, 0x00D8, UtfFlag_Lowercase },

	// u + grave
	{ 0x00F9, 0x0075, 0x0300,      0, 0x00D9, UtfFlag_Lowercase },

	// u + acute
	{ 0x00FA, 0x0075, 0x0301,      0, 0x00DA, UtfFlag_Lowercase },

	// u + circumflex
	{ 0x00FB, 0x0075, 0x0302,      0, 0x00DB, UtfFlag_Lowercase },

	// u + diaeresis
	{ 0x00FC, 0x0075, 0x0308,      0, 0x00DC, UtfFlag_Lowercase },

	// y + acute
	{ 0x00FD, 0x0079, 0x0301,      0, 0x00DD, UtfFlag_Lowercase },

	// Latin letter 'thorn'
	{ 0x00FE, 0x00FE,      0,      0, 0x00DE, UtfFlag_Lowercase },

	// y + diaeresis -- uppercase is in different part of table
	{ 0x00FF, 0x0079, 0x0308,      0, 0x0178, UtfFlag_Lowercase },

	// A + macron
	{ 0x0100, 0x0041, 0x0304,      0, 0x0101, UtfFlag_Uppercase },

	// a + macron
	{ 0x0101, 0x0061, 0x0304,      0, 0x0100, UtfFlag_Lowercase },

	// A + breve
	{ 0x0102, 0x0041, 0x0306,      0, 0x0103, UtfFlag_Uppercase },

	// a + breve
	{ 0x0103, 0x0061, 0x0306,      0, 0x0102, UtfFlag_Lowercase },

	// A + ogonek
	{ 0x0104, 0x0041, 0x0328,      0, 0x0105, UtfFlag_Uppercase },

	// a + ogonek
	{ 0x0105, 0x0061, 0x0328,      0, 0x0104, UtfFlag_Lowercase },

	// C + acute
	{ 0x0106, 0x0043, 0x0301,      0, 0x0107, UtfFlag_Uppercase },

	// c + acute
	{ 0x0107, 0x0063, 0x0301,      0, 0x0106, UtfFlag_Lowercase },

	// C + circumflex
	{ 0x0108, 0x0043, 0x0302,      0, 0x0109, UtfFlag_Uppercase },

	// c + circumflex
	{ 0x0109, 0x0063, 0x0302,      0, 0x0108, UtfFlag_Lowercase },

	// C + dot above
	{ 0x010A, 0x0043, 0x0307,      0, 0x010B, UtfFlag_Uppercase },

	// c + dot above
	{ 0x010B, 0x0063, 0x0307,      0, 0x010A, UtfFlag_Lowercase },

	// C + caron
	{ 0x010C, 0x0043, 0x030C,      0, 0x010D, UtfFlag_Uppercase },

	// c + caron
	{ 0x010D, 0x0063, 0x030C,      0, 0x010C, UtfFlag_Lowercase },

	// D + caron
	{ 0x010E, 0x0044, 0x030C,      0, 0x010F, UtfFlag_Uppercase },

	// d + caron
	{ 0x010F, 0x010F,      0,      0, 0x010E, UtfFlag_Lowercase },

	// D + stroke
	{ 0x0110, 0x0110,      0,      0, 0x0111, UtfFlag_Uppercase },

	// d + stroke
	{ 0x0111, 0x0111,      0,      0, 0x0110, UtfFlag_Lowercase },

	// E + macron
	{ 0x0112, 0x0045, 0x0304,      0, 0x0113, UtfFlag_Uppercase },

	// e + macron
	{ 0x0113, 0x0065, 0x0304,      0, 0x0112, UtfFlag_Lowercase },

	// E + breve
	{ 0x0114, 0x0045, 0x0306,      0, 0x0115, UtfFlag_Uppercase },

	// e + breve
	{ 0x0115, 0x0065, 0x0306,      0, 0x0114, UtfFlag_Lowercase },

	// E + dot above
	{ 0x0116, 0x0045, 0x0307,      0, 0x0117, UtfFlag_Uppercase },

	// e + dot above
	{ 0x0117, 0x0065, 0x0307,      0, 0x0116, UtfFlag_Lowercase },

	// E + ogonek
	{ 0x0118, 0x0045, 0x0328,      0, 0x0119, UtfFlag_Uppercase },

	// e + ogonek
	{ 0x0119, 0x0065, 0x0328,      0, 0x0118, UtfFlag_Lowercase },

	// E + caron
	{ 0x011A, 0x0045, 0x030C,      0, 0x011B, UtfFlag_Uppercase },

	// e + caron
	{ 0x011B, 0x0065, 0x030C,      0, 0x011A, UtfFlag_Lowercase },

	// G + circumflex
	{ 0x011C, 0x0047, 0x0302,      0, 0x011D, UtfFlag_Uppercase },

	// g + circumflex
	{ 0x011D, 0x0067, 0x0302,      0, 0x011C, UtfFlag_Lowercase },

	// G + breve
	{ 0x011E, 0x0047, 0x0306,      0, 0x011F, UtfFlag_Uppercase },

	// g + breve
	{ 0x011F, 0x0067, 0x0306,      0, 0x011E, UtfFlag_Lowercase },

	// G + dot above
	{ 0x0120, 0x0047, 0x0307,      0, 0x0121, UtfFlag_Uppercase },

	// g + dot above
	{ 0x0121, 0x0067, 0x0307,      0, 0x0120, UtfFlag_Lowercase },

	// G + cedilla (cedilla is drawn as comma below character)
	{ 0x0122, 0x0047, 0x0326,      0, 0x0123, UtfFlag_Uppercase },

	// g + cedilla (cedilla is drawn as apostrophy over character)
	{ 0x0123, 0x0067, 0x0312,      0, 0x0122, UtfFlag_Lowercase },

	// H + circumflex
	{ 0x0124, 0x0048, 0x0302,      0, 0x0125, UtfFlag_Uppercase },

	// h + circumflex
	{ 0x0125, 0x0068, 0x0302,      0, 0x0124, UtfFlag_Lowercase },

	// H + stroke
//	{ 0x0126, 0x0126,      0,      0, 0x0127, UtfFlag_Uppercase },

	// h + stroke
//	{ 0x0127, 0x0127,      0,      0, 0x0126, UtfFlag_Lowercase },

	// I + tilde
	{ 0x0128, 0x0049, 0x0303,      0, 0x0129, UtfFlag_Uppercase },

	// i + tilde
	{ 0x0129, 0xFFD0, 0x0303,      0, 0x0128, UtfFlag_Lowercase },

	// I + macron
	{ 0x012A, 0x0049, 0x0304,      0, 0x012B, UtfFlag_Uppercase },

	// i + macron
	{ 0x012B, 0xFFD0, 0x0304,      0, 0x012A, UtfFlag_Lowercase },

	// I + breve
	{ 0x012C, 0x0049, 0x0306,      0, 0x012D, UtfFlag_Uppercase },

	// i + breve
	{ 0x012D, 0xFFD0, 0x0306,      0, 0x012C, UtfFlag_Lowercase },

	// I + ogonek
	{ 0x012E, 0x0049, 0x0328,      0, 0x012F, UtfFlag_Uppercase },

	// i + ogonek
	{ 0x012F, 0x0069, 0x0328,      0, 0x012E, UtfFlag_Lowercase },

	// I + dot above -- lowercase is 0x0069
//	{ 0x0130, 0x0049, 0x0307,      0, 0x0069, UtfFlag_Uppercase },

	// dotless i
//	{ 0x0131, 0xFFD0,      0,      0,      0, UtfFlag_Lowercase },

	// IJ
//	{ 0x0132,       ,      0,      0, 0x0133, UtfFlag_Uppercase },

	// ij
//	{ 0x0133,       ,      0,      0, 0x0132, UtfFlag_Lowercase },

	// J + circumflex
	{ 0x0134, 0x004A, 0x0302,      0, 0x0135, UtfFlag_Uppercase },

	// j + circumflex
	{ 0x0135, 0xFFD1, 0x0302,      0, 0x0134, UtfFlag_Lowercase },

	// K + cedilla (cedilla is drawn as comma below character)
	{ 0x0136, 0x004B, 0x0326,      0, 0x0137, UtfFlag_Uppercase },

	// k + cedilla (cedilla is drawn as comma below character)
	{ 0x0137, 0x006B, 0x0326,      0, 0x0136, UtfFlag_Lowercase },

	// kra
//	{ 0x0138,       ,      0,      0,      0, 0 },

	// L + acute
	{ 0x0139, 0x004C, 0x0301,      0, 0x013A, UtfFlag_Uppercase },

	// l + acute
	{ 0x013A, 0x006C, 0x0301,      0, 0x0139, UtfFlag_Lowercase },

	// L + cedilla (cedilla is drawn as comma below character)
	{ 0x013B, 0x004C, 0x0326,      0, 0x013C, UtfFlag_Uppercase },

	// l + cedilla (cedilla is drawn as comma below character)
	{ 0x013C, 0x006C, 0x0326,      0, 0x013B, UtfFlag_Lowercase },

	// L + caron -- should use apostrophy instead of caron
	{ 0x013D, 0x004C, 0x030C,      0, 0x013E, UtfFlag_Uppercase },

	// l + caron -- should use apostrophy instead of caron
	{ 0x013E, 0x006C, 0x030C,      0, 0x013D, UtfFlag_Lowercase },

	// L + middle dot
	{ 0x013F, 0x013F,      0,      0, 0x0140, UtfFlag_Uppercase },

	// l + middle dot
	{ 0x0140, 0x0140,      0,      0, 0x013F, UtfFlag_Lowercase },

	// L + stroke
	{ 0x0141, 0x0141,      0,      0, 0x0142, UtfFlag_Uppercase },

	// l + stroke
	{ 0x0142, 0x0142,      0,      0, 0x0141, UtfFlag_Lowercase },

	// N + acute
	{ 0x0143, 0x004E, 0x0301,      0, 0x0144, UtfFlag_Uppercase },

	// n + acute
	{ 0x0144, 0x006E, 0x0301,      0, 0x0143, UtfFlag_Lowercase },

	// N + cedilla (cedilla is drawn as comma below character)
	{ 0x0145, 0x004E, 0x0326,      0, 0x0146, UtfFlag_Uppercase },

	// n + cedilla (cedilla is drawn as comma below character)
	{ 0x0146, 0x006E, 0x0326,      0, 0x0145, UtfFlag_Lowercase },

	// N + caron
	{ 0x0147, 0x004E, 0x030C,      0, 0x0148, UtfFlag_Uppercase },

	// n + caron
	{ 0x0148, 0x006E, 0x030C,      0, 0x0147, UtfFlag_Lowercase },

	// n + apostrophy
//	{ 0x0149, 0x0149,      0,      0,      0, UtfFlat_Lowercase },

	// ENJ
	{ 0x014A, 0x014A,      0,      0, 0x014B, UtfFlag_Uppercase },

	// enj
	{ 0x014B, 0x014B,      0,      0, 0x014A, UtfFlag_Lowercase },

	// O + macron
	{ 0x014C, 0x004F, 0x0304,      0, 0x014D, UtfFlag_Uppercase },

	// o + macron
	{ 0x014D, 0x006F, 0x0304,      0, 0x014C, UtfFlag_Lowercase },

	// O + breve
	{ 0x014E, 0x004F, 0x0306,      0, 0x014F, UtfFlag_Uppercase },

	// o + breve
	{ 0x014F, 0x006F, 0x0306,      0, 0x014E, UtfFlag_Lowercase },

	// O + double acute
	{ 0x0150, 0x004F, 0x030B,      0, 0x0151, UtfFlag_Uppercase },

	// o + double acute
	{ 0x0151, 0x006F, 0x030B,      0, 0x0150, UtfFlag_Lowercase },

	// OE
	{ 0x0152, 0x0152,      0,      0, 0x0153, UtfFlag_Uppercase },

	// oe
	{ 0x0153, 0x0153,      0,      0, 0x0152, UtfFlag_Lowercase },

	// R + acute
	{ 0x0154, 0x0052, 0x0301,      0, 0x0155, UtfFlag_Uppercase },

	// r + acute
	{ 0x0155, 0x0072, 0x0301,      0, 0x0154, UtfFlag_Lowercase },

	// R + cedilla (cedilla is drawn as apostrophy below character)
	{ 0x0156, 0x0052, 0x0326,      0, 0x0157, UtfFlag_Uppercase },

	// r + cedilla (cedilla is drawn as apostrophy below character)
	{ 0x0157, 0x0072, 0x0326,      0, 0x0156, UtfFlag_Lowercase },

	// R + caron
	{ 0x0158, 0x0052, 0x030C,      0, 0x0159, UtfFlag_Uppercase },

	// r + caron
	{ 0x0159, 0x0072, 0x030C,      0, 0x0158, UtfFlag_Lowercase },

	// S + acute
	{ 0x015A, 0x0053, 0x0301,      0, 0x015B, UtfFlag_Uppercase },

	// s + acute
	{ 0x015B, 0x0073, 0x0301,      0, 0x015A, UtfFlag_Lowercase },

	// S + circumflex
	{ 0x015C, 0x0053, 0x0302,      0, 0x015D, UtfFlag_Uppercase },

	// s + circumflex
	{ 0x015D, 0x0073, 0x0302,      0, 0x015C, UtfFlag_Lowercase },

	// S + cedilla
	{ 0x015E, 0x0053, 0x0327,      0, 0x015F, UtfFlag_Uppercase },

	// s + cedilla
	{ 0x015F, 0x0073, 0x0327,      0, 0x015E, UtfFlag_Lowercase },

	// S + caron
	{ 0x0160, 0x0053, 0x030C,      0, 0x0161, UtfFlag_Uppercase },

	// s + caron
	{ 0x0161, 0x0073, 0x030C,      0, 0x0160, UtfFlag_Lowercase },

	// T + cedilla
	{ 0x0162, 0x0054, 0x0327,      0, 0x0163, UtfFlag_Uppercase },

	// t + cedilla
	{ 0x0163, 0x0074, 0x0327,      0, 0x0162, UtfFlag_Lowercase },

	// T + caron
	{ 0x0164, 0x0054, 0x030C,      0, 0x0165, UtfFlag_Uppercase },

	// t + caron -- should use apostrophy
	{ 0x0165, 0x0074, 0x030C,      0, 0x0164, UtfFlag_Lowercase },

	// T + stroke
//	{ 0x0166, 0x0054,       ,      0, 0x0167, UtfFlag_Uppercase },

	// t + stroke
//	{ 0x0167, 0x0074,       ,      0, 0x0166, UtfFlag_Lowercase },

	// U + tilde
	{ 0x0168, 0x0055, 0x0303,      0, 0x0169, UtfFlag_Uppercase },

	// u + tilde
	{ 0x0169, 0x0075, 0x0303,      0, 0x0168, UtfFlag_Lowercase },

	// U + macron
	{ 0x016A, 0x0055, 0x0304,      0, 0x016B, UtfFlag_Uppercase },

	// u + macron
	{ 0x016B, 0x0075, 0x0304,      0, 0x016A, UtfFlag_Lowercase },

	// U + breve
	{ 0x016C, 0x0055, 0x0306,      0, 0x016D, UtfFlag_Uppercase },

	// u + breve
	{ 0x016D, 0x0075, 0x0306,      0, 0x016C, UtfFlag_Lowercase },

	// U + ring above
	{ 0x016E, 0x0055, 0x030A,      0, 0x016F, UtfFlag_Uppercase },

	// u + ring above
	{ 0x016F, 0x0075, 0x030A,      0, 0x016E, UtfFlag_Lowercase },

	// U + double acute
	{ 0x0170, 0x0055, 0x030B,      0, 0x0171, UtfFlag_Uppercase },

	// u + double acute
	{ 0x0171, 0x0075, 0x030B,      0, 0x0170, UtfFlag_Lowercase },

	// U + ogonek
	{ 0x0172, 0x0055, 0x0328,      0, 0x0173, UtfFlag_Uppercase },

	// u + ogonek
	{ 0x0173, 0x0075, 0x0328,      0, 0x0172, UtfFlag_Lowercase },

	// W + circumflex
	{ 0x0174, 0x0057, 0x0302,      0, 0x0175, UtfFlag_Uppercase },

	// w + circumflex
	{ 0x0175, 0x0077, 0x0302,      0, 0x0174, UtfFlag_Lowercase },

	// Y + circumflex
	{ 0x0176, 0x0059, 0x0302,      0, 0x0177, UtfFlag_Uppercase },

	// y + circumflex
	{ 0x0177, 0x0079, 0x0302,      0, 0x0176, UtfFlag_Lowercase },

	// Y + diaeresis -- lower case is in different part of table
	{ 0x0178, 0x0059, 0x0308,      0, 0x00FF, UtfFlag_Uppercase },

	// Z + acute
	{ 0x0179, 0x005A, 0x0301,      0, 0x017A, UtfFlag_Uppercase },

	// z + acute
	{ 0x017A, 0x007A, 0x0301,      0, 0x0179, UtfFlag_Lowercase },

	// Z + dot above
	{ 0x017B, 0x005A, 0x0307,      0, 0x017C, UtfFlag_Uppercase },

	// z + dot above
	{ 0x017C, 0x007A, 0x0307,      0, 0x017B, UtfFlag_Lowercase },

	// Z + caron
	{ 0x017D, 0x005A, 0x030C,      0, 0x017E, UtfFlag_Uppercase },

	// z + caron
	{ 0x017E, 0x007A, 0x030C,      0, 0x017D, UtfFlag_Lowercase },

	// old-style small 's', looks like sylized 'f'
	{ 0x017F, 0x017F,      0,      0,      0, UtfFlag_Lowercase },



	// combining grave accent
	{ 0x0300, 0x0300,      0,      0,      0, UtfFlag_CombineHiMid },

	// combining acute accent
	{ 0x0301, 0x0301,      0,      0,      0, UtfFlag_CombineHiMid },

	// combining circumflex accent
	{ 0x0302, 0x0302,      0,      0,      0, UtfFlag_CombineHiMid },

	// combining tilde
	{ 0x0303, 0x0303,      0,      0,      0, UtfFlag_CombineHiMid },

	// combining macron
	{ 0x0304, 0x0304,      0,      0,      0, UtfFlag_CombineHiMid },

	// combining breve
	{ 0x0306, 0x0306,      0,      0,      0, UtfFlag_CombineHiMid },

	// combining dot above
	{ 0x0307, 0x0307,      0,      0,      0, UtfFlag_CombineHiMid },

	// combining diaeresis above
	{ 0x0308, 0x0308,      0,      0,      0, UtfFlag_CombineHiMid },

	// combining hook above
	{ 0x0309, 0x0309,      0,      0,      0, UtfFlag_CombineHiRight },

	// combining ring above
	{ 0x030A, 0x030A,      0,      0,      0, UtfFlag_CombineHiMid },

	// combining double acute
	{ 0x030B, 0x030B,      0,      0,      0, UtfFlag_CombineHiMid },

	// combining caron
	{ 0x030C, 0x030C,      0,      0,      0, UtfFlag_CombineHiMid },

	// combining turned comma above
	{ 0x0312, 0x0312,      0,      0,      0, UtfFlag_CombineHiMid },

	// combining dot below
	{ 0x0323, 0x0323,      0,      0,      0, UtfFlag_CombineLoMid },

	// combining diaeresis below
	{ 0x0324, 0x0324,      0,      0,      0, UtfFlag_CombineLoMid },

	// combining ring below
	{ 0x0325, 0x0325,      0,      0,      0, UtfFlag_CombineLoMid },

	// combining comma below
	{ 0x0326, 0x0326,      0,      0,      0, UtfFlag_CombineLoMid },

	// combining cedilla
	{ 0x0327, 0x0327,      0,      0,      0, UtfFlag_CombineLoMid },

	// combining ogonek
	{ 0x0328, 0x0328,      0,      0,      0, UtfFlag_CombineLoRight },

	// combining circumflex below
	{ 0x032D, 0x032D,      0,      0,      0, UtfFlag_CombineLoMid },

	// combining breve below
	{ 0x032E, 0x032E,      0,      0,      0, UtfFlag_CombineLoMid },

	// combining tilde below
	{ 0x0330, 0x0330,      0,      0,      0, UtfFlag_CombineLoMid },

	// combining macron below
	{ 0x0331, 0x0331,      0,      0,      0, UtfFlag_CombineLoMid },



	// A + ring below
	{ 0x1E00, 0x0041, 0x0325,      0, 0x1E01, UtfFlag_Uppercase },

	// a + ring below
	{ 0x1E01, 0x0061, 0x0325,      0, 0x1E00, UtfFlag_Lowercase },

	// B + dot above
	{ 0x1E02, 0x0042, 0x0307,      0, 0x1E03, UtfFlag_Uppercase },

	// b + dot above -- dot should be in middle of symbol
	{ 0x1E03, 0x0062, 0x0307,      0, 0x1E02, UtfFlag_Lowercase },

	// B + dot below
	{ 0x1E04, 0x0042, 0x0323,      0, 0x1E05, UtfFlag_Uppercase },

	// b + dot below
	{ 0x1E05, 0x0062, 0x0323,      0, 0x1E04, UtfFlag_Lowercase },

	// B + macron below
	{ 0x1E06, 0x0042, 0x0331,      0, 0x1E07, UtfFlag_Uppercase },

	// b + macron below
	{ 0x1E07, 0x0062, 0x0331,      0, 0x1E06, UtfFlag_Lowercase },

	// C + cedilla + acute
	{ 0x1E08, 0x0043, 0x0327, 0x0301, 0x1E09, UtfFlag_Uppercase },

	// c + cedilla + acute
	{ 0x1E09, 0x0063, 0x0327, 0x0301, 0x1E08, UtfFlag_Lowercase },

	// D + dot above
	{ 0x1E0A, 0x0044, 0x0307,      0, 0x1E0B, UtfFlag_Uppercase },

	// d + dot above -- dot should be inside symbol
	{ 0x1E0B, 0x0064, 0x0307,      0, 0x1E0A, UtfFlag_Lowercase },

	// D + dot below
	{ 0x1E0C, 0x0044, 0x0323,      0, 0x1E0D, UtfFlag_Uppercase },

	// d + dot below
	{ 0x1E0D, 0x0064, 0x0323,      0, 0x1E0C, UtfFlag_Lowercase },

	// D + macron below
	{ 0x1E0E, 0x0044, 0x0331,      0, 0x1E0F, UtfFlag_Uppercase },

	// d + macron below
	{ 0x1E0F, 0x0064, 0x0331,      0, 0x1E0E, UtfFlag_Lowercase },

	// D + cedilla (cedilla is drawn as comma below character)
	{ 0x1E10, 0x0044, 0x0326,      0, 0x1E11, UtfFlag_Uppercase },

	// d + cedilla (cedilla is drawn as comma below character)
	{ 0x1E11, 0x0064, 0x0326,      0, 0x1E10, UtfFlag_Lowercase },

	// D + circumflex below
	{ 0x1E12, 0x0044, 0x032D,      0, 0x1E13, UtfFlag_Uppercase },

	// d + circumflex below
	{ 0x1E13, 0x0064, 0x032D,      0, 0x1E12, UtfFlag_Lowercase },

	// E + macron + grave
	{ 0x1E14, 0x0045, 0x0304, 0x0300, 0x1E15, UtfFlag_Uppercase },

	// e + macron + grave
	{ 0x1E15, 0x0065, 0x0304, 0x0300, 0x1E14, UtfFlag_Lowercase },

	// E + macron + acute
	{ 0x1E16, 0x0045, 0x0304, 0x0301, 0x1E17, UtfFlag_Uppercase },

	// e + macron + acute
	{ 0x1E17, 0x0065, 0x0304, 0x0301, 0x1E16, UtfFlag_Lowercase },

	// E + circumflex below
	{ 0x1E18, 0x0045, 0x032D,      0, 0x1E19, UtfFlag_Uppercase },

	// e + circumflex below
	{ 0x1E19, 0x0065, 0x032D,      0, 0x1E18, UtfFlag_Lowercase },

	// E + tilde below
	{ 0x1E1A, 0x0045, 0x0330,      0, 0x1E1B, UtfFlag_Uppercase },

	// e + tilde below
	{ 0x1E1B, 0x0065, 0x0330,      0, 0x1E1A, UtfFlag_Lowercase },

	// E + cedilla + breve
	{ 0x1E1C, 0x0045, 0x0327, 0x0306, 0x1E1D, UtfFlag_Uppercase },

	// e + cedilla + breve
	{ 0x1E1D, 0x0065, 0x0327, 0x0306, 0x1E1C, UtfFlag_Lowercase },

	// F + dot above
	{ 0x1E1E, 0x0046, 0x0307,      0, 0x1E1F, UtfFlag_Uppercase },

	// f + dot above
	{ 0x1E1F, 0x0066, 0x0307,      0, 0x1E1E, UtfFlag_Lowercase },

	// G + macron
	{ 0x1E20, 0x0047, 0x0304,      0, 0x1E21, UtfFlag_Uppercase },

	// g + macron
	{ 0x1E21, 0x0067, 0x0304,      0, 0x1E20, UtfFlag_Lowercase },

	// H + dot above
	{ 0x1E22, 0x0048, 0x0307,      0, 0x1E23, UtfFlag_Uppercase },

	// h + dot above
	{ 0x1E23, 0x0068, 0x0307,      0, 0x1E22, UtfFlag_Lowercase },

	// H + dot below
	{ 0x1E24, 0x0048, 0x0323,      0, 0x1E25, UtfFlag_Uppercase },

	// h + dot below
	{ 0x1E25, 0x0068, 0x0323,      0, 0x1E24, UtfFlag_Lowercase },

	// H + diaeresis
	{ 0x1E26, 0x0048, 0x0308,      0, 0x1E27, UtfFlag_Uppercase },

	// h + diaeresis
	{ 0x1E27, 0x0068, 0x0308,      0, 0x1E26, UtfFlag_Lowercase },

	// H + cedilla
	{ 0x1E28, 0x0048, 0x0327,      0, 0x1E29, UtfFlag_Uppercase },

	// h + cedilla
	{ 0x1E29, 0x0068, 0x0327,      0, 0x1E28, UtfFlag_Lowercase },

	// H + breve below
	{ 0x1E2A, 0x0048, 0x032E,      0, 0x1E2B, UtfFlag_Uppercase },

	// h + breve below
	{ 0x1E2B, 0x0068, 0x032E,      0, 0x1E2A, UtfFlag_Lowercase },

	// I + tilde below
	{ 0x1E2C, 0x0049, 0x0330,      0, 0x1E2D, UtfFlag_Uppercase },

	// i + tilde below
	{ 0x1E2D, 0x0069, 0x0330,      0, 0x1E2C, UtfFlag_Lowercase },

	// I + diaeresis + acute
	{ 0x1E2E, 0x0049, 0x0308, 0x0301, 0x1E2F, UtfFlag_Uppercase },

	// i + diaeresis + acute
	{ 0x1E2F, 0xFFD0, 0x0308, 0x0301, 0x1E2E, UtfFlag_Lowercase },

	// K + acute
	{ 0x1E30, 0x004B, 0x0301,      0, 0x1E31, UtfFlag_Uppercase },

	// k + acute
	{ 0x1E31, 0x006B, 0x0301,      0, 0x1E30, UtfFlag_Lowercase },

	// K + dot below
	{ 0x1E32, 0x004B, 0x0323,      0, 0x1E33, UtfFlag_Uppercase },

	// k + dot below
	{ 0x1E33, 0x006B, 0x0323,      0, 0x1E32, UtfFlag_Lowercase },

	// K + macron below
	{ 0x1E34, 0x004B, 0x0331,      0, 0x1E35, UtfFlag_Uppercase },

	// k + macron below
	{ 0x1E35, 0x006B, 0x0331,      0, 0x1E34, UtfFlag_Lowercase },

	// L + dot below
	{ 0x1E36, 0x004C, 0x0323,      0, 0x1E37, UtfFlag_Uppercase },

	// l + dot below
	{ 0x1E37, 0x006C, 0x0323,      0, 0x1E36, UtfFlag_Lowercase },

	// L + dot below + macron
	{ 0x1E38, 0x004C, 0x0323, 0x0304, 0x1E39, UtfFlag_Uppercase },

	// l + dot below + macron
	{ 0x1E39, 0x006C, 0x0323, 0x0304, 0x1E38, UtfFlag_Lowercase },

	// L + macron below
	{ 0x1E3A, 0x004C, 0x0331,      0, 0x1E3B, UtfFlag_Uppercase },

	// l + macron below
	{ 0x1E3B, 0x006C, 0x0331,      0, 0x1E3A, UtfFlag_Lowercase },

	// L + circumflex below
	{ 0x1E3C, 0x004C, 0x032D,      0, 0x1E3D, UtfFlag_Uppercase },

	// l + circumflex below
	{ 0x1E3D, 0x006C, 0x032D,      0, 0x1E3C, UtfFlag_Lowercase },

	// M + acute
	{ 0x1E3E, 0x004D, 0x0301,      0, 0x1E3F, UtfFlag_Uppercase },

	// m + acute
	{ 0x1E3F, 0x006D, 0x0301,      0, 0x1E3E, UtfFlag_Lowercase },

	// M + dot above
	{ 0x1E40, 0x004D, 0x0307,      0, 0x1E41, UtfFlag_Uppercase },

	// m + dot above
	{ 0x1E41, 0x006D, 0x0307,      0, 0x1E40, UtfFlag_Lowercase },

	// M + dot below
	{ 0x1E42, 0x004D, 0x0323,      0, 0x1E43, UtfFlag_Uppercase },

	// m + dot below
	{ 0x1E43, 0x006D, 0x0323,      0, 0x1E42, UtfFlag_Lowercase },

	// N + dot above
	{ 0x1E44, 0x004E, 0x0307,      0, 0x1E45, UtfFlag_Uppercase },

	// n + dot above
	{ 0x1E45, 0x006E, 0x0307,      0, 0x1E44, UtfFlag_Lowercase },

	// N + dot below
	{ 0x1E46, 0x004E, 0x0323,      0, 0x1E47, UtfFlag_Uppercase },

	// n + dot below
	{ 0x1E47, 0x006E, 0x0323,      0, 0x1E46, UtfFlag_Lowercase },

	// N + macron below
	{ 0x1E48, 0x004E, 0x0331,      0, 0x1E49, UtfFlag_Uppercase },

	// n + macron below
	{ 0x1E49, 0x006E, 0x0331,      0, 0x1E48, UtfFlag_Lowercase },

	// N + circumflex below
	{ 0x1E4A, 0x004E, 0x032D,      0, 0x1E4B, UtfFlag_Uppercase },

	// n + circumflex below
	{ 0x1E4B, 0x006E, 0x032D,      0, 0x1E4A, UtfFlag_Lowercase },

	// O + tilde + acute
	{ 0x1E4C, 0x004F, 0x0303, 0x0301, 0x1E4D, UtfFlag_Uppercase },

	// o + tilde + acute
	{ 0x1E4D, 0x006F, 0x0303, 0x0301, 0x1E4C, UtfFlag_Lowercase },

	// O + tilde + diaeresis
	{ 0x1E4E, 0x004F, 0x0303, 0x0308, 0x1E4F, UtfFlag_Uppercase },

	// o + tilde + diaeresis
	{ 0x1E4F, 0x006F, 0x0303, 0x0308, 0x1E4E, UtfFlag_Lowercase },

	// O + macron + grave
	{ 0x1E50, 0x004F, 0x0304, 0x0300, 0x1E51, UtfFlag_Uppercase },

	// o + macron + grave
	{ 0x1E51, 0x006F, 0x0304, 0x0300, 0x1E50, UtfFlag_Lowercase },

	// O + macron + acute
	{ 0x1E52, 0x004F, 0x0304, 0x0301, 0x1E53, UtfFlag_Uppercase },

	// o + macron + acute
	{ 0x1E53, 0x006F, 0x0304, 0x0301, 0x1E52, UtfFlag_Lowercase },

	// P + acute
	{ 0x1E54, 0x0050, 0x0301,      0, 0x1E55, UtfFlag_Uppercase },

	// p + acute
	{ 0x1E55, 0x0070, 0x0301,      0, 0x1E54, UtfFlag_Lowercase },

	// P + dot above
	{ 0x1E56, 0x0050, 0x0307,      0, 0x1E57, UtfFlag_Uppercase },

	// p + dot above
	{ 0x1E57, 0x0070, 0x0307,      0, 0x1E56, UtfFlag_Lowercase },

	// R + dot above
	{ 0x1E58, 0x0052, 0x0307,      0, 0x1E59, UtfFlag_Uppercase },

	// r + dot above
	{ 0x1E59, 0x0072, 0x0307,      0, 0x1E58, UtfFlag_Lowercase },

	// R + dot below
	{ 0x1E5A, 0x0052, 0x0323,      0, 0x1E5B, UtfFlag_Uppercase },

	// r + dot below
	{ 0x1E5B, 0x0072, 0x0323,      0, 0x1E5A, UtfFlag_Lowercase },

	// R + dot below + macron
	{ 0x1E5C, 0x0052, 0x0323, 0x0304, 0x1E5D, UtfFlag_Uppercase },

	// r + dot below + macron
	{ 0x1E5D, 0x0072, 0x0323, 0x0304, 0x1E5C, UtfFlag_Lowercase },

	// R + macron below
	{ 0x1E5E, 0x0052, 0x0331,      0, 0x1E5F, UtfFlag_Uppercase },

	// r + macron below
	{ 0x1E5F, 0x0072, 0x0331,      0, 0x1E5E, UtfFlag_Lowercase },

	// S + dot above
	{ 0x1E60, 0x0053, 0x0307,      0, 0x1E61, UtfFlag_Uppercase },

	// s + dot above
	{ 0x1E61, 0x0073, 0x0307,      0, 0x1E60, UtfFlag_Lowercase },

	// S + dot below
	{ 0x1E62, 0x0053, 0x0323,      0, 0x1E63, UtfFlag_Uppercase },

	// s + dot below
	{ 0x1E63, 0x0073, 0x0323,      0, 0x1E62, UtfFlag_Lowercase },

	// S + acute + dot above
	{ 0x1E64, 0x0053, 0x0301, 0x0307, 0x1E65, UtfFlag_Uppercase },

	// s + acute + dot above
	{ 0x1E65, 0x0073, 0x0301, 0x0307, 0x1E64, UtfFlag_Lowercase },

	// S + caron + dot above
	{ 0x1E66, 0x0053, 0x030C, 0x0307, 0x1E67, UtfFlag_Uppercase },

	// s + caron + dot above
	{ 0x1E67, 0x0073, 0x030C, 0x0307, 0x1E66, UtfFlag_Lowercase },

	// S + dot above + dot below
	{ 0x1E68, 0x0053, 0x0307, 0x0323, 0x1E69, UtfFlag_Uppercase },

	// s + dot above + dot below
	{ 0x1E69, 0x0073, 0x0307, 0x0323, 0x1E68, UtfFlag_Lowercase },

	// T + dot above
	{ 0x1E6A, 0x0054, 0x0307,      0, 0x1E6B, UtfFlag_Uppercase },

	// t + dot above
	{ 0x1E6B, 0x0074, 0x0307,      0, 0x1E6A, UtfFlag_Lowercase },

	// T + dot below
	{ 0x1E6C, 0x0054, 0x0323,      0, 0x1E6D, UtfFlag_Uppercase },

	// t + dot below
	{ 0x1E6D, 0x0074, 0x0323,      0, 0x1E6C, UtfFlag_Lowercase },

	// T + macron below
	{ 0x1E6E, 0x0054, 0x0331,      0, 0x1E6F, UtfFlag_Uppercase },

	// t + macron below
	{ 0x1E6F, 0x0074, 0x0331,      0, 0x1E6E, UtfFlag_Lowercase },

	// T + circumflex below
	{ 0x1E70, 0x0054, 0x032D,      0, 0x1E71, UtfFlag_Uppercase },

	// t + circumflex below
	{ 0x1E71, 0x0074, 0x032D,      0, 0x1E70, UtfFlag_Lowercase },

	// U + diaeresis below
	{ 0x1E72, 0x0055, 0x0324,      0, 0x1E73, UtfFlag_Uppercase },

	// u + diaeresis below
	{ 0x1E73, 0x0075, 0x0324,      0, 0x1E72, UtfFlag_Lowercase },

	// U + tilde below
	{ 0x1E74, 0x0055, 0x0330,      0, 0x1E75, UtfFlag_Uppercase },

	// u + tilde below
	{ 0x1E75, 0x0075, 0x0330,      0, 0x1E74, UtfFlag_Lowercase },

	// U + circumflex below
	{ 0x1E76, 0x0055, 0x032D,      0, 0x1E77, UtfFlag_Uppercase },

	// u + circumflex below
	{ 0x1E77, 0x0075, 0x032D,      0, 0x1E76, UtfFlag_Lowercase },

	// U + tilde + acute
	{ 0x1E78, 0x0055, 0x0303, 0x0301, 0x1E79, UtfFlag_Uppercase },

	// u + tilde + acute
	{ 0x1E79, 0x0075, 0x0303, 0x0301, 0x1E78, UtfFlag_Lowercase },

	// U + macron + diaeresis
	{ 0x1E7A, 0x0055, 0x0304, 0x0308, 0x1E7B, UtfFlag_Uppercase },

	// u + macron + diaeresis
	{ 0x1E7B, 0x0075, 0x0304, 0x0308, 0x1E7A, UtfFlag_Lowercase },

	// V + tilde
	{ 0x1E7C, 0x0056, 0x0303,      0, 0x1E7D, UtfFlag_Uppercase },

	// v + tilde
	{ 0x1E7D, 0x0076, 0x0303,      0, 0x1E7C, UtfFlag_Lowercase },

	// V + dot below
	{ 0x1E7E, 0x0056, 0x0323,      0, 0x1E7F, UtfFlag_Uppercase },

	// v + dot below
	{ 0x1E7F, 0x0076, 0x0323,      0, 0x1E7E, UtfFlag_Lowercase },

	// W + grave
	{ 0x1E80, 0x0057, 0x0300,      0, 0x1E81, UtfFlag_Uppercase },

	// w + grave
	{ 0x1E81, 0x0077, 0x0300,      0, 0x1E80, UtfFlag_Lowercase },

	// W + acute
	{ 0x1E82, 0x0057, 0x0301,      0, 0x1E83, UtfFlag_Uppercase },

	// w + acute
	{ 0x1E83, 0x0077, 0x0301,      0, 0x1E82, UtfFlag_Lowercase },

	// W + diaeresis
	{ 0x1E84, 0x0057, 0x0308,      0, 0x1E85, UtfFlag_Uppercase },

	// w + diaeresis
	{ 0x1E85, 0x0077, 0x0308,      0, 0x1E84, UtfFlag_Lowercase },

	// W + dot above
	{ 0x1E86, 0x0057, 0x0307,      0, 0x1E87, UtfFlag_Uppercase },

	// w + dot above
	{ 0x1E87, 0x0077, 0x0307,      0, 0x1E86, UtfFlag_Lowercase },

	// W + dot below
	{ 0x1E88, 0x0057, 0x0323,      0, 0x1E89, UtfFlag_Uppercase },

	// w + dot below
	{ 0x1E89, 0x0077, 0x0323,      0, 0x1E88, UtfFlag_Lowercase },

	// X + dot above
	{ 0x1E8A, 0x0058, 0x0307,      0, 0x1E8B, UtfFlag_Uppercase },

	// x + dot above
	{ 0x1E8B, 0x0078, 0x0307,      0, 0x1E8A, UtfFlag_Lowercase },

	// X + diaeresis
	{ 0x1E8C, 0x0058, 0x0308,      0, 0x1E8D, UtfFlag_Uppercase },

	// x + diaeresis
	{ 0x1E8D, 0x0078, 0x0308,      0, 0x1E8C, UtfFlag_Lowercase },

	// Y + dot above
	{ 0x1E8E, 0x0059, 0x0307,      0, 0x1E8F, UtfFlag_Uppercase },

	// y + dot above
	{ 0x1E8F, 0x0079, 0x0307,      0, 0x1E8E, UtfFlag_Lowercase },

	// Z + circumflex
	{ 0x1E90, 0x005A, 0x0302,      0, 0x1E91, UtfFlag_Uppercase },

	// z + circumflex
	{ 0x1E91, 0x007A, 0x0302,      0, 0x1E90, UtfFlag_Lowercase },

	// Z + dot below
	{ 0x1E92, 0x005A, 0x0323,      0, 0x1E93, UtfFlag_Uppercase },

	// z + dot below
	{ 0x1E93, 0x007A, 0x0323,      0, 0x1E92, UtfFlag_Lowercase },

	// Z + macron below
	{ 0x1E94, 0x005A, 0x0331,      0, 0x1E95, UtfFlag_Uppercase },

	// z + macron below
	{ 0x1E95, 0x007A, 0x0331,      0, 0x1E94, UtfFlag_Lowercase },

	// h + macron below
	{ 0x1E96, 0x0068, 0x0331,      0,      0, UtfFlag_Lowercase },

	// t + diaeresis
	{ 0x1E97, 0x0074, 0x0308,      0,      0, UtfFlag_Lowercase },

	// w + ring above
	{ 0x1E98, 0x0077, 0x030A,      0,      0, UtfFlag_Lowercase },

	// y + ring above
	{ 0x1E99, 0x0079, 0x030A,      0,      0, UtfFlag_Lowercase },

	// a + right half ring
//	{ 0x1E9A, 0x0061, 0x02BE,      0,      0, UtfFlag_Lowercase },

	// Gaelic 'f' + dot above
	{ 0x1E9B, 0x017F, 0x0307,      0,      0, UtfFlag_Lowercase },

	// Y + grave
	{ 0x1EF2, 0x0059, 0x0300,      0, 0x1EF3, UtfFlag_Uppercase },

	// y + grave
	{ 0x1EF3, 0x0079, 0x0300,      0, 0x1EF2, UtfFlag_Lowercase },

	// Y + dot below
	{ 0x1EF4, 0x0059, 0x0323,      0, 0x1EF5, UtfFlag_Uppercase },

	// y + dot below
	{ 0x1EF5, 0x0079, 0x0323,      0, 0x1EF4, UtfFlag_Lowercase },

	// Y + hook above
	{ 0x1EF6, 0x0059, 0x0309,      0, 0x1EF7, UtfFlag_Uppercase },

	// y + hook above
	{ 0x1EF7, 0x0079, 0x0309,      0, 0x1EF6, UtfFlag_Lowercase },

	// Y + tilde
	{ 0x1EF8, 0x0059, 0x0303,      0, 0x1EF9, UtfFlag_Uppercase },

	// y + tilde
	{ 0x1EF9, 0x0079, 0x0303,      0, 0x1EF8, UtfFlag_Lowercase },

	// middle-welsh LL
//	{ 0x1EFA, 0x1EFA,      0,      0,      0, UtfFlag_Uppercase },

	// middle-welsh ll
//	{ 0x1EFB, 0x1EFB,      0,      0,      0, UtfFlag_Lowercase },

	// middle-welsh VV
//	{ 0x1EFC, 0x1EFC,      0,      0,      0, UtfFlag_Uppercase },

	// middle-welsh vv
//	{ 0x1EFD, 0x1EFD,      0,      0,      0, UtfFlag_Lowercase },

	// Y with loop
//	{ 0x1EFE, 0x1EFE,      0,      0,      0, UtfFlag_Uppercase },

	// y with loop
//	{ 0x1EFF, 0x1EFF,      0,      0,      0, UtfFlag_Lowercase },

	// en dash
	{ 0x2013, 0x2013,      0,      0,      0, 0 },

	// em dash
	{ 0x2014, 0x2014,      0,      0,      0, 0 },

	// left single quote
	{ 0x2018, 0x2018,      0,      0,      0, 0 },

	// right single quote
	{ 0x2019, 0x2019,      0,      0,      0, 0 },

	// left double quote
	{ 0x201C, 0x201C,      0,      0,      0, 0 },

	// right double quote
	{ 0x201D, 0x201D,      0,      0,      0, 0 },

	// bullet
	{ 0x2022, 0x2022,      0,      0,      0, 0 },


	/////////////////////////////////////////////////////////////////////////
	//
	//  All remaining characters are in the reserved 0xFFD0-0xFFEF range.
	//  These are special characters defined internally only for rendering,
	//  and the numerical values for them only occur as the results of
	//  look-up operations within the font renderer.  No other piece of code
	//  should see these value, or expect to deal with them.
	//

	// i "soft dotted" -- only used for rendering
	{ 0xFFD0, 0xFFD0, 0, 0,      0, UtfFlag_Lowercase },

	// j "soft dotted" -- only used for rendering
	{ 0xFFD1, 0xFFD1, 0, 0,      0, UtfFlag_Lowercase },

	// unknown symbol
	{ 0xFFFD, 0xFFFD, 0, 0,      0, 0 },
};


struct UtfNormTable_t
{
	Utf16_t Symbol;
	Utf16_t C1;
	Utf16_t C2;
	Utf16_t C3;
};


/////////////////////////////////////////////////////////////////////////////
//
//	This table contains mappings that convert internal composed values to
//	the canonical decomposed/normalized symbols.
//
//	Note that this array contains different values from g_UtfSymbolTable[].
//	That array contains values used for rendering, which do not always map
//	to meaningful Unicode values.  This array always maps to valid Unicode
//	symbols, and all of these mapping should be reversed using the contents
//	of g_UtfComposeValues[].
//
static UtfNormTable_t g_UtfNormTable[] =
{
	{ 0x00C0, 0x0041, 0x0300,      0 },		// A + grave
	{ 0x00C1, 0x0041, 0x0301,      0 },		// A + acute
	{ 0x00C2, 0x0041, 0x0302,      0 },		// A + circumflex
	{ 0x00C3, 0x0041, 0x0303,      0 },		// A + tilde
	{ 0x00C4, 0x0041, 0x0308,      0 },		// A + diaeresis
	{ 0x00C5, 0x0041, 0x030A,      0 },		// A + ring above
	{ 0x00C7, 0x0043, 0x0327,      0 },		// C + cedilla
	{ 0x00C8, 0x0045, 0x0300,      0 },		// E + grave
	{ 0x00C9, 0x0045, 0x0301,      0 },		// E + acute
	{ 0x00CA, 0x0045, 0x0302,      0 },		// E + circumflex
	{ 0x00CB, 0x0045, 0x0308,      0 },		// E + diaeresis
	{ 0x00CC, 0x0049, 0x0300,      0 },		// I + grave
	{ 0x00CD, 0x0049, 0x0301,      0 },		// I + acute
	{ 0x00CE, 0x0049, 0x0302,      0 },		// I + circumflex
	{ 0x00CF, 0x0049, 0x0308,      0 },		// I + diaeresis
	{ 0x00D1, 0x004E, 0x0303,      0 },		// N + tilde
	{ 0x00D2, 0x004F, 0x0300,      0 },		// O + grave
	{ 0x00D3, 0x004F, 0x0301,      0 },		// O + acute
	{ 0x00D4, 0x004F, 0x0302,      0 },		// O + circumflex
	{ 0x00D5, 0x004F, 0x0303,      0 },		// O + tilde
	{ 0x00D6, 0x004F, 0x0308,      0 },		// O + diaeresis
	{ 0x00D9, 0x0055, 0x0300,      0 },		// U + grave
	{ 0x00DA, 0x0055, 0x0301,      0 },		// U + acute
	{ 0x00DB, 0x0055, 0x0302,      0 },		// U + circumflex
	{ 0x00DC, 0x0055, 0x0308,      0 },		// U + diaeresis
	{ 0x00DD, 0x0059, 0x0301,      0 },		// Y + acute
	{ 0x00E0, 0x0061, 0x0300,      0 },		// a + grave
	{ 0x00E1, 0x0061, 0x0301,      0 },		// a + acute
	{ 0x00E2, 0x0061, 0x0302,      0 },		// a + circumflex
	{ 0x00E3, 0x0061, 0x0303,      0 },		// a + tilde
	{ 0x00E4, 0x0061, 0x0308,      0 },		// a + diaeresis
	{ 0x00E5, 0x0061, 0x030A,      0 },		// a + ring above
	{ 0x00E7, 0x0063, 0x0327,      0 },		// c + cedilla
	{ 0x00E8, 0x0065, 0x0300,      0 },		// e + grave
	{ 0x00E9, 0x0065, 0x0301,      0 },		// e + acute
	{ 0x00EA, 0x0065, 0x0302,      0 },		// e + circumflex
	{ 0x00EB, 0x0065, 0x0308,      0 },		// e + diaeresis
	{ 0x00EC, 0x0069, 0x0300,      0 },		// i + grave
	{ 0x00ED, 0x0069, 0x0301,      0 },		// i + acute
	{ 0x00EE, 0x0069, 0x0302,      0 },		// i + circumflex
	{ 0x00EF, 0x0069, 0x0308,      0 },		// i + diaeresis
	{ 0x00F1, 0x006E, 0x0303,      0 },		// n + tilde
	{ 0x00F2, 0x006F, 0x0300,      0 },		// o + grave
	{ 0x00F3, 0x006F, 0x0301,      0 },		// o + acute
	{ 0x00F4, 0x006F, 0x0302,      0 },		// o + circumflex
	{ 0x00F5, 0x006F, 0x0303,      0 },		// o + tilde
	{ 0x00F6, 0x006F, 0x0308,      0 },		// o + diaeresis
	{ 0x00F9, 0x0075, 0x0300,      0 },		// u + grave
	{ 0x00FA, 0x0075, 0x0301,      0 },		// u + acute
	{ 0x00FB, 0x0075, 0x0302,      0 },		// u + circumflex
	{ 0x00FC, 0x0075, 0x0308,      0 },		// u + diaeresis
	{ 0x00FD, 0x0079, 0x0301,      0 },		// y + acute
	{ 0x00FF, 0x0079, 0x0308,      0 },		// y + diaeresis
	{ 0x0100, 0x0041, 0x0304,      0 },		// A + macron
	{ 0x0101, 0x0061, 0x0304,      0 },		// a + macron
	{ 0x0102, 0x0041, 0x0306,      0 },		// A + breve
	{ 0x0103, 0x0061, 0x0306,      0 },		// a + breve
	{ 0x0104, 0x0041, 0x0328,      0 },		// A + ogonek
	{ 0x0105, 0x0061, 0x0328,      0 },		// a + ogonek
	{ 0x0106, 0x0043, 0x0301,      0 },		// C + acute
	{ 0x0107, 0x0063, 0x0301,      0 },		// c + acute
	{ 0x0108, 0x0043, 0x0302,      0 },		// C + circumflex
	{ 0x0109, 0x0063, 0x0302,      0 },		// c + circumflex
	{ 0x010A, 0x0043, 0x0307,      0 },		// C + dot above
	{ 0x010B, 0x0063, 0x0307,      0 },		// c + dot above
	{ 0x010C, 0x0043, 0x030C,      0 },		// C + caron
	{ 0x010D, 0x0063, 0x030C,      0 },		// c + caron
	{ 0x010E, 0x0044, 0x030C,      0 },		// D + caron
	{ 0x010F, 0x0064, 0x030C,      0 },		// d + caron
	{ 0x0112, 0x0045, 0x0304,      0 },		// E + macron
	{ 0x0113, 0x0065, 0x0304,      0 },		// e + macron
	{ 0x0114, 0x0045, 0x0306,      0 },		// E + breve
	{ 0x0115, 0x0065, 0x0306,      0 },		// e + breve
	{ 0x0116, 0x0045, 0x0307,      0 },		// E + dot above
	{ 0x0117, 0x0065, 0x0307,      0 },		// e + dot above
	{ 0x0118, 0x0045, 0x0328,      0 },		// E + ogonek
	{ 0x0119, 0x0065, 0x0328,      0 },		// e + ogonek
	{ 0x011A, 0x0045, 0x030C,      0 },		// E + caron
	{ 0x011B, 0x0065, 0x030C,      0 },		// e + caron
	{ 0x011C, 0x0047, 0x0302,      0 },		// G + circumflex
	{ 0x011D, 0x0067, 0x0302,      0 },		// g + circumflex
	{ 0x011E, 0x0047, 0x0306,      0 },		// G + breve
	{ 0x011F, 0x0067, 0x0306,      0 },		// g + breve
	{ 0x0120, 0x0047, 0x0307,      0 },		// G + dot above
	{ 0x0121, 0x0067, 0x0307,      0 },		// g + dot above
	{ 0x0122, 0x0047, 0x0327,      0 },		// G + cedilla
	{ 0x0123, 0x0067, 0x0327,      0 },		// g + cedilla (cedilla is drawn as apostrophy over character)
	{ 0x0124, 0x0048, 0x0302,      0 },		// H + circumflex
	{ 0x0125, 0x0068, 0x0302,      0 },		// h + circumflex
	{ 0x0128, 0x0049, 0x0303,      0 },		// I + tilde
	{ 0x0129, 0x0069, 0x0303,      0 },		// i + tilde
	{ 0x012A, 0x0049, 0x0304,      0 },		// I + macron
	{ 0x012B, 0x0069, 0x0304,      0 },		// i + macron
	{ 0x012C, 0x0049, 0x0306,      0 },		// I + breve
	{ 0x012D, 0x0069, 0x0306,      0 },		// i + breve
	{ 0x012E, 0x0049, 0x0328,      0 },		// I + ogonek
	{ 0x012F, 0x0069, 0x0328,      0 },		// i + ogonek
	{ 0x0130, 0x0049, 0x0307,      0 },		// I + dot above
	{ 0x0134, 0x004A, 0x0302,      0 },		// J + circumflex
	{ 0x0135, 0x006A, 0x0302,      0 },		// j + circumflex
	{ 0x0136, 0x004B, 0x0327,      0 },		// K + cedilla
	{ 0x0137, 0x006B, 0x0327,      0 },		// k + cedilla
	{ 0x0139, 0x004C, 0x0301,      0 },		// L + acute
	{ 0x013A, 0x006C, 0x0301,      0 },		// l + acute
	{ 0x013B, 0x004C, 0x0327,      0 },		// L + cedilla
	{ 0x013C, 0x006C, 0x0327,      0 },		// l + cedilla
	{ 0x013D, 0x004C, 0x030C,      0 },		// L + caron -- should use apostrophy instead of caron
	{ 0x013E, 0x006C, 0x030C,      0 },		// l + caron -- should use apostrophy instead of caron
	{ 0x013F, 0x004C, 0x00B7,      0 },		// L + middle dot
	{ 0x0140, 0x006C, 0x00B7,      0 },		// l + middle dot
	{ 0x0143, 0x004E, 0x0301,      0 },		// N + acute
	{ 0x0144, 0x006E, 0x0301,      0 },		// n + acute
	{ 0x0145, 0x004E, 0x0327,      0 },		// N + cedilla
	{ 0x0146, 0x006E, 0x0327,      0 },		// n + cedilla
	{ 0x0147, 0x004E, 0x030C,      0 },		// N + caron
	{ 0x0148, 0x006E, 0x030C,      0 },		// n + caron
	{ 0x0149, 0x006E, 0x02BC,      0 },		// n + apostrophy
	{ 0x014C, 0x004F, 0x0304,      0 },		// O + macron
	{ 0x014D, 0x006F, 0x0304,      0 },		// o + macron
	{ 0x014E, 0x004F, 0x0306,      0 },		// O + breve
	{ 0x014F, 0x006F, 0x0306,      0 },		// o + breve
	{ 0x0150, 0x004F, 0x030B,      0 },		// O + double acute
	{ 0x0151, 0x006F, 0x030B,      0 },		// o + double acute
	{ 0x0154, 0x0052, 0x0301,      0 },		// R + acute
	{ 0x0155, 0x0072, 0x0301,      0 },		// r + acute
	{ 0x0156, 0x0052, 0x0327,      0 },		// R + cedilla
	{ 0x0157, 0x0072, 0x0327,      0 },		// r + cedilla
	{ 0x0158, 0x0052, 0x030C,      0 },		// R + caron
	{ 0x0159, 0x0072, 0x030C,      0 },		// r + caron
	{ 0x015A, 0x0053, 0x0301,      0 },		// S + acute
	{ 0x015B, 0x0073, 0x0301,      0 },		// s + acute
	{ 0x015C, 0x0053, 0x0302,      0 },		// S + circumflex
	{ 0x015D, 0x0073, 0x0302,      0 },		// s + circumflex
	{ 0x015E, 0x0053, 0x0327,      0 },		// S + cedilla
	{ 0x015F, 0x0073, 0x0327,      0 },		// s + cedilla
	{ 0x0160, 0x0053, 0x030C,      0 },		// S + caron
	{ 0x0161, 0x0073, 0x030C,      0 },		// s + caron
	{ 0x0162, 0x0054, 0x0327,      0 },		// T + cedilla
	{ 0x0163, 0x0074, 0x0327,      0 },		// t + cedilla
	{ 0x0164, 0x0054, 0x030C,      0 },		// T + caron
	{ 0x0165, 0x0074, 0x030C,      0 },		// t + caron -- should use apostrophy
	{ 0x0168, 0x0055, 0x0303,      0 },		// U + tilde
	{ 0x0169, 0x0075, 0x0303,      0 },		// u + tilde
	{ 0x016A, 0x0055, 0x0304,      0 },		// U + macron
	{ 0x016B, 0x0075, 0x0304,      0 },		// u + macron
	{ 0x016C, 0x0055, 0x0306,      0 },		// U + breve
	{ 0x016D, 0x0075, 0x0306,      0 },		// u + breve
	{ 0x016E, 0x0055, 0x030A,      0 },		// U + ring above
	{ 0x016F, 0x0075, 0x030A,      0 },		// u + ring above
	{ 0x0170, 0x0055, 0x030B,      0 },		// U + double acute
	{ 0x0171, 0x0075, 0x030B,      0 },		// u + double acute
	{ 0x0172, 0x0055, 0x0328,      0 },		// U + ogonek
	{ 0x0173, 0x0075, 0x0328,      0 },		// u + ogonek
	{ 0x0174, 0x0057, 0x0302,      0 },		// W + circumflex
	{ 0x0175, 0x0077, 0x0302,      0 },		// w + circumflex
	{ 0x0176, 0x0059, 0x0302,      0 },		// Y + circumflex
	{ 0x0177, 0x0079, 0x0302,      0 },		// y + circumflex
	{ 0x0178, 0x0059, 0x0308,      0 },		// Y + diaeresis
	{ 0x0179, 0x005A, 0x0301,      0 },		// Z + acute
	{ 0x017A, 0x007A, 0x0301,      0 },		// z + acute
	{ 0x017B, 0x005A, 0x0307,      0 },		// Z + dot above
	{ 0x017C, 0x007A, 0x0307,      0 },		// z + dot above
	{ 0x017D, 0x005A, 0x030C,      0 },		// Z + caron
	{ 0x017E, 0x007A, 0x030C,      0 },		// z + caron
	{ 0x0300, 0x0020, 0x0300,      0 },		// combining grave accent
	{ 0x0301, 0x0020, 0x0301,      0 },		// combining acute accent
	{ 0x0302, 0x0020, 0x0302,      0 },		// combining circumflex accent
	{ 0x0303, 0x0020, 0x0303,      0 },		// combining tilde
	{ 0x0304, 0x0020, 0x0304,      0 },		// combining macron
	{ 0x0306, 0x0020, 0x0306,      0 },		// combining breve
	{ 0x0307, 0x0020, 0x0307,      0 },		// combining dot above
	{ 0x0308, 0x0020, 0x0308,      0 },		// combining diaeresis above
	{ 0x0309, 0x0020, 0x0309,      0 },		// combining hook above
	{ 0x030A, 0x0020, 0x030A,      0 },		// combining ring above
	{ 0x030B, 0x0020, 0x030B,      0 },		// combining double acute
	{ 0x030C, 0x0020, 0x030C,      0 },		// combining caron
	{ 0x0323, 0x0020, 0x0323,      0 },		// combining dot below
	{ 0x0324, 0x0020, 0x0324,      0 },		// combining diaeresis below
	{ 0x0325, 0x0020, 0x0325,      0 },		// combining ring below
	{ 0x0327, 0x0020, 0x0327,      0 },		// combining cedilla
	{ 0x0328, 0x0020, 0x0328,      0 },		// combining ogonek
	{ 0x032D, 0x0020, 0x032D,      0 },		// combining circumflex below
	{ 0x032E, 0x0020, 0x032E,      0 },		// combining breve below
	{ 0x0330, 0x0020, 0x0330,      0 },		// combining tilde below
	{ 0x0331, 0x0020, 0x0331,      0 },		// combining macron below
	{ 0x1E00, 0x0041, 0x0325,      0 },		// A + ring below
	{ 0x1E01, 0x0061, 0x0325,      0 },		// a + ring below
	{ 0x1E02, 0x0042, 0x0307,      0 },		// B + dot above
	{ 0x1E03, 0x0062, 0x0307,      0 },		// b + dot above
	{ 0x1E04, 0x0042, 0x0323,      0 },		// B + dot below
	{ 0x1E05, 0x0062, 0x0323,      0 },		// b + dot below
	{ 0x1E06, 0x0042, 0x0331,      0 },		// B + macron below
	{ 0x1E07, 0x0062, 0x0331,      0 },		// b + macron below
	{ 0x1E08, 0x0043, 0x0327, 0x0301 },		// C + cedilla + acute
	{ 0x1E09, 0x0063, 0x0327, 0x0301 },		// c + cedilla + acute
	{ 0x1E0A, 0x0044, 0x0307,      0 },		// D + dot above
	{ 0x1E0B, 0x0064, 0x0307,      0 },		// d + dot above
	{ 0x1E0C, 0x0044, 0x0323,      0 },		// D + dot below
	{ 0x1E0D, 0x0064, 0x0323,      0 },		// d + dot below
	{ 0x1E0E, 0x0044, 0x0331,      0 },		// D + macron below
	{ 0x1E0F, 0x0064, 0x0331,      0 },		// d + macron below
	{ 0x1E10, 0x0044, 0x0327,      0 },		// D + cedilla
	{ 0x1E11, 0x0064, 0x0327,      0 },		// d + cedilla
	{ 0x1E12, 0x0044, 0x032D,      0 },		// D + circumflex below
	{ 0x1E13, 0x0064, 0x032D,      0 },		// d + circumflex below
	{ 0x1E14, 0x0045, 0x0304, 0x0300 },		// E + macron + grave
	{ 0x1E15, 0x0065, 0x0304, 0x0300 },		// e + macron + grave
	{ 0x1E16, 0x0045, 0x0304, 0x0301 },		// E + macron + acute
	{ 0x1E17, 0x0065, 0x0304, 0x0301 },		// e + macron + acute
	{ 0x1E18, 0x0045, 0x032D,      0 },		// E + circumflex below
	{ 0x1E19, 0x0065, 0x032D,      0 },		// e + circumflex below
	{ 0x1E1A, 0x0045, 0x0330,      0 },		// E + tilde below
	{ 0x1E1B, 0x0065, 0x0330,      0 },		// e + tilde below
	{ 0x1E1C, 0x0045, 0x0306, 0x0327 },		// E + breve + cedilla
	{ 0x1E1D, 0x0065, 0x0306, 0x0327 },		// e + breve + cedilla
	{ 0x1E1E, 0x0046, 0x0307,      0 },		// F + dot above
	{ 0x1E1F, 0x0066, 0x0307,      0 },		// f + dot above
	{ 0x1E20, 0x0047, 0x0304,      0 },		// G + macron
	{ 0x1E21, 0x0067, 0x0304,      0 },		// g + macron
	{ 0x1E22, 0x0048, 0x0307,      0 },		// H + dot above
	{ 0x1E23, 0x0068, 0x0307,      0 },		// h + dot above
	{ 0x1E24, 0x0048, 0x0323,      0 },		// H + dot below
	{ 0x1E25, 0x0068, 0x0323,      0 },		// h + dot below
	{ 0x1E26, 0x0048, 0x0308,      0 },		// H + diaeresis
	{ 0x1E27, 0x0068, 0x0308,      0 },		// h + diaeresis
	{ 0x1E28, 0x0048, 0x0327,      0 },		// H + cedilla
	{ 0x1E29, 0x0068, 0x0327,      0 },		// h + cedilla
	{ 0x1E2A, 0x0048, 0x032E,      0 },		// H + breve below
	{ 0x1E2B, 0x0068, 0x032E,      0 },		// h + breve below
	{ 0x1E2C, 0x0049, 0x0330,      0 },		// I + tilde below
	{ 0x1E2D, 0x0069, 0x0330,      0 },		// i + tilde below
	{ 0x1E2E, 0x0049, 0x0308, 0x0301 },		// I + diaeresis + acute
	{ 0x1E2F, 0x0069, 0x0308, 0x0301 },		// i + diaeresis + acute
	{ 0x1E30, 0x004B, 0x0301,      0 },		// K + acute
	{ 0x1E31, 0x006B, 0x0301,      0 },		// k + acute
	{ 0x1E32, 0x004B, 0x0323,      0 },		// K + dot below
	{ 0x1E33, 0x006B, 0x0323,      0 },		// k + dot below
	{ 0x1E34, 0x004B, 0x0331,      0 },		// K + macron below
	{ 0x1E35, 0x006B, 0x0331,      0 },		// k + macron below
	{ 0x1E36, 0x004C, 0x0323,      0 },		// L + dot below
	{ 0x1E37, 0x006C, 0x0323,      0 },		// l + dot below
	{ 0x1E38, 0x004C, 0x0323, 0x0304 },		// L + dot below + macron
	{ 0x1E39, 0x006C, 0x0323, 0x0304 },		// l + dot below + macron
	{ 0x1E3A, 0x004C, 0x0331,      0 },		// L + macron below
	{ 0x1E3B, 0x006C, 0x0331,      0 },		// l + macron below
	{ 0x1E3C, 0x004C, 0x032D,      0 },		// L + circumflex below
	{ 0x1E3D, 0x006C, 0x032D,      0 },		// l + circumflex below
	{ 0x1E3E, 0x004D, 0x0301,      0 },		// M + acute
	{ 0x1E3F, 0x006D, 0x0301,      0 },		// m + acute
	{ 0x1E40, 0x004D, 0x0307,      0 },		// M + dot above
	{ 0x1E41, 0x006D, 0x0307,      0 },		// m + dot above
	{ 0x1E42, 0x004D, 0x0323,      0 },		// M + dot below
	{ 0x1E43, 0x006D, 0x0323,      0 },		// m + dot below
	{ 0x1E44, 0x004E, 0x0307,      0 },		// N + dot above
	{ 0x1E45, 0x006E, 0x0307,      0 },		// n + dot above
	{ 0x1E46, 0x004E, 0x0323,      0 },		// N + dot below
	{ 0x1E47, 0x006E, 0x0323,      0 },		// n + dot below
	{ 0x1E48, 0x004E, 0x0331,      0 },		// N + macron below
	{ 0x1E49, 0x006E, 0x0331,      0 },		// n + macron below
	{ 0x1E4A, 0x004E, 0x032D,      0 },		// N + circumflex below
	{ 0x1E4B, 0x006E, 0x032D,      0 },		// n + circumflex below
	{ 0x1E4C, 0x004F, 0x0303, 0x0301 },		// O + tilde + acute
	{ 0x1E4D, 0x006F, 0x0303, 0x0301 },		// o + tilde + acute
	{ 0x1E4E, 0x004F, 0x0303, 0x0308 },		// O + tilde + diaeresis
	{ 0x1E4F, 0x006F, 0x0303, 0x0308 },		// o + tilde + diaeresis
	{ 0x1E50, 0x004F, 0x0304, 0x0300 },		// O + macron + grave
	{ 0x1E51, 0x006F, 0x0304, 0x0300 },		// o + macron + grave
	{ 0x1E52, 0x004F, 0x0304, 0x0301 },		// O + macron + acute
	{ 0x1E53, 0x006F, 0x0304, 0x0301 },		// o + macron + acute
	{ 0x1E54, 0x0050, 0x0301,      0 },		// P + acute
	{ 0x1E55, 0x0070, 0x0301,      0 },		// p + acute
	{ 0x1E56, 0x0050, 0x0307,      0 },		// P + dot above
	{ 0x1E57, 0x0070, 0x0307,      0 },		// p + dot above
	{ 0x1E58, 0x0052, 0x0307,      0 },		// R + dot above
	{ 0x1E59, 0x0072, 0x0307,      0 },		// r + dot above
	{ 0x1E5A, 0x0052, 0x0323,      0 },		// R + dot below
	{ 0x1E5B, 0x0072, 0x0323,      0 },		// r + dot below
	{ 0x1E5C, 0x0052, 0x0323, 0x0304 },		// R + dot below + macron
	{ 0x1E5D, 0x0072, 0x0323, 0x0304 },		// r + dot below + macron
	{ 0x1E5E, 0x0052, 0x0331,      0 },		// R + macron below
	{ 0x1E5F, 0x0072, 0x0331,      0 },		// r + macron below
	{ 0x1E60, 0x0053, 0x0307,      0 },		// S + dot above
	{ 0x1E61, 0x0073, 0x0307,      0 },		// s + dot above
	{ 0x1E62, 0x0053, 0x0323,      0 },		// S + dot below
	{ 0x1E63, 0x0073, 0x0323,      0 },		// s + dot below
	{ 0x1E64, 0x0053, 0x0301, 0x0307 },		// S + acute + dot above
	{ 0x1E65, 0x0073, 0x0301, 0x0307 },		// s + acute + dot above
	{ 0x1E66, 0x0053, 0x030C, 0x0307 },		// S + caron + dot above
	{ 0x1E67, 0x0073, 0x030C, 0x0307 },		// s + caron + dot above
	{ 0x1E68, 0x0053, 0x0307, 0x0323 },		// S + dot above + dot below
	{ 0x1E69, 0x0073, 0x0307, 0x0323 },		// s + dot above + dot below
	{ 0x1E6A, 0x0054, 0x0307,      0 },		// T + dot above
	{ 0x1E6B, 0x0074, 0x0307,      0 },		// t + dot above
	{ 0x1E6C, 0x0054, 0x0323,      0 },		// T + dot below
	{ 0x1E6D, 0x0074, 0x0323,      0 },		// t + dot below
	{ 0x1E6E, 0x0054, 0x0331,      0 },		// T + macron below
	{ 0x1E6F, 0x0074, 0x0331,      0 },		// t + macron below
	{ 0x1E70, 0x0054, 0x032D,      0 },		// T + circumflex below
	{ 0x1E71, 0x0074, 0x032D,      0 },		// t + circumflex below
	{ 0x1E72, 0x0055, 0x0324,      0 },		// U + diaeresis below
	{ 0x1E73, 0x0075, 0x0324,      0 },		// u + diaeresis below
	{ 0x1E74, 0x0055, 0x0330,      0 },		// U + tilde below
	{ 0x1E75, 0x0075, 0x0330,      0 },		// u + tilde below
	{ 0x1E76, 0x0055, 0x032D,      0 },		// U + circumflex below
	{ 0x1E77, 0x0075, 0x032D,      0 },		// u + circumflex below
	{ 0x1E78, 0x0055, 0x0303, 0x0301 },		// U + tilde + acute
	{ 0x1E79, 0x0075, 0x0303, 0x0301 },		// u + tilde + acute
	{ 0x1E7A, 0x0055, 0x0304, 0x0308 },		// U + macron + diaeresis
	{ 0x1E7B, 0x0075, 0x0304, 0x0308 },		// u + macron + diaeresis
	{ 0x1E7C, 0x0056, 0x0303,      0 },		// V + tilde
	{ 0x1E7D, 0x0076, 0x0303,      0 },		// v + tilde
	{ 0x1E7E, 0x0056, 0x0323,      0 },		// V + dot below
	{ 0x1E7F, 0x0076, 0x0323,      0 },		// v + dot below
	{ 0x1E80, 0x0057, 0x0300,      0 },		// W + grave
	{ 0x1E81, 0x0077, 0x0300,      0 },		// w + grave
	{ 0x1E82, 0x0057, 0x0301,      0 },		// W + acute
	{ 0x1E83, 0x0077, 0x0301,      0 },		// w + acute
	{ 0x1E84, 0x0057, 0x0308,      0 },		// W + diaeresis
	{ 0x1E85, 0x0077, 0x0308,      0 },		// w + diaeresis
	{ 0x1E86, 0x0057, 0x0307,      0 },		// W + dot above
	{ 0x1E87, 0x0077, 0x0307,      0 },		// w + dot above
	{ 0x1E88, 0x0057, 0x0323,      0 },		// W + dot below
	{ 0x1E89, 0x0077, 0x0323,      0 },		// w + dot below
	{ 0x1E8A, 0x0058, 0x0307,      0 },		// X + dot above
	{ 0x1E8B, 0x0078, 0x0307,      0 },		// x + dot above
	{ 0x1E8C, 0x0058, 0x0308,      0 },		// X + diaeresis
	{ 0x1E8D, 0x0078, 0x0308,      0 },		// x + diaeresis
	{ 0x1E8E, 0x0059, 0x0307,      0 },		// Y + dot above
	{ 0x1E8F, 0x0079, 0x0307,      0 },		// y + dot above
	{ 0x1E90, 0x0060, 0x0302,      0 },		// Z + circumflex
	{ 0x1E91, 0x0080, 0x0302,      0 },		// z + circumflex
	{ 0x1E92, 0x0060, 0x0323,      0 },		// Z + dot below
	{ 0x1E93, 0x0080, 0x0323,      0 },		// z + dot below
	{ 0x1E94, 0x0060, 0x0331,      0 },		// Z + macron below
	{ 0x1E95, 0x0080, 0x0331,      0 },		// z + macron below
	{ 0x1E96, 0x0068, 0x0331,      0 },		// h + macron below
	{ 0x1E97, 0x0074, 0x0308,      0 },		// t + diaeresis
	{ 0x1E98, 0x0077, 0x030A,      0 },		// w + ring above
	{ 0x1E99, 0x0079, 0x030A,      0 },		// y + ring above
	{ 0x1E9B, 0x017F, 0x0307,      0 },		// Gaelic 'f' + dot above
	{ 0x1EF2, 0x0059, 0x0300,      0 },		// Y + grave
	{ 0x1EF3, 0x0079, 0x0300,      0 },		// y + grave
	{ 0x1EF4, 0x0059, 0x0323,      0 },		// Y + dot below
	{ 0x1EF5, 0x0079, 0x0323,      0 },		// y + dot below
	{ 0x1EF6, 0x0059, 0x0309,      0 },		// Y + hook above
	{ 0x1EF7, 0x0079, 0x0309,      0 },		// y + hook above
	{ 0x1EF8, 0x0059, 0x0303,      0 },		// Y + tilde
	{ 0x1EF9, 0x0079, 0x0303,      0 },		// y + tilde
};



/////////////////////////////////////////////////////////////////////////////
//
//	Maps ASCII symbols to the corresponding Unicode symbols.  Only the
//	symbols between 0x80 and 0xA0 are different.  This table only covers the
//	symbols in that range (plus a few extra to strip out the soft hypen).
//
static U16 g_MapAscii[46] =
{
	0x0000,  // 128 = Euro currency
	0x0000,  // 129 = not supported
	0x002C,  // 130 = comma
	0x017F,  // 131 = old-style 's'
	0x0000,  // 132 = low double quote
	0x0000,  // 133 = ellipsis
	0x0000,  // 134 = dagger
	0x0000,  // 135 = double dagger
	0x005E,  // 136 = ^
	0x0000,  // 137 = permill
	0x0160,  // 138 = s + caron
	0x003C,  // 139 = <
	0x0152,  // 140 = OE
	0x0000,  // 141 = not supported
	0x017D,  // 142 = Z + caron
	0x0000,  // 143 = not supported
	0x0000,  // 144 = not supported
	0x0027,  // 145 = '
	0x0027,  // 146 = '
	0x0022,  // 147 = "
	0x0022,  // 148 = "
	0x00B7,  // 149 = middle dot
	0x2013,  // 150 = en dash
	0x2014,  // 151 = em dash
	0x007E,  // 152 = ~
	0x0000,  // 153 = TM
	0x0161,  // 154 = s + caron
	0x003E,  // 155 = >
	0x0153,  // 156 = oe
	0x0000,  // 157 = not supported
	0x017E,  // 158 = z + caron
	0x0178,  // 159 = Y + diaeresis
	0x0020,  // 160 = space
	0x00A1,  // 161 = inverted exclamation mark
	0x00A2,  // 162 = cent sign
	0x00A3,  // 163 = pound sign
	0x00A4,  // 164 = currency sign
	0x00A5,  // 165 = yen sign
	0x00A6,  // 166 = broken bar
	0x00A7,  // 167 = section sign
	0x00A8,  // 168 = diaeresis
	0x00A9,  // 169 = copyright sign
	0x00AA,  // 170 = Spanish feminine ordinal
	0x00AB,  // 171 = left guillemet
	0x00AC,  // 172 = NOT sign
	0x0000   // 173 = soft hyphen
};


/////////////////////////////////////////////////////////////////////////////
//
//	This table contains the default sorting weights used for lexical sorting.
//	The sorting works in four passes, with each pass having different weights
//	used in the comparisons.  Generally speaking, the first pass compares
//	basic characters (ignoring case and diacritics), the second pass tests
//	diacritics, the third pass tests upper/lower case, and the fourth pass
//	tests if different symbols alias to the same character in the current
//	system locale.
//
//	The default weights in this table are used as the basis for all of the
//	languages that are supported.  Specific languages will apply a few minor
//	modifications to the weights, changing the sorting order of some of the
//	letters, or only those letters that have specific accent marks.
//
UtfSortEntry_t g_UtfDefaultSortTable[] =
{
	{ 0x0020, 0x0209, 0x0020, 0x0002, 0x0020 },	// space
	{ 0x00A0, 0x0209, 0x0020, 0x001B, 0x00A0 },	// non-breaking space
	{ 0x0060, 0x020D, 0x0020, 0x0002, 0x0060 },	// ` grave
	{ 0x00B4, 0x020E, 0x0020, 0x0002, 0x00B4 },	// acute accent
	{ 0x005E, 0x0210, 0x0020, 0x0002, 0x005E },	// ^ circumflex
	{ 0x0300, 0x0210, 0x0020, 0x0002, 0x0300 },	// combining grave accent
	{ 0x0301, 0x0210, 0x0020, 0x0002, 0x0301 },	// combining acute accent
	{ 0x0302, 0x0210, 0x0020, 0x0002, 0x0302 },	// combining circumflex accent
	{ 0x0303, 0x0210, 0x0020, 0x0002, 0x0303 },	// combining tilde
	{ 0x0304, 0x0210, 0x0020, 0x0002, 0x0304 },	// combining macron
	{ 0x0306, 0x0210, 0x0020, 0x0002, 0x0306 },	// combining breve
	{ 0x0307, 0x0210, 0x0020, 0x0002, 0x0307 },	// combining dot above
	{ 0x0308, 0x0210, 0x0020, 0x0002, 0x0308 },	// combining diaeresis above
	{ 0x0309, 0x0210, 0x0020, 0x0002, 0x0309 },	// combining hook above
	{ 0x030A, 0x0210, 0x0020, 0x0002, 0x030A },	// combining ring above
	{ 0x030B, 0x0210, 0x0020, 0x0002, 0x030B },	// combining double acute
	{ 0x030C, 0x0210, 0x0020, 0x0002, 0x030C },	// combining caron
	{ 0x0312, 0x0210, 0x0020, 0x0002, 0x030C },	// combining turned comma above
	{ 0x0323, 0x0210, 0x0020, 0x0002, 0x0307 },	// combining dot below
	{ 0x0324, 0x0210, 0x0020, 0x0002, 0x0308 },	// combining diaeresis below
	{ 0x0325, 0x0210, 0x0020, 0x0002, 0x030A },	// combining ring below
	{ 0x0326, 0x0210, 0x0020, 0x0002, 0x0326 },	// combining comma below
	{ 0x0327, 0x0210, 0x0020, 0x0002, 0x0327 },	// combining cedilla
	{ 0x0328, 0x0210, 0x0020, 0x0002, 0x0328 },	// combining ogonek
	{ 0x032D, 0x0210, 0x0020, 0x0002, 0x032D },	// combining circumflex below
	{ 0x032E, 0x0210, 0x0020, 0x0002, 0x032E },	// combining breve below
	{ 0x0330, 0x0210, 0x0020, 0x0002, 0x0330 },	// combining tilde below
	{ 0x0331, 0x0210, 0x0020, 0x0002, 0x0331 },	// combining macron below
	{ 0x00AF, 0x0211, 0x0020, 0x0002, 0x00AF },	// macron
	{ 0x00A8, 0x0215, 0x0020, 0x0002, 0x00A8 },	// diaeresis
	{ 0x00B8, 0x021A, 0x0020, 0x0002, 0x00B8 },	// cedilla
	{ 0x005F, 0x021C, 0x0020, 0x0002, 0x005F },	// _
	{ 0x002D, 0x0222, 0x0020, 0x0002, 0x002D },	// -
	{ 0x2013, 0x0229, 0x0020, 0x0002, 0x2013 },	// en dash
	{ 0x2014, 0x022A, 0x0020, 0x0002, 0x2014 },	// em dash
	{ 0x2018, 0x02A1, 0x0020, 0x0002, 0x2018 },	// left single quote
	{ 0x2019, 0x02A2, 0x0020, 0x0002, 0x2019 },	// right single quote
	{ 0x201C, 0x02A8, 0x0020, 0x0002, 0x201C },	// left double quote
	{ 0x201D, 0x02A9, 0x0020, 0x0002, 0x201D },	// right double quote
	{ 0x2022, 0x030D, 0x0020, 0x0002, 0x2022 },	// bullet
	{ 0x002C, 0x0232, 0x0020, 0x0002, 0x002C },	// ,
	{ 0x003B, 0x023E, 0x0020, 0x0002, 0x003B },	// ;
	{ 0x003A, 0x0241, 0x0020, 0x0002, 0x003A },	// :
	{ 0x0021, 0x0257, 0x0020, 0x0002, 0x0021 },	// !
	{ 0x00A1, 0x0258, 0x0020, 0x0002, 0x00A1 },	// inverted exclamation mark
	{ 0x003F, 0x025C, 0x0020, 0x0002, 0x003F },	// ?
	{ 0x00BF, 0x025D, 0x0020, 0x0002, 0x00BF },	// inverted question mark
	{ 0x002E, 0x0266, 0x0020, 0x0002, 0x002E },	// .
	{ 0x00B7, 0x0273, 0x0020, 0x0002, 0x00B7 },	// middle dot
	{ 0x0027, 0x02A0, 0x0020, 0x0002, 0x0027 },	// '
	{ 0x0022, 0x02A7, 0x0020, 0x0002, 0x0022 },	// "
	{ 0x00AB, 0x02AF, 0x0020, 0x0002, 0x00AB },	// left guillemet
	{ 0x00BB, 0x02B0, 0x0020, 0x0002, 0x00BB },	// right guillemet
	{ 0x0028, 0x02B1, 0x0020, 0x0002, 0x0028 },	// (
	{ 0x0029, 0x02B2, 0x0020, 0x0002, 0x0029 },	// )
	{ 0x005B, 0x02B3, 0x0020, 0x0002, 0x005B },	// [
	{ 0x005D, 0x02B4, 0x0020, 0x0002, 0x005D },	// ]
	{ 0x007B, 0x02B5, 0x0020, 0x0002, 0x007B },	// {
	{ 0x007D, 0x02B6, 0x0020, 0x0002, 0x007D },	// }
	{ 0x00A7, 0x02F5, 0x0020, 0x0002, 0x00A7 },	// section sign
	{ 0x00B6, 0x02F6, 0x0020, 0x0002, 0x00B6 },	// pilcrow/paragraph sign
	{ 0x00A9, 0x02F8, 0x0020, 0x0002, 0x00A9 },	// copyright sign (C)
	{ 0x00AE, 0x02F9, 0x0020, 0x0002, 0x00AE },	// registered sign (R)
	{ 0x0040, 0x02FA, 0x0020, 0x0002, 0x0040 },	// @
	{ 0x002A, 0x02FB, 0x0020, 0x0002, 0x002A },	// *
	{ 0x002F, 0x02FF, 0x0020, 0x0002, 0x002F },	// /
	{ 0x005C, 0x0301, 0x0020, 0x0002, 0x005C },	// '\'
	{ 0x0026, 0x0302, 0x0020, 0x0002, 0x0026 },	// &
	{ 0x0023, 0x0305, 0x0020, 0x0002, 0x0023 },	// #
	{ 0x0025, 0x0306, 0x0020, 0x0002, 0x0025 },	// %
	{ 0x00B0, 0x03BF, 0x0020, 0x0002, 0x00B0 },	// degree sign
	{ 0x002B, 0x04B8, 0x0020, 0x0002, 0x002B },	// +
	{ 0x00B1, 0x04B9, 0x0020, 0x0002, 0x00B1 },	// plus-minus sign
	{ 0x00F7, 0x04BA, 0x0020, 0x0002, 0x00F7 },	// division sign
	{ 0x00D7, 0x04BB, 0x0020, 0x0002, 0x00D7 },	// multiplication sign
	{ 0x003C, 0x04BC, 0x0020, 0x0002, 0x003C },	// <
	{ 0x003D, 0x04BD, 0x0020, 0x0002, 0x003D },	// =
	{ 0x003E, 0x04BE, 0x0020, 0x0002, 0x003E },	// >
	{ 0x00AC, 0x04BF, 0x0020, 0x0002, 0x00AC },	// NOT sign
	{ 0x007C, 0x04C0, 0x0020, 0x0002, 0x007C },	// |
	{ 0x00A6, 0x04C1, 0x0020, 0x0002, 0x00A6 },	// broken bar, old version of '|'
	{ 0x007E, 0x04C3, 0x0020, 0x0002, 0x007E },	// ~
	{ 0x00A2, 0x0FA6, 0x0020, 0x0002, 0x00A2 },	// cent sign
	{ 0x0024, 0x0FA7, 0x0020, 0x0002, 0x0024 },	// $
	{ 0x00A3, 0x0FA8, 0x0020, 0x0002, 0x00A3 },	// pound sign
	{ 0x00A4, 0x0FA5, 0x0020, 0x0002, 0x00A4 },	// currency sign
	{ 0x00A5, 0x0FA9, 0x0020, 0x0002, 0x00A5 },	// yen sign
	{ 0x0030, 0x0FC6, 0x0020, 0x0002, 0x0030 },	// 0
	{ 0x0031, 0x0FC7, 0x0020, 0x0002, 0x0031 },	// 1
	{ 0x00B9, 0x0FC7, 0x0020, 0x0014, 0x00B9 },	// super-script 1
	{ 0x00BC, 0x0FC7, 0x0020, 0x001E, 0x00BC },	// one-quarter fraction
	{ 0x00BD, 0x0FC7, 0x0020, 0x001E, 0x00BD },	// one-half fraction
	{ 0x0032, 0x0FC8, 0x0020, 0x0002, 0x0032 },	// 2
	{ 0x00B2, 0x0FC8, 0x0020, 0x0014, 0x00B2 },	// super-script 2
	{ 0x0033, 0x0FC9, 0x0020, 0x0002, 0x0033 },	// 3
	{ 0x00B3, 0x0FC9, 0x0020, 0x0014, 0x00B3 },	// super-script 3
	{ 0x00BE, 0x0FC9, 0x0020, 0x001E, 0x00BE },	// three-quarter fraction
	{ 0x0034, 0x0FCA, 0x0020, 0x0002, 0x0034 },	// 4
	{ 0x0035, 0x0FCB, 0x0020, 0x0002, 0x0035 },	// 5
	{ 0x0036, 0x0FCC, 0x0020, 0x0002, 0x0036 },	// 6
	{ 0x0037, 0x0FCD, 0x0020, 0x0002, 0x0037 },	// 7
	{ 0x0038, 0x0FCE, 0x0020, 0x0002, 0x0038 },	// 8
	{ 0x0039, 0x0FCF, 0x0020, 0x0002, 0x0039 },	// 9
	{ 0x0041, 0x0FD0, 0x0020, 0x0002, 0x0041 },	// A
	{ 0x0061, 0x0FD0, 0x0020, 0x0008, 0x0061 },	// a
	{ 0x00AA, 0x0FD0, 0x0020, 0x0014, 0x00AA },	// Spanish feminine ordinal
	{ 0x00C1, 0x0FD0, 0x0032, 0x0002, 0x0301 },	// A + acute
	{ 0x00E1, 0x0FD0, 0x0032, 0x0008, 0x0301 },	// a + acute
	{ 0x00C0, 0x0FD0, 0x0035, 0x0002, 0x0300 },	// A + grave
	{ 0x00E0, 0x0FD0, 0x0035, 0x0008, 0x0300 },	// a + grave
	{ 0x0102, 0x0FD0, 0x0037, 0x0002, 0x0306 },	// A + breve
	{ 0x0103, 0x0FD0, 0x0037, 0x0008, 0x0306 },	// a + breve
	{ 0x00C2, 0x0FD0, 0x003C, 0x0002, 0x0302 },	// A + circumflex
	{ 0x00E2, 0x0FD0, 0x003C, 0x0008, 0x0302 },	// a + circumflex
	{ 0x00C5, 0x0FD0, 0x0043, 0x0002, 0x030A },	// A + ring above
	{ 0x00E5, 0x0FD0, 0x0043, 0x0008, 0x030A },	// a + ring above
	{ 0x00C4, 0x0FD0, 0x0047, 0x0002, 0x0308 },	// A + diaeresis
	{ 0x00E4, 0x0FD0, 0x0047, 0x0008, 0x0308 },	// a + diaeresis
	{ 0x00C3, 0x0FD0, 0x004E, 0x0002, 0x0303 },	// A + tilde
	{ 0x00E3, 0x0FD0, 0x004E, 0x0008, 0x0303 },	// a + tilde
	{ 0x0104, 0x0FD0, 0x0059, 0x0002, 0x0328 },	// A + ogonek
	{ 0x0105, 0x0FD0, 0x0059, 0x0008, 0x0328 },	// a + ogonek
	{ 0x0100, 0x0FD0, 0x005B, 0x0002, 0x0304 },	// A + macron
	{ 0x0101, 0x0FD0, 0x005B, 0x0008, 0x0304 },	// a + macron
	{ 0x1E00, 0x0FD0, 0x0080, 0x0002, 0x0325 },	// A + ring below
	{ 0x1E01, 0x0FD0, 0x0080, 0x0008, 0x0325 },	// a + ring below
	{ 0x00C6, 0x0FD0, 0x0020, 0x0004, 0x00C6 },	// 'AE'
	{ 0x00E6, 0x0FD0, 0x0020, 0x000A, 0x00E6 },	// 'ae'
	{ 0x0042, 0x0FE6, 0x0020, 0x0002, 0x0042 },	// B
	{ 0x0062, 0x0FE6, 0x0020, 0x0008, 0x0062 },	// b
	{ 0x1E02, 0x0FE6, 0x0052, 0x0002, 0x0307 },	// B + dot above
	{ 0x1E03, 0x0FE6, 0x0052, 0x0008, 0x0307 },	// b + dot above
	{ 0x1E04, 0x0FE6, 0x007A, 0x0002, 0x0323 },	// B + dot below
	{ 0x1E05, 0x0FE6, 0x007A, 0x0008, 0x0323 },	// b + dot below
	{ 0x1E06, 0x0FE6, 0x008A, 0x0002, 0x0331 },	// B + macron below
	{ 0x1E07, 0x0FE6, 0x008A, 0x0008, 0x0331 },	// b + macron below
	{ 0x0043, 0x0FFE, 0x0020, 0x0002, 0x0043 },	// C
	{ 0x0063, 0x0FFE, 0x0020, 0x0008, 0x0063 },	// c
	{ 0x0106, 0x0FFE, 0x0032, 0x0002, 0x0301 },	// C + acute
	{ 0x0107, 0x0FFE, 0x0032, 0x0008, 0x0301 },	// c + acute
	{ 0x0108, 0x0FFE, 0x003C, 0x0002, 0x0302 },	// C + circumflex
	{ 0x0109, 0x0FFE, 0x003C, 0x0008, 0x0302 },	// c + circumflex
	{ 0x010C, 0x0FFE, 0x0041, 0x0002, 0x030C },	// C + caron
	{ 0x010D, 0x0FFE, 0x0041, 0x0008, 0x030C },	// c + caron
	{ 0x010A, 0x0FFE, 0x0052, 0x0002, 0x0307 },	// C + dot above
	{ 0x010B, 0x0FFE, 0x0052, 0x0008, 0x0307 },	// c + dot above
	{ 0x00C7, 0x0FFE, 0x0056, 0x0002, 0x0327 },	// C + cedilla
	{ 0x00E7, 0x0FFE, 0x0056, 0x0008, 0x0327 },	// c + cedilla
	{ 0x1E08, 0x0FFE, 0x0057, 0x0002, 0x0327 },	// C + cedilla + acute
	{ 0x1E09, 0x0FFE, 0x0057, 0x0008, 0x0327 },	// c + cedilla + acute
	{ 0x0044, 0x1010, 0x0020, 0x0002, 0x0044 },	// D
	{ 0x00F0, 0x1010, 0x0020, 0x0004, 0x00F0 },	// Latin letter 'eth'
	{ 0x0064, 0x1010, 0x0020, 0x0008, 0x0064 },	// d
	{ 0x010E, 0x1010, 0x0041, 0x0002, 0x030C },	// D + caron
	{ 0x010F, 0x1010, 0x0041, 0x0008, 0x030C },	// d + caron
	{ 0x1E0A, 0x1010, 0x0052, 0x0002, 0x0307 },	// D + dot above
	{ 0x1E0B, 0x1010, 0x0052, 0x0008, 0x0307 },	// d + dot above
	{ 0x1E10, 0x1010, 0x0056, 0x0002, 0x0327 },	// D + cedilla
	{ 0x1E11, 0x1010, 0x0056, 0x0008, 0x0327 },	// d + cedilla
	{ 0x1E0C, 0x1010, 0x007A, 0x0002, 0x0323 },	// D + dot below
	{ 0x1E0D, 0x1010, 0x007A, 0x0008, 0x0323 },	// d + dot below
	{ 0x1E12, 0x1010, 0x0086, 0x0002, 0x032D },	// D + circumflex below
	{ 0x1E13, 0x1010, 0x0086, 0x0008, 0x032D },	// d + circumflex below
	{ 0x1E0E, 0x1010, 0x008A, 0x0002, 0x0331 },	// D + macron below
	{ 0x1E0F, 0x1010, 0x008A, 0x0008, 0x0331 },	// d + macron below
	{ 0x0110, 0x1010, 0x008D, 0x0002, 0x0335 },	// D + stroke
	{ 0x0111, 0x1010, 0x008D, 0x0008, 0x0335 },	// d + stroke
	{ 0x00D0, 0x1010, 0x0199, 0x0004, 0x00D0 },	// Latin letter 'ETH'
	{ 0x0045, 0x1029, 0x0020, 0x0002, 0x0045 },	// E
	{ 0x0065, 0x1029, 0x0020, 0x0008, 0x0065 },	// e
	{ 0x00C9, 0x1029, 0x0032, 0x0002, 0x0301 },	// E + acute
	{ 0x00E9, 0x1029, 0x0032, 0x0008, 0x0301 },	// e + acute
	{ 0x1E16, 0x1029, 0x0032, 0x0002, 0x0301 },	// E + macron + acute
	{ 0x1E17, 0x1029, 0x0032, 0x0008, 0x0301 },	// e + macron + acute
	{ 0x00C8, 0x1029, 0x0035, 0x0002, 0x0300 },	// E + grave
	{ 0x00E8, 0x1029, 0x0035, 0x0008, 0x0300 },	// e + grave
	{ 0x1E14, 0x1029, 0x0035, 0x0002, 0x0300 },	// E + macron + grave
	{ 0x1E15, 0x1029, 0x0035, 0x0008, 0x0300 },	// e + macron + grave
	{ 0x0114, 0x1029, 0x0037, 0x0002, 0x0306 },	// E + breve
	{ 0x0115, 0x1029, 0x0037, 0x0008, 0x0306 },	// e + breve
	{ 0x1E1C, 0x1029, 0x0037, 0x0002, 0x0306 },	// E + cedilla + breve
	{ 0x1E1D, 0x1029, 0x0037, 0x0008, 0x0306 },	// e + cedilla + breve
	{ 0x00CA, 0x1029, 0x003C, 0x0002, 0x0302 },	// E + circumflex
	{ 0x00EA, 0x1029, 0x003C, 0x0008, 0x0302 },	// e + circumflex
	{ 0x011A, 0x1029, 0x0041, 0x0002, 0x030C },	// E + caron
	{ 0x011B, 0x1029, 0x0041, 0x0008, 0x030C },	// e + caron
	{ 0x00CB, 0x1029, 0x0047, 0x0002, 0x0308 },	// E + diaeresis
	{ 0x00EB, 0x1029, 0x0047, 0x0008, 0x0308 },	// e + diaeresis
	{ 0x0116, 0x1029, 0x0052, 0x0002, 0x0307 },	// E + dot above
	{ 0x0117, 0x1029, 0x0052, 0x0008, 0x0307 },	// e + dot above
	{ 0x0118, 0x1029, 0x0059, 0x0002, 0x0328 },	// E + ogonek
	{ 0x0119, 0x1029, 0x0059, 0x0008, 0x0328 },	// e + ogonek
	{ 0x0112, 0x1029, 0x005B, 0x0002, 0x0304 },	// E + macron
	{ 0x0113, 0x1029, 0x005B, 0x0008, 0x0304 },	// e + macron
	{ 0x1E18, 0x1029, 0x0086, 0x0002, 0x032D },	// E + circumflex below
	{ 0x1E19, 0x1029, 0x0086, 0x0008, 0x032D },	// e + circumflex below
	{ 0x1E1A, 0x1029, 0x0089, 0x0002, 0x0330 },	// E + tilde below
	{ 0x1E1B, 0x1029, 0x0089, 0x0008, 0x0330 },	// e + tilde below
	{ 0x0046, 0x105F, 0x0020, 0x0002, 0x0046 },	// F
	{ 0x0066, 0x105F, 0x0020, 0x0008, 0x0066 },	// f
	{ 0x1E1E, 0x105F, 0x0052, 0x0002, 0x0307 },	// F + dot above
	{ 0x1E1F, 0x105F, 0x0052, 0x0008, 0x0307 },	// f + dot above
	{ 0x0047, 0x106A, 0x0020, 0x0002, 0x0047 },	// G
	{ 0x0067, 0x106A, 0x0020, 0x0008, 0x0067 },	// g
	{ 0x011E, 0x106A, 0x0037, 0x0002, 0x0306 },	// G + breve
	{ 0x011F, 0x106A, 0x0037, 0x0008, 0x0306 },	// g + breve
	{ 0x011C, 0x106A, 0x003C, 0x0002, 0x0302 },	// G + circumflex
	{ 0x011D, 0x106A, 0x003C, 0x0008, 0x0302 },	// g + circumflex
	{ 0x0120, 0x106A, 0x0052, 0x0002, 0x0307 },	// G + dot above
	{ 0x0121, 0x106A, 0x0052, 0x0008, 0x0307 },	// g + dot above
	{ 0x0122, 0x106A, 0x0056, 0x0002, 0x0327 },	// G + cedilla
	{ 0x0123, 0x106A, 0x0056, 0x0008, 0x0327 },	// g + cedilla
	{ 0x1E20, 0x106A, 0x005B, 0x0002, 0x0304 },	// G + macron
	{ 0x1E21, 0x106A, 0x005B, 0x0008, 0x0304 },	// g + macron
	{ 0x0048, 0x108D, 0x0020, 0x0002, 0x0048 },	// H
	{ 0x0068, 0x108D, 0x0020, 0x0008, 0x0068 },	// h
	{ 0x0124, 0x108D, 0x003C, 0x0002, 0x0302 },	// H + circumflex
	{ 0x0125, 0x108D, 0x003C, 0x0008, 0x0302 },	// h + circumflex
	{ 0x1E26, 0x108D, 0x0047, 0x0002, 0x0308 },	// H + diaeresis
	{ 0x1E27, 0x108D, 0x0047, 0x0008, 0x0308 },	// h + diaeresis
	{ 0x1E22, 0x108D, 0x0052, 0x0002, 0x0307 },	// H + dot above
	{ 0x1E23, 0x108D, 0x0052, 0x0008, 0x0307 },	// h + dot above
	{ 0x1E28, 0x108D, 0x0056, 0x0002, 0x0327 },	// H + cedilla
	{ 0x1E29, 0x108D, 0x0056, 0x0008, 0x0327 },	// h + cedilla
	{ 0x1E24, 0x108D, 0x007A, 0x0002, 0x0323 },	// H + dot below
	{ 0x1E25, 0x108D, 0x007A, 0x0008, 0x0323 },	// h + dot below
	{ 0x1E2A, 0x108D, 0x0087, 0x0002, 0x032E },	// H + breve below
	{ 0x1E2B, 0x108D, 0x0087, 0x0008, 0x032E },	// h + breve below
	{ 0x1E96, 0x108D, 0x008A, 0x0002, 0x0331 },	// h + macron below
	{ 0x0126, 0x108D, 0x008D, 0x0008, 0x0335 },	// H + stroke
	{ 0x0127, 0x108D, 0x008D, 0x0002, 0x0335 },	// h + stroke
	{ 0x0049, 0x10A5, 0x0020, 0x0002, 0x0049 },	// I
	{ 0x0069, 0x10A5, 0x0020, 0x0008, 0x0069 },	// i
	{ 0xFFD0, 0x10A5, 0x0020, 0x0008, 0x0049 },	// i + soft dot (doesn't need to be in this table, but necessary to aviod triggering error in unit tests)
	{ 0x00CD, 0x10A5, 0x0032, 0x0002, 0x0301 },	// I + acute
	{ 0x00ED, 0x10A5, 0x0032, 0x0008, 0x0301 },	// i + acute
	{ 0x1E2E, 0x10A5, 0x0032, 0x0002, 0x0301 },	// I + diaeresis + acute
	{ 0x1E2F, 0x10A5, 0x0032, 0x0008, 0x0301 },	// i + diaeresis + acute
	{ 0x00CC, 0x10A5, 0x0035, 0x0002, 0x0300 },	// I + grave
	{ 0x00EC, 0x10A5, 0x0035, 0x0008, 0x0300 },	// i + grave
	{ 0x012C, 0x10A5, 0x0037, 0x0002, 0x0306 },	// I + breve
	{ 0x012D, 0x10A5, 0x0037, 0x0008, 0x0306 },	// i + breve
	{ 0x00CE, 0x10A5, 0x003C, 0x0002, 0x0302 },	// I + circumflex
	{ 0x00EE, 0x10A5, 0x003C, 0x0008, 0x0302 },	// i + circumflex
	{ 0x00CF, 0x10A5, 0x0047, 0x0002, 0x0308 },	// I + diaeresis
	{ 0x00EF, 0x10A5, 0x0047, 0x0008, 0x0308 },	// i + diaeresis
	{ 0x0128, 0x10A5, 0x004E, 0x0002, 0x0303 },	// I + tilde
	{ 0x0129, 0x10A5, 0x004E, 0x0008, 0x0303 },	// i + tilde
	{ 0x012E, 0x10A5, 0x0059, 0x0002, 0x0328 },	// I + ogonek
	{ 0x012F, 0x10A5, 0x0059, 0x0008, 0x0328 },	// i + ogonek
	{ 0x012A, 0x10A5, 0x005B, 0x0002, 0x0304 },	// I + macron
	{ 0x012B, 0x10A5, 0x005B, 0x0008, 0x0304 },	// i + macron
	{ 0x1E2C, 0x10A5, 0x0089, 0x0002, 0x0330 },	// I + tilde below
	{ 0x1E2D, 0x10A5, 0x0089, 0x0008, 0x0330 },	// i + tilde below
	{ 0x004A, 0x10BD, 0x0020, 0x0002, 0x004A },	// J
	{ 0x006A, 0x10BD, 0x0020, 0x0008, 0x006A },	// j
	{ 0xFFD1, 0x10BD, 0x0020, 0x0008, 0x004A },	// j + soft dot (doesn't need to be in this table, but necessary to aviod triggering error in unit tests)
	{ 0x0134, 0x10BD, 0x003C, 0x0002, 0x0302 },	// J + circumflex
	{ 0x0135, 0x10BD, 0x003C, 0x0008, 0x0302 },	// j + circumflex
	{ 0x004B, 0x10D6, 0x0020, 0x0002, 0x004B },	// K
	{ 0x006B, 0x10D6, 0x0020, 0x0008, 0x006B },	// k
	{ 0x1E30, 0x10D6, 0x0032, 0x0002, 0x0301 },	// K + acute
	{ 0x1E31, 0x10D6, 0x0032, 0x0008, 0x0301 },	// k + acute
	{ 0x0136, 0x10D6, 0x0056, 0x0002, 0x0327 },	// K + cedilla
	{ 0x0137, 0x10D6, 0x0056, 0x0008, 0x0327 },	// k + cedilla
	{ 0x1E32, 0x10D6, 0x007A, 0x0002, 0x0323 },	// K + dot below
	{ 0x1E33, 0x10D6, 0x007A, 0x0008, 0x0323 },	// k + dot below
	{ 0x1E34, 0x10D6, 0x008A, 0x0002, 0x0331 },	// K + macron below
	{ 0x1E35, 0x10D6, 0x008A, 0x0008, 0x0331 },	// k + macron below
	{ 0x004C, 0x10E5, 0x0020, 0x0002, 0x004C },	// L
	{ 0x006C, 0x10E5, 0x0020, 0x0008, 0x006C },	// l
	{ 0x0139, 0x10E5, 0x0032, 0x0002, 0x0301 },	// L + acute
	{ 0x013A, 0x10E5, 0x0032, 0x0008, 0x0301 },	// l + acute
	{ 0x013D, 0x10E5, 0x0041, 0x0002, 0x030C },	// L + caron
	{ 0x013E, 0x10E5, 0x0041, 0x0008, 0x030C },	// l + caron
	{ 0x013B, 0x10E5, 0x0056, 0x0002, 0x0327 },	// L + cedilla
	{ 0x013C, 0x10E5, 0x0056, 0x0008, 0x0327 },	// l + cedilla
	{ 0x1E38, 0x10E5, 0x005B, 0x0002, 0x0304 },	// L + dot below + macron
	{ 0x1E39, 0x10E5, 0x005B, 0x0008, 0x0304 },	// l + dot below + macron
	{ 0x1E36, 0x10E5, 0x007A, 0x0002, 0x0323 },	// L + dot below
	{ 0x1E37, 0x10E5, 0x007A, 0x0008, 0x0323 },	// l + dot below
	{ 0x1E3C, 0x10E5, 0x0086, 0x0002, 0x032D },	// L + circumflex below
	{ 0x1E3D, 0x10E5, 0x0086, 0x0008, 0x032D },	// l + circumflex below
	{ 0x1E3A, 0x10E5, 0x008A, 0x0002, 0x0331 },	// L + macron below
	{ 0x1E3B, 0x10E5, 0x008A, 0x0008, 0x0331 },	// l + macron below
	{ 0x0141, 0x10E5, 0x008D, 0x0002, 0x0335 },	// L + stroke
	{ 0x0142, 0x10E5, 0x008D, 0x0008, 0x0335 },	// l + stroke
	{ 0x013F, 0x10E5, 0x0199, 0x000A, 0x013F },	// L + middle dot
	{ 0x0140, 0x10E5, 0x0199, 0x0004, 0x013F },	// l + middle dot
	{ 0x004D, 0x1110, 0x0020, 0x0002, 0x004D },	// M
	{ 0x006D, 0x1110, 0x0020, 0x0008, 0x006D },	// m
	{ 0x1E3E, 0x1110, 0x0032, 0x0002, 0x0301 },	// M + acute
	{ 0x1E3F, 0x1110, 0x0032, 0x0008, 0x0301 },	// m + acute
	{ 0x1E40, 0x1110, 0x0052, 0x0002, 0x0307 },	// M + dot above
	{ 0x1E41, 0x1110, 0x0052, 0x0008, 0x0307 },	// m + dot above
	{ 0x1E42, 0x1110, 0x007A, 0x0002, 0x0323 },	// M + dot below
	{ 0x1E43, 0x1110, 0x007A, 0x0008, 0x0323 },	// m + dot below
	{ 0x004E, 0x111B, 0x0020, 0x0002, 0x004E },	// N
	{ 0x006E, 0x111B, 0x0020, 0x0008, 0x006E },	// n
	{ 0x0149, 0x111B, 0x0020, 0x000A, 0x0149 },	// n + apostrophy
	{ 0x0143, 0x111B, 0x0032, 0x0002, 0x0301 },	// N + acute
	{ 0x0144, 0x111B, 0x0032, 0x0008, 0x0301 },	// n + acute
	{ 0x0147, 0x111B, 0x0041, 0x0002, 0x030C },	// N + caron
	{ 0x0148, 0x111B, 0x0041, 0x0008, 0x030C },	// n + caron
	{ 0x00D1, 0x111B, 0x004E, 0x0002, 0x0303 },	// N + tilde
	{ 0x00F1, 0x111B, 0x004E, 0x0008, 0x0303 },	// n + tilde
	{ 0x0145, 0x111B, 0x0056, 0x0002, 0x0327 },	// N + cedilla
	{ 0x0146, 0x111B, 0x0056, 0x0008, 0x0327 },	// n + cedilla
	{ 0x1E44, 0x111B, 0x0052, 0x0002, 0x0307 },	// N + dot above
	{ 0x1E45, 0x111B, 0x0052, 0x0008, 0x0307 },	// n + dot above
	{ 0x1E46, 0x111B, 0x007A, 0x0002, 0x0323 },	// N + dot below
	{ 0x1E47, 0x111B, 0x007A, 0x0008, 0x0323 },	// n + dot below
	{ 0x1E4A, 0x111B, 0x0086, 0x0002, 0x032D },	// N + circumflex below
	{ 0x1E4B, 0x111B, 0x0086, 0x0008, 0x032D },	// n + circumflex below
	{ 0x1E48, 0x111B, 0x008A, 0x0002, 0x0331 },	// N + macron below
	{ 0x1E49, 0x111B, 0x008A, 0x0008, 0x0331 },	// n + macron below
	{ 0x014A, 0x1137, 0x0020, 0x0002, 0x014A },	// ENJ
	{ 0x014B, 0x1137, 0x0020, 0x0008, 0x014B },	// enj
	{ 0x004F, 0x113B, 0x0020, 0x0002, 0x004F },	// O
	{ 0x0152, 0x113B, 0x0020, 0x0004, 0x0152 },	// OE
	{ 0x006F, 0x113B, 0x0020, 0x0008, 0x006F },	// o
	{ 0x0153, 0x113B, 0x0020, 0x000A, 0x0152 },	// oe
	{ 0x00BA, 0x113B, 0x0020, 0x0014, 0x00BA },	// Spanish masculine ordinal
	{ 0x00D3, 0x113B, 0x0032, 0x0002, 0x0301 },	// O + acute
	{ 0x00F3, 0x113B, 0x0032, 0x0008, 0x0301 },	// o + acute
	{ 0x1E4C, 0x113B, 0x0032, 0x0002, 0x0301 },	// O + tilde + acute
	{ 0x1E4D, 0x113B, 0x0032, 0x0008, 0x0301 },	// o + tilde + acute
	{ 0x1E52, 0x113B, 0x0032, 0x0002, 0x0301 },	// O + macron + acute
	{ 0x1E53, 0x113B, 0x0032, 0x0008, 0x0301 },	// o + macron + acute
	{ 0x00D2, 0x113B, 0x0035, 0x0002, 0x0300 },	// O + grave
	{ 0x00F2, 0x113B, 0x0035, 0x0008, 0x0300 },	// o + grave
	{ 0x1E50, 0x113B, 0x0035, 0x0002, 0x0300 },	// O + macron + grave
	{ 0x1E51, 0x113B, 0x0035, 0x0008, 0x0300 },	// o + macron + grave
	{ 0x014E, 0x113B, 0x0037, 0x0002, 0x0306 },	// O + breve
	{ 0x014F, 0x113B, 0x0037, 0x0008, 0x0306 },	// o + breve
	{ 0x00D4, 0x113B, 0x003C, 0x0002, 0x0302 },	// O + circumflex
	{ 0x00F4, 0x113B, 0x003C, 0x0008, 0x0302 },	// o + circumflex
	{ 0x00D5, 0x113B, 0x004E, 0x0002, 0x0303 },	// O + tilde
	{ 0x00F5, 0x113B, 0x004E, 0x0008, 0x0303 },	// o + tilde
	{ 0x00D6, 0x113B, 0x0047, 0x0002, 0x0308 },	// O + diaeresis
	{ 0x00F6, 0x113B, 0x0047, 0x0008, 0x0308 },	// o + diaeresis
	{ 0x1E4E, 0x113B, 0x0047, 0x0002, 0x0308 },	// O + tilde + diaeresis
	{ 0x1E4F, 0x113B, 0x0047, 0x0008, 0x0308 },	// o + tilde + diaeresis
	{ 0x0150, 0x113B, 0x004D, 0x0002, 0x030B },	// O + double acute
	{ 0x0151, 0x113B, 0x004D, 0x0008, 0x030B },	// o + double acute
	{ 0x00D8, 0x113B, 0x0054, 0x0002, 0x0338 },	// O + stroke
	{ 0x00F8, 0x113B, 0x0054, 0x0008, 0x0338 },	// o + stroke
	{ 0x014C, 0x113B, 0x005B, 0x0002, 0x0304 },	// O + macron
	{ 0x014D, 0x113B, 0x005B, 0x0008, 0x0304 },	// o + macron
	{ 0x0050, 0x115D, 0x0020, 0x0002, 0x0050 },	// P
	{ 0x0070, 0x115D, 0x0020, 0x0008, 0x0070 },	// p
	{ 0x1E54, 0x115D, 0x0032, 0x0002, 0x0301 },	// P + acute
	{ 0x1E55, 0x115D, 0x0032, 0x0008, 0x0301 },	// p + acute
	{ 0x1E56, 0x115D, 0x0052, 0x0002, 0x0307 },	// P + dot above
	{ 0x1E57, 0x115D, 0x0052, 0x0008, 0x0307 },	// p + dot above
	{ 0x0051, 0x116E, 0x0020, 0x0002, 0x0051 },	// Q
	{ 0x0071, 0x116E, 0x0020, 0x0008, 0x0071 },	// q
	{ 0x0052, 0x117E, 0x0020, 0x0002, 0x0052 },	// R
	{ 0x0072, 0x117E, 0x0020, 0x0008, 0x0072 },	// r
	{ 0x0154, 0x117E, 0x0032, 0x0002, 0x0301 },	// R + acute
	{ 0x0155, 0x117E, 0x0032, 0x0008, 0x0301 },	// r + acute
	{ 0x0158, 0x117E, 0x0041, 0x0002, 0x030C },	// R + caron
	{ 0x0159, 0x117E, 0x0041, 0x0008, 0x030C },	// r + caron
	{ 0x1E58, 0x117E, 0x0052, 0x0002, 0x0307 },	// R + dot above
	{ 0x1E59, 0x117E, 0x0052, 0x0008, 0x0307 },	// r + dot above
	{ 0x0156, 0x117E, 0x0056, 0x0002, 0x0327 },	// R + cedilla
	{ 0x0157, 0x117E, 0x0056, 0x0008, 0x0327 },	// r + cedilla
	{ 0x1E5C, 0x117E, 0x005B, 0x0002, 0x0304 },	// R + dot below + macron
	{ 0x1E5D, 0x117E, 0x005B, 0x0008, 0x0304 },	// r + dot below + macron
	{ 0x1E5A, 0x117E, 0x007A, 0x0002, 0x0323 },	// R + dot below
	{ 0x1E5B, 0x117E, 0x007A, 0x0008, 0x0323 },	// r + dot below
	{ 0x1E5E, 0x117E, 0x008A, 0x0002, 0x0331 },	// R + macron below
	{ 0x1E5F, 0x117E, 0x008A, 0x0008, 0x0331 },	// r + macron below
	{ 0x0053, 0x11AF, 0x0020, 0x0002, 0x0053 },	// S
	{ 0x0073, 0x11AF, 0x0020, 0x0008, 0x0073 },	// s
	{ 0x017F, 0x11AF, 0x0020, 0x0004, 0x017F },	// old-style small 's', looks like sylized 'f'
	{ 0x00DF, 0x11AF, 0x0020, 0x0004, 0x00DF },	// German 'ss'
	{ 0x015A, 0x11AF, 0x0032, 0x0002, 0x0301 },	// S + acute
	{ 0x015B, 0x11AF, 0x0032, 0x0008, 0x0301 },	// s + acute
	{ 0x1E64, 0x11AF, 0x0052, 0x0002, 0x0307 },	// S + acute + dot above
	{ 0x1E65, 0x11AF, 0x0052, 0x0008, 0x0307 },	// s + acute + dot above
	{ 0x1E66, 0x11AF, 0x0052, 0x0002, 0x0307 },	// S + caron + dot above
	{ 0x1E67, 0x11AF, 0x0052, 0x0008, 0x0307 },	// s + caron + dot above
	{ 0x1E68, 0x11AF, 0x0052, 0x0002, 0x0307 },	// S + dot above + dot below
	{ 0x1E69, 0x11AF, 0x0052, 0x0008, 0x0307 },	// s + dot above + dot below
	{ 0x015C, 0x11AF, 0x003C, 0x0002, 0x0302 },	// S + circumflex
	{ 0x015D, 0x11AF, 0x003C, 0x0008, 0x0302 },	// s + circumflex
	{ 0x0160, 0x11AF, 0x0041, 0x0002, 0x030C },	// S + caron
	{ 0x0161, 0x11AF, 0x0041, 0x0008, 0x030C },	// s + caron
	{ 0x1E60, 0x11AF, 0x0052, 0x0002, 0x0307 },	// S + dot above
	{ 0x1E61, 0x11AF, 0x0052, 0x0008, 0x0307 },	// s + dot above
	{ 0x015E, 0x11AF, 0x0056, 0x0002, 0x0327 },	// S + cedilla
	{ 0x015F, 0x11AF, 0x0056, 0x0008, 0x0327 },	// s + cedilla
	{ 0x1E62, 0x11AF, 0x007A, 0x0002, 0x0323 },	// S + dot below
	{ 0x1E63, 0x11AF, 0x007A, 0x0008, 0x0323 },	// s + dot below
	{ 0x1E9B, 0x11AF, 0x019A, 0x0004, 0x1E9B },	// Gaelic 'f' + dot above
	{ 0x0054, 0x11CF, 0x0020, 0x0002, 0x0054 },	// T
	{ 0x0074, 0x11CF, 0x0020, 0x0008, 0x0074 },	// t
	{ 0x0166, 0x11D4, 0x0020, 0x0002, 0x0166 },	// T + stroke
	{ 0x0167, 0x11D4, 0x0020, 0x0008, 0x0167 },	// t + stroke
	{ 0x0164, 0x11CF, 0x0041, 0x0002, 0x030C },	// T + caron
	{ 0x0165, 0x11CF, 0x0041, 0x0008, 0x030C },	// t + caron
	{ 0x1E97, 0x11CF, 0x0047, 0x0008, 0x0308 },	// t + diaeresis
	{ 0x0162, 0x11CF, 0x0056, 0x0002, 0x0327 },	// T + cedilla
	{ 0x0163, 0x11CF, 0x0056, 0x0008, 0x0327 },	// t + cedilla
	{ 0x1E6A, 0x11CF, 0x0052, 0x0002, 0x0307 },	// T + dot above
	{ 0x1E6B, 0x11CF, 0x0052, 0x0008, 0x0307 },	// t + dot above
	{ 0x1E6C, 0x11CF, 0x007A, 0x0002, 0x0323 },	// T + dot below
	{ 0x1E6D, 0x11CF, 0x007A, 0x0008, 0x0323 },	// t + dot below
	{ 0x1E70, 0x11CF, 0x0086, 0x0002, 0x032D },	// T + circumflex below
	{ 0x1E71, 0x11CF, 0x0086, 0x0008, 0x032D },	// t + circumflex below
	{ 0x1E6E, 0x11CF, 0x008A, 0x0002, 0x0331 },	// T + macron below
	{ 0x1E6F, 0x11CF, 0x008A, 0x0008, 0x0331 },	// t + macron below
	{ 0x0055, 0x11EE, 0x0020, 0x0002, 0x0055 },	// U
	{ 0x0075, 0x11EE, 0x0020, 0x0008, 0x0075 },	// u
	{ 0x00DA, 0x11EE, 0x0032, 0x0002, 0x0301 },	// U + acute
	{ 0x00FA, 0x11EE, 0x0032, 0x0008, 0x0301 },	// u + acute
	{ 0x00D9, 0x11EE, 0x0035, 0x0002, 0x0300 },	// U + grave
	{ 0x00F9, 0x11EE, 0x0035, 0x0008, 0x0300 },	// u + grave
	{ 0x016C, 0x11EE, 0x0037, 0x0002, 0x0306 },	// U + breve
	{ 0x016D, 0x11EE, 0x0037, 0x0008, 0x0306 },	// u + breve
	{ 0x00DB, 0x11EE, 0x003C, 0x0002, 0x0302 },	// U + circumflex
	{ 0x00FB, 0x11EE, 0x003C, 0x0008, 0x0302 },	// u + circumflex
	{ 0x016E, 0x11EE, 0x0043, 0x0002, 0x030A },	// U + ring above
	{ 0x016F, 0x11EE, 0x0043, 0x0008, 0x030A },	// u + ring above
	{ 0x00DC, 0x11EE, 0x0047, 0x0002, 0x0308 },	// U + diaeresis
	{ 0x00FC, 0x11EE, 0x0047, 0x0008, 0x0308 },	// u + diaeresis
	{ 0x0170, 0x11EE, 0x004D, 0x0002, 0x030B },	// U + double acute
	{ 0x0171, 0x11EE, 0x004D, 0x0008, 0x030B },	// u + double acute
	{ 0x0168, 0x11EE, 0x004E, 0x0002, 0x0303 },	// U + tilde
	{ 0x0169, 0x11EE, 0x004E, 0x0008, 0x0303 },	// u + tilde
	{ 0x1E78, 0x11EE, 0x004E, 0x0002, 0x0303 },	// U + tilde + acute
	{ 0x1E79, 0x11EE, 0x004E, 0x0008, 0x0303 },	// u + tilde + acute
	{ 0x0172, 0x11EE, 0x0059, 0x0002, 0x0328 },	// U + ogonek
	{ 0x0173, 0x11EE, 0x0059, 0x0008, 0x0328 },	// u + ogonek
	{ 0x016A, 0x11EE, 0x005B, 0x0002, 0x0304 },	// U + macron
	{ 0x016B, 0x11EE, 0x005B, 0x0008, 0x0304 },	// u + macron
	{ 0x1E7A, 0x11EE, 0x005B, 0x0002, 0x0304 },	// U + macron + diaeresis
	{ 0x1E7B, 0x11EE, 0x005B, 0x0008, 0x0304 },	// u + macron + diaeresis
	{ 0x1E72, 0x11EE, 0x007F, 0x0002, 0x0324 },	// U + diaeresis below
	{ 0x1E73, 0x11EE, 0x007F, 0x0008, 0x0324 },	// u + diaeresis below
	{ 0x1E76, 0x11EE, 0x0086, 0x0002, 0x032D },	// U + circumflex below
	{ 0x1E77, 0x11EE, 0x0086, 0x0008, 0x032D },	// u + circumflex below
	{ 0x1E74, 0x11EE, 0x0089, 0x0002, 0x0330 },	// U + tilde below
	{ 0x1E75, 0x11EE, 0x0089, 0x0008, 0x0330 },	// u + tilde below
	{ 0x0056, 0x1216, 0x0020, 0x0002, 0x0056 },	// V
	{ 0x0076, 0x1216, 0x0020, 0x0008, 0x0076 },	// v
	{ 0x1E7C, 0x1216, 0x004E, 0x0002, 0x0303 },	// V + tilde
	{ 0x1E7D, 0x1216, 0x004E, 0x0008, 0x0303 },	// v + tilde
	{ 0x1E7E, 0x1216, 0x007A, 0x0002, 0x0323 },	// V + dot below
	{ 0x1E7F, 0x1216, 0x007A, 0x0008, 0x0323 },	// v + dot below
	{ 0x0057, 0x1225, 0x0020, 0x0002, 0x0057 },	// W
	{ 0x0077, 0x1225, 0x0020, 0x0008, 0x0077 },	// w
	{ 0x1E82, 0x1225, 0x0032, 0x0002, 0x0301 },	// W + acute
	{ 0x1E83, 0x1225, 0x0032, 0x0008, 0x0301 },	// w + acute
	{ 0x1E80, 0x1225, 0x0035, 0x0002, 0x0300 },	// W + grave
	{ 0x1E81, 0x1225, 0x0035, 0x0008, 0x0300 },	// w + grave
	{ 0x0174, 0x1225, 0x003C, 0x0002, 0x0302 },	// W + circumflex
	{ 0x0175, 0x1225, 0x003C, 0x0008, 0x0302 },	// w + circumflex
	{ 0x1E98, 0x1225, 0x0043, 0x0008, 0x030A },	// w + ring above
	{ 0x1E84, 0x1225, 0x0047, 0x0002, 0x0308 },	// W + diaeresis
	{ 0x1E85, 0x1225, 0x0047, 0x0008, 0x0308 },	// w + diaeresis
	{ 0x1E86, 0x1225, 0x0052, 0x0002, 0x0307 },	// W + dot above
	{ 0x1E87, 0x1225, 0x0052, 0x0008, 0x0307 },	// w + dot above
	{ 0x1E88, 0x1225, 0x007A, 0x0002, 0x0323 },	// W + dot below
	{ 0x1E89, 0x1225, 0x007A, 0x0008, 0x0323 },	// w + dot below
	{ 0x0058, 0x122E, 0x0020, 0x0002, 0x0058 },	// X
	{ 0x0078, 0x122E, 0x0020, 0x0008, 0x0078 },	// x
	{ 0x1E8C, 0x122E, 0x0047, 0x0002, 0x0308 },	// X + diaeresis
	{ 0x1E8D, 0x122E, 0x0047, 0x0008, 0x0308 },	// x + diaeresis
	{ 0x1E8A, 0x122E, 0x0052, 0x0002, 0x0307 },	// X + dot above
	{ 0x1E8B, 0x122E, 0x0052, 0x0008, 0x0307 },	// x + dot above
	{ 0x0059, 0x1233, 0x0020, 0x0002, 0x0059 },	// Y
	{ 0x0079, 0x1233, 0x0020, 0x0008, 0x0079 },	// y
	{ 0x00DD, 0x1233, 0x0032, 0x0002, 0x0301 },	// Y + acute
	{ 0x00FD, 0x1233, 0x0032, 0x0008, 0x0301 },	// y + acute
	{ 0x1EF2, 0x1233, 0x0035, 0x0002, 0x0300 },	// Y + grave
	{ 0x1EF3, 0x1233, 0x0035, 0x0008, 0x0300 },	// y + grave
	{ 0x0176, 0x1233, 0x003C, 0x0002, 0x0302 },	// Y + circumflex
	{ 0x0177, 0x1233, 0x003C, 0x0008, 0x0302 },	// y + circumflex
	{ 0x1E99, 0x1233, 0x0043, 0x0008, 0x030A },	// y + ring above
	{ 0x0178, 0x1233, 0x0047, 0x0002, 0x0308 },	// Y + diaeresis
	{ 0x00FF, 0x1233, 0x0047, 0x0008, 0x0308 },	// y + diaeresis
	{ 0x1EF8, 0x1233, 0x004E, 0x0002, 0x0303 },	// Y + tilde
	{ 0x1EF9, 0x1233, 0x004E, 0x0008, 0x0303 },	// y + tilde
	{ 0x1E8E, 0x1233, 0x0052, 0x0002, 0x0307 },	// Y + dot above
	{ 0x1E8F, 0x1233, 0x0052, 0x0008, 0x0307 },	// y + dot above
	{ 0x1EF6, 0x1233, 0x0060, 0x0002, 0x0309 },	// Y + hook above
	{ 0x1EF7, 0x1233, 0x0060, 0x0008, 0x0309 },	// y + hook above
	{ 0x1EF4, 0x1233, 0x007A, 0x0002, 0x0323 },	// Y + dot below
	{ 0x1EF5, 0x1233, 0x007A, 0x0008, 0x0323 },	// y + dot below
	{ 0x005A, 0x1243, 0x0020, 0x0002, 0x005A },	// Z
	{ 0x007A, 0x1243, 0x0020, 0x0008, 0x007A },	// z
	{ 0x0179, 0x1243, 0x0032, 0x0002, 0x0301 },	// Z + acute
	{ 0x017A, 0x1243, 0x0032, 0x0008, 0x0301 },	// z + acute
	{ 0x1E90, 0x1243, 0x003C, 0x0002, 0x0302 },	// Z + circumflex
	{ 0x1E91, 0x1243, 0x003C, 0x0008, 0x0302 },	// z + circumflex
	{ 0x017D, 0x1243, 0x0041, 0x0002, 0x030C },	// Z + caron
	{ 0x017E, 0x1243, 0x0041, 0x0008, 0x030C },	// z + caron
	{ 0x017B, 0x1243, 0x0052, 0x0002, 0x0307 },	// Z + dot above
	{ 0x017C, 0x1243, 0x0052, 0x0008, 0x0307 },	// z + dot above
	{ 0x1E92, 0x1243, 0x007A, 0x0002, 0x0323 },	// Z + dot below
	{ 0x1E93, 0x1243, 0x007A, 0x0008, 0x0323 },	// z + dot below
	{ 0x1E94, 0x1243, 0x008A, 0x0002, 0x0331 },	// Z + macron below
	{ 0x1E95, 0x1243, 0x008A, 0x0008, 0x0331 },	// z + macron below
	{ 0x00DE, 0x1275, 0x0020, 0x0002, 0x00DE },	// Latin letter 'THORN'
	{ 0x00FE, 0x1275, 0x0020, 0x0008, 0x00FE },	// Latin letter 'thorn'
	{ 0x00B5, 0x12DD, 0x0020, 0x0004, 0x00B5 },	// micro sign
	{ 0xFFFD, 0x0F12, 0x0020, 0x0002, 0xFFFD },	// replacement character
};


/////////////////////////////////////////////////////////////////////////////
//
//	Polish
//
//	"s + acute" is unique letter, between 's' and 't'
//
UtfSortEntry_t g_UtfPatchPolishSort[] =
{
	{ 0x015A, 0x11B0, 0x0032, 0x0002, 0x0301 },	// S + acute
	{ 0x015B, 0x11B0, 0x0032, 0x0008, 0x0301 },	// s + acute
	{ 0x1E64, 0x11B0, 0x0052, 0x0002, 0x0307 },	// S + acute + dot above
	{ 0x1E65, 0x11B0, 0x0052, 0x0008, 0x0307 },	// s + acute + dot above
	{ 0x1E66, 0x11B0, 0x0052, 0x0002, 0x0307 },	// S + caron + dot above
	{ 0x1E67, 0x11B0, 0x0052, 0x0008, 0x0307 },	// s + caron + dot above
};


/////////////////////////////////////////////////////////////////////////////
//
//	Spanish
//
//	"n + tilde" is unique letter, after 'n'
//
UtfSortEntry_t g_UtfPatchSpanishSort[] =
{
	{ 0x00D1, 0x111C, 0x004E, 0x0002, 0x0303 },	// N + tilde
	{ 0x00F1, 0x111C, 0x004E, 0x0008, 0x0303 },	// n + tilde
};


/////////////////////////////////////////////////////////////////////////////
//
//	UtfComposeTableSize()
//
//	Since this array is declared in this file, the other UTF routines can't
//	directly check the size of the array.  This function will report the
//	number of entries in the table so we don't have to manually count them
//	up and expose the size in the header file's extern.
//
U32 UtfComposeTableSize(void)
{
	return ArraySize(g_UtfComposeValues);
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfSymbolTableSize()
//
//	Since this array is declared in this file, the other UTF routines can't
//	directly check the size of the array.  This function will report the
//	number of entries in the table so we don't have to manually count them
//	up and expose the size in the header file's extern.
//
U32 UtfSymbolTableSize(void)
{
	return ArraySize(g_UtfSymbolTable);
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfDefaultSortTableSize()
//
//	Since this array is declared in this file, the other UTF routines can't
//	directly check the size of the array.  This function will report the
//	number of entries in the table so we don't have to manually count them
//	up and expose the size in the header file's extern.
//
U32 UtfDefaultSortTableSize(void)
{
	return ArraySize(g_UtfDefaultSortTable);
}


// Unicode is a 21-bit space, although we can restrict it to 16 bits if we
// ignore CJKV.
#define c_UnicodeSpaceSize			(Unicode_MaxValidCode + 1)

// Page size needs to be power-of-2 to allow for bit-masking.
#define c_SymbolTablePageSize		256
#define c_SymbolTablePageMask		(c_SymbolTablePageSize-1)

// Since only a few sub-ranges of values are used, we're going to create a
// bi-level table.  The first-level LUT is an array of pointers, which will
// be left NULL unless there is at least one symbol in that range.  If there
// is a symbol, the pointer will redirect into the second-level LUT
#define c_SymbolTableIndexSize		(c_UnicodeSpaceSize / c_SymbolTablePageSize)

// This uses a page-table approach.  Each page is an array of pointers into
// the symbol table.  If the symbol does not exist, the pointer is left NULL.
// Only pages that are needed will be allocated, which reduces the total
// memory footprint, making look-ups more efficient (due to locality of
// reference).
struct UtfSymbolPage_t
{
	UtfSymbolTable_t* pElements[c_SymbolTablePageSize];
};

// The page table index is an array of pointers to pages.  Since 99% of
// these are NULL, we can eliminate unneeded pages.  Each page will in turn
// point to the actual symbols within it (or NULL if the symbol does not
// exist).
static UtfSymbolPage_t** g_ppSymbolTableIndex = NULL;
static UtfSymbolPage_t*  g_pSymbolTablePages  = NULL;


/////////////////////////////////////////////////////////////////////////////
//
//	UtfBuildSymbolPageTable()
//
static void UtfBuildSymbolPageTable(void)
{
	// Create an array of pointers that will serve as the index into the
	// page table.
	g_ppSymbolTableIndex = new UtfSymbolPage_t*[c_SymbolTableIndexSize];

	memset(g_ppSymbolTableIndex, 0, c_SymbolTableIndexSize * sizeof(UtfSymbolPage_t*));

	// Now figure out which pages will be needed.  Scan through all of the
	// symbols, marking each index with a non-NULL pointer if it has any
	// symbols within its range.
	UtfSymbolPage_t dummy;
	for (U32 i = 0; i < ArraySize(g_UtfSymbolTable); ++i) {
		U32 symbol = g_UtfSymbolTable[i].Symbol;

		// This test is a bit unnecessary, since the current implementation
		// uses 16-bit values to hold the symbols, but that may change in
		// the future.
		if (symbol < c_UnicodeSpaceSize) {
			g_ppSymbolTableIndex[symbol / c_SymbolTablePageSize] = &dummy;
		}
	}

	// Scan through the index to determine how many pages are actually
	// required to store all symbol references.
	U32 pageCount = 0;
	for (U32 i = 0; i < c_SymbolTableIndexSize; ++i) {
		if (NULL != g_ppSymbolTableIndex[i]) {
			++pageCount;
		}
	}

	// Allocate a page table with exactly the number of needed pages.
	// This saves us from needing to allocate 8 MB (or 16 MB on a 64-bit
	// system) to store all possible pointers, since most of them are not
	// going to be used.
	g_pSymbolTablePages = new UtfSymbolPage_t[pageCount];

	memset(g_pSymbolTablePages, 0, pageCount * sizeof(UtfSymbolPage_t));

	// Scan through the page table index a second time to assign a page of
	// pointers to that index location.
	U32 pageNum = 0;
	for (U32 i = 0; i < c_SymbolTableIndexSize; ++i) {
		if (NULL != g_ppSymbolTableIndex[i]) {
			g_ppSymbolTableIndex[i] = &(g_pSymbolTablePages[pageNum++]);
		}
	}

	QzAssert(pageNum == pageCount);

	// Now scan through the symbol table.  Given the numerical value of a
	// symbol, we can compute the bi-level index-->page-->symbol addressing,
	// and then cache the pointer to the symbol in the correct page location.
	for (U32 i = 0; i < ArraySize(g_UtfSymbolTable); ++i) {
		U32 symbol = g_UtfSymbolTable[i].Symbol;

		// This test is a bit unnecessary, since the current implementation
		// uses 16-bit values to hold the symbols, but that may change in
		// the future.
		if (symbol < c_UnicodeSpaceSize) {
			UtfSymbolPage_t *pPage = g_ppSymbolTableIndex[symbol / c_SymbolTablePageSize];
			QzAssert(NULL != pPage);
			QzAssert(NULL == pPage->pElements[symbol & c_SymbolTablePageMask]);
			pPage->pElements[symbol & c_SymbolTablePageMask] = &(g_UtfSymbolTable[i]);
		}
	}
}


//
// This next chunk of code creates a page-table index for the sorting table,
// which works the same as the page-table for the symbol table.
//

struct UtfSortPage_t
{
	UtfSortEntry_t* pElements[c_SymbolTablePageSize];
};

static UtfSortPage_t** g_ppSortTableIndex = NULL;
static UtfSortPage_t*  g_pSortTablePages  = NULL;



/////////////////////////////////////////////////////////////////////////////
//
//	UtfRefillSortIndex()
//
//	Resets the contents of the sorting table.  This will undo any custom
//	changes that were applied for a specific language's sorting rules.
//
static void UtfRefillSortIndex(void)
{
	for (U32 i = 0; i < ArraySize(g_UtfDefaultSortTable); ++i) {
		U32 symbol = g_UtfDefaultSortTable[i].Symbol;

		if (symbol < c_UnicodeSpaceSize) {
			UtfSortPage_t *pPage = g_ppSortTableIndex[symbol / c_SymbolTablePageSize];
			QzAssert(NULL != pPage);
			pPage->pElements[symbol & c_SymbolTablePageMask] = &(g_UtfDefaultSortTable[i]);
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfPatchSortIndex()
//
//	Replace some of the default entries in the sorting page-table.  This is
//	used to patch modified weights into place for specific languages.  In
//	most cases, the default weights handle almost all symbols, but some
//	adjustment is required to handle certain accented characters correctly.
//
static void UtfPatchSortIndex(UtfSortEntry_t *pPatch, U32 patchCount)
{
	for (U32 i = 0; i < patchCount; ++i) {
		U32 symbol = pPatch[i].Symbol;

		if (symbol < c_UnicodeSpaceSize) {
			UtfSortPage_t *pPage = g_ppSortTableIndex[symbol / c_SymbolTablePageSize];
			QzAssert(NULL != pPage);
			pPage->pElements[symbol & c_SymbolTablePageMask] = &(pPatch[i]);
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfBuildSortPageTable()
//
//	The sorting table is built up using the same page-table logic as the
//	symbol table.  The only difference is that the filling in the pointers
//	in the page entries is handled by UtfRefillSortIndex(), since that will
//	need to be called whenever the current language is changed by a call to
//	UtfPatchSortIndex().
//
static void UtfBuildSortPageTable(void)
{
	// Create an array of pointers that will serve as the index into the
	// page table.
	g_ppSortTableIndex = new UtfSortPage_t*[c_SymbolTableIndexSize];

	memset(g_ppSortTableIndex, 0, c_SymbolTableIndexSize * sizeof(UtfSortPage_t*));

	// Now figure out which pages will be needed.  Scan through all of the
	// symbols, marking each index with a non-NULL pointer if it has any
	// symbols within its range.
	UtfSortPage_t dummy;
	for (U32 i = 0; i < ArraySize(g_UtfDefaultSortTable); ++i) {
		U32 symbol = g_UtfDefaultSortTable[i].Symbol;

		// This test is a bit unnecessary, since the current implementation
		// uses 16-bit values to hold the symbols, but that may change in
		// the future.
		if (symbol < c_UnicodeSpaceSize) {
			g_ppSortTableIndex[symbol / c_SymbolTablePageSize] = &dummy;
		}
	}

	// Scan through the index to determine how many pages are actually
	// required to store all symbol references.
	U32 pageCount = 0;
	for (U32 i = 0; i < c_SymbolTableIndexSize; ++i) {
		if (NULL != g_ppSortTableIndex[i]) {
			++pageCount;
		}
	}

	// Allocate a page table with exactly the number of needed pages.
	// This saves us from needing to allocate 8 MB (or 16 MB on a 64-bit
	// system) to store all possible pointers, since most of them are not
	// going to be used.
	g_pSortTablePages = new UtfSortPage_t[pageCount];

	memset(g_pSortTablePages, 0, pageCount * sizeof(UtfSortPage_t));

	// Scan through the page table index a second time to assign a page of
	// pointers to that index location.
	U32 pageNum = 0;
	for (U32 i = 0; i < c_SymbolTableIndexSize; ++i) {
		if (NULL != g_ppSortTableIndex[i]) {
			g_ppSortTableIndex[i] = &(g_pSortTablePages[pageNum++]);
		}
	}

	QzAssert(pageNum == pageCount);

	// This call will assign the specific sorting pointers into the
	// second-level page table.
	UtfRefillSortIndex();
}



// This must be a power of 2.
#define c_ComposeBinCount 256

// The hashing function will provide an index into g_ComposeBinOffset[].
// Since this is a hash function, the symbol-pair may or may not be found
// at this location.
static U16 g_ComposeBinOffset[c_ComposeBinCount];

// The composition table contains all of the symbols, with one extra zero
// terminator at the end of each sequence.  Therefore the exact amount of
// memory required is a known quantity, so we can statically create a
// buffer that is exactly the size required for this hash table.
static UtfComposeTable_t g_ComposeTable[ArraySize(g_UtfComposeValues) + c_ComposeBinCount];


/////////////////////////////////////////////////////////////////////////////
//
//	ComposeHash()
//
static inline U32 ComposeHash(Utf32_t a, Utf32_t b)
{
	return U32((a * 915488749) + (b * 702395077)) & (c_ComposeBinCount - 1);
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfBuildComposeHashTable()
//
//	Composing characters involves taking two symbols, then doing a look-up
//	operation in the translation table.  If that pair of symbols is in the
//	table, it will be replaced by a single symbol.  Since this look-up needs
//	two values, we cannot use simple index/page logic like with the symbol
//	and sorting tables.  Here we need to construct a hash table that supports
//	look-ups with symbol pairs.
//
static void UtfBuildComposeHashTable(void)
{
	U32 binSize[c_ComposeBinCount];
	U32 binIndex[c_ComposeBinCount];
	for (U32 i = 0; i < c_ComposeBinCount; ++i) {
		binSize[i]  = 0;
		binIndex[i] = 0;
	}

	// Start off by counting up how many elements will be stored within each
	// of the bins.
	for (U32 i = 0; i < ArraySize(g_UtfComposeValues); ++i) {
		U32 index = ComposeHash(g_UtfComposeValues[i].First, g_UtfComposeValues[i].Second);

		binSize[index] += 1;
	}

	// Now that we know how many elements will be stored in each bin, we can
	// compute the base offset for each bin within the table.
	U32 offset = 0;
	for (U32 i = 0; i < c_ComposeBinCount; ++i) {
		g_ComposeBinOffset[i] = U16(offset);

		// The last element within each bin is marked with a sentinel to
		// avoid scanning into the next bin (or off the end of the array).
		g_ComposeTable[offset + binSize[i]].Composed = 0;

		offset += binSize[i] + 1;
	}

	// Now scan through all of the symbols, inserting them into their
	// respective bins.
	for (U32 i = 0; i < ArraySize(g_UtfComposeValues); ++i) {
		U32 index = ComposeHash(g_UtfComposeValues[i].First, g_UtfComposeValues[i].Second);

		// Poke this symbol into the next available slot in the correct bin.
		U32 binNum = g_ComposeBinOffset[index] + binIndex[index];

		g_ComposeTable[binNum] = g_UtfComposeValues[i];

		// Update the position within this bin so we know where to write the
		// next symbol that hashes to this bin.
		binIndex[index] += 1;
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfInitialize()
//
//	This must to be called when the app starts up to guarantee that all of
//	the hash tables and page tables have been constructed.
//
void UtfInitialize(void)
{
	UtfBuildComposeHashTable();
	UtfBuildSymbolPageTable();
	UtfBuildSortPageTable();
	UtfSetLanguage(LangID_English);
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfUninitialize()
//
//	This must be called before the app shuts down to free any memory that
//	was allocated for look-up tables.
//
void UtfUninitialize(void)
{
	SafeDeleteArray(g_ppSymbolTableIndex);
	SafeDeleteArray(g_pSymbolTablePages);
	SafeDeleteArray(g_ppSortTableIndex);
	SafeDeleteArray(g_pSortTablePages);
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfMapASCII()
//
//	Converts an 8-bit ASCII character into its matching Unicode symbol.
//	Most of the characters are the same, except for those in the range
//	0x80 to 0xA0.  This will return the Unicode symbol, or zero to indicate
//	that the symbol is not supported.
//
Utf32_t UtfMapASCII(Utf08_t symbol)
{
	if (symbol < 0x80) {
		return symbol;
	}

	if ((symbol - 0x80) >= ArraySize(g_MapAscii)) {
		return symbol;
	}

	return g_MapAscii[symbol - 0x80];
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfComposeChars()
//
//	Returns the number of characters that were composed:
//		0: if these characters cannot be composed
//		1: if only the first character needs to be replaced
//		2: if both characters should be composed into one
//
Utf32_t UtfComposeChars(Utf32_t a, Utf32_t b, Utf32_t &result)
{
	U32 binNum = g_ComposeBinOffset[ComposeHash(a, b)];

	while ('\0' != g_ComposeTable[binNum].Composed) {
		if (g_ComposeTable[binNum].First == a) {

			if ('\0' == g_ComposeTable[binNum].Second) {
				result = g_ComposeTable[binNum].Composed;
				return 1;
			}

			if (g_ComposeTable[binNum].Second == b) {
				result = g_ComposeTable[binNum].Composed;
				return 2;
			}
		}

		++binNum;
	}

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfDecomposeChar()
//
//	Decomposes the given symbol to its canonical decomposition form (which
//	is usually called the normalized form).
//
//	This could also be optimized with a page-table index, but string
//	decomposition is an unusual operation, so we're not as concerned about
//	performance for this operation.  A binary look-up is adequate until
//	proven otherwise.
//
void UtfDecomposeChar(Utf32_t symbol, Utf32_t &c1, Utf32_t &c2, Utf32_t &c3)
{
	S32 lo = 0;
	S32 hi = ArraySize(g_UtfNormTable) - 1;

	// Do a binary search to locate the symbol in the table.
	while (lo <= hi) {
		S32 mid = (lo + hi) / 2;

		if (symbol < g_UtfNormTable[mid].Symbol) {
			hi = mid - 1;
		}
		else if (symbol > g_UtfNormTable[mid].Symbol) {
			lo = mid + 1;
		}
		else {
			c1 = g_UtfNormTable[mid].C1;
			c2 = g_UtfNormTable[mid].C2;
			c3 = g_UtfNormTable[mid].C3;
			return;
		}
	}

	// If we reach there, the symbol is not in the decomposition table,
	// so there is no decomposition and the symbol remains unchanged.
	c1 = symbol;
	c2 = 0;
	c3 = 0;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfDecomposeValidate()
//
//	The contents of the g_UtfNormTable[] should be sorted to allow for binary
//	searches.  Return false if the table is not sorted.  This function is
//	only intended for use with unit tests, to do a one-time verification that
//	the table is properly constructed.
//
bool UtfDecomposeValidate(void)
{
	for (U32 i = 1; i < ArraySize(g_UtfNormTable); ++i) {
		if (g_UtfNormTable[i].Symbol <= g_UtfNormTable[i-1].Symbol) {
			return false;
		}
	}

	return true;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfGetDecimal()
//
//	Returns symbol used for decimal points for the current language.
//
Utf08_t UtfGetDecimal(void)
{
	return g_UtfDecimal;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfGetThousands()
//
//	Returns the thousands separator symbol for the current language.
//
Utf08_t UtfGetThousands(void)
{
	return g_UtfThousands;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfSetLanguage()
//
void UtfSetLanguage(U32 langID)
{
	// The first thing we need to do is build the default hash table used
	// for lexicographic sorting.
	UtfRefillSortIndex();

	g_UtfLanguage  = langID;
	g_UtfDecimal   = '.';
	g_UtfThousands = ',';

	// Now we need to apply any adjustments to the table entries based on
	// the specific language requested.

	switch (langID) {
		case LangID_French:
			g_UtfDecimal   = ',';
			g_UtfThousands = '.';
			break;

		case LangID_German:
			g_UtfDecimal   = ',';
			g_UtfThousands = '.';
			break;

		case LangID_Irish:
			break;

		case LangID_Italian:
			break;

		case LangID_Polish:
			UtfPatchSortIndex(g_UtfPatchPolishSort, ArraySize(g_UtfPatchPolishSort));
			g_UtfDecimal   = ',';
			g_UtfThousands = Unicode_NBSP;
			break;

		case LangID_Russian:
			g_UtfDecimal   = ',';
			g_UtfThousands = Unicode_NBSP;
			break;

		case LangID_Spanish:
			UtfPatchSortIndex(g_UtfPatchSpanishSort, ArraySize(g_UtfPatchSpanishSort));
			break;
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfGetLanguage()
//
//	Returns the ID of the language in which the sorting tables have been
//	configured.  This is also used in some places for locale-specific logic,
//	such as formatting dates and numbers.
//
U32 UtfGetLanguage(void)
{
	return g_UtfLanguage;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfLookUpSort()
//
//	Given a symbol, retrieve the sorting information for that symbol.  This
//	is only used for lexical comparisons between two strings.
//
UtfSortEntry_t* UtfLookUpSort(Utf32_t symbol)
{
	if (symbol < c_UnicodeSpaceSize) {
		UtfSortPage_t *pPage = g_ppSortTableIndex[symbol / c_SymbolTablePageSize];
		if (NULL != pPage) {
			return pPage->pElements[symbol & c_SymbolTablePageMask];
		}
	}

	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
//
//	UtfLookUpSymbol()
//
UtfSymbolTable_t* UtfLookUpSymbol(Utf32_t symbol)
{
	if (symbol < c_UnicodeSpaceSize) {
		UtfSymbolPage_t *pPage = g_ppSymbolTableIndex[symbol / c_SymbolTablePageSize];
		if (NULL != pPage) {
			return pPage->pElements[symbol & c_SymbolTablePageMask];
		}
	}

	return NULL;
}


