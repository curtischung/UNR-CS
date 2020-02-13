/////////////////////////////////////////////////////////////////////////////
//
//	File: LeftLeaningRedBlack.h
//
//	$Header: $
//
//
//	This version of red-black trees is based on Robert Sedgewick's new
//	algorithm, "Left Leaning Red Black Trees", based on papers published in
//	2008 and 2009.
//
//	A Left Leaning Red-Black Tree ("LLRB" for short) is a variation of the
//	standard red-black tree.  It requires that if a node has one red child,
//	that child must be the left child (hence the tree "leans to the left").
//
//	It also allows a black node to have two red children, effectively making
//	the node a 4-node.
//
//	Depending on how values are inserted into the tree, the tree may emulate
//	either a 2-3 tree or a 2-3-4 tree.  The USE_234_TREE symbol controls
//	which rule is applied.  Empirically, 2-3 trees are slightly faster for
//	insertions and look-ups, since fewer special cases are triggered due to
//	how the nodes in the tree are arranged.  Test it if it matters, but in
//	general, leave the USE_234_TREE symbol undefined for better overall
//	performance.
//
//	By enforcing the left-leaning rule, fewer special cases need to be tested
//	when performing insertions and deletions, meaning that less code needs to
//	be written to implement an LLRB.
//
//	In theory, this also makes the code faster.  However, in practice the
//	LLRB is slower than a normal red-black tree.  Due to the need to touch
//	and rearrange more child nodes to test and enforce the left-leaning
//	rule, the LLRB has to access more memory, which makes the code perform
//	slower on modern computers -- main memory is much slower than the CPU,
//	so all of those extra memory accesses cause more pipeline stalls, which
//	inhibits performance.  When running on a system where main memory runs
//	at the same speed as the CPU, LLRBs do demonstrate better performance.
//
//	However, due to limits on specific embedded processors used for this
//	testing, any performance generalization is not reliable.  If you need to
//	use balanced trees on an embedded processor (or any other processor that
//	has the CPU and memory running at similar clock speeds), the different
//	algorithms should be tested on that hardware to determine which one
//	yields the best performance.
//
/////////////////////////////////////////////////////////////////////////////


#pragma once


#include "VoidRef.h"
#include <iostream>
#include <cstddef>
#include <stdio.h>
#include <string.h>

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

struct LLTB_t
{
	VoidRef_t Ref;

	bool IsRed;

	LLTB_t *pLeft;
	LLTB_t *pRight;
};


class LeftLeaningRedBlack
{
private:
	LLTB_t* m_pRoot;

public:
	LeftLeaningRedBlack(void);
	~LeftLeaningRedBlack(void);

	bool IsEmpty(void) { return NULL == m_pRoot; }

	void FreeAll(void);
	void Free(LLTB_t *pNode);

	LLTB_t* NewNode(void);

	void* LookUp(const int value);

	bool Insert(VoidRef_t ref);
	LLTB_t* InsertRec(LLTB_t *pNode, VoidRef_t ref);

	void Delete(const int value);
	LLTB_t* DeleteRec(LLTB_t *pNode, const int value);
	LLTB_t* DeleteMin(LLTB_t *pNode);

	void SanityCheck(void);
	void SanityCheckRec(LLTB_t *pNode, bool isParentRed, int blackDepth, int &minBlack, int &maxBlack);

	int  KeyCount(void);
	int  KeyCountRec(LLTB_t *pNode);

	void LeafDepth(void);
	int  LeafDepthRec(LLTB_t *pNode, int depth, int ary[], int depthLimit, int blackDepth, int &minBlack, int &maxBlack);

	void Traverse(void);
	void TraverseRec(LLTB_t *pNode, int &prev);

	bool printParentRec(LLTB_t *pNode, int key);
	bool printParent(int key);
};



