/////////////////////////////////////////////////////////////////////////////
//
//	File: QzCommon.cpp
//
//	$Header: /Projects/Qz/QzCommon.cpp  6  2010/4/4 3:56:24p  Lee $
//
/////////////////////////////////////////////////////////////////////////////


#include "QzCommon.h"


/////////////////////////////////////////////////////////////////////////////
//
//	QzAllocString()
//
Utf08_t* QzAllocString(const Utf08_t strg[])
{
	if (NULL == strg) {
		return NULL;
	}

	U32      length = UtfByteCount(strg) + 1;
	Utf08_t* pNew   = new Utf08_t[length];

	memcpy(pNew, strg, length);

	return pNew;
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzAppendSlash()
//
//	Assumes that it is given a string containing the name of a folder.  If
//	the string does not end with a slash, a slash will be appended to the
//	string.
//
U32 QzAppendSlash(Utf08_t strg[], U32 maxLength)
{
	U32 byteCount = UtfByteCount(strg);

	if (0 == byteCount) {
		strg[0] = '/';
		strg[1] = '\0';
		return 1;
	}

	if (('/' != strg[byteCount-1]) &&
		('\\' != strg[byteCount-1]) &&
		((byteCount + 2) < maxLength))
	{
		strg[byteCount  ] = '/';
		strg[byteCount+1] = '\0';
		return (byteCount + 1);
	}

	return byteCount;
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzBackslashToSlash()
//
//	Scans through a string and replaces all of the Microsoftian '\\' with
//	'/' for platform independence.
//
//	Note that with current versions of Windows, the file routines are smart
//	enough to correctly parse filenames that use '/', so all code should be
//	using forward slashes for platform independence.
//
//	Returns the length of the string.
//
U32 QzBackslashToSlash(Utf08_t strg[], U32 maxLength)
{
	if (NULL == strg) {
		return 0;
	}

	for (U32 i = 0; i < maxLength; ++i) {
		if ('\\' == strg[i]) {
			strg[i] = '/';
		}

		if ('\0' == strg[i]) {
			return i;
		}
	}

	// If we fell out of the loop, there is no '\0' at the end of the buffer.
	// Return the length of the buffer and let the caller decide what to do
	// about any potential overflow -- there are cases where strings have an
	// exact length, with no '\0' at the end, so this is not always an error.
	return maxLength;
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzBitArrayGet()
//
//	This maps an array of bits onto an array of 32-bit values.  This allows
//	a boolean array to be packed into the minimum amount of space.
//
//	The caller is responsible for bounds checking the given array.
//
bool QzBitArrayGet(U32 *pArray, U32 bitNum)
{
	return (0 != (pArray[bitNum / 32] & (1 << (bitNum & 0x1F))));
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzBitArraySet()
//
//	This maps an array of bits onto an array of 32-bit values.  This allows
//	a boolean array to be packed into the minimum amount of space.
//
//	The caller is responsible for bounds checking the given array.
//
void QzBitArraySet(U32 *pArray, U32 bitNum, bool enable)
{
	if (enable) {
		pArray[bitNum / 32] |= 1 << (bitNum & 0x1F);
	}
	else {
		pArray[bitNum / 32] &= ~(1 << (bitNum & 0x1F));
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzCatPathNames()
//
//	Concatenates two path names together, guaranteeing that there will be a
//	'/' between the two parts, no extra '/' will be duplicated, and any
//	'\' will be converted to '/'.
//
U32 QzCatPathNames(const Utf08_t a[], const Utf08_t b[], Utf08_t strg[], U32 maxLength)
{
	U32 offset = 0;

	if (NULL != a) {
		U32 posA = 0;
		while (((offset + 1) < maxLength) && ('\0' != a[posA])) {
			if ('\\' == a[posA]) {
				strg[offset++] = '/';
			}
			else {
				strg[offset++] = a[posA];
			}

			++posA;
		}

		// Make certain a slash occurs at the end of the first part.
		if ((offset > 0) && ('/' != strg[offset-1])) {
			strg[offset++] = '/';
		}
	}

	if (NULL != b) {
		U32 posB = 0;

		// Discard any leading slashes.
		while (('/' == b[posB]) || ('\\' == b[posB])) {
			++posB;
		}

		while (((offset + 1) < maxLength) && ('\0' != b[posB])) {
			if ('\\' == b[posB]) {
				strg[offset++] = '/';
			}
			else {
				strg[offset++] = b[posB];
			}

			++posB;
		}
	}

	strg[offset] = '\0';

	return offset;
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzCleanTextBuffer()
//
//	Removes any CR ('\r') symbols from the buffer, and also removes any stray
//	whitespace at the end of each line.
//
U32 QzCleanTextBuffer(U08 *pData, U32 byteCount)
{
	U32 offset = 0;

	for (U32 forward = 0; forward < byteCount; ++forward) {
		if ('\r' != pData[forward]) {

			// When we reach the end of a line, start scanning backwards.
			// Discard all whitespace at the end of each line until we either
			// reach non-whitespace, the end of the previous line, or the
			// start of the buffer.
			if ('\n' == pData[forward]) {
				while ((offset > 0) && ((' ' == pData[offset-1]) || ('\t' == pData[offset-1]))) {
					--offset;
				}
			}

			pData[offset++] = pData[forward];
		}
	}

	return offset;
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzColorAdd()
//
//	Adds two 32-bit color values together, clamping the results.
//
U32 QzColorAdd(U32 color1, U32 color2)
{
	U32 b = ((color1      ) & 0xFF) + ((color2      ) & 0xFF);
	U32 g = ((color1 >>  8) & 0xFF) + ((color2 >>  8) & 0xFF);
	U32 r = ((color1 >> 16) & 0xFF) + ((color2 >> 16) & 0xFF);
	U32 a = ((color1 >> 24) & 0xFF) + ((color2 >> 24) & 0xFF);

	QuickClamp(b, 255);
	QuickClamp(g, 255);
	QuickClamp(r, 255);
	QuickClamp(a, 255);

	return b | (g << 8) | (r << 16) | (a << 24);
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzColorBlend()
//
//	Blends between colorA (blend == 0.0) and colorB (blend == 1.0).
//
U32 QzColorBlend(U32 colorA, U32 colorB, float blend)
{
	S32 b = S32((float((colorB      ) & 0xFF) - float((colorA      ) & 0xFF)) * blend) + S32((colorA      ) & 0xFF);
	S32 g = S32((float((colorB >>  8) & 0xFF) - float((colorA >>  8) & 0xFF)) * blend) + S32((colorA >>  8) & 0xFF);
	S32 r = S32((float((colorB >> 16) & 0xFF) - float((colorA >> 16) & 0xFF)) * blend) + S32((colorA >> 16) & 0xFF);
	S32 a = S32((float((colorB >> 24) & 0xFF) - float((colorA >> 24) & 0xFF)) * blend) + S32((colorA >> 24) & 0xFF);

	QuickClamp(b, 255);
	QuickClamp(g, 255);
	QuickClamp(r, 255);
	QuickClamp(a, 255);

	return b | (g << 8) | (r << 16) | (a << 24);
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzColorFade()
//
//	Takes a 32-bit ARGB color value, fading it towards zero.  Makes certain
//	that the results are clamped to legal ranges.
//
U32 QzColorFade(U32 color, float fade)
{
	U32 b = QzFloatToInt(float((color      ) & 0xFF) * fade);
	U32 g = QzFloatToInt(float((color >>  8) & 0xFF) * fade);
	U32 r = QzFloatToInt(float((color >> 16) & 0xFF) * fade);

	QuickClamp(b, 255);
	QuickClamp(g, 255);
	QuickClamp(r, 255);

	return b | (g << 8) | (r << 16) | (color & 0xFF000000);
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzConditionPathName()
//
//	Conditions the path name to be a proper name, replaced '\' with '/', and
//	makes certain that there is a '/' at the end of the string.
//
void QzConditionPathName(Utf08_t path[], U32 dstLimit)
{
	U32 byteCount = QzBackslashToSlash(path, dstLimit);

	if (0 == byteCount) {
		path[0] = '.';
		path[1] = '/';
		path[2] = '\0';
	}
	else if (('/' != path[byteCount-1]) && ((byteCount + 1) < dstLimit)) {
		path[byteCount  ] = '/';
		path[byteCount+1] = '\0';
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzCurrentDirectory()
//
//	Returns the name of the current working directory.  Guarantees that the
//	string will end with '/'.
//
//	Returns the number of bytes stored in path[], or zero if the path cannot
//	be resolved.
//
U32 QzCurrentDirectory(Utf08_t path[], U32 maxLength)
{
	U32 byteCount = QzAbsolutePathOfFile(path, maxLength, reinterpret_cast<const Utf08_t*>("."));

	if ((byteCount + 2) >= maxLength) {
		return 0;
	}

	// Make certain that the path ends with a slash.  But only if the
	// string is not empty.  An empty string indicates that the code
	// should parse the list of drive names.
	if ((byteCount > 0) && ('/' != path[byteCount-1])) {
		path[byteCount  ] = '/';
		path[byteCount+1] = '\0';
		byteCount += 1;
	}

	return byteCount;
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzExtensionOfFile()
//
//	Returns the extension of the given file name.  If the name contains more
//	than one '.', only the final extension is returned.  This will return a
//	pointer to the first character after the '.', so if the filename is
//	"foo.tga", this will return a pointer to the "tga" extension.
//
//	Returns NULL if the file has no extension, or if the '.' is the last
//	character.
//
Utf08_t* QzExtensionOfFile(const Utf08_t filename[])
{
	if ((NULL == filename) || ('\0' == filename[0])) {
		return NULL;
	}

	Utf08_t *pExtension = NULL;
	Utf08_t *pScan      = const_cast<Utf08_t*>(filename);

	while ('\0' != *pScan) {
		if ('.' == *pScan) {

			// It's only a proper file extension if there is a character
			// after the '.', otherwise we would return a pointer to an
			// empty string, which the caller is not expecting.
			if ('\0' == pScan[1]) {
				pExtension = NULL;
			}
			else {
				pExtension = pScan + 1;
			}
		}

		// Ignore dots that occur within the name of a parent folder.
		else if ('/' == *pScan) {
			pExtension = NULL;
		}

		++pScan;
	}

	// Ignore cases like ".txt".
	if (pExtension == (filename + 1)) {
		return NULL;
	}

	return pExtension;
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzExtensionClip()
//
//	This is a destructive version of QzExtensionOfFile().  If there is an
//	extension in the file name, the '.' will be replaced with '\0' and a
//	pointer returned to the remaining extension, which is located after
//	the '\0'.
//
//	Special case: When given a string like ".txt", it will return NULL since
//	replacing the '.' with '\0' will produce an invalid file name.
//
//	Special case: When given a string like "foo.", it will return NULL since
//	there is nothing located after the '.'.
//
Utf08_t* QzExtensionClip(Utf08_t filename[])
{
	if ((NULL == filename) || ('\0' == filename[0])) {
		return NULL;
	}

	Utf08_t *pLastDot = NULL;
	Utf08_t *pScan    = const_cast<Utf08_t*>(filename);

	while ('\0' != *pScan) {
		if ('.' == *pScan) {

			// It's only a proper file extension if there is a character
			// after the '.', otherwise we would return a pointer to an
			// empty string, which the caller is not expecting.
			if ('\0' == pScan[1]) {
				pLastDot = NULL;
			}
			else {
				pLastDot = pScan;
			}
		}

		// Ignore dots that occur within the name of a parent folder.
		else if ('/' == *pScan) {
			pLastDot = NULL;
		}

		++pScan;
	}

	// Ignore the following cases:
	//   * there is no '.' in the string
	//   * the '.' is the first character in the string
	//   * the '.' is the last character in the string
	if ((NULL == pLastDot) ||
		(pLastDot == filename) ||
		(pLastDot[1] == '\0'))
	{
		return NULL;
	}

	// Replace the '.' with a '\0' so we clip off the file extension.
	pLastDot[0] = '\0';

	return pLastDot + 1;
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzNextPowerOf2()
//
//	Rounds the given value up to the next power of 2 (or does nothing if it
//	is already a power of 2).
//
U32 QzNextPowerOf2(U32 value)
{
	if (value > 0) {
		// Subtract 1 so that any value 2^n becomes (2^n)-1.
		value -= 1;

		// Apply bit-shift hack to replicate the highest non-zero bit into
		// all lower bit positions.  After this is done, we're guaranteed
		// that value == (2^n)-1 for some value of n.
		value |= value >>  1;
		value |= value >>  2;
		value |= value >>  4;
		value |= value >>  8;
		value |= value >> 16;
	}

	// Now add +1 back in to make (2^n).
	return (value + 1);
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzPathFragment()
//
//	Used to fragment a path into individual file/folder names.  This is
//	designed to be called multiple times, each time carving off the next
//	name until it either hits the end of the string or the next '/'.
//
Utf08_t* QzPathFragment(const Utf08_t path[], U32 &pathPos, Utf08_t fragment[], U32 maxLength)
{
	if ((NULL == path) || ('\0' == path[pathPos])) {
		return NULL;
	}

	U32 dstPos = 0;

	// Skip any leading slashes.  It is the caller's responsibility to
	// handle the leading slash (if there is one) that would map to the
	// root directory.
	while (('/' == path[pathPos]) || ('\\' == path[pathPos])) {
		++pathPos;
	}

	// Scan to the end of the string or the next slash.
	while (('\0' != path[pathPos]) && ('/' != path[pathPos]) && ('\\' != path[pathPos])) {
		if ((dstPos + 1) < maxLength) {
			fragment[dstPos++] = path[pathPos];
		}

		++pathPos;
	}

	if (0 == dstPos) {
		return NULL;
	}

	fragment[dstPos] = '\0';

	return fragment;
}


/////////////////////////////////////////////////////////////////////////////
//
//	QzReadFileToBuffer()
//
//	Reads the contents of a file into a new memory buffer, which is returned.
//	All file operations are limited to this function.  Once complete, the
//	entire contents of the file is stored in this buffer, and the caller is
//	responsible for freeing the buffer.
//
U08* QzReadFileToBuffer(const Utf08_t filename[], U32 &byteCount)
{
	FILE *pFile = QzFileOpen(filename, QzFileOpen_Read);
	if (NULL == pFile) {
		return false;
	}

	// Jump through hoops to find out how large the file is.
	fseek(pFile, 0, SEEK_END);
	byteCount = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);

	// Allocate a buffer with eight extra bytes in it.  We need to add a '\0'
	// sentinel to the end in case this is a text file, so it will be safe
	// to compose the string.  Since this may be a UTF-32 file, the sentinel
	// could be a 32-bit value, so we need four extra bytes instead of one.
	// Worse, the sentinel may be not aligned to a 32-bit boundary.  Over
	// allocate the buffer by 8 bytes, then we can zero them all out and
	// (hopefully) guarantee that nothing will try to scan past the end of
	// the file.
	//
	// Note that the UTF functions should limit input by char/byte count,
	// so those should not have to actually look at the sentinel since we're
	// returning the byte count.
	//
	U08 *pBuffer = new U08[byteCount+8];

	if (byteCount != fread(pBuffer, 1, byteCount, pFile)) {
		fclose(pFile);
		SafeDeleteArray(pBuffer);
		return NULL;
	}

	fclose(pFile);

	// Zero out all of those extra bytes.
	pBuffer[byteCount  ] = '\0';
	pBuffer[byteCount+1] = '\0';
	pBuffer[byteCount+2] = '\0';
	pBuffer[byteCount+3] = '\0';
	pBuffer[byteCount+4] = '\0';
	pBuffer[byteCount+5] = '\0';
	pBuffer[byteCount+6] = '\0';
	pBuffer[byteCount+7] = '\0';

	return pBuffer;
}


