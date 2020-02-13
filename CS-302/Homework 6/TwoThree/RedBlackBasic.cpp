/////////////////////////////////////////////////////////////////////////////
//
//	File: RedBlackBasic.cpp
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


#include "QzCommon.h"
#include "RedBlackBasic.h"


#ifdef USE_MALLOC_MACRO
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
//
//	constructor
//
RedBlackBasic::RedBlackBasic(void)
	:	m_pRoot(NULL)
{
}


/////////////////////////////////////////////////////////////////////////////
//
//	destructor
//
RedBlackBasic::~RedBlackBasic(void)
{
	Free(m_pRoot);
}


/////////////////////////////////////////////////////////////////////////////
//
//	FreeAll()
//
void RedBlackBasic::FreeAll(void)
{
	Free(m_pRoot);

	m_pRoot = NULL;
}


/////////////////////////////////////////////////////////////////////////////
//
//	Free()
//
void RedBlackBasic::Free(RedBlackBasic_t *pNode)
{
	if (NULL != pNode) {
		if (NULL != pNode->pChild[0]) {
			Free(pNode->pChild[0]);
		}
		if (NULL != pNode->pChild[1]) {
			Free(pNode->pChild[1]);
		}

		SafeDelete(pNode);
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	NewNode()
//
RedBlackBasic_t* RedBlackBasic::NewNode(void)
{
	RedBlackBasic_t *pNew = new RedBlackBasic_t;

	pNew->Ref.Key      = 0;
	pNew->Ref.pContext = NULL;
	pNew->IsRed        = true;
	pNew->pChild[0]    = NULL;
	pNew->pChild[1]    = NULL;

	return pNew;
}


/////////////////////////////////////////////////////////////////////////////
//
//	LookUp()
//
//	Searches the binary tree for the given key, returning the associated
//	context pointer.
//
//	Returns NULL if the key is not in the tree.
//
void* RedBlackBasic::LookUp(U32 key)
{
	RedBlackBasic_t *pNode = m_pRoot;

	while (NULL != pNode) {
		// Is this the key for which we're looking?
		if (key == pNode->Ref.Key) {
			return pNode->Ref.pContext;
		}

		// If not, we need to traverse left or right down the binary tree.
		else if (key < pNode->Ref.Key) {
			pNode = pNode->pChild[0];
		}
		else {
			pNode = pNode->pChild[1];
		}
	}

	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
//
//	SingleRotation()
//
//	Applies a single rotation to the specified node.
//
//	The <dir> parameter controls whether this is a clockwise or counter
//	clockwise rotation.
//
//	If dir == 0, nodes N (pNode) and S (pSave) will be rotated like this:
//
//		N              S
//	 1     S   ==>  N     3
//	      2 3      1 2
//
//	If dir == 1, nodes N (pNode) and S (pSave) will be rotated like this:
//
//	     N             S
//	  S    3   ==>  1     N
//	 1 2                 2 3
//
//	Note that these two rotates are the inverse of each other.
//
static RedBlackBasic_t* SingleRotation(RedBlackBasic_t *pNode, int dir)
{
	RedBlackBasic_t *pSave = pNode->pChild[!dir];

	pNode->pChild[!dir] = pSave->pChild[dir];
	pSave->pChild[dir]  = pNode;
 
	pNode->IsRed = true;
	pSave->IsRed = false;

	return pSave;
}


/////////////////////////////////////////////////////////////////////////////
//
//	DoubleRotation()
//
static RedBlackBasic_t* DoubleRotation(RedBlackBasic_t *pNode, int dir)
{
	pNode->pChild[!dir] = SingleRotation(pNode->pChild[!dir], !dir);

	return SingleRotation(pNode, dir);
}


/////////////////////////////////////////////////////////////////////////////
//
//	IsRed()
//
//	Use a function to test if a node is black, since some pointers may be
//	NULL.
//
static inline bool IsRed(RedBlackBasic_t *pNode)
{
	return ((NULL != pNode) && pNode->IsRed);
}


/////////////////////////////////////////////////////////////////////////////
//
//	Insert()
//
//	This is an iterative top-down insertion.
//
void RedBlackBasic::Insert(VoidRef_t ref)
{
	// The standard case is inserting into a non-empty tree.
	if (NULL != m_pRoot) {
		// Use a dummy placeholder to initialize the traversal pointers.
		// This will simplify looping logic, since it makes certain that
		// the pointers are never NULL even when looking at the root of
		// the tree.  This eliminates the need for special-case handling
		// of NULL pointers in the inner loop below.
		RedBlackBasic_t head = {0};

		int prevDir = 0;
		int currDir = 0;

		head.pChild[1] = m_pRoot;

		// Use a couple pointers to traverse through the binary tree.
		// Also record the previous value for each of these pointers,
		// which are needed to update child pointers in the parent nodes.
		RedBlackBasic_t *pParentPrev = &head;
		RedBlackBasic_t *pParentCurr = NULL;
		RedBlackBasic_t *pEnumPrev   = NULL;
		RedBlackBasic_t *pEnumCurr   = m_pRoot;

		// Use an infinite loop for traversing the tree.  There is an ending
		// condition, but that condition is most easily tested in the middle
		// of the loop.
		for ( ; ; ) {
			// Once we hit a leaf node, insert the value below the last node
			// that was examined (pEnumPrev is the parent of pEnumCurr).
			if (NULL == pEnumCurr) {
				pEnumCurr                  = NewNode();
				pEnumPrev->pChild[currDir] = pEnumCurr;
				pEnumCurr->Ref             = ref;

				// NOTE: Even though we have inserting the new value into
				// the tree, we do not break out of the loop.  We still need
				// to execute some more code to fix any red violations that
				// were introduced by the insertion.
			}

			// Only one child of a black node can be red.  If we encounter a
			// node where both children are red, we have placed the tree in
			// an invalid state.  Apply a color flip to force the current
			// node to be red and make both of its children black.
			else if (IsRed(pEnumCurr->pChild[0]) && IsRed(pEnumCurr->pChild[1])) {
				pEnumCurr->IsRed            = true;
				pEnumCurr->pChild[0]->IsRed = false;
				pEnumCurr->pChild[1]->IsRed = false;
			}

			// A red violation occurs if the both the current node and its
			// parent are red.  This indicates that the tree is not properly
			// balanced.  Apply a rotation to rebalance this part of the
			// tree, which will also fix the red violation.
			if (IsRed(pEnumCurr) && IsRed(pEnumPrev)) {
				int dir2 = (pParentPrev->pChild[1] == pParentCurr);

				if (pEnumCurr == pEnumPrev->pChild[prevDir]) {
					pParentPrev->pChild[dir2] = SingleRotation(pParentCurr, !prevDir);
				}
				else {
					pParentPrev->pChild[dir2] = DoubleRotation(pParentCurr, !prevDir);
				}
			}

			// If we see the key we're trying to write, we can break out
			// of the loop.  In most cases the key was just inserted, and
			// this code is being applied after fixing any red violations.
			// However, it is possible that the key was already stored in
			// the tree, so we need to write the key/value pair again to
			// handle the case where an existing key is being updated with
			// a new value.
			if (pEnumCurr->Ref.Key == ref.Key) {
				pEnumCurr->Ref = ref;
				break;
			}

			// Keep track of whether we traversed left or right on the
			// previous iteration, so we know which child pointer to update.
			prevDir = currDir;
			currDir = pEnumCurr->Ref.Key < ref.Key;

			// Keep track of the previous parent pointer so we can update
			// its child pointers on subsequent iterations.
			if (NULL != pParentCurr) {
				pParentPrev = pParentCurr;
			}

			pParentCurr = pEnumPrev;
			pEnumPrev   = pEnumCurr;
			pEnumCurr   = pEnumCurr->pChild[currDir];
		}

		// If rotations propagated all the way to the root, the updated
		// root pointer will now be stored in the dummy structure.
		m_pRoot = head.pChild[1];
	}

	// Special case for inserting into an empty tree.
	else {
		m_pRoot = NewNode();
		m_pRoot->Ref = ref;
	}

	// Rotations may have resulted in a red node becoming the root of the
	// tree.  Force the root node to always be black to conform with the
	// rules of a red-black tree.
	m_pRoot->IsRed = false;
}


/////////////////////////////////////////////////////////////////////////////
//
//	Delete()
//
void RedBlackBasic::Delete(const U32 key)
{
	if (NULL == m_pRoot) {
		return;
	}

	// Use a dummy placeholder to initialize the traversal pointers.
	// This will simplify looping logic, since it makes certain that
	// the pointers are never NULL even when looking at the root of
	// the tree.  This eliminates the need for special-case handling
	// of NULL pointers in the inner loop below.
	RedBlackBasic_t head = {0};

	RedBlackBasic_t *pPrev  = NULL;
	RedBlackBasic_t *pCurr  = NULL;
	RedBlackBasic_t *pScan  = &head;
	RedBlackBasic_t *pFound = NULL;

	head.pChild[1] = m_pRoot;

	int prevDir = 1;
	int currDir = 1;

	// Keep scanning until we hit a leaf.  Once we find a leaf, the
	// key stored in that leaf can be used to replace the key to be
	// deleted (assuming that the target key is found in the tree).
	while (NULL != pScan->pChild[currDir]) {
		pPrev   = pCurr;
		pCurr   = pScan;
		pScan   = pScan->pChild[currDir];
		prevDir = currDir;
		currDir = (pScan->Ref.Key < key);

		// If we found the key to be deleted, record which node contains
		// it.  This value will need to be replaced with another key (the
		// in-order successor of the key to delete), but we won't know
		// what value to use until we have exited this loop.
		if (pScan->Ref.Key == key) {
			pFound = pScan;
		}

		// Traverse down black links in the tree.
		//
		// Red nodes are not ignored.  Instead, they are processed
		// from their parent node (which is always a black node).
		//
		if ((false == IsRed(pScan)) &&
			(false == IsRed(pScan->pChild[currDir])))
		{
			// Once we get into this code block, we know that pScan is a
			// black node.  Since currDir indicates which child will be
			// processed next, we need to check the other child (!currDir).
			//
			// If the opposite child is red, then the deletion may cause
			// the target child to be turned into a red node.  Since a
			// black node may not have two red children, we need to apply
			// a rotation to the opposite child to fix this.
			//
			if (IsRed(pScan->pChild[!currDir])) {
				// A rotation is needed to avoid red violations.  Since
				// the rotation changes the local arrangement of the tree,
				// we also need to update pCurr to keep proper track of
				// how we reached this node.

				pCurr->pChild[prevDir] = SingleRotation(pScan, currDir);
				pCurr                  = pCurr->pChild[prevDir];
			}

			// Otherwise the target node is black, so we need to examine
			// its children, fix coloring to avoid red violations, and
			// possibly rotate part of the tree to rebalance the tree to
			// prepare for removing a node.
			else {
				RedBlackBasic_t *pTemp = pCurr->pChild[!prevDir];

				if (NULL != pTemp) {

					// If we end up with this particular arrangement of all
					// black nodes, we can safely convert a couple of them
					// to be red nodes without introducing a red violation.
					if ((false == IsRed(pTemp->pChild[0])) &&
						(false == IsRed(pTemp->pChild[1])))
					{
						pCurr->IsRed = false;
						pTemp->IsRed = true;
						pScan->IsRed = true;
					}

					// Otherwise one of the children is red.  If a red
					// violation is detected, we have to fix the violation
					// by applying a rotation to the current node.
					else {
						int tempDir = (pPrev->pChild[1] == pCurr);

						if (IsRed(pTemp->pChild[prevDir])) {
							pPrev->pChild[tempDir] = DoubleRotation(pCurr, prevDir);
						}
						else if (IsRed(pTemp->pChild[!prevDir])) {
							pPrev->pChild[tempDir] = SingleRotation(pCurr, prevDir);
						}

						// Mark all of these nodes to have a safe
						// arrangement of red and black that cannot
						// possibly have red violations.
						pScan->IsRed                             = true;
						pPrev->pChild[tempDir]->IsRed            = true;
						pPrev->pChild[tempDir]->pChild[0]->IsRed = false;
						pPrev->pChild[tempDir]->pChild[1]->IsRed = false;
					}
				}
			}
		}
	}

	// Now that we have finished scanning the tree, we know
	// whether or not the search key is present in the tree.
	//
	// If the key is in the tree, we can remove the key by replacing it
	// with the value in pScan.  Since pScan is guaranteed to be a leaf
	// node, we then delete that node, since it is no longer needed in
	// the tree.
	if (NULL != pFound) {
		pFound->Ref = pScan->Ref;
		pCurr->pChild[pCurr->pChild[1] == pScan] = pScan->pChild[pScan->pChild[0] == NULL];
		Free(pScan);
	}

	// Rotations may have propagated back to the root of the tree.
	// If so, the new root pointer is cached in the dummy structure.
	m_pRoot = head.pChild[1];

	// The root of a red-black tree must always be black.  It is safe to
	// mark the root as always being black to satisfy this requirement.
	if (NULL != m_pRoot) {
		m_pRoot->IsRed = false;
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	SanityCheck()
//
//	Verifies that the red-black tree is properly formed.
//
void RedBlackBasic::SanityCheck(void)
{
	if (NULL != m_pRoot) {
		U32 minBlack = 0xFFFFFFFF;
		U32 maxBlack = 0;

		// The root of the tree must always be a black node.
		QzAssert(false == m_pRoot->IsRed);

		SanityCheckRec(m_pRoot, false, 0, minBlack, maxBlack);

		QzAssert(minBlack == maxBlack);
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	SanityCheckRec()
//
void RedBlackBasic::SanityCheckRec(RedBlackBasic_t *pNode, bool isParentRed, U32 blackDepth, U32 &minBlack, U32 &maxBlack)
{
	if (pNode->IsRed) {
		// The parent of a red node must be black.
		QzAssert(false == isParentRed);

	}
	else {
		++blackDepth;
	}

	// If the node has two children, only one of them may be red.
	// The other must be black.
/* Do not apply this test.  This version of red-black trees does not enforce this rule.
	if ((NULL != pNode->pChild[0]) && (NULL != pNode->pChild[1])) {
		if (IsRed(pNode->pChild[0])) {
			QzAssert(false == IsRed(pNode->pChild[1]));
		}
		if (IsRed(pNode->pChild[1])) {
			QzAssert(false == IsRed(pNode->pChild[0]));
		}
	}
*/

	// The child of a red node must be black.
	if (isParentRed) {
		QzAssert(false == pNode->IsRed);
	}

	if (NULL != pNode->pChild[0]) {
		// The left child must come before this node in sorting order.
		QzAssert(pNode->pChild[0]->Ref.Key < pNode->Ref.Key);

		SanityCheckRec(pNode->pChild[0], pNode->IsRed, blackDepth, minBlack, maxBlack);
	}
	else {
		minBlack = Min(minBlack, blackDepth);
		maxBlack = Max(maxBlack, blackDepth);
	}

	if (NULL != pNode->pChild[1]) {
		// The right child must come after this node in sorting order.
		QzAssert(pNode->pChild[1]->Ref.Key > pNode->Ref.Key);

		SanityCheckRec(pNode->pChild[1], pNode->IsRed, blackDepth, minBlack, maxBlack);
	}
	else {
		minBlack = Min(minBlack, blackDepth);
		maxBlack = Max(maxBlack, blackDepth);
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	KeyCount()
//
//	Recursively count the number of keys in the tree.
//
U32 RedBlackBasic::KeyCount(void)
{
	return KeyCountRec(m_pRoot);
}


/////////////////////////////////////////////////////////////////////////////
//
//	KeyCountRec()
//
U32 RedBlackBasic::KeyCountRec(RedBlackBasic_t *pNode)
{
	U32 tally = 0;

	if (NULL != pNode) {
		tally += 1;

		if (NULL != pNode->pChild[0]) {
			tally += KeyCountRec(pNode->pChild[0]);
		}

		if (NULL != pNode->pChild[1]) {
			tally += KeyCountRec(pNode->pChild[1]);
		}
	}

	return tally;
}


/////////////////////////////////////////////////////////////////////////////
//
//	LeafDepth()
//
//	Tests the "black depth" of every leaf node.  This measure is the number
//	of black nodes between each leaf and the root (including the root, since
//	the root of a red-black tree must be black).  The tree depth of every
//	leaf node will be greater, since red nodes are not included in this
//	measure.
//
//	Once complete, it will report the minimum and maximum number of black
//	nodes found for each leaf.  Due to the nature of a red-black tree, these
//	two values must be the same.
//
void RedBlackBasic::LeafDepth(void)
{
	// This array will be used to track the number of nodes at each depth of
	// the binary tree.  This is the true depth of each node, not the black
	// depth.
	U32 ary[64];
	for (U32 i = 0; i < ArraySize(ary); ++i) {
		ary[i] = 0;
	}

	U32 maxDepth = 0;
	U32 minBlack = 0xFFFFFFFF;
	U32 maxBlack = 0;

	if (NULL != m_pRoot) {
		maxDepth = LeafDepthRec(m_pRoot, 0, ary, ArraySize(ary), 0, minBlack, maxBlack);
	}

	U32 tally = 0;

	for (U32 i = 0; i < maxDepth; ++i) {
		tally += ary[i];

		printf("%3d: %5d = %5d\n", i, ary[i], tally);
	}

	printf("black depth: %d to %d\n", minBlack, maxBlack);

	// All leaf nodes must have the same number of black nodes between the
	// root and each leaf.  Therefore the minimum and maximum number of black
	// nodes must be the same.
	QzAssert(minBlack == maxBlack);
}


/////////////////////////////////////////////////////////////////////////////
//
//	LeafDepthRec()
//
U32 RedBlackBasic::LeafDepthRec(RedBlackBasic_t *pNode, U32 depth, U32 ary[], U32 depthLimit, U32 blackDepth, U32 &minBlack, U32 &maxBlack)
{
	// Increment the count of nodes at the current depth of the tree.
	ary[depth] += 1;

	// If this is a black node, increment the count of black nodes along
	// this path from the root.
	if (false == pNode->IsRed) {
		++blackDepth;
	}

	// If this is a leaf node, return the total depth of this node.
	if ((NULL == pNode->pChild[0]) && (NULL == pNode->pChild[1])) {
		return depth + 1;
	}

	// Protect against overflowing the array.  This should not be necessary,
	// since the tree would have to contain on the order of 2^64 nodes to
	// overflow the array.
	if (depth >= depthLimit) {
		return depth;
	}

	// Record the total depth along both the left and right children.
	U32 d1 = 0;
	U32 d2 = 0;

	if (NULL != pNode->pChild[0]) {
		d1 = LeafDepthRec(pNode->pChild[0], depth + 1, ary, depthLimit, blackDepth, minBlack, maxBlack);
	}
	else {
		minBlack = Min(minBlack, blackDepth);
		maxBlack = Max(maxBlack, blackDepth);
	}

	if (NULL != pNode->pChild[1]) {
		d2 = LeafDepthRec(pNode->pChild[1], depth + 1, ary, depthLimit, blackDepth, minBlack, maxBlack);
	}
	else {
		minBlack = Min(minBlack, blackDepth);
		maxBlack = Max(maxBlack, blackDepth);
	}

	// Return the maximum node depth found along this path from the root.
	return Max(d1, d2);
}


/////////////////////////////////////////////////////////////////////////////
//
//	Traverse()
//
//	Perform an in-order traversal of the tree, printing out all of the key
//	values in the tree.  This should display all keys in their sorted order.
//
void RedBlackBasic::Traverse(void)
{
	if (NULL != m_pRoot) {
		U32 prev = 0;
		TraverseRec(m_pRoot, prev);
		printf("\n\n");
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	TraverseRec()
//
void RedBlackBasic::TraverseRec(RedBlackBasic_t *pNode, U32 &prev)
{
	QzAssert(NULL != pNode);

	if (NULL != pNode->pChild[0]) {
		TraverseRec(pNode->pChild[0], prev);
	}

	QzAssert(prev < pNode->Ref.Key);
	prev = pNode->Ref.Key;
	printf("%4d", pNode->Ref.Key);

	if (NULL != pNode->pChild[1]) {
		TraverseRec(pNode->pChild[1], prev);
	}
}


