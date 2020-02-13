/////////////////////////////////////////////////////////////////////////////
//
//	File: TwoThreeInterior.cpp
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


#include "QzCommon.h"
#include "TwoThreeInterior.h"


#ifdef USE_MALLOC_MACRO
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
//
//	constructor
//
TwoThreeInterior::TwoThreeInterior(void)
	:	m_pRoot(NULL)
{
}


/////////////////////////////////////////////////////////////////////////////
//
//	destructor
//
TwoThreeInterior::~TwoThreeInterior(void)
{
	Free(m_pRoot);
	m_pRoot = NULL;
}


/////////////////////////////////////////////////////////////////////////////
//
//	FreeAll()
//
void TwoThreeInterior::FreeAll(void)
{
	Free(m_pRoot);

	m_pRoot = NULL;
}


/////////////////////////////////////////////////////////////////////////////
//
//	Free()
//
void TwoThreeInterior::Free(TwoThreeInterior_t *pNode)
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
TwoThreeInterior_t* TwoThreeInterior::NewNode(void)
{
	TwoThreeInterior_t *pNew = new TwoThreeInterior_t;

	pNew->RefLo.Key      = 0;
	pNew->RefLo.pContext = NULL;
	pNew->RefHi.Key      = 0;
	pNew->RefHi.pContext = NULL;
	pNew->pChild1        = NULL;
	pNew->pChild2        = NULL;
	pNew->pChild3        = NULL;

	return pNew;
}


/////////////////////////////////////////////////////////////////////////////
//
//	LookUp()
//
//	Iteratively traverses the tree looking for the given search key.
//	Returns the context pointer if the key exists within the tree.
//	Returns NULL if the key cannot be found.
//
void* TwoThreeInterior::LookUp(const U32 key)
{
	if (NULL == m_pRoot) {
		return NULL;
	}

	TwoThreeInterior_t *pScan = m_pRoot;

	while (NULL != pScan) {
		if (key < pScan->RefLo.Key) {
			pScan = pScan->pChild1;
		}
		else if (key == pScan->RefLo.Key) {
			return pScan->RefLo.pContext;
		}

		// If this is a 2-node, iterate down to the second child.
		else if (NULL == pScan->RefHi.pContext) {
			pScan = pScan->pChild2;
		}

		// Otherwise we need to compare against the higher key to
		// determine whether we traverse the second or third child.
		else if (key < pScan->RefHi.Key) {
			pScan = pScan->pChild2;
		}
		else if (key > pScan->RefHi.Key) {
			pScan = pScan->pChild3;
		}
		else {
			return pScan->RefHi.pContext;
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
void TwoThreeInterior::Insert(VoidRef_t ref)
{
	// In the normal case, the tree is not empty.
	if (NULL != m_pRoot) {
		TwoThreeInterior_t *pN1  = NULL;
		TwoThreeInterior_t *pN2  = NULL;

		// If the recursive operation returns true, we need to split the
		// root of the tree.  The existing root node has already been
		// changed to maintain sorting order (the current root pointer is
		// returned in pN1), and a new node is returned in pN2.  Since
		// all conditioning was done by the recursive routine, all we
		// need to do is create a new root node and make the two pointers
		// the children of the new root node.
		//
		// Also note that ref is pass-by-reference, so upon exiting the
		// recursive function, ref will now contain the correct key for
		// the new root node.
		//
		if (InsertRec(m_pRoot, ref, &pN1, &pN2)) {
			m_pRoot = NewNode();

			m_pRoot->RefLo   = ref;
			m_pRoot->pChild1 = pN1;
			m_pRoot->pChild2 = pN2;
		}
	}

	// Special case for inserting into an empty tree.
	else {
		m_pRoot = NewNode();
		m_pRoot->RefLo  = ref;
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	InsertRec()
//
//	This is a recursive method that does all of the work for locating the
//	position where the insertion should occur, rearranging the contents of
//	the tree, and returning info needed by the parent to perform a split.
//
//	If the node ends up being split, this method will return true, so the
//	caller knows it needs to finish the splitting logic.  This method cannot
//	do the split itself, since it does not have a reference to the parent
//	node.  This is a necessity to generalize the code to also handle the
//	root of the tree.  Because the root does not have a parent, it is more
//	simple to create a new node (if necessary), return that in ppN2, and
//	let the caller handle creating a new node to contain the split.
//
//	If a split is required, this code will rearrange data to guarantee
//	that all values stored in ppN1 and ppN2 are sorted, that ppN1 will
//	be positioned to the left (less-than) of ppN2, and that ref will be
//	modified to hold the key that falls between ppN1 and ppN2.
//
//	This means that the contents of pNode will be changed when a split is
//	required.  Technically, this operation could be considered destructive,
//	but it is necessary to assure that the two sibling nodes at this level
//	are organized correctly.  It also makes it easy for the caller to
//	finish the split by allocating a new node, making ppN1 and ppN2 its
//	two children, and setting returned value of ref as the key that comes
//	between the two modified nodes.
//
bool TwoThreeInterior::InsertRec(TwoThreeInterior_t *pNode, VoidRef_t &ref, TwoThreeInterior_t **ppN1, TwoThreeInterior_t **ppN2)
{
	TwoThreeInterior_t *pN1  = NULL;
	TwoThreeInterior_t *pN2  = NULL;
	TwoThreeInterior_t *pNew = NULL;

	VoidRef_t key[3];

	// If a split operation occurs, we'll have to turn a 3-node into a
	// 4-node.  However, there is no such thing as a 4-node in a 2-3
	// tree.  This array is used as a placeholder while rearranging the
	// four nodes in the correct order.
	TwoThreeInterior_t *pC[4];

	// This is an internal node.  We need to start off by recursing down
	// to the appropriate child.  If the recursion caused a split, we
	// need to finish the split logic here.
	if (NULL != pNode->pChild1) {

		// Recurse down the first child.
		if (ref.Key < pNode->RefLo.Key) {
			if (false == InsertRec(pNode->pChild1, ref, &pN1, &pN2)) {
				return false;
			}

			// A split of the child nodes was performed.  Figure out what
			// work is needed to clean up at this level.

			// If this is a 2-node, we just need to rearrange the keys and
			// child pointers.
			//
			// Since we recursed down the left, the second child remains
			// unchanged.  All we need to do for it is shift it over to
			// become the third child.
			//
			// Then we poke the returned values in as the first and second
			// child.
			//
			// In this case, the splitting is now contained, so we return
			// false.
			//
			if (NULL == pNode->RefHi.pContext) {
				pNode->RefHi   = pNode->RefLo;
				pNode->RefLo   = ref;
				pNode->pChild3 = pNode->pChild2;
				pNode->pChild2 = pN2;
				pNode->pChild1 = pN1;

				return false;
			}

			// Otherwise we need to convert this 3-node into a 4-node.
			// Poke the values into the arrays so the clean-up logic can
			// split this node.
			else {
				key[0] = ref;
				key[1] = pNode->RefLo;
				key[2] = pNode->RefHi;
				pC[0]  = pN1;
				pC[1]  = pN2;
				pC[2]  = pNode->pChild2;
				pC[3]  = pNode->pChild3;
			}
		}

		// If this is a 3-node, we need to decide whether to recurse down
		// the second or third child.
		else if (NULL != pNode->RefHi.pContext) {

			// Recurse down the middle child.
			if (ref.Key < pNode->RefHi.Key) {
				if (false == InsertRec(pNode->pChild2, ref, &pN1, &pN2)) {
					return false;
				}

				// The recursive call caused a split.  Since we now have a
				// 4-node, we have no choice but to split this node as well.
				key[0] = pNode->RefLo;
				key[1] = ref;
				key[2] = pNode->RefHi;
				pC[0]  = pNode->pChild1;
				pC[1]  = pN1;
				pC[2]  = pN2;
				pC[3]  = pNode->pChild3;
			}

			// Recurse down the third child.
			else if (ref.Key > pNode->RefHi.Key) {
				if (false == InsertRec(pNode->pChild3, ref, &pN1, &pN2)) {
					return false;
				}

				key[0] = pNode->RefLo;
				key[1] = pNode->RefHi;
				key[2] = ref;
				pC[0]  = pNode->pChild1;
				pC[1]  = pNode->pChild2;
				pC[2]  = pN1;
				pC[3]  = pN2;
			}

			else {
				pNode->RefHi = ref;
				return false;
			}
		}

		// Otherwise this is a 2-node, so we can only recurse down the
		// middle child.
		else if (ref.Key > pNode->RefLo.Key) {
			if (false == InsertRec(pNode->pChild2, ref, &pN1, &pN2)) {
				return false;
			}

			// A split occurred.  However, since this is a 2-node, the split
			// is contained to this level of the tree.  Since the recursive
			// call did all of the rearranging work, we replace the current
			// key and pointers with the values returned from the recursive
			// call.
			pNode->RefHi   = ref;
			pNode->pChild2 = pN1;
			pNode->pChild3 = pN2;

			return false;
		}

		else {
			pNode->RefLo = ref;
			return false;
		}
	}

	// Recursive base case:  This is where we hit a leaf node.  Unless
	// the key value is found in this node, we will try to insert the new
	// key in this node, possibly causing a split.
	else {
		// This leaf only contains one key.  No split is required, we just
		// need to insert the new key into the correct place (or replace
		// an existing value).
		if (NULL == pNode->RefHi.pContext) {

			// If the new key is bigger than the one key in this node,
			// it becomes the second key.
			if (ref.Key > pNode->RefLo.Key) {
				pNode->RefHi = ref;
			}

			// Otherwise we need to shift the existing key over to be
			// the second key, then drop the new key in as the first key.
			else if (ref.Key < pNode->RefLo.Key) {
				pNode->RefHi = pNode->RefLo;
				pNode->RefLo = ref;
			}

			// Otherwise we're replacing an existing key with a new value.
			// This does not insert a new value, it only replaces the
			// existing value.  
			else {
				pNode->RefLo = ref;
			}

			// No split was required, so no fix-up operations will be
			// required as we back out of the recursion.
			return false;
		}

		// Otherwise splitting is required.  We don't need to deal with
		// any pointers, since there are no child nodes, but we do need to
		// rearrange the existing keys so that they are correctly sorted.
		//
		// Note the special cases where the key being replaced matches a
		// key already present.  Here we just need to replace the old key,
		// under the assumption that its value has been updated.

		if (ref.Key < pNode->RefLo.Key) {
			key[0] = ref;
			key[1] = pNode->RefLo;
			key[2] = pNode->RefHi;
		}

		// Special case: replace an existing value.
		else if (ref.Key == pNode->RefLo.Key) {
			pNode->RefLo = ref;
			return false;
		}

		else if (ref.Key < pNode->RefHi.Key) {
			key[0] = pNode->RefLo;
			key[1] = ref;
			key[2] = pNode->RefHi;
		}

		// Special case: replace an existing value.
		else if (ref.Key == pNode->RefHi.Key) {
			pNode->RefHi = ref;
			return false;
		}

		else {
			key[0] = pNode->RefLo;
			key[1] = pNode->RefHi;
			key[2] = ref;
		}

		// Since this is a leaf node, leave all child pointers NULL.
		pC[0] = NULL;
		pC[1] = NULL;
		pC[2] = NULL;
		pC[3] = NULL;
	}


	// The remaining code is only executed if we have created a 4-node.
	// This forces pNode to be split.  The code above has already stored
	// the keys and child pointers in the correct order.

	// Destructively replace the contents of pNode to contain the first
	// two child nodes.
	pNode->RefLo          = key[0];
	pNode->RefHi.Key      = 0;
	pNode->RefHi.pContext = NULL;
	pNode->pChild1        = pC[0];
	pNode->pChild2        = pC[1];
	pNode->pChild3        = NULL;

	// Create a new node to contain the last two child pointers.
	pNew = NewNode();
	pNew->RefLo           = key[2];
	pNew->pChild1         = pC[2];
	pNew->pChild2         = pC[3];
	pNew->pChild3         = NULL;

	// Return the info about these two sibling nodes so the caller can
	// handle the split at its level.
	ref   = key[1];
	*ppN1 = pNode;
	*ppN2 = pNew;

	return true;
}


/////////////////////////////////////////////////////////////////////////////
//
//	InOrderSuccessor()
//
//	Given a key value, find the smallest key that comes immediately after
//	this node.  This simply amounts to finding the leftmost child.
//
//	Since this code is used specifically for deletion, it will swap the
//	in-order successor into the given node, and place the old value of the
//	current node in ref.  This simplifies the swapping logic needed when
//	merging nodes.
//
bool TwoThreeInterior::InOrderSuccessor(TwoThreeInterior_t *pNode, VoidRef_t &ref)
{
	// Keep traversing until we hit a leaf node.
	while (NULL != pNode->pChild1) {
		QzAssert(NULL != pNode);

		pNode = pNode->pChild1;
	}

	// Return the old value stored in pNode.
	ref = pNode->RefLo;

	pNode->RefLo          = pNode->RefHi;
	pNode->RefHi.Key      = 0;
	pNode->RefHi.pContext = NULL;

	return (NULL == pNode->RefLo.pContext);
}


/////////////////////////////////////////////////////////////////////////////
//
//	FixInOrderSuccessor()
//
//	If an in-order successor was found, InOrderSuccessor() will have modified
//	the contents of a node, leaving it in a broken state.  This method will
//	recurse down the tree and repair the damage using successive calls to
//	FixFirst().
//
bool TwoThreeInterior::FixInOrderSuccessor(TwoThreeInterior_t *pNode)
{
	if (NULL == pNode->pChild1) {
		return true;
	}

	// If this call returns true, the first child needs to be removed from
	// the tree.
	if (FixInOrderSuccessor(pNode->pChild1)) {
		return FixFirst(pNode);
	}

	return false;
}


/////////////////////////////////////////////////////////////////////////////
//
//	FixFirst()
//
//	This is called when pNode->pChild1 has ended up as a degenerate 1-node.
//
//	If pNode->pChild2 is a 3-node, the nodes can be rebalanced by rotating
//	one of the keys from pChild2 to pChild1.
//
//	Otherwise the code has to merge pChild1 and pChild2.  If pNode only
//	had two children before the merge, this will cause pNode to become a
//	degenerate 1-node.
//
//	Returns true if pNode ends up with only one child after the merge.
//
bool TwoThreeInterior::FixFirst(TwoThreeInterior_t *pNode)
{
	TwoThreeInterior_t *pC1 = pNode->pChild1;
	TwoThreeInterior_t *pC2 = pNode->pChild2;

	// If pChild2 is a 3-node, we rotate one of its children over to
	// pChild1.  No merge is needed, so the fix-up work on the tree is
	// now complete.
	if (NULL != pC2->RefHi.pContext) {
		pC1->RefLo   = pNode->RefLo;
		pNode->RefLo = pC2->RefLo;
		pC2->RefLo   = pC2->RefHi;
		pC2->RefHi.Key      = 0;
		pC2->RefHi.pContext = NULL;
		pC1->pChild2 = pC2->pChild1;
		pC2->pChild1 = pC2->pChild2;
		pC2->pChild2 = pC2->pChild3;
		pC2->pChild3 = NULL;

		return false;
	}

	// If we get to here, pChild2 is a 2-node.  We have no choice but to
	// merge pChild1 and pChild2.

	// The first key in pNode and the only key in pChild2 are moved into
	// pChild1, turning pChild1 into a 3-node.
	pC1->RefLo = pNode->RefLo;
	pC1->RefHi = pC2->RefLo;
	pC2->RefLo.Key      = 0;
	pC2->RefLo.pContext = NULL;

	// The two children of pChild2 are moved into pChild1.
	pC1->pChild2 = pC2->pChild1;
	pC1->pChild3 = pC2->pChild2;

	// Clear out the pointers to avoid problems with freeing pChild2.
	pC2->pChild1 = NULL;
	pC2->pChild2 = NULL;
	pC2->pChild3 = NULL;

	// pChild3 becomes pChild2.  This may turn pNode into a degenerate
	// 1-node.
	pNode->RefLo = pNode->RefHi;
	pNode->RefHi.Key      = 0;
	pNode->RefHi.pContext = NULL;
	pNode->pChild2 = pNode->pChild3;
	pNode->pChild3 = NULL;

	Free(pC2);

	// Following the merge, pNode may have turned into a 1-node.  Return
	// true if the merge needs to propagate up the tree.
	return (NULL == pNode->RefLo.pContext);
}


/////////////////////////////////////////////////////////////////////////////
//
//	FixSecond()
//
//	This is called when pNode->pChild2 has ended up as a degenerate 1-node.
//
//	This is the most complex of the fix-up routines.  If either pChild1 or
//	pChild3 is a 3-node, the extra node can be rotated into pChild2 to turn
//	it back into a 2-node.  If we cannot rotate a value in from a sibling,
//	we will be forced to merge pChild1 and pChild2.
//
//	Returns true if pNode ends up as a 1-node, which forces the fix-up
//	operation to propagate up to the next level of the tree.
//
//	NOTE: This pNode is never a leaf node.  Before this call, the node is
//	legal and has at least two children.
//
bool TwoThreeInterior::FixSecond(TwoThreeInterior_t *pNode)
{
	TwoThreeInterior_t *pC1 = pNode->pChild1;
	TwoThreeInterior_t *pC2 = pNode->pChild2;
	TwoThreeInterior_t *pC3 = pNode->pChild3;

	// If pChild3 exists and is a 3-node, rotate a value over to pChild2.
	if ((NULL != pC3) && (NULL != pC3->RefHi.pContext)) {
		pC2->RefLo   = pNode->RefHi;
		pNode->RefHi = pC3->RefLo;
		pC3->RefLo   = pC3->RefHi;
		pC3->RefHi.Key      = 0;
		pC3->RefHi.pContext = NULL;

		pC2->pChild2 = pC3->pChild1;
		pC3->pChild1 = pC3->pChild2;
		pC3->pChild2 = pC3->pChild3;
		pC3->pChild3 = NULL;

		return false;
	}
	
	// If pChild1 is a 3-node, rotate its extra child over to pChild2.
	if (NULL != pC1->RefHi.pContext) {
		pC2->RefLo   = pNode->RefLo;
		pNode->RefLo = pC1->RefHi;
		pC1->RefHi.Key      = 0;
		pC1->RefHi.pContext = NULL;

		pC2->pChild2 = pC2->pChild1;
		pC2->pChild1 = pC1->pChild3;
		pC1->pChild3 = NULL;

		return false;
	}
	
	// If pChild3 exists, merge it with pChild2.
	if (NULL != pNode->RefHi.pContext) {
		pC2->RefLo = pNode->RefHi;
		pC2->RefHi = pC3->RefLo;
		pNode->RefHi.Key      = 0;
		pNode->RefHi.pContext = NULL;
		pC3->RefLo.Key        = 0;
		pC3->RefLo.pContext   = NULL;

		pC2->pChild2 = pC3->pChild1;
		pC2->pChild3 = pC3->pChild2;
		pC3->pChild1 = NULL;
		pC3->pChild2 = NULL;

		pNode->pChild3= NULL;

		Free(pC3);

		return false;
	}

	// The last resort is to merge pChild1 and pChild2.
	// This will turn pNode into a degenerate 1-node.
	pC1->RefHi = pNode->RefLo;
	pNode->RefLo.Key      = 0;
	pNode->RefLo.pContext = NULL;

	pC1->pChild3   = pC2->pChild1;
	pC2->pChild1   = NULL;
	pNode->pChild2 = NULL;

	Free(pC2);

	// Return true since pNode is degenerate and the fix-up operation
	// must propagate up to the next level of the tree.
	return true;
}


/////////////////////////////////////////////////////////////////////////////
//
//	FixThird()
//
//	This is called when pNode->pChild3 has ended up as a degenerate 1-node.
//
//	In this case, pNode is a 3-node, so we have enough values to safely
//	contain the merging.  At worst, we may delete pChild3.
//
//	Note that in this case, since merging is contained to this node, this
//	function does not need to return anything.
//
void TwoThreeInterior::FixThird(TwoThreeInterior_t *pNode)
{
	TwoThreeInterior_t *pC2 = pNode->pChild2;
	TwoThreeInterior_t *pC3 = pNode->pChild3;

	// If pChild2 is a 3-node, we need to perform a rotation.
	if (NULL != pC2->RefHi.pContext) {
		pC3->RefLo   = pNode->RefHi;
		pNode->RefHi = pC2->RefHi;
		pC2->RefHi.Key    = 0;
		pC2->RefHi.pContext = NULL;

		pC3->pChild2 = pC3->pChild1;
		pC3->pChild1 = pC2->pChild3;
		pC2->pChild3 = NULL;
	}

	// Otherwise pChild2 is a 2-node.  We have to fix pChild3 by merging it
	// with pChild2, making pNode a 2-node.
	else {
		pC2->RefHi = pNode->RefHi;
		pNode->RefHi.Key      = 0;
		pNode->RefHi.pContext = NULL;

		pC2->pChild3 = pC3->pChild1;
		pC3->pChild1 = NULL;

		pNode->pChild3 = NULL;

		Free(pC3);
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	Delete()
//
//	This is the main routine for deleting a key from the tree.  
//
void TwoThreeInterior::Delete(const U32 key)
{
	if (DeleteRec(m_pRoot, key)) {

		// If a merge propagated all the way to the root, we need to delete
		// the root of the tree.
		//
		// Note that this is the only way that the depth of the tree can
		// change.  By deleting the root, the depth of every leaf node has
		// just decreased by 1.

		TwoThreeInterior_t *pTemp = m_pRoot;

		m_pRoot = m_pRoot->pChild1;

		pTemp->pChild1 = NULL;

		Free(pTemp);
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	DeleteRec()
//
//	Recursively locate the node to delete (if it even exists in the tree),
//	then remove the node.  If this causes a degenerate 1-node, we need to
//	start merging this node with one of its siblings.
//
//	Returns true if the node needs to be merged.  The caller is responsible
//	for completing the merge (or in the case of the root, deleting the old
//	root of the tree).
//
bool TwoThreeInterior::DeleteRec(TwoThreeInterior_t *pNode, const U32 key)
{
	// This is an interior node.  Determine which child should be recursed.
	// If the recursive call returned true, the node need to be merged.
	if (NULL != pNode->pChild1) {
		// Recurse down the leftmost child.
		if (key < pNode->RefLo.Key) {
			if (DeleteRec(pNode->pChild1, key)) {
				// pChild1 was turned into a 1-node.  Perform a fix-up
				// operation to turn pChild1 into a 2-node.
				return FixFirst(pNode);
			}
		}

		// The key to delete is the first key in pNode.
		else if (key == pNode->RefLo.Key) {
			VoidRef_t succ;

			// Find the smallest key that is larger than the key being
			// deleted.  This will replace the first key in pNode.
			if (InOrderSuccessor(pNode->pChild2, succ)) {
				pNode->RefLo = succ;

				// If InOrderSuccessor() returned true, the node that used
				// to contain the successor key was turned into a 1-node,
				// so we need to apply a fix-up operation to that node,
				// which in turn may cause merging operation to progress up
				// the tree.
				if (FixInOrderSuccessor(pNode->pChild2)) {
					return FixSecond(pNode);
				}
			}
			else {
				pNode->RefLo = succ;
			}
		}

		// If pNode is a 3-node, we need to recurse down either pChild2 or
		// pChild3.
		else if (NULL != pNode->RefHi.pContext) {

			// Recurse down the second child.
			if (key < pNode->RefHi.Key) {
				if (DeleteRec(pNode->pChild2, key)) {
					return FixSecond(pNode);
				}
			}

			// Recurse down the third child.
			else if (key > pNode->RefHi.Key) {
				if (DeleteRec(pNode->pChild3, key)) {
					FixThird(pNode);
					return false;
				}
			}

			// If we get here, pNode's second key is the one that needs to
			// be deleted.  That key needs to be replaced by the in-order
			// successor to the key being deleted.
			else {
				VoidRef_t succ;

				// Find the smallest key that is larger than the key being
				// deleted.  This will replace the first key in pNode.
				if (InOrderSuccessor(pNode->pChild3, succ)) {
					pNode->RefHi = succ;

					// If InOrderSuccessor() returned true, the node that
					// used to contain the successor key was turned into a
					// 1-node, so we need to apply a fix-up operation to
					// that node, which in turn may cause merging operations
					// to progress up the tree.
					if (FixInOrderSuccessor(pNode->pChild3)) {
						FixThird(pNode);
						return false;
					}
				}
				else {
					pNode->RefHi = succ;
				}
			}
		}

		// Otherwise pNode is a 2-node, so the only option is to recurse
		// down the second child in search of the key to delete.
		else {
			if (DeleteRec(pNode->pChild2, key)) {
				return FixSecond(pNode);
			}
		}
	}

	// Special case for hitting a leaf node.
	else {
		// If the key to remove is the first key, we try to replace the
		// first key with the second.
		if (key == pNode->RefLo.Key) {

			// Assume there is a second key and replace the first with it.
			// If not, we have at least succeeded in NULLing out the first
			// node.
			pNode->RefLo          = pNode->RefHi;
			pNode->RefHi.Key      = 0;
			pNode->RefHi.pContext = NULL;

			// Return true if we ended up with a node that contains no keys.
			// The parent will need to handle merging the node.
			return (NULL == pNode->RefLo.pContext);
		}

		// If there is a second node, check to see if it is the key we need
		// to remove.  If so, we only need to NULL out the reference.
		// Always return false, since no merge is needed in this case.
		else if ((NULL != pNode->RefHi.pContext) && (key == pNode->RefHi.Key)) {
			pNode->RefHi.Key      = 0;
			pNode->RefHi.pContext = NULL;
		}
	}

	return false;
}


/////////////////////////////////////////////////////////////////////////////
//
//	SanityCheck()
//
//	Wrapper function around the code for recursively validating the contents
//	of the tree.
//
void TwoThreeInterior::SanityCheck(void)
{
	if (NULL != m_pRoot) {
		SanityCheckRec(m_pRoot);
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	SanityCheckRec()
//
//	Recurse through the tree and verify that the state of the tree meets all
//	of the requirements of an interior 2-3 tree.
//
void TwoThreeInterior::SanityCheckRec(TwoThreeInterior_t *pNode)
{
	QzAssert(NULL != pNode);

	// A leaf node cannot have any child pointers.
	if (NULL == pNode->pChild1) {
		QzAssert(NULL == pNode->pChild1);
		QzAssert(NULL == pNode->pChild2);
		QzAssert(NULL == pNode->pChild3);
	}

	// A non-leaf node must have at least two child pointers.
	else {
		QzAssert(NULL != pNode->pChild1);
		QzAssert(NULL != pNode->pChild2);

		// If there is a second key, there must also be a third child.
		if (NULL != pNode->RefHi.pContext) {
			QzAssert(pNode->RefLo.Key < pNode->RefHi.Key);
			QzAssert(NULL != pNode->pChild3);
		}

		// If the first child is a leaf, all children must be a leaf.
		if (NULL == pNode->pChild1->pChild1) {
			QzAssert(NULL == pNode->pChild2->pChild1);

			if (NULL != pNode->RefHi.pContext) {
				QzAssert(NULL == pNode->pChild3->pChild1);
			}
		}

		// If the first child is not a leaf, none of the children can be
		// leaves.
		else {
			QzAssert(NULL != pNode->pChild2->pChild1);

			if (NULL != pNode->RefHi.pContext) {
				QzAssert(NULL != pNode->pChild3->pChild1);
			}
		}

		// Recurse over all of the children.
		SanityCheckRec(pNode->pChild1);
		SanityCheckRec(pNode->pChild2);

		if (NULL != pNode->RefHi.pContext) {
			SanityCheckRec(pNode->pChild3);
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	KeyCount()
//
//	This is just a wrapper function for the recursive routine that counts
//	all keys in the tree.
//
U32 TwoThreeInterior::KeyCount(void)
{
	return KeyCountRec(m_pRoot);
}


/////////////////////////////////////////////////////////////////////////////
//
//	KeyCountRec()
//
//	Recurse through all child nodes, counting the total number of keys in the
//	tree.
//
U32 TwoThreeInterior::KeyCountRec(TwoThreeInterior_t *pNode)
{
	U32 tally = 0;

	if (NULL != pNode->RefLo.pContext) {
		tally += 1;
	}
	if (NULL != pNode->RefHi.pContext) {
		tally += 1;
	}

	if (NULL != pNode->pChild1) {
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
//	This is a sanity-checking routine.  It will figure out the depth of every
//	leaf node, then print out the resulting count.  Unless the tree has ended
//	up corrupted in some way, every leaf should be at the exact same depth.
//
void TwoThreeInterior::LeafDepth(void)
{
	U32 ary[64];
	for (U32 i = 0; i < ArraySize(ary); ++i) {
		ary[i] = 0;
	}

	U32 maxDepth = LeafDepthRec(m_pRoot, 0, ary, ArraySize(ary));

	U32 tally = 0;

	for (U32 i = 0; i < maxDepth; ++i) {
		tally += ary[i];

		printf("%3d: %5d = %5d\n", i, ary[i], tally);
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	LeafDepthRec()
//
//	Recurse through the tree to locate all leaf nodes.  This will track the
//	depth of every leaf node.  In a balanced tree, every leaf node should be
//	at exactly the same depth.  This method is used to verify that the tree
//	has not ended up unbalanced.
//
U32 TwoThreeInterior::LeafDepthRec(TwoThreeInterior_t *pNode, U32 depth, U32 ary[], U32 depthLimit)
{
	ary[depth] += 1;

	if (NULL != pNode->RefHi.pContext) {
		ary[depth] += 1;
	}

	if (NULL == pNode->pChild1) {
		return depth + 1;
	}

	if (depth >= depthLimit) {
		return depth;
	}

	U32 d1 = LeafDepthRec(pNode->pChild1, depth + 1, ary, depthLimit);
	U32 d2 = LeafDepthRec(pNode->pChild2, depth + 1, ary, depthLimit);

	U32 maxDepth = Max(d1, d2);

	if (NULL != pNode->RefHi.pContext) {
		U32 d3 = LeafDepthRec(pNode->pChild3, depth + 1, ary, depthLimit);

		maxDepth = Max(maxDepth, d3);
	}

	return maxDepth;
}


/////////////////////////////////////////////////////////////////////////////
//
//	Traverse()
//
void TwoThreeInterior::Traverse(void)
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
void TwoThreeInterior::TraverseRec(TwoThreeInterior_t *pNode, U32 &prev)
{
	QzAssert(NULL != pNode);

	if (NULL == pNode->pChild1) {
		QzAssert(prev < pNode->RefLo.Key);
		prev = pNode->RefLo.Key;
		printf("%4d", pNode->RefLo.Key);

		if (NULL != pNode->RefHi.pContext) {
			QzAssert(prev < pNode->RefHi.Key);
			prev = pNode->RefHi.Key;
			printf("%4d", pNode->RefHi.Key);
		}

		QzAssert(NULL == pNode->pChild1);
		QzAssert(NULL == pNode->pChild2);
		QzAssert(NULL == pNode->pChild3);
	}
	else {
		TraverseRec(pNode->pChild1, prev);

		QzAssert(prev < pNode->RefLo.Key);
		prev = pNode->RefLo.Key;
		printf("%4d", pNode->RefLo.Key);

		TraverseRec(pNode->pChild2, prev);

		if (NULL != pNode->pChild3) {
			QzAssert(prev < pNode->RefHi.Key);
			prev = pNode->RefHi.Key;
			printf("%4d", pNode->RefHi.Key);

			TraverseRec(pNode->pChild3, prev);
		}
	}
}






