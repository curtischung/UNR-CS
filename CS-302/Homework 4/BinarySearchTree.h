#ifndef BINARYSEARCHTREE_H
#define BINARYSEARCHTREE_H

#include "BinaryNode.h"
#include <memory>
#include <iostream>
#include <algorithm>

template <class ItemType>
class BinarySearchTree {
private:

	std::shared_ptr<BinaryNode<ItemType>> rootPtr;

protected:
	std::shared_ptr<BinaryNode<ItemType>> placeNode(std::shared_ptr<BinaryNode<ItemType>> subTreePtr,
                  									std::shared_ptr<BinaryNode<ItemType>> newNodePtr);

	void preorderHelper (std::shared_ptr<BinaryNode<ItemType>> subTreePtr) const;
	void inorderHelper (std::shared_ptr<BinaryNode<ItemType>> subTreePtr) const;
	void postorderHelper (std::shared_ptr<BinaryNode<ItemType>> subTreePtr) const;
	int heightHelper(std::shared_ptr<BinaryNode<ItemType>> subTreePtr);

public:
    BinarySearchTree();

    int getHeight();
    bool isEmpty() const;

    bool add(const ItemType& newEntry);

    bool remove(const ItemType& target);

	void preorderTraverse() const;
	void inorderTraverse() const;
	void postorderTraverse() const;
};

template <class ItemType>
BinarySearchTree<ItemType>::BinarySearchTree():
	rootPtr(nullptr) {} //default constructor

//isEmpty()
template <class ItemType>
bool BinarySearchTree<ItemType>::isEmpty() const {
	return rootPtr == nullptr; //returns if the rootptr is empty
}

//placeNode
template <class ItemType>
std::shared_ptr<BinaryNode<ItemType>> BinarySearchTree<ItemType>::placeNode(std::shared_ptr<BinaryNode<ItemType>> subTreePtr,
                  								std::shared_ptr<BinaryNode<ItemType>> newNodePtr)
{
	std::shared_ptr<BinaryNode<ItemType>> temp;
	if(subTreePtr == nullptr) {
		return newNodePtr; //if subtreeptr is empty, return 
	} else 
		if (newNodePtr -> getItem() < subTreePtr -> getItem()) { //if the item is newnode is smaller than the subtree value
			temp = placeNode(subTreePtr -> getLeftChildPtr(), newNodePtr); //recursive call
			subTreePtr -> setLeftChildPtr(temp); //set the left child pointer to the value at the node of temp
	} else {
			temp = placeNode(subTreePtr -> getRightChildPtr(), newNodePtr);
			subTreePtr -> setRightChildPtr(temp);
	}
	return subTreePtr;
}

//add function
template <class ItemType>
bool BinarySearchTree<ItemType>::add(const ItemType& newEntry) {
	auto newPtr = std::make_shared<BinaryNode<ItemType>> (newEntry);
	rootPtr = placeNode(rootPtr, newPtr);
	return true;
}

/////////////////////////////////////
////////// Helper Functions /////////
////////////////////////////////////

// Each helper function was made for all the order prints
// as well as grant access to those function to access and edit rootptr

//preOrderHelper function
template<class ItemType>
void BinarySearchTree<ItemType>::preorderHelper (std::shared_ptr<BinaryNode<ItemType>> subTreePtr) const{
	if(subTreePtr == nullptr) { //only call if makes sense
		return;
	}

	std::cout << subTreePtr -> getItem() << " ";
	preorderHelper(subTreePtr -> getLeftChildPtr());
	preorderHelper(subTreePtr -> getRightChildPtr());
}

//inorderHelper function
template<class ItemType>
void BinarySearchTree<ItemType>::inorderHelper (std::shared_ptr<BinaryNode<ItemType>> subTreePtr) const{
	if(subTreePtr == nullptr) { //only call if makes sense
		return;
	}

	inorderHelper(subTreePtr -> getLeftChildPtr());
	std::cout << subTreePtr -> getItem() << " ";
	inorderHelper(subTreePtr -> getRightChildPtr());
}

//postorderHelper
template<class ItemType>
void BinarySearchTree<ItemType>::postorderHelper (std::shared_ptr<BinaryNode<ItemType>> subTreePtr) const{
	if(subTreePtr == nullptr) {
		return;
	}

	postorderHelper(subTreePtr -> getLeftChildPtr());
	postorderHelper(subTreePtr -> getRightChildPtr());
	std::cout << subTreePtr -> getItem() << " ";
}

//traversal function, calling the helper function in order to access the private vriables and edit rootptr
template <class ItemType>
void BinarySearchTree<ItemType>::preorderTraverse() const{
	preorderHelper(rootPtr);
}

//
template <class ItemType>
void BinarySearchTree<ItemType>::inorderTraverse() const{
	inorderHelper(rootPtr);
}

template <class ItemType>
void BinarySearchTree<ItemType>::postorderTraverse() const{
	postorderHelper(rootPtr);
}


template <class ItemType>
int BinarySearchTree<ItemType>::heightHelper(std::shared_ptr<BinaryNode<ItemType>> subTreePtr) {
	int lheight, rheight;

	if(subTreePtr == nullptr) {
		return 0; //only do if makes sense
	} else {
		lheight = heightHelper(subTreePtr -> getLeftChildPtr()); //recursive call to reach all the way to the left
		rheight = heightHelper(subTreePtr -> getRightChildPtr()); //call to reach to the right
		return std::max(lheight, rheight) + 1; //return the counted calls by 1
	}
}	

template <class ItemType>
int BinarySearchTree<ItemType>::getHeight() {
	return heightHelper(rootPtr);
}
#endif