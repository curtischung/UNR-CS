#ifndef BINARYNODE_H
#define BINARYNODE_H

#include <memory>

template<class ItemType>
class BinaryNode {
private:
	ItemType item; // Data portion
	std::shared_ptr<BinaryNode<ItemType>> leftChildPtr; // Pointer to left child
	std::shared_ptr<BinaryNode<ItemType>> rightChildPtr; // Pointer to right child
public:
	BinaryNode();
	BinaryNode(const ItemType& anItem);

	void setItem(const ItemType& anItem);
	ItemType getItem() const;

	std::shared_ptr<BinaryNode<ItemType>> getLeftChildPtr() const;
	std::shared_ptr<BinaryNode<ItemType>> getRightChildPtr() const;

	void setLeftChildPtr(std::shared_ptr<BinaryNode<ItemType>> leftPtr);
	void setRightChildPtr(std::shared_ptr<BinaryNode<ItemType>> rightPtr);

}; // end BinaryNode

template<class ItemType>
BinaryNode<ItemType>::BinaryNode():
	leftChildPtr(nullptr),
	rightChildPtr(nullptr) {}

template <class ItemType>
BinaryNode<ItemType>::BinaryNode(const ItemType & anItem) :
	item(anItem),
	leftChildPtr(nullptr),
	rightChildPtr(nullptr) {}

template <class ItemType>
ItemType BinaryNode<ItemType>::getItem() const {
	return item;
}

template <class ItemType>
std::shared_ptr<BinaryNode<ItemType>> BinaryNode<ItemType>::getLeftChildPtr() const {
	return leftChildPtr;
}
template <class ItemType>
std::shared_ptr<BinaryNode<ItemType>> BinaryNode<ItemType>::getRightChildPtr() const {
	return rightChildPtr;
}

template <class ItemType>
void BinaryNode<ItemType>::setLeftChildPtr(std::shared_ptr<BinaryNode<ItemType>> leftPtr) {
	leftChildPtr = leftPtr;
}

template <class ItemType>
void BinaryNode<ItemType>::setRightChildPtr(std::shared_ptr<BinaryNode<ItemType>> rightPtr) {
	rightChildPtr = rightPtr;
}
#endif