/////////////////////////////////////////////////////////////////////////////
//
//	File: RedBlackBasic.h
//
//	$Header: $
//
//
//	This implementation is based on Julienne Walker's version of red-black
//	trees:
//
//	http://www.eternallyconfuzzled.com/tuts/datastructures/jsw_tut_rbtree.aspx
//
//	A red-black tree conforms to the following rules:
//	  * A red node's parent is black.
//	  * A red node is either a leaf, or it has two black children.
//	  * Every path from the root to a leaf must pass through the same number
//	    of black nodes.
//
//	In its strictest definition, a red-black tree is a 2-3 tree.  The first
//	key of node is black, and the second key is red.  Vertical links between
//	keys are black, and horizontal links are red.
//
//	This carries the implication that if a black node has two children, at
//	most one of those children is red -- the other must be black.  However,
//	most implementations (including this one) are more general, and will
//	allow a black node to have two red children.  All of the other rules of
//	red-black trees are enforced, but when we allow a black node to have two
//	red children, the red-black tree is no longer a one-to-one mapping of a
//	2-3 tree.
//
/////////////////////////////////////////////////////////////////////////////


#pragma once


#include "VoidRef.h"


struct RedBlackBasic_t
{
	VoidRef_t Ref;

	bool IsRed;

	RedBlackBasic_t *pChild[2];
};


class RedBlackBasic
{
private:
	RedBlackBasic_t* m_pRoot;

public:
	RedBlackBasic(void);
	~RedBlackBasic(void);

	bool IsEmpty(void) { return NULL == m_pRoot; }

	void FreeAll(void);
	void Free(RedBlackBasic_t *pNode);

	RedBlackBasic_t* NewNode(void);

	void* LookUp(const U32 value);

	void Insert(VoidRef_t ref);

	void Delete(const U32 value);

	void SanityCheck(void);
	void SanityCheckRec(RedBlackBasic_t *pNode, bool isParentRed, U32 blackDepth, U32 &minBlack, U32 &maxBlack);

	U32  KeyCount(void);
	U32  KeyCountRec(RedBlackBasic_t *pNode);

	void LeafDepth(void);
	U32  LeafDepthRec(RedBlackBasic_t *pNode, U32 depth, U32 ary[], U32 depthLimit, U32 blackDepth, U32 &minBlack, U32 &maxBlack);

	void Traverse(void);
	void TraverseRec(RedBlackBasic_t *pNode, U32 &prev);
};



