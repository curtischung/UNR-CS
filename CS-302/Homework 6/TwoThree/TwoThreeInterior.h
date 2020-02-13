/////////////////////////////////////////////////////////////////////////////
//
//	File: TwoThreeInterior.h
//
//	$Header: $
//
//
//	This implementation is based on Lewis and Denenberg, "Data Structures and
//	Their Algorithms", 1991, pages 229-236.  According to their definition,
//	a 2-3 tree has three properties:
//	  1. All leaves are at the same depth and contain 1 or 2 keys.
//	  2. An interior node (a node that is not a leaf) either:
//	     a) contains one key and has two children (a 2-node) or
//	     b) contains two keys and has three children (a 3-node).
//	  3. A key in an interior node is between (in the dictionary order) the
//	     keys in the subtrees of its adjacent children.  If the node is a
//	     2-node, this is just the binary search tree property; in a 3-node
//	     the two keys split the keys in the subtrees into three ranges,
//       those less than the smaller of key value, those between the two
//	     key values, and those greater than the larger key value.
//
//	This is referred to as "interior" since key values are stored in all nodes
//	of the tree (as opposed to an "exterior" tree, where keys are only stored
//	in leaf nodes.
//
//	If every node is a 2-node, the height of the tree is log2(n).  If every
//	node is a 3-node, the height of the tree is log3(n).  Therefore, the
//	height of any arbitrary interior 2-3 tree is in the range:
//				log3(n) <= height <= log2(n)
//
//	Since the height of an interior 2-3 tree is between log2(n) and log3(n),
//	the maximum number of nodes needed to search is, on average, less than
//	that require to search an exterior 2-3 tree.
//
//	Additionally, since the values being searched may be stored in interior
//	leaves, in the best case a look-up operation may find the desired key
//	before reaching a leaf node.  This makes the best-case and average-case
//	time better than exterior trees.  So from an algorithmic analysis view,
//	interior 2-3 trees are faster than exterior 2-3 trees for sufficiently
//	large values of n.
//
//	Because of the balancing (merge, split, and rotate) logic of 2-3 trees,
//	all leaves of the tree are kept at the same level of the tree.  This
//	happens because balancing moves nodes and data values around without
//	changing the height of the tree.  The only way the height of the tree can
//	change is for the root node to be deleted (due to a merge), or for a new
//	root node to be inserted (due to a split).
//
//	Except when applied to the root of the tree, splitting and merging do
//	not change the depth of the tree, only its width.  This is how 2-3
//	trees remain balanced.
//
//	Each node stores at most three child pointers, along with two key values
//	that allow look-up operations to determine whether they need to traverse
//	the left, middle, or right child.
//
/////////////////////////////////////////////////////////////////////////////


#pragma once


#include "VoidRef.h"


struct TwoThreeInterior_t
{
	TwoThreeInterior_t *pChild1;
	TwoThreeInterior_t *pChild2;
	TwoThreeInterior_t *pChild3;

	VoidRef_t RefLo;
	VoidRef_t RefHi;
};


class TwoThreeInterior
{
private:
	TwoThreeInterior_t* m_pRoot;

public:
	TwoThreeInterior(void);
	~TwoThreeInterior(void);

	bool IsEmpty(void) { return NULL == m_pRoot; }

	void FreeAll(void);
	void Free(TwoThreeInterior_t *pNode);

	TwoThreeInterior_t* NewNode(void);

	void* LookUp(const U32 key);

	void Insert(VoidRef_t ref);
	bool InsertRec(TwoThreeInterior_t *pNode, VoidRef_t &ref, TwoThreeInterior_t **ppN1, TwoThreeInterior_t **ppN2);

	bool InOrderSuccessor(TwoThreeInterior_t *pNode, VoidRef_t &ref);
	bool FixInOrderSuccessor(TwoThreeInterior_t *pNode);
	bool FixFirst(TwoThreeInterior_t *pNode);
	bool FixSecond(TwoThreeInterior_t *pNode);
	void FixThird(TwoThreeInterior_t *pNode);

	void Delete(const U32 key);
	bool DeleteRec(TwoThreeInterior_t *pNode, const U32 key);

	void SanityCheck(void);
	void SanityCheckRec(TwoThreeInterior_t *pNode);

	U32  KeyCount(void);
	U32  KeyCountRec(TwoThreeInterior_t *pNode);

	void LeafDepth(void);
	U32  LeafDepthRec(TwoThreeInterior_t *pNode, U32 depth, U32 ary[], U32 depthLimit);

	void Traverse(void);
	void TraverseRec(TwoThreeInterior_t *pNode, U32 &prev);
};



