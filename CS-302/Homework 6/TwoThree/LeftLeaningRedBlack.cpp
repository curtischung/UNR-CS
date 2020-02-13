/////////////////////////////////////////////////////////////////////////////
//
//	File: LeftLeaningRedBlack.cpp
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


#include "QzCommon.h"
#include "LeftLeaningRedBlack.h"


#ifdef USE_MALLOC_MACRO
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// Define this symbol to configure the red-black tree as a 2-3-4 tree.
// If this is not defined, the tree is arranged as a 2-3 tree.
//
// In general, defining this symbol will reduce performance of all
// operations on the LLRB.
//
//#define USE_234_TREE


/////////////////////////////////////////////////////////////////////////////
//
//	constructor
//
LeftLeaningRedBlack::LeftLeaningRedBlack(void)
	:	m_pRoot(NULL)
{
}


/////////////////////////////////////////////////////////////////////////////
//
//	destructor
//
LeftLeaningRedBlack::~LeftLeaningRedBlack(void)
{
	Free(m_pRoot);
}


/////////////////////////////////////////////////////////////////////////////
//
//	FreeAll()
//
void LeftLeaningRedBlack::FreeAll(void)
{
	Free(m_pRoot);

	m_pRoot = NULL;
}


/////////////////////////////////////////////////////////////////////////////
//
//	Free()
//
void LeftLeaningRedBlack::Free(LLTB_t *pNode)
{
	if (NULL != pNode) {
		if (NULL != pNode->pLeft) {
			Free(pNode->pLeft);
		}
		if (NULL != pNode->pRight) {
			Free(pNode->pRight);
		}

		SafeDelete(pNode);
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	NewNode()
//
//	Note that a new node defaults to being red.
//
LLTB_t* LeftLeaningRedBlack::NewNode(void)
{
	LLTB_t *pNew = new LLTB_t;

	pNew->Ref.Key      = 0;
	pNew->Ref.pContext = NULL;
	pNew->IsRed        = true;
	pNew->pLeft        = NULL;
	pNew->pRight       = NULL;

	return pNew;
}


/////////////////////////////////////////////////////////////////////////////
//
//	LookUp()
//
//	Since a red-black tree is a binary tree, look-up operations are done
//	using iterative traversal.
//
//	This assumes that all keys stored in the tree have been assigned non-NULL
//	values.
//
//	If the key is not in the tree, this will return NULL.
//
void* LeftLeaningRedBlack::LookUp(U32 key)
{
	LLTB_t *pNode = m_pRoot;

	while (NULL != pNode) {
		if (key == pNode->Ref.Key) {
			return pNode->Ref.pContext;
		}
		else if (key < pNode->Ref.Key) {
			pNode = pNode->pLeft;
		}
		else {
			pNode = pNode->pRight;
		}
	}

	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
//
//	IsRed()
//
static inline bool IsRed(LLTB_t *pNode)
{
	return ((NULL != pNode) && pNode->IsRed);
}


/////////////////////////////////////////////////////////////////////////////
//
//	RotateLeft()
//
//	Rotate the subtree to the left:
//
//	   4            6
//	  / \          /
//	 2   6  -->   4
//	             /
//	            2
//
//	Node '6' will take on the color '4' used to have, while '4' becomes a
//	red node.
//
static LLTB_t* RotateLeft(LLTB_t *pNode)
{
	LLTB_t *pTemp = pNode->pRight;
	pNode->pRight   = pTemp->pLeft;
	pTemp->pLeft    = pNode;
	pTemp->IsRed    = pNode->IsRed;
	pNode->IsRed    = true;

	return pTemp;
}


/////////////////////////////////////////////////////////////////////////////
//
//	RotateRight()
//
//	Rotate the subtree to the right:
//
//	   4        2
//	  / \        \
//	 2   6  -->   4
//	               \
//	                6
//
//	Node '2' will take on the color '4' used to have, while '4' becomes a
//	red node.
//
static LLTB_t* RotateRight(LLTB_t *pNode)
{
	LLTB_t *pTemp = pNode->pLeft;
	pNode->pLeft    = pTemp->pRight;
	pTemp->pRight   = pNode;
	pTemp->IsRed    = pNode->IsRed;
	pNode->IsRed    = true;

	return pTemp;
}


/////////////////////////////////////////////////////////////////////////////
//
//	ColorFlip()
//
//	Flips the color of the node and both of its children from black to red
//	(or red to black).
//
//	Any time that a color flip is applied, it may cause pNode to end up
//	with an invalid color relative to its parent node.  Therefore some kind
//	of fix-up operation will be required after this function is called.
//
static void ColorFlip(LLTB_t *pNode)
{
	pNode->IsRed = !pNode->IsRed;

	if (NULL != pNode->pLeft) {
		pNode->pLeft->IsRed  = !pNode->pLeft->IsRed;
	}

	if (NULL != pNode->pRight) {
		pNode->pRight->IsRed = !pNode->pRight->IsRed;
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	Insert()
//
bool LeftLeaningRedBlack::Insert(VoidRef_t ref)
{
	m_pRoot = InsertRec(m_pRoot, ref);

	// The root node of a red-black tree must be black.
	m_pRoot->IsRed = false;

	return true;
}


/////////////////////////////////////////////////////////////////////////////
//
//	InsertRec()
//
//	Note that during rebalancing, a rotation may change the value assigned
//	to pNode.  This will change which node is the root of the subtree that
//	is being modified.  The caller will need to use the returned pointer
//	to replace whichever value was passed into InsertRec.
//
LLTB_t* LeftLeaningRedBlack::InsertRec(LLTB_t* pNode, VoidRef_t ref)
{
	// Special case for inserting a leaf.  Just return the pointer;
	// the caller will insert the new node into the parent node.
	if (NULL == pNode) {
		pNode      = NewNode();
		pNode->Ref = ref;
		return pNode;
	}

	// If we perform the color flip here, the tree is assembled as a
	// mapping of a 2-3-4 tree.
#if defined(USE_234_TREE)
	// This color flip will effectively split 4-nodes on the way down
	// the tree (since 4-nodes must be represented by a node with two
	// red children).  By performing the color flip here, the 4-nodes
	// will remain in the tree after the insertion.
	if (IsRed(pNode->pLeft) && IsRed(pNode->pRight)) {
		ColorFlip(pNode);
	}
#endif

	// Check to see if the value is already in the tree.  If so, we
	// simply replace the value of the key, since duplicate keys are
	// not allowed.
	if (ref.Key == pNode->Ref.Key) {
		pNode->Ref = ref;
	}

	// Otherwise recurse left or right depending on key value.
	//
	// Note: pLeft or pRight may be a NULL pointer before recursing.
	// This indicates that pNode is a leaf (or only has one child),
	// so the new node will be inserted using the return value.
	//
	// The other reason for pass-by-value, followed by an assignment,
	// is that the recursive call may perform a rotation, so the
	// pointer that gets passed in may end up not being the root of
	// the subtree once the recursion returns.
	//
	else {
		if (ref.Key < pNode->Ref.Key) {
			pNode->pLeft = InsertRec(pNode->pLeft, ref);
		}
		else {
			pNode->pRight = InsertRec(pNode->pRight, ref);
		}
	}

	// If necessary, apply a rotation to get the correct representation
	// in the parent node as we're backing out of the recursion.  This
	// places the tree in a state where the parent can safely apply a
	// rotation to restore the required black/red balance of the tree.

	// Fix a right-leaning red node: this will assure that a 3-node is
	// the left child.
	if (IsRed(pNode->pRight) && (false == IsRed(pNode->pLeft))) {
		pNode = RotateLeft(pNode);
	}

	// Fix two reds in a row: this will rebalance a 4-node.
	if (IsRed(pNode->pLeft) && IsRed(pNode->pLeft->pLeft)) {
		pNode = RotateRight(pNode);
	}

	// If we perform the color flip here, the tree is assembled as a
	// mapping of a 2-3 tree.
#if !defined(USE_234_TREE)
	// This color flip will effectively split 4-nodes on the way back
	// out of the tree.  By doing this here, there will be no 4-nodes
	// left in the tree after the insertion is complete.
	if (IsRed(pNode->pLeft) && IsRed(pNode->pRight)) {
		ColorFlip(pNode);
	}
#endif

	// Return the new root of the subtree that was just updated,
	// since rotations may have changed the value of this pointer.
	return pNode;
}


/////////////////////////////////////////////////////////////////////////////
//
//	MoveRedLeft()
//
//	This code assumes that either pNode or pNode->pLeft are red.
//
static LLTB_t* MoveRedLeft(LLTB_t *pNode)
{
	// If both children are black, we turn these three nodes into a
	// 4-node by applying a color flip.
	ColorFlip(pNode);

	// But we may end up with a case where pRight has a red child.
	// Apply a pair of rotations and a color flip to make pNode a
	// red node, both of its children become black nodes, and pLeft
	// becomes a 3-node.
	if ((NULL != pNode->pRight) && IsRed(pNode->pRight->pLeft)) {
		pNode->pRight = RotateRight(pNode->pRight);
		pNode         = RotateLeft(pNode);

		ColorFlip(pNode);
	}

	return pNode;
}


/////////////////////////////////////////////////////////////////////////////
//
//	MoveRedRight()
//
//	This code assumes that either pNode or pNode->Right is a red node.
//
static LLTB_t* MoveRedRight(LLTB_t *pNode)
{
	// Applying a color flip may turn pNode into a 4-node,
	// with both of its children being red.
	ColorFlip(pNode);

	// However, this may cause a situation where both of pNode's
	// children are red, along with pNode->pLeft->pLeft.  Applying a
	// rotation and a color flip will fix this special case, since
	// it makes pNode red and pNode's children black.
	if ((NULL != pNode->pLeft) && IsRed(pNode->pLeft->pLeft)) {
		pNode = RotateRight(pNode);

		ColorFlip(pNode);
	}

	return pNode;
}


/////////////////////////////////////////////////////////////////////////////
//
//	FindMin()
//
//	Find the node under pNode that contains the smallest key value.
//
static LLTB_t* FindMin(LLTB_t *pNode)
{
	while (NULL != pNode->pLeft) {
		pNode = pNode->pLeft;
	}

	return pNode;
}


/////////////////////////////////////////////////////////////////////////////
//
//	FixUp()
//
//	Apply fix-up logic during deletion.  Applying color flips and rotations
//	will preserve perfect black-link balance.  This will also fix any
//	right-leaning red nodes and eliminate 4-nodes when backing out of
//	recursive calls.
//
static LLTB_t* FixUp(LLTB_t *pNode)
{
	// Fix right-leaning red nodes.
	if (IsRed(pNode->pRight)) {
		pNode = RotateLeft(pNode);
	}

	// Detect if there is a 4-node that traverses down the left.
	// This is fixed by a right rotation, making both of the red
	// nodes the children of pNode.
	if (IsRed(pNode->pLeft) && IsRed(pNode->pLeft->pLeft)) {
		pNode = RotateRight(pNode);
	}

	// Split 4-nodes.
	if (IsRed(pNode->pLeft) && IsRed(pNode->pRight)) {
		ColorFlip(pNode);
	}

	return pNode;
}


/////////////////////////////////////////////////////////////////////////////
//
//	Delete()
//
void LeftLeaningRedBlack::Delete(const U32 key)
{
	if (NULL != m_pRoot) {
		m_pRoot = DeleteRec(m_pRoot, key);

		// Assuming we have not deleted the last node from the tree, we
		// need to force the root to be a black node to conform with the
		// the rules of a red-black tree.
		if (NULL != m_pRoot) {
			m_pRoot->IsRed = false;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	DeleteRec()
//
LLTB_t* LeftLeaningRedBlack::DeleteRec(LLTB_t *pNode, const U32 key)
{
	if (key < pNode->Ref.Key) {
		if (NULL != pNode->pLeft) {
			// If pNode and pNode->pLeft are black, we may need to
			// move pRight to become the left child if a deletion
			// would produce a red node.
			if ((false == IsRed(pNode->pLeft)) && (false == IsRed(pNode->pLeft->pLeft))) {
				pNode = MoveRedLeft(pNode);
			}

			pNode->pLeft = DeleteRec(pNode->pLeft, key);
		}
	}
	else {
		// If the left child is red, apply a rotation so we make
		// the right child red.
		if (IsRed(pNode->pLeft)) {
			pNode = RotateRight(pNode);
		}

		// Special case for deletion of a leaf node.
		// The arrangement logic of LLRBs assures that in this case,
		// pNode cannot have a left child.
		if ((key == pNode->Ref.Key) && (NULL == pNode->pRight)) {
			Free(pNode);
			return NULL;
		}

		// If we get here, we need to traverse down the right node.
		// However, if there is no right node, then the target key is
		// not in the tree, so we can break out of the recursion.
		if (NULL != pNode->pRight) {
			if ((false == IsRed(pNode->pRight)) && (false == IsRed(pNode->pRight->pLeft))) {
				pNode = MoveRedRight(pNode);
			}

			// Deletion of an internal node: We cannot delete this node
			// from the tree, so we have to find the node containing
			// the smallest key value that is larger than the key we're
			// deleting.  This other key will replace the value we're
			// deleting, then we can delete the node that previously
			// held the key/value pair we just moved.
			if (key == pNode->Ref.Key) {
				pNode->Ref    = FindMin(pNode->pRight)->Ref;
				pNode->pRight = DeleteMin(pNode->pRight);
			}
			else {
				pNode->pRight = DeleteRec(pNode->pRight, key);
			}
		}
	}

	// Fix right-leaning red nodes and eliminate 4-nodes on the way up.
	// Need to avoid allowing search operations to terminate on 4-nodes,
	// or searching may not locate intended key.
	return FixUp(pNode);
}


/////////////////////////////////////////////////////////////////////////////
//
//	DeleteMin()
//
//	Delete the bottom node on the left spine while maintaining balance.
//	To do so, we maintain the invariant that the current node or its left
//	child is red.
//
//	The only reason this is a member function instead of a static function
//	like all of the other support functions is because it needs to call
//	Free().
//
LLTB_t* LeftLeaningRedBlack::DeleteMin(LLTB_t *pNode)
{
	// If this node has no children, we're done.
	// Due to the arrangement of an LLRB tree, the node cannot have a
	// right child.
	if (NULL == pNode->pLeft) {
		Free(pNode);
		return NULL;
	}

	// If these nodes are black, we need to rearrange this subtree to
	// force the left child to be red.
	if ((false == IsRed(pNode->pLeft)) && (false == IsRed(pNode->pLeft->pLeft))) {
		pNode = MoveRedLeft(pNode);
	}

	// Continue recursing to locate the node to delete.
	pNode->pLeft = DeleteMin(pNode->pLeft);

	// Fix right-leaning red nodes and eliminate 4-nodes on the way up.
	// Need to avoid allowing search operations to terminate on 4-nodes,
	// or searching may not locate intended key.
	return FixUp(pNode);
}


/////////////////////////////////////////////////////////////////////////////
//
//	SanityCheck()
//
void LeftLeaningRedBlack::SanityCheck(void)
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
//	Verifies that the red-black tree is properly formed.
//
void LeftLeaningRedBlack::SanityCheckRec(LLTB_t *pNode, bool isParentRed, U32 blackDepth, U32 &minBlack, U32 &maxBlack)
{
	if (pNode->IsRed) {
		// The parent of a red node must be black.
		QzAssert(false == isParentRed);

	}
	else {
		++blackDepth;
	}

	// The child of a red node must be black.
	if (isParentRed) {
		QzAssert(false == pNode->IsRed);
	}

	// Only apply this test if the LLRB is arranged as a 2-3 tree.
	// Otherwise it is a 2-3-4 tree, in which case this rule does
	// not apply.
#if !defined(USE_234_TREE)
	// If the node has two children, only one of them may be red.
	// The other must be black.
	if ((NULL != pNode->pLeft) && (NULL != pNode->pRight)) {
		if (IsRed(pNode->pLeft)) {
			QzAssert(false == IsRed(pNode->pRight));
		}
		if (IsRed(pNode->pRight)) {
			QzAssert(false == IsRed(pNode->pLeft));
		}
	}
#endif

	if (NULL != pNode->pLeft) {
		// The left child must come before this node in sorting order.
		QzAssert(pNode->pLeft->Ref.Key < pNode->Ref.Key);

		SanityCheckRec(pNode->pLeft, pNode->IsRed, blackDepth, minBlack, maxBlack);
	}
	else {
		minBlack = Min(minBlack, blackDepth);
		maxBlack = Max(maxBlack, blackDepth);
	}

	if (NULL != pNode->pRight) {
		// The right child must come after this node in sorting order.
		QzAssert(pNode->pRight->Ref.Key > pNode->Ref.Key);

		SanityCheckRec(pNode->pRight, pNode->IsRed, blackDepth, minBlack, maxBlack);
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
U32 LeftLeaningRedBlack::KeyCount(void)
{
	return KeyCountRec(m_pRoot);
}


/////////////////////////////////////////////////////////////////////////////
//
//	KeyCountRec()
//
U32 LeftLeaningRedBlack::KeyCountRec(LLTB_t *pNode)
{
	U32 tally = 0;

	if (NULL != pNode) {
		tally += 1;

		if (NULL != pNode->pLeft) {
			tally += KeyCountRec(pNode->pLeft);
		}

		if (NULL != pNode->pRight) {
			tally += KeyCountRec(pNode->pRight);
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
void LeftLeaningRedBlack::LeafDepth(void)
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
U32 LeftLeaningRedBlack::LeafDepthRec(LLTB_t *pNode, U32 depth, U32 ary[], U32 depthLimit, U32 blackDepth, U32 &minBlack, U32 &maxBlack)
{
	// Increment the count of nodes at the current depth of the tree.
	ary[depth] += 1;

	// If this is a black node, increment the count of black nodes along
	// this path from the root.
	if (false == pNode->IsRed) {
		++blackDepth;
	}

	// If this is a leaf node, return the total depth of this node.
	if ((NULL == pNode->pLeft) && (NULL == pNode->pRight)) {
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

	if (NULL != pNode->pLeft) {
		d1 = LeafDepthRec(pNode->pLeft, depth + 1, ary, depthLimit, blackDepth, minBlack, maxBlack);
	}
	else {
		minBlack = Min(minBlack, blackDepth);
		maxBlack = Max(maxBlack, blackDepth);
	}

	if (NULL != pNode->pRight) {
		d2 = LeafDepthRec(pNode->pRight, depth + 1, ary, depthLimit, blackDepth, minBlack, maxBlack);
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
void LeftLeaningRedBlack::Traverse(void)
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
void LeftLeaningRedBlack::TraverseRec(LLTB_t *pNode, U32 &prev)
{
	QzAssert(NULL != pNode);

	if (NULL != pNode->pLeft) {
		TraverseRec(pNode->pLeft, prev);
	}

	QzAssert(prev < pNode->Ref.Key);
	prev = pNode->Ref.Key;
	printf("%4d", pNode->Ref.Key);

	if (NULL != pNode->pRight) {
		TraverseRec(pNode->pRight, prev);
	}
}


