/////////////////////////////////////////////////////////////////////////////
//
//	File: QzCommon.h
//
//	$Header: /Projects/Qz/QzCommon.h  36  2010/5/15 7:35:54p  Lee $
//
//
//	This is the base header used in all files.  It defines custom data types
//	determined by the platform for which the code is being compiled.  This
//	header must always be the first header included in each source file.
//
/////////////////////////////////////////////////////////////////////////////


#pragma once


#if defined(_WIN32)

typedef unsigned char      U08;
typedef signed char        S08;
typedef unsigned short     U16;
typedef signed short       S16;
typedef unsigned long      U32;
typedef signed long        S32;
typedef unsigned __int64   U64;
typedef signed __int64     S64;
typedef unsigned char      Utf08_t;
typedef unsigned short     Utf16_t;
typedef unsigned long      Utf32_t;
typedef void*              Handle_t;

#elif defined(__APPLE_CC__)

typedef unsigned char      U08;
typedef signed char        S08;
typedef unsigned short     U16;
typedef signed short       S16;
typedef unsigned long      U32;
typedef signed long        S32;
typedef unsigned long long U64;
typedef signed long long   S64;
typedef unsigned char      Utf08_t;
typedef unsigned short     Utf16_t;
typedef unsigned long      Utf32_t;
typedef void*              Handle_t;

#else

#error Unknown target platform

#endif


// Some floating point hacks are used in the code for simplification or
// optimization.  When the bits of a float are remapped as an integer, the
// 9 bits containing the sign and exponent will appear as either the high
// 9 bits of an integer, or as the low 8 bits plus the ninth bits, which
// requires shuffling the bits around.
//
// Define ALLOW_FLOAT_HACKS to allow floating-point hacks.  These will often
// accelerate simple floating point tests by looking directly at the bits of
// the floats instead of going through the internal function calls or FPU.
// However, for some systems this can cause numerical problems when the
// processor is not using standard IEEE floats.
//
// Most systems (including both little-endian and big-endian) will have the
// bytes containing a floating point number ordered in the same order as
// integers.  This means that the exponent bits of the float are in the same
// memory location as the high bits of an integer, allowing the exponent to
// be accessed by simple bit shifts.  This is indicated by having the
// NORMAL_ENDIAN_FLOATS symbol defined.  Comment out this symbol if the
// floating-point bytes are in the reverse order (or alternately, comment
// out the ALLOW_FLOAT_HACKS symbol entirely, since this version of the code
// has never been tested on a system such as this -- in my experience, they
// are rare ducks indeed).
//
// Some compilers are "smart enough" to detect casting float pointers to
// integer pointers and will "helpfully" store a zero in the integer, which
// breaks these hacks.  So far I've only seen this on embedded compilers.
// This can be worked around using unions to map the bits of a float to an
// int, such as the QzFloatIntUnion_t union defined below.
//
#define ALLOW_FLOAT_HACKS 1
#define NORMAL_ENDIAN_FLOATS 1

// Define this symbol when compiling on a big-endian system.
//#define IS_BIG_ENDIAN 1


/////////////////////////////////////////////////////////////////////////////
//
//	QzFloatIntUnion_t
//
//	Utility union for remapping the bits of a float as an integer so we can
//	perform bit manipulation hacks.  This only needs to be used when an
//	expression like *((U32*)&f) does not work correctly (some compilers or
//	hardware architectures will return a zero from this expression instead
//	of the actual floating point bits.
//
union QzFloatIntUnion_t
{
	float f;
	S32   i;
};


/////////////////////////////////////////////////////////////////////////////
//
//	QzFloatToInt()
//
//	Cast a float to an integer.  This requires rounding up for positive
//	values, and rounding down for negative values.
//
inline S32 QzFloatToInt(float value)
{
//#if defined(ALLOW_FLOAT_HACKS)
#if 0
	// From Game Programming Gems 2, by Yossarian King.
	//
	// This uses IEEE bit-manipulation hacks to add a bias based on sign,
	// writing the bias into place as an integer to avoid any possible FPU
	// involvement for the write.  Then it adds the bias to value with the
	// FPU, then returns the bits in integer format.
	//
	// The trick is that adding the bias value is 2^23, which causes the
	// float's mantissa to be shifted so that the ones bit is in bit zero,
	// where it should be for an integer.  This results in the 32-bit value
	// containing the number in integer format, but it still contains the
	// exponent bits.  Subtracting the bias before returning will remove the
	// exponent, returning only the integer value.
	//
	// This obviously breaks down for floats > 2^23 or < -2^22.
	//
	// For a Core2 CPU, this is 3x faster than letting the FPU do the
	// float-to-int conversion.
	//
	QzFloatIntUnion_t i, bias;
	if (value >= 0.0f) {
		bias.i = (23 + 127) << 23;
		i.f    = value + bias.f;
		return i.i - bias.i;
	}

	bias.i = ((23 + 127) << 23) + (1 << 22);
	i.f    = value + bias.f;
	return i.i - bias.i;
#else
	if (value >= 0.0f) {
		return S32(value + 0.5f);
	}

	return S32(value - 0.5f);
#endif
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzFloatExponent()
//
//	Extract the exponent of a float and return it as an integer.  This is
//	useful for floating-point hacks, since the exponent can be used to test
//	how close the value is to zero.
//
//	For normal-endian architectures, the exponent should be found in bits
//	0x7F800000, so a simple bit shift and mask is required to convert to an
//	integer.
//
//	For reverse-endian, the high 7 bits of the exponent are found in 0x0000007F,
//	while the low bit is found in 0x00000100, so some shuffling around of
//	the bits is required to get the eight exponent bits in the correct
//	ordering.
//
#if defined(ALLOW_FLOAT_HACKS)
inline U32 QzFloatExponent(U32 f)
{
	#if defined(NORMAL_ENDIAN_FLOATS)
		return ((f >> 23) & 0xFF);
	#else
		return (((f << 1) | ((f >> 8) & 1)) & 0xFF);
	#endif
}
#endif


/////////////////////////////////////////////////////////////////////////////
//
//	QzFloatIsZero()
//
//	Test if a float is really close to zero.  This uses knowledge of the
//	IEEE floating point format for the test, and requires knowing where
//	the exponent bits are located, which differs between normal-endian and
//	reverse-endian architectures.
//
//	For a 32-bit float on little-endian architectures, the exponent is
//	stored in bits 30-23, with zero centered at 127.  We need to mask off
//	the sign bit (bit 31) and shift down so only the exponent is stored in
//	the integer.  Zero is then considered to be any value (either positive
//	or negative) with an exponent that is "small enough".
//
//	Since the exponent is centered at 127, picking a threshold of 116 means
//	any value smaller than (1.0 / 2^11) is zero.
//
//	There are three versions of this function: One that is accurate to about
//	10 bits, one to about 16 bis, and one to 20 bits.  An extra bit is added
//	to this so the comparison is accurate to 1/2 LSB (Least Significant Bit).
//
inline bool QzFloatIsZero10(float f)
{
#if defined(ALLOW_FLOAT_HACKS)
	#if defined(NORMAL_ENDIAN_FLOATS)
		return ((((*reinterpret_cast<U32*>(&f)) >> 23) & 0xFF) < 116);
	#elif
		return (QzFloatExponent(*reinterpret_cast<U32*>(&f)) < 116);
	#endif
#else
	return (fabsf(f) < (0.5f / 1024.0f));
#endif
}

inline bool QzFloatIsZero16(float f)
{
#if defined(ALLOW_FLOAT_HACKS)
	#if defined(NORMAL_ENDIAN_FLOATS)
		return ((((*reinterpret_cast<U32*>(&f)) >> 23) & 0xFF) < 110);
	#else
		return (QzFloatExponent(*reinterpret_cast<U32*>(&f)) < 110);
	#endif
#else
	return (fabsf(f) < (0.5f / 65536.0f));
#endif
}

inline bool QzFloatIsZero20(float f)
{
#if defined(ALLOW_FLOAT_HACKS)
	#if defined(NORMAL_ENDIAN_FLOATS)
		return ((((*reinterpret_cast<U32*>(&f)) >> 23) & 0xFF) < 106);
	#else
		return (QzFloatExponent(*reinterpret_cast<U32*>(&f)) < 106);
	#endif
#else
	return (fabsf(f) < (0.5f / 1048576.0f));
#endif
}


inline bool QzEqualFloats10(const float a, const float b)
{
	return QzFloatIsZero10(a - b);
}
inline bool QzEqualFloats16(const float a, const float b)
{
	return QzFloatIsZero16(a - b);
}
inline bool QzEqualFloats20(const float a, const float b)
{
	return QzFloatIsZero20(a - b);
}


// Utility macro for defining Four-CC values.  The bytes are arranged in
// little-endian ordering for all platforms (this way the bytes appear in
// the correct order when viewed as strings or in hex dumps).
#ifdef IS_BIG_ENDIAN
#define QzMakeFourCC(ch0, ch1, ch2, ch3)  ((U32(U08(ch3))) | (U32(U08(ch2)) << 8) | (U32(U08(ch1)) << 16) | (U32(U08(ch0)) << 24))
#else
#define QzMakeFourCC(ch0, ch1, ch2, ch3)  ((U32(U08(ch0))) | (U32(U08(ch1)) << 8) | (U32(U08(ch2)) << 16) | (U32(U08(ch3)) << 24))
#endif


#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#define c_MaxPathLength 260


#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#ifdef _WIN32
#include <malloc.h>
#include <crtdbg.h>
#endif
#include <ctype.h>
#include <string.h>
#include "QzRect.h"
#include "QzSystem.h"
#include "UtfString.h"
#include "UtfFormat.h"


// The _DEBUG symbol is only defined for DevStudio, and only for debug builds.
#ifdef _DEBUG
#define USE_MALLOC_MACRO
#endif

#ifdef USE_MALLOC_MACRO
inline void* _cdecl operator new(size_t size, const char *pFileName, int lineNum)
{
    return ::operator new(size, 1, pFileName, lineNum);
}
inline void __cdecl operator delete(void *p, const char* /*pFileName*/, int /*lineNum*/)
{
	::operator delete(p);
}
#define DEBUG_NEW new(THIS_FILE, __LINE__)
#define MALLOC_DBG(x) _malloc_dbg(x, 1, THIS_FILE, __LINE__);
#define malloc(x) MALLOC_DBG(x)
#endif // USE_MALLOC_MACRO


/////////////////////////////////////////////////////////////////////////////
//
// The following routines are accessors for writing messages to the current
// log file or shared memory buffer, if either option is enabled by the app.
//
void LogMessage(const char message[], UtfFormat &fmt);
void LogMessage(const char message[]);
void LogErrorMessage(const char message[], UtfFormat &fmt);
void LogErrorMessage(const char message[]);


/////////////////////////////////////////////////////////////////////////////
//
// These are custom assertion routines, which are always defined, even in
// release builds.
//
#ifdef QZ_TERSE_ASSERTS

#define QzAssert(x)			{ if (!(x)) { QzAssertHandler(__LINE__, __FILE__); } }
#define QzAssertAlways(x)	QzAssertHandler(x, __LINE__, __FILE__)
void QzAssertHandler(U32 lineNum, char file[]);

#else

#define QzAssert(x)			{ if (!(x)) { QzAssertHandler(#x, __LINE__, __FILE__); } }
#define QzAssertAlways(x)	QzAssertHandler(x, __LINE__, __FILE__)
void QzAssertHandler(const char message[], U32 lineNum, char file[]);

#endif // QZ_TERSE_ASSERTS



#define c_PI	3.14159265358979323846f

inline float RadiansToDegrees(float degrees)	{ return degrees * 57.29577951308232286465f; }
inline float DegreesToRadians(float radians)	{ return radians *  0.01745329251994329547f; }
inline U32   RoundToMultiple(U32 x, U32 y)		{ return ((x + (y - 1)) / y) * y; }

#define SignedRoundToMultiple(x, y)	((x < 0) ? ((x - y + 1) / y) : ((x + y - 1) / y))

//
// Standard masking operation to set/clear a selection of bits in a mask.
//
inline void SetBitMask(U32 &bits, U32 mask, bool enable)
{
	if (enable)	bits |=  mask;
	else		bits &= ~mask;
}


#define Min(x,y)			(((x) < (y)) ? (x) : (y))
#define Max(x,y)			(((x) > (y)) ? (x) : (y))

#define SafeRelease(x)		{ if (NULL != (x)) { (x)->Release(); (x) = NULL; } }
#define SafeDelete(x)		{ if (NULL != (x)) { delete (x);     (x) = NULL; } }
#define SafeDeleteArray(x)	{ if (NULL != (x)) { delete [] (x);  (x) = NULL; } }
#define SafeCloseHandle(x)	{ if (NULL != (x)) { CloseHandle(x); (x) = NULL; } }
#define SafeRemoveWindow(x)	{ if (NULL != (x)) { (x)->RemoveFromParent(); } }
#define SafeStrCopy(d, s)	UtfCopy(d, ArraySize(d), s)
#define SafeStrAppend(d, s)	UtfAppend(d, ArraySize(d), s)
#define SafeZeroVar(x)		memset(&(x), 0, sizeof(x))
#define SafeZeroArray(x)	memset((x), 0, sizeof(x))
#define IsBitSet(x,y)		(0 != ((x) & (y)))
#define ArraySize(x)		(sizeof(x) / (sizeof((x)[0])))
#define ClampRange(lo,v,hi)	(((v) < (lo)) ? (lo) : (((v) > (hi)) ? (hi) : (v)))
#define QuickClamp(x,hi)	{ if (U32(x) > U32(hi)) { if (S32(x) > S32(hi)) x = hi; else x = 0; } }		// clamps number to 0..hi range
#define GrowRange(lo,hi,x)	{ if ((x) < (lo)) { lo = x; } else if ((x) > (hi)) { hi = x; } }

template <class T> void Swap(T &a, T &b) { T temp; temp = a; a = b; b = temp; }



// Some numerical values use fixed-point values with 10-bits of fraction in
// place of floats.  Use this symbolic to represent 1.0.
#define QzFixedPoint_One		1024


// Symbolic value used to denote NULL handles (where a handle is an integer).
#define c_NullIndex				0xFFFFFFFF


#define QzFont_SimpleLo			 0
#define QzFont_SimpleMid		 1
#define QzFont_SimpleHi			 2
#define QzFont_MonospaceLo		 3
#define QzFont_MonospaceMid		 4
#define QzFont_MonospaceHi		 5
#define QzFont_NormalMid		 6
#define QzFont_ItalicMid		 7
#define QzFont_BoldMid			 8
#define QzFont_TitleMid			 9
#define QzFont_Dingbat1			10
#define QzFont_Dingbat2			11
#define QzFont_Dingbat3			12
#define QzFont_Dingbat4			13
#define QzFont_Dingbat5			14
#define QzFont_Dingbat6			15
#define QzFont_ArraySize		16

#define QzMouseFlag_Shift		0x0004
#define QzMouseFlag_Control		0x0008
#define QzMouseWheel_Delta		120			// magic number needed for compatibility with Windows

#define QzKeyMask_Alt			0x00000010
#define QzKeyMask_Control		0x00000020
#define QzKeyMask_CapsLock		0x00000040
#define QzKeyMask_NumLock		0x00000080
#define QzKeyMask_ScrollLock	0x00000100
#define QzKeyMask_Shift			0x00000200

#define QzKeyMask_WasDown		0x00000001	// were any key-down events received during last update
#define QzKeyMask_WasUp			0x00000002	// were any key-up events received during last update
#define QzKeyMask_IsDown		0x00000004	// key is currently depressed

#define QzKey_Garbage			0x000		// all ignored VK_... values map to this spot
#define QzKey_Backspace			0x08		// 0x08 VK_BACK
#define QzKey_Tab				0x09		// 0x09 VK_TAB
#define QzKey_Enter				0x0D		// 0x0D VK_RETURN
#define QzKey_Escape			0x0E		// 0x1B VK_ESCAPE
#define QzKey_Plus				'+'			// 0x6B VK_ADD
#define QzKey_Minus				'-'			// 0x6D VK_SUBTRACT
#define QzKey_Multiply			'*'			// 0x6A VK_MULTIPLY
#define QzKey_Divide			'/'			// 0x6F VK_DIVIDE
#define QzKey_Equal				'='			// 0xBB VK_OEM_PLUS and 0x92 VK_OEM_NEC_EQUAL
#define QzKey_Decimal			'.'			// 0x6E VK_DECIMAL
#define QzKey_Underscore		'_'			// 0xBD VK_OEM_MINUS
#define QzKey_Comma				','			// 0xBC VK_OEM_COMMA
#define QzKey_Period			'.'			// 0xBE VK_OEM_PERIOD
#define QzKey_Slash				'/'			// 0xBF VK_OEM_2
#define QzKey_BackSlash			'\\'		// 0xDC VK_OEM_5
#define QzKey_Tilde				'~'			// 0xC0 VK_OEM_3
#define QzKey_LeftBracket		'['			// 0xDB VK_OEM_4
#define QzKey_RightBracket		']'			// 0xDD VK_OEM_6
#define QzKey_Quote				'"'			// 0xDE VK_OEM_7
#define QzKey_Semicolon			';'			// 0xBA VK_OEM_1
#define QzKey_Space				' '			// 0x20 VK_SPACE
#define QzKey_Apostrophe		'\''
#define QzKey_0					'0'
#define QzKey_1					'1'
#define QzKey_2					'2'
#define QzKey_3					'3'
#define QzKey_4					'4'
#define QzKey_5					'5'
#define QzKey_6					'6'
#define QzKey_7					'7'
#define QzKey_8					'8'
#define QzKey_9					'9'
#define QzKey_A					'A'
#define QzKey_B					'B'
#define QzKey_C					'C'
#define QzKey_D					'D'
#define QzKey_E					'E'
#define QzKey_F					'F'
#define QzKey_G					'G'
#define QzKey_H					'H'
#define QzKey_I					'I'
#define QzKey_J					'J'
#define QzKey_K					'K'
#define QzKey_L					'L'
#define QzKey_M					'M'
#define QzKey_N					'N'
#define QzKey_O					'O'
#define QzKey_P					'P'
#define QzKey_Q					'Q'
#define QzKey_R					'R'
#define QzKey_S					'S'
#define QzKey_T					'T'
#define QzKey_U					'U'
#define QzKey_V					'V'
#define QzKey_W					'W'
#define QzKey_X					'X'
#define QzKey_Y					'Y'
#define QzKey_Z					'Z'

#define QzKey_Alt				0x101		// 0x12 VK_MENU
#define QzKey_CapsLock			0x102		// 0x14 VK_CAPITAL
#define QzKey_Control			0x103		// 0x11 VK_CONTROL
#define QzKey_NumLock			0x104		// 0x90 VK_NUMLOCK
#define QzKey_ScrollLock		0x105		// 0x91 VK_SCROLL
#define QzKey_Shift				0x106		// 0x10 VK_SHIFT
#define QzKey_Pause				0x107		// 0x13 VK_PAUSE
#define QzKey_PrintScreen		0x108		// 0x2C VK_SNAPSHOT
#define QzKey_Insert			0x109		// 0x2D VK_INSERT
#define QzKey_Delete			0x10A		// 0x2E VK_DELETE
#define QzKey_PageUp			0x10B		// 0x21 VK_PRIOR
#define QzKey_PageDown			0x10C		// 0x22 VK_NEXT
#define QzKey_End				0x10D		// 0x23 VK_END
#define QzKey_Home				0x10E		// 0x24 VK_HOME
#define QzKey_NumpadCenter		0x10F		// 0x0C VK_CLEAR == keypad 5 if NumLock is off
#define QzKey_Left				0x110		// 0x25 VK_LEFT
#define QzKey_Up				0x111		// 0x26 VK_UP
#define QzKey_Right				0x112		// 0x27 VK_RIGHT
#define QzKey_Down				0x113		// 0x28 VK_DOWN
#define QzKey_F1				0x114		// 0x70 VK_F1
#define QzKey_F2				0x115		// 0x71 VK_F2
#define QzKey_F3				0x116		// 0x72 VK_F3
#define QzKey_F4				0x117		// 0x73 VK_F4
#define QzKey_F5				0x118		// 0x74 VK_F5
#define QzKey_F6				0x119		// 0x75 VK_F6
#define QzKey_F7				0x11A		// 0x76 VK_F7
#define QzKey_F8				0x11B		// 0x77 VK_F8
#define QzKey_F9				0x11C		// 0x78 VK_F9
#define QzKey_F10				0x11D		// 0x79 VK_F10
#define QzKey_F11				0x11E		// 0x7A VK_F11
#define QzKey_F12				0x11F		// 0x7B VK_F12
#define QzKey_ArraySize			0x120


enum QzMouseButton_t
{
	QzMouseButton_Left,
	QzMouseButton_Middle,
	QzMouseButton_Right,
};

enum QzMouseClick_t
{
	QzMouseClick_Down,
	QzMouseClick_Double,
	QzMouseClick_Up,
	QzMouseClick_Cancel				// this indicates mouse was lost while button was down
};


inline bool IsMouseDownClick(QzMouseClick_t click)
{
	return ((QzMouseClick_Down == click) || (QzMouseClick_Double == click));
}


enum QzTextureFormat_t
{
	QzTexFormat_None		=  0,	// empty texture, results from creating a new texture resource
	QzTexFormat_Alpha		=  1,	// 8-bit alpha
	QzTexFormat_Greyscale	=  2,	// 8-bit luma
	QzTexFormat_RGB16		=  3,
	QzTexFormat_RGB24		=  4,
	QzTexFormat_ARGB32		=  5,
	QzTexFormat_DXT1		=  6,
	QzTexFormat_DXT1alpha	=  7,	// same as DXT1, but explicitly indicates alpha needs to be saved
	QzTexFormat_DXT2		=  8,
	QzTexFormat_DXT3		=  9,
	QzTexFormat_DXT4		= 10,
	QzTexFormat_DXT5		= 11,
	QzTexFormat_ArraySize
};


/////////////////////////////////////////////////////////////////////////////
//
//	Axis-aligned bounding box.
//
struct QzAABB_t
{
	float MinX;
	float MaxX;
	float MinY;
	float MaxY;
	float MinZ;
	float MaxZ;
};


/////////////////////////////////////////////////////////////////////////////
//
struct QzTex_t
{
	float U;
	float V;
};


/////////////////////////////////////////////////////////////////////////////
//
//	This is an integer version of a vector, often used to store fixed-point
//	values in 1.21.10 format.
//
struct QzIntVector_t
{
	S32 X;
	S32 Y;
	S32 Z;
};


/////////////////////////////////////////////////////////////////////////////
//
struct QzVector_t
{
	float X;
	float Y;
	float Z;
};


/////////////////////////////////////////////////////////////////////////////
//
struct QzVectorQuad_t
{
	QzVector_t TL;
	QzVector_t TR;
	QzVector_t BR;
	QzVector_t BL;
};


/////////////////////////////////////////////////////////////////////////////
//
struct QzPoint_t
{
	S32 x;
	S32 y;
};


/////////////////////////////////////////////////////////////////////////////
//
struct QzMatrix3x3_t
{
	union {
		float c3x3[3][3];
		float cFlat[9];
	};
};


/////////////////////////////////////////////////////////////////////////////
//
struct QzMatrix4x4_t
{
	union {
		float c4x4[4][4];
		float cFlat[16];
	};
};


/////////////////////////////////////////////////////////////////////////////
//
//	An array of these values is used when rendering formatted text, with a
//	mixture of different fonts.
//
struct QzMetaChar_t
{
	Utf32_t Symbol;
	U32     Color;
	S32     PosX;
	S32     PosY;
	U08     FontID;
	U08     Underline;
	U08     Reserved1;
	U08     Reserved2;
};


Utf08_t* QzAllocString(const Utf08_t strg[]);
U32      QzAppendSlash(Utf08_t strg[], U32 maxLength);
U32      QzBackslashToSlash(Utf08_t strg[], U32 maxLength);
bool     QzBitArrayGet(U32 *pArray, U32 bitNum);
void     QzBitArraySet(U32 *pArray, U32 bitNum, bool enable);
U32      QzCatPathNames(const Utf08_t a[], const Utf08_t b[], Utf08_t strg[], U32 maxLength);
U32      QzCleanTextBuffer(U08 *pData, U32 byteCount);
U32      QzColorAdd(U32 color1, U32 color2);
U32      QzColorBlend(U32 colorA, U32 colorB, float blend);
U32      QzColorFade(U32 color, float fade);
void     QzConditionPathName(Utf08_t path[], U32 dstLimit);
U32      QzCurrentDirectory(Utf08_t path[], U32 maxLength);
Utf08_t* QzExtensionOfFile(const Utf08_t filename[]);
Utf08_t* QzExtensionClip(Utf08_t filename[]);
U32      QzNextPowerOf2(U32 value);
Utf08_t* QzPathFragment(const Utf08_t path[], U32 &pathPos, Utf08_t fragment[], U32 maxLength);
U08*     QzReadFileToBuffer(const Utf08_t filename[], U32 &byteCount);



