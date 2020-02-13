/////////////////////////////////////////////////////////////////////////////
//
//	File: TwoThreeExterior.cpp
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


#include "QzCommon.h"
#include "TwoThreeExterior.h"


#ifdef USE_MALLOC_MACRO
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define Child_1		0
#define Child_2		1
#define Child_3		2


/////////////////////////////////////////////////////////////////////////////
//
//	constructor
//
TwoThreeExterior::TwoThreeExterior(void)
	:	m_pRoot(NULL)
{
}


/////////////////////////////////////////////////////////////////////////////
//
//	destructor
//
TwoThreeExterior::~TwoThreeExterior(void)
{
	Free(m_pRoot);
	m_pRoot = NULL;
}


/////////////////////////////////////////////////////////////////////////////
//
//	FreeAll()
//
void TwoThreeExterior::FreeAll(void)
{
	Free(m_pRoot);

	m_pRoot = NULL;
}


/////////////////////////////////////////////////////////////////////////////
//
//	Free()
//
void TwoThreeExterior::Free(TwoThreeExterior_t *pNode)
{
	if (NULL != pNode) {
		if (NULL != pNode->pChild1) {
			Free(pNode->pChild1);
		}
		if (NULL != pNode->pChild2) {
			Free(pNode->pChild2);
		}
		if (NULL != pNode->pChild3) {
			Free(pNode->pChild3);
		}

		SafeDelete(pNode);
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	NewNode()
//
TwoThreeExterior_t* TwoThreeExterior::NewNode(void)
{
	TwoThreeExterior_t *pNew = new TwoThreeExterior_t;

	pNew->Ref.Key      = 0;
	pNew->Ref.pContext = NULL;
	pNew->Low1         = 0;
	pNew->Low2         = 0;
	pNew->Low3         = 0;
	pNew->IsLeaf       = true;
	pNew->pChild1      = NULL;
	pNew->pChild2      = NULL;
	pNew->pChild3      = NULL;

	return pNew;
}


/////////////////////////////////////////////////////////////////////////////
//
//	ChildCount()
//
U32 TwoThreeExterior::ChildCount(TwoThreeExterior_t *pNode)
{
	if (NULL != pNode) {
		if (NULL != pNode->pChild3) {
			return 3;
		}

		if (NULL != pNode->pChild2) {
			return 2;
		}

		if (NULL != pNode->pChild1) {
			return 1;
		}
	}

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
//
//	FixLows()
//
//	Brute-force hack to update all of the cached low keys for a node.
//	It should be possible to avoid this function when inserting or deleting,
//	but that can make the code cumbersome since child nodes may shift up,
//	down, or into sibling nodes when balancing.  The safest way to make
//	certain the cached keys are correct is to update all of them if there
//	is any possibility that one of them has changed.
//
void TwoThreeExterior::FixLows(TwoThreeExterior_t *pNode)
{
	if (NULL != pNode) {
//		U32 l1 = pNode->Low1;
//		U32 l2 = pNode->Low2;
//		U32 l3 = pNode->Low3;
		// If each child node exists, we can grab the Low1 key cached in
		// in it, since cached keys are always kept updated.
		// However, if this is a leaf, the child point will be NULL and the
		// correct look-up key will be stored in Ref.
		pNode->Low1 = (NULL != pNode->pChild1) ? pNode->pChild1->Low1 : pNode->pChild1->Ref.Key;
		pNode->Low2 = (NULL != pNode->pChild2) ? pNode->pChild2->Low1 : 0;
		pNode->Low3 = (NULL != pNode->pChild3) ? pNode->pChild3->Low1 : 0;
//		QzAssert(l1 == pNode->Low1);
//		QzAssert(l2 == pNode->Low2);
//		QzAssert(l3 == pNode->Low3);
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	ShiftChildrenDown()
//
//	Shifts all of the child nodes down the sequence of pointers (or to the
//	left in the tree, depending on how you look at the operation).  This is
//	useful when removing the first child due to balancing (when the first
//	child is rotated up a level or to a sibling node).
//
//	When this is done, the third child will end up as a NULL pointer.
//
void TwoThreeExterior::ShiftChildrenDown(TwoThreeExterior_t *pNode)
{
	pNode->pChild1 = pNode->pChild2;
	pNode->pChild2 = pNode->pChild3;
	pNode->pChild3 = NULL;

	pNode->Low1 = pNode->Low2;
	pNode->Low2 = pNode->Low3;
	pNode->Low3 = 0;
}


/*
/////////////////////////////////////////////////////////////////////////////
//
//	ShiftChildrenUp()
//
//	Shifts the child pointers upward in the sequence (to the right in the
//	tree), leaving the first child a NULL pointer.  This is used when the
//	third child needs to be rotated up the tree or to a sibling node to
//	rebalance the tree.
//
//	This leaves the first child NULL for safety.  In all cases where this
//	function is called, the first child will be filled in after this function
//	has opened up a hole for the new pointer.
//
void TwoThreeExterior::ShiftChildrenUp(TwoThreeExterior_t *pNode)
{
	pNode->pChild3 = pNode->pChild2;
	pNode->pChild2 = pNode->pChild1;
	pNode->pChild1 = NULL;

	pNode->Low3 = pNode->Low2;
	pNode->Low2 = pNode->Low1;
	pNode->Low1 = 0;
}
*/


/////////////////////////////////////////////////////////////////////////////
//
//	LookUp()
//
//	Iteratively traverses the tree looking for the given search key.
//	Returns the context pointer if the key exists within the tree.
//	Returns NULL if the key cannot be found.
//
void* TwoThreeExterior::LookUp(const U32 key)
{
	if (NULL == m_pRoot) {
		return NULL;
	}

	TwoThreeExterior_t *pScan = m_pRoot;

	while (NULL != pScan) {
		// Data values are only stored in leaf nodes.  Once we hit a leaf,
		// we can check if this is the leaf that contains the given key.
		if (pScan->IsLeaf) {
			if (key == pScan->Ref.Key) {
				return pScan->Ref.pContext;
			}
		}

		// The third child may not exist, so only traverse that way when the
		// node is present.
		if ((NULL != pScan->pChild3) && (key >= pScan->Low3)) {
			pScan = pScan->pChild3;
		}
		else if (key >= pScan->Low2) {
			pScan = pScan->pChild2;
		}
		else {
			pScan = pScan->pChild1;
		}
	}

	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
//
//	Insert()
//
//	Insertions are moderately complex, since various special cases must be
//	addressed to rebalance the tree following the insertion.  Insertion is
//	a recursive operation (though it could be made iterative if a stack is
//	used), so most of the complexity resides in the recursive sub-function.
//
void TwoThreeExterior::Insert(VoidRef_t ref)
{
	// In the normal case, the tree is not empty.
	if (NULL != m_pRoot) {

		// Normally all of the insertion work will be done recursively by
		// InsertRec().  However, insertion may cause nodes to be split,
		// which can propagate up the tree to the root.
		//
		// If this returns a non-NULL pointer, then the current root has
		// been split, and pBack is a sibling for the current root.
		//
		TwoThreeExterior_t *pBack = InsertRec(m_pRoot, ref);

		// Create a new root, with m_pRoot and pBack as its children.
		//
		// InsertRec() will have swapped the contents of the current m_pRoot
		// with pNode to assure that m_pRoot < pNode.  Therefore this
		// code can safely make m_pRoot the first child and pNode the second
		// child of the new root node.
		//
		// Note that this increases the depth of the tree by one.
		//
		if (NULL != pBack) {
			TwoThreeExterior_t *pNew = NewNode();
			pNew->IsLeaf  = false;
			pNew->pChild1 = m_pRoot;
			pNew->pChild2 = pBack;
			pNew->Low1    = m_pRoot->Low1;
			pNew->Low2    = pBack->Low1;
			m_pRoot       = pNew;
		}
	}

	// Special case for inserting into an empty tree.
	else {
		m_pRoot = NewNode();
		m_pRoot->IsLeaf = true;
		m_pRoot->Ref    = ref;
		m_pRoot->Low1   = ref.Key;
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	InsertRec()
//
//	Recursively inserts an element into the tree.  It must first traverse
//	down to the leaf where the insertion should occur, insert the element,
//	then rebalance the tree while recursing back out again.
//
//	If a non-NULL pointer is returned, this indicates that pNode had to be
//	split, and that the returned node needs to be inserted into the parent
//	of pNode.
//
//	Note that one difference between this code and Aho is that their version
//	tracks the smallest key of pNew and returns it.  Their approach is
//	mildly cumbersome (and more an artifact of the Pascal-like format of
//	their pseudocode), and requires passing more data around.  Due to the
//	logic of insertion, after a recursive call the lows of pChild will be
//	correct, so we can directly reference pNode's children to update its
//	lows and keep everything in sync.
//
TwoThreeExterior_t* TwoThreeExterior::InsertRec(TwoThreeExterior_t *pNode, VoidRef_t ref)
{
	TwoThreeExterior_t *pNew = NULL;

	// The first half of this function is dedicated to descending down the
	// tree until it reaches the leaf node where the insertion should occur.
	//
	if (false == pNode->IsLeaf) {

		//
		// Determine which of the 2 or 3 children should contain the element
		// that is being inserted.
		//

		U32 childIndex = Child_3;
		TwoThreeExterior_t *pChild = pNode->pChild3;

		if (ref.Key < pNode->Low2) {
			childIndex = Child_1;
			pChild     = pNode->pChild1;
		}
		else if ((NULL == pNode->pChild3) || (ref.Key < pNode->Low3)) {
			childIndex = Child_2;
			pChild     = pNode->pChild2;
		}

		QzAssert(NULL != pChild);

		// Recurse down the appropriate child.
		TwoThreeExterior_t *pBack = InsertRec(pChild, ref);

		// If a non-NULL pointer was returned, we need to insert it into
		// pNode, which may require splitting pNode.
		if (NULL != pBack) {

			// If pNode only contains two children, we can insert pBack
			// into pNode without requiring a split.
			//
			// Note that due to the logic of InsertRec(), pBack should be
			// the second child of pNode since it is guaranteed to contain a
			// key larger than pChild.  The recursive call to InsertRec()
			// will swap the contents of pBack and pChild to guarantee that
			// this is true.
			//
			if (NULL == pNode->pChild3) {
				pNode->IsLeaf = false;

				// If the insertion was to the second child, we can drop
				// pBack in as the third child without any extra effort.
				if (Child_2 == childIndex) {
					pNode->pChild3 = pBack;
					pNode->Low3    = pBack->Low1;
				}

				// However, if the insertion was to the first child, pBack
				// needs to be inserted after it -- again making it the
				// second child.  This requires shifting the current second
				// child over to be the third child, then dropping pBack in
				// as the third child.
				else {
					pNode->pChild3 = pNode->pChild2;
					pNode->Low3    = pNode->pChild2->Low1;
					pNode->pChild2 = pBack;
					pNode->Low2    = pBack->Low1;

					// Note that we must fix up the cached low key for the
					// first child, since the recursive call may have swapped
					// the contents of pChild and pBack before returning,
					// meaning that pNode's cached key is now invalid.
					pNode->Low1    = pNode->pChild1->Low1;
				}
			}

			// Otherwise pNode is a 3-node, so we need to split it into a
			// pair of 2-nodes.
			else {
				pNew = NewNode();

				pNew->IsLeaf = false;

				// pBack is to the right of child 3, so child 3 and pBack
				// become the children of the new node.
				if (Child_3 == childIndex) {
					pNew->pChild1  = pNode->pChild3;
					pNew->Low1     = pNode->pChild3->Low1;
					pNew->pChild2  = pBack;
					pNew->Low2     = pBack->Low1;
				}

				// pBack comes between child 2 and child 3, so pBack and
				// child 3 become the children of pNew -- the difference
				// from the previous condition being the order in which
				// they are stored in pNew
				else if (Child_2 == childIndex) {
					pNew->pChild1  = pBack;
					pNew->Low1     = pBack->Low1;
					pNew->pChild2  = pNode->pChild3;
					pNew->Low2     = pNode->pChild3->Low1;
				}

				// Otherwise pBack comes between child 1 and child 2, so
				// we have to move child 2 and child 3 into pNew, then
				// make pBack the second child of pNode
				else {
					pNew->pChild1  = pNode->pChild2;
					pNew->Low1     = pNode->pChild2->Low1;
					pNew->pChild2  = pNode->pChild3;
					pNew->Low2     = pNode->pChild3->Low1;
					pNode->pChild2 = pBack;
					pNode->Low2    = pBack->Low1;

					// Again, the recursive call to InsertRec() may have
					// swapped pChild and pBack before returning, so we
					// have to fix up the cached low key for child 1.
					pNode->Low1    = pNode->pChild1->Low1;
				}

				pNode->pChild3 = NULL;
				pNode->Low3    = 0;
			}
		}

		// Otherwise the child node was changed, but pNode does not need to
		// be split after the insert.  However, we still need to update the
		// correct low key for the node to guarantee that it is being
		// cached correctly.
		//
		else if (Child_1 == childIndex) {
			pNode->Low1 = pNode->pChild1->Low1;
		}
		else if (Child_2 == childIndex) {
			pNode->Low2 = pNode->pChild2->Low1;
		}
		else {
			pNode->Low3 = pNode->pChild3->Low1;
		}
	}

	// Once the code hits a leaf, this block of code will create a new node
	// to contain the data (or return NULL if nothing was inserted).
	else {

		// Leaf nodes should never be empty unless we're dealing with a
		// degenerate tree (possibly resulting from deleting all elements,
		// then attempting to insert a new element).
		//
		// But we do need to detect and prevent a key from being inserted
		// into the tree multiple times -- this implementation of 2-3 trees
		// does not support duplicate keys.
		//
		if ((NULL != pNode->Ref.pContext) && (ref.Key != pNode->Ref.Key)) {

			pNew = NewNode();

			// Arrange pNode and pNew so that pNode contains the smaller
			// of the two keys.  We'll return pNew so that the caller can
			// insert the new node into the parent (or split the parent if
			// necessary).
			if (pNode->Ref.Key < ref.Key) {
				pNew->Ref   = ref;
				pNew->Low1  = ref.Key;
			}
			else {
				pNew->Ref   = pNode->Ref;
				pNew->Low1  = pNode->Ref.Key;
				pNode->Ref  = ref;
				pNode->Low1 = ref.Key;
			}
		}

		// Special case for when the leaf node has nothing in it.
		// This is needed for inserting into a tree that contains a
		// single, empty node -- in practice, this should never happen.
		//
		// Otherwise, this condition will handle replacing the old value
		// when the key is already in the tree: no allocation is required,
		// just overwrite the old value.
		//
		else {
			pNode->Ref  = ref;
			pNode->Low1 = ref.Key;

			// NOTE: This leaves pNew set to NULL, so the caller knows that
			// nothing needs to be updated.
		}
	}

	// If a new node was created, we need to pass it to the caller so it can
	// add it to the parent node (possibly requiring the parent to be split).
	return pNew;
}


/////////////////////////////////////////////////////////////////////////////
//
//	Delete()
//
//	This function is called to delete a node from the tree.  Most of the
//	work is handled recursively by DeleteRec().  This function only needs to
//	trap a couple of special cases, and to handle deleting the root when
//	merging nodes.
//
void TwoThreeExterior::Delete(const U32 key)
{
	// Special case for an empty tree.
	if (NULL == m_pRoot) {
		return;
	}

	// Special case for deleting the last node in the tree.
	if (m_pRoot->IsLeaf && (key == m_pRoot->Ref.Key)) {
		Free(m_pRoot);
		m_pRoot = NULL;
		return;
	}

	// DeleteRec() will do all of the work.  The only special effort needed
	// is when m_pRoot ends up with a single child (that is not a leaf).
	// If this happens, we need to delete the current root node and make its
	// child the new root of the tree (which causes the depth of the tree to
	// decrease by one level, and is the only way in which the depth of the
	// leaves will change due to deletion).
	//
	if (DeleteRec(m_pRoot, key)) {
		TwoThreeExterior_t *pTemp = m_pRoot;

		m_pRoot = m_pRoot->pChild1;

		pTemp->pChild1 = NULL;

		Free(pTemp);
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	DeleteRec()
//
//	Returns true if merging caused pNode to end up with a single child,
//	which requires the caller to merge pNode with a sibling (or delete it
//	if pNode is the root of the tree).
//
bool TwoThreeExterior::DeleteRec(TwoThreeExterior_t *pNode, const U32 key)
{
	// If we reach a leaf that does not contain the key for deletion,
	// then we know that the key is not in the tree.  No deletion needs
	// to be returned, nor will the caller need to perform any fix-up
	// operations.  Return false so the caller knows no further work is
	// required.
	if (pNode->IsLeaf && (key != pNode->Ref.Key)) {
		return false;
	}

	// We cannot be in a leaf if the key has not yet been found.  That case
	// is handled in the parent node of the leaf, since all of the fix-up
	// work needs to be done in that node.
	QzAssert(false == pNode->IsLeaf);


	///////////////////////////////
	//  Deletion of leaf nodes.  //
	///////////////////////////////

	// Special case: Child 1 is a leaf that contains the key to delete.
	if ((NULL != pNode->pChild1) &&
		(pNode->pChild1->IsLeaf) &&
		(key == pNode->pChild1->Ref.Key))
	{
		// Delete the first child, then shift the two remaining children down.
		// Since this is not a leaf node, it contains at least two children.
		Free(pNode->pChild1);
		ShiftChildrenDown(pNode);

		// If the deletion resulted in this node having only one child,
		// return true so fix-up operations can be performed on the parent.
		return (NULL == pNode->pChild2);
	}

	// Special case: Child 2 is a leaf that contains the key to delete.
	if ((NULL != pNode->pChild2) &&
		(pNode->pChild2->IsLeaf) &&
		(key == pNode->pChild2->Ref.Key))
	{
		// Delete the second child.
		Free(pNode->pChild2);

		// Shift the third child down to replace the second.
		pNode->pChild2 = pNode->pChild3;
		pNode->pChild3 = NULL;

		// Third child is now a NULL reference.
		pNode->Low2 = pNode->Low3;
		pNode->Low3 = 0;

		// If there is no second child, return true so fix-up operations can
		// be applied to the parent node.
		return (NULL == pNode->pChild2);
	}

	// Special case: Child 3 is a leaf that contains the key to delete.
	if ((NULL != pNode->pChild3) &&
		(pNode->pChild3->IsLeaf) &&
		(key == pNode->pChild3->Ref.Key))
	{
		// This is the easiest case.  Just delete the third node and return
		// false.  No fix-up operation will be required since we just turned
		// a 3-node into a 2-node.
		Free(pNode->pChild3);

		pNode->pChild3 = NULL;
		pNode->Low3    = 0;

		return false;
	}


	//////////////////////////
	//  Fix-up operations.  //
	//////////////////////////

	bool result = false;

	// The key to delete is contained in the third child.  The recursive
	// call to DeleteRec() will remove the target node from the tree.
	if ((NULL != pNode->pChild3) && (key >= pNode->Low3)) {
		bool doFixup = DeleteRec(pNode->pChild3, key);

		// If pChild3 ended up with only one child, we need to apply a fix-up
		// operation so it will have two children.
		if (doFixup) {
			TwoThreeExterior_t *three = pNode->pChild3;
			TwoThreeExterior_t *two   = pNode->pChild2;

			// The simple case is when child 2 has three children.  We just
			// need to move child 2's third child over to child 3.  No
			// deletion is required in this case.
			if (3 == ChildCount(two)) {
				// Three's child1 becomes child2.
				three->pChild3 = three->pChild2;
				three->pChild2 = three->pChild1;

				// Two's child3 becomes three's child 1.
				three->pChild1 = two->pChild3;
				two->pChild3   = NULL;

				// Directly fix up the low keys.
				three->Low3    = three->Low2;
				three->Low2    = three->Low1;
				three->Low1    = two->Low3;
				two->Low3      = 0;
			}

			// Otherwise child two only has 2 children, so we need to merge.
			else {
				// Three's only child become two's third child.
				two->pChild3   = three->pChild1;
				two->Low3      = three->Low1;

				// Clear the pointer before we delete the node.
				three->pChild1 = NULL;
				Free(three);

				// Fix this node, since it no longer has a third child.
				pNode->pChild3 = NULL;
				pNode->Low3    = 0;
			}
		}

		// Repair the low key references to the children.  This needs to
		// always be performed, even if no fix-up was required for this node.
		// If any fix-up was applied to the child node, its low keys may
		// have changed, so we must propagate the updated low keys up the
		// tree.
		FixLows(pNode);

		// We can always resolve merging or swapping at this level, so the
		// caller does not need to do any more repair work.
		return false;
	}


	// The node to delete is contained within the second child.
	if ((NULL != pNode->pChild2) && (key >= pNode->Low2)) {
		bool doFixup = DeleteRec(pNode->pChild2, key);

		// If child 2 ended up with only one child, we need to either move a
		// second node into child 2, or delete child 2.
		if (doFixup) {
			TwoThreeExterior_t *two   = pNode->pChild2;
			TwoThreeExterior_t *one   = pNode->pChild1;
			TwoThreeExterior_t *three = pNode->pChild3;

			// If child 1 contains three nodes, we can move one of those
			// nodes over to child 2.
			if (3 == ChildCount(one)) {
				// Two's child 1 becomes its child 2.
				two->pChild2 = two->pChild1;
				two->Low2    = two->Low1;

				// One's child 3 becomes two's child 1.
				two->pChild1 = one->pChild3;
				two->Low1    = one->Low3;

				// One no longer has a child 3.
				one->pChild3 = NULL;
				one->Low3    = 0;
			}

			// If child 3 has three nodes, we can move one of those nodes
			// into child 2.
			//
			// Note that ChildCount() returns 0 if three does not exist,
			// so we don't need extra testing logic here.
			//
			else if (3 == ChildCount(three)) {
				// Three's child 1 becomes two's child 2.
				two->pChild2 = three->pChild1;
				two->Low2    = three->Low1;

				// Three's other children shift down one to fill up the hole.
				three->pChild1 = three->pChild2;
				three->pChild2 = three->pChild3;
				three->pChild3 = NULL;
				three->Low1    = three->Low2;
				three->Low2    = three->Low3;
				three->Low3    = 0;
			}

			// Otherwise child 1 only has two nodes (and child 3 either has
			// only two children, or it does not exist at all).
			else {
				// Two's only child becomes one's child 3.
				one->pChild3 = two->pChild1;
				one->Low3    = two->Low1;

				// Clear the pointer so we can delete the node.
				two->pChild1 = NULL;
				Free(two);

				// Child 3 may or may not exist.  It is safe to copy it over
				// to replace child 2, since at this stage we just need to
				// fix up the pointers in this node.
				pNode->pChild2 = pNode->pChild3;
				pNode->Low2    = pNode->Low3;
				pNode->pChild3 = NULL;
				pNode->Low3    = 0;

				// However, if there was no Child 3 before, then we've
				// created a degenerate node.  Return true if there is no
				// Child 2 so that the caller knows it has fix-up work to do.
				result = (NULL == pNode->pChild2);
			}
		}

		// Repair the low key references to the children.  This needs to
		// always be performed, even if no fix-up was required for this node.
		// If any fix-up was applied to the child node, its low keys may
		// have changed, so we must propagate the updated low keys up the
		// tree.
		FixLows(pNode);

		return result;
	}


	// Otherwise the node to delete exists under the first child.  Let the
	// recursive logic handle the deletion.
	bool doFixup = DeleteRec(pNode->pChild1, key);

	// If child 1 ended up with only one child, we need to fix it up so it
	// has two children, or delete it.
	if (doFixup) {
		TwoThreeExterior_t *one = pNode->pChild1;
		TwoThreeExterior_t *two = pNode->pChild2;

		// If child 2 has three children, then we can move one of them over
		// to fill in child 1.
		if (3 == ChildCount(two)) {
			// Child 2's first child is moved over to Child 1.
			one->pChild2 = two->pChild1;
			one->Low2    = two->Low1;

			// Shift Child 2's other children down to fill up the hole.
			two->pChild1 = two->pChild2;
			two->pChild2 = two->pChild3;
			two->pChild3 = NULL;

			two->Low1 = two->Low2;
			two->Low2 = two->Low3;
			two->Low3 = 0;
		}

		// Otherwise Child 2 only has two children, so we have to merge
		// Child 1 into child 2.
		else {
			// Shift Child 2's children up one to open up a hole.
			two->pChild3 = two->pChild2;
			two->Low3    = two->Low2;
			two->pChild2 = two->pChild1;
			two->Low2    = two->Low1;

			// Child 1's only child is moved over to child 2.
			two->pChild1 = one->pChild1;
			two->Low1    = one->Low1;

			// Clear the pointer so we can delete this node.
			one->pChild1 = NULL;
			Free(one);

			// We've now made a hole in the current node, so we need to
			// shift it's children down one to fill in the hole.
			ShiftChildrenDown(pNode);

			// If the current node has ended up with only one child, we need
			// to return true so that the caller knows it needs to perform
			// fix-up operations on the parent node.
			result = (NULL == pNode->pChild2);
		}
	}

	// Repair the low key references to the children.  This needs to always
	// be performed, even if no fix-up was required for this node.  If any
	// fix-up was applied to the child node, its low keys may have changed,
	// so we must propagate the updated low keys up the tree.
	FixLows(pNode);

	return result;
}


/////////////////////////////////////////////////////////////////////////////
//
//	SanityCheck()
//
//	This is a wrapper around logic to recursively scan through the tree and
//	verify that the tree is properly formed.
//
void TwoThreeExterior::SanityCheck(void)
{
	if (NULL != m_pRoot) {
		SanityCheckRec(m_pRoot);
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	SanityCheckRec()
//
void TwoThreeExterior::SanityCheckRec(TwoThreeExterior_t *pNode)
{
	QzAssert(NULL != pNode);

	if (pNode->IsLeaf) {
		QzAssert(pNode->Low1 == pNode->Ref.Key);
		QzAssert(0 == pNode->Low2);
		QzAssert(0 == pNode->Low3);
		QzAssert(NULL == pNode->pChild1);
		QzAssert(NULL == pNode->pChild2);
		QzAssert(NULL == pNode->pChild3);
	}
	else {
		QzAssert(NULL != pNode->pChild1);
		QzAssert(NULL != pNode->pChild2);

		QzAssert(pNode->Low1 == pNode->pChild1->Low1);
		QzAssert(pNode->Low2 == pNode->pChild2->Low1);

		QzAssert(pNode->Low1 < pNode->Low2);

		if (NULL != pNode->pChild3) {
			QzAssert(pNode->Low3 == pNode->pChild3->Low1);
			QzAssert(pNode->Low2 < pNode->Low3);
		}

		if (pNode->pChild1->IsLeaf) {
			QzAssert(pNode->pChild2->IsLeaf);

			if (NULL != pNode->pChild3) {
				QzAssert(pNode->pChild3->IsLeaf);
			}
		}

		SanityCheckRec(pNode->pChild1);
		SanityCheckRec(pNode->pChild2);

		if (NULL != pNode->pChild3) {
			SanityCheckRec(pNode->pChild3);
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	KeyCount()
//
//	Recursively count the number of keys in the tree.
//
U32 TwoThreeExterior::KeyCount(void)
{
	return KeyCountRec(m_pRoot);
}


/////////////////////////////////////////////////////////////////////////////
//
//	KeyCountRec()
//
U32 TwoThreeExterior::KeyCountRec(TwoThreeExterior_t *pNode)
{
	U32 tally = 0;

	if (pNode->IsLeaf) {
		tally += 1;
	}
	else {
		tally += KeyCountRec(pNode->pChild1) + KeyCountRec(pNode->pChild2);

		if (NULL != pNode->pChild3) {
			tally += KeyCountRec(pNode->pChild3);
		}
	}

	return tally;
}


/////////////////////////////////////////////////////////////////////////////
//
//	LeafDepth()
//
//	Recursively verify the depth of every leaf in the tree.  For a properly
//	formed 2-3 tree, every leaf should be at exactly the same depth, and
//	there should be one leaf for every key in the tree.
//
void TwoThreeExterior::LeafDepth(void)
{
	U32 ary[64];
	for (U32 i = 0; i < ArraySize(ary); ++i) {
		ary[i] = 0;
	}

	U32 maxDepth = LeafDepthRec(m_pRoot, 0, ary, ArraySize(ary));

	for (U32 i = 0; i < maxDepth; ++i) {
		printf("%3d: %5d\n", i, ary[i]);
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	LeafDepthRec()
//
U32 TwoThreeExterior::LeafDepthRec(TwoThreeExterior_t *pNode, U32 depth, U32 ary[], U32 depthLimit)
{
	if (pNode->IsLeaf) {
		ary[depth] += 1;
		return depth + 1;
	}

	if (depth >= depthLimit) {
		return depth;
	}

	U32 d1 = LeafDepthRec(pNode->pChild1, depth + 1, ary, depthLimit);
	U32 d2 = LeafDepthRec(pNode->pChild2, depth + 1, ary, depthLimit);

	U32 maxDepth = Max(d1, d2);

	if (NULL != pNode->pChild3) {
		U32 d3 = LeafDepthRec(pNode->pChild3, depth + 1, ary, depthLimit);

		maxDepth = Max(maxDepth, d3);
	}

	return maxDepth;
}


/////////////////////////////////////////////////////////////////////////////
//
//	Traverse()
//
void TwoThreeExterior::Traverse(void)
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
void TwoThreeExterior::TraverseRec(TwoThreeExterior_t *pNode, U32 &prev)
{
	QzAssert(NULL != pNode);

	if (pNode->IsLeaf) {
		QzAssert(prev < pNode->Ref.Key);
		prev = pNode->Ref.Key;

		printf("%4d", pNode->Ref.Key);

		QzAssert(NULL == pNode->pChild1);
		QzAssert(NULL == pNode->pChild2);
		QzAssert(NULL == pNode->pChild3);
	}
	else {
		TraverseRec(pNode->pChild1, prev);
		TraverseRec(pNode->pChild2, prev);

		if (NULL != pNode->pChild3) {
			TraverseRec(pNode->pChild3, prev);
		}
	}
}






