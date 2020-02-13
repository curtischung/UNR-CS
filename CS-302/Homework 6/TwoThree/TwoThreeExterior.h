/////////////////////////////////////////////////////////////////////////////
//
//	File: TwoThreeExterior.h
//
//	$Header: $
//
//
//	This implementation of 2-3 trees is based on the description presented in
//	Aho, Hopcroft, and Ullman, "Data Structures and Algorithms", in the 1987
//	reprint, pages 169-180.
//
//	This is referred to as an "external" representation since all data values
//	are stored at the leaves of the tree, where the internal nodes only hold
//	references to the smallest key under each child to facilitate searches
//	and insertions.
//
//	This is in contrast to an "internal" representation where data values may
//	also be stored in internal nodes.
//
//	Since all data values are stored in the leaves, look-ups and insertions
//	always take exactly O(log n) time (unlike internal trees, where these
//	operations may take less than O(log n) time).
//
//	Because of the balancing (merge, split, and rotate) logic of 2-3 trees,
//	all leaves of the tree are kept at the same level of the tree.  This
//	happens because balancing moves nodes and data values around without
//	changing the depth of the tree.  The only way the depth of the tree can
//	change is for the root node to be deleted (due to a merge), or for a new
//	root node to be inserted (due to a split).
//
//	Splitting and merging do not change the depth of the tree, only its
//	width.  The depth only changes when the root is split, causing a new
//	root be be added (increasing the depth by one level), or when merging
//	causes the root to end up with only one child, forcing the root node to
//	be deleted (decreasing the depth by one level).
//
//	According to Aho et al, a 2-3 tree has two properties:
//	  1: Each interior node has two or three children.
//	  2: Each path from the root to a leaf has the same length.
//	The only exception is a tree with zero or one node, which are handled as
//	special cases.
//
//	To enable searching through the tree, each node stores a "low" key for
//	each of its children.  This allows traversal logic to decide whether to
//	descend into the first, second, or (if present) third child of the node.
//	When balancing the tree, these "low" keys need to be updated to avoid
//	inconsistencies, and the updates must be propagated up the tree.
//
//	One difference from the Aho version of 2-3 trees is that Aho only caches
//	the low key for the second and third children of each node.  While this
//	is adequate for searches, it causes problems when balancing the tree.
//	Since child pointers may be rotated up, down, or sideways in the tree,
//	caching the low key for the first child makes is much easier to keep
//	all of the keys in sync when balancing an insertion or deletion from
//	the tree.
//
/////////////////////////////////////////////////////////////////////////////


#pragma once


#include "VoidRef.h"


struct TwoThreeExterior_t
{
	TwoThreeExterior_t *pChild1;
	TwoThreeExterior_t *pChild2;
	TwoThreeExterior_t *pChild3;

	VoidRef_t Ref;

	U32 Low1;
	U32 Low2;
	U32 Low3;

	bool IsLeaf;
};


class TwoThreeExterior
{
private:
	TwoThreeExterior_t* m_pRoot;

public:
	TwoThreeExterior(void);
	~TwoThreeExterior(void);

	bool IsEmpty(void) { return NULL == m_pRoot; }

	void FreeAll(void);
	void Free(TwoThreeExterior_t *pNode);

	TwoThreeExterior_t* NewNode(void);

	U32  ChildCount(TwoThreeExterior_t *pNode);
	void FixLows(TwoThreeExterior_t *pNode);

	void ShiftChildrenDown(TwoThreeExterior_t *pNode);
//	void ShiftChildrenUp(TwoThreeExterior_t *pNode);

	void* LookUp(const U32 key);

	void Insert(VoidRef_t ref);
	TwoThreeExterior_t* InsertRec(TwoThreeExterior_t *pNode, VoidRef_t ref);

	void Delete(const U32 key);
	bool DeleteRec(TwoThreeExterior_t *pNode, const U32 key);

	void SanityCheck(void);
	void SanityCheckRec(TwoThreeExterior_t *pNode);

	U32  KeyCount(void);
	U32  KeyCountRec(TwoThreeExterior_t *pNode);

	void LeafDepth(void);
	U32  LeafDepthRec(TwoThreeExterior_t *pNode, U32 depth, U32 ary[], U32 depthLimit);

	void Traverse(void);
	void TraverseRec(TwoThreeExterior_t *pNode, U32 &prev);
};



