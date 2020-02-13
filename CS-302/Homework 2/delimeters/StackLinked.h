//--------------------------------------------------------------------
//
//  StackArray.h
// 
//  Class declaration for the array implementation of the Stack ADT
//
//--------------------------------------------------------------------


#ifndef STACKARRAY_H
#define STACKARRAY_H

#include <stdexcept>
#include <iostream>

using namespace std;

#include "Stack.h"

template <typename DataType>
class StackLinked : public Stack<DataType> {

  public:

    StackLinked(int maxNumber = Stack<DataType>::MAX_STACK_SIZE);
    StackLinked(const StackLinked & other);
    StackLinked& operator=(const StackLinked & rhs);
    ~StackLinked();

    void push(const DataType& newDataItem) throw (logic_error);
    DataType pop() throw (logic_error);

    void clear();

    bool isEmpty() const;
    bool isFull() const;

    void showStructure() const;

  private:

    class StackNode {
      public:
    StackNode(const DataType& nodeData, StackNode* nextPtr);

    DataType dataItem;
    StackNode* next;
    };

    StackNode* top;
};

template <typename DataType> //constructor
StackLinked<DataType>::StackLinked(int maxNumber)
:top(nullptr) { } //sets top to null ptr

template<class DataType> //copy constructor
StackLinked<DataType>::StackLinked(const StackLinked & other) {
    StackNode * curr = top; //sets node to top 
    StackNode * currOther = other.top; //sets another node to the other top

    while(currOther) { //while the other node exists

        if(!top) {
            top = curr; //set top equal to the curr pointer
        }

        curr -> dataItem = currOther -> dataItem; //sets data of both pointer equal to one another
        currOther = currOther -> next; //increments through the pointers
        curr = curr -> next;
    }
}

template<class DataType>
StackLinked<DataType>& StackLinked<DataType>::operator=(const StackLinked & rhs) { //assignment operator
    if (this != rhs) { //checks if rhs is equal to the current class object
        top = rhs.top; //sets the top equal to the rhs top
    }
    return *this; //returns the class object
}

template<class DataType>
StackLinked<DataType>::~StackLinked() { //destructor
    clear(); //clears all data
}

template<class DataType>
void StackLinked<DataType>::push(const DataType & newDataItem) throw(logic_error){
    top = new StackNode(newDataItem, top); //creates a new node to push data to the new top
}

template<class DataType>
DataType StackLinked<DataType>::pop() throw(logic_error) {
    if (isEmpty()) { //if stack is empty check
        throw logic_error("Stack is empty, cannot pop"); //cannot pop
    }
    DataType temp = top->dataItem; //creates a datatype temp and sets it equal to the top data
    top = top->next; //moves the top to the next itemon stack

    return temp;
}

template<class DataType>
void StackLinked<DataType>::clear() {
    StackNode *curr = top; //node pointer to the top
    StackNode *prev = nullptr; //creates a previous pointer to store the data wanting to delete

    while (curr != nullptr) {
        prev = curr;//sets the previous to the curr data
        curr = curr->next; //moves through the data

        delete[] prev; //deletes previous,while still storing the data in the curr
        top = nullptr; //sets to the top
    }
}

template<class DataType>
bool StackLinked<DataType>::isEmpty() const {
    return top == nullptr; //if top data is null, then it is empty
}

template<class DataType>
bool StackLinked<DataType>::isFull() const {
    return false; //linkedlist should never be full
}

template<class DataType>
void StackLinked<DataType>::showStructure() const {
    if (isEmpty()) {  //if the stack is empty
        std::cout << "Empty Stack"; //it returns as empty
    } else {
        StackNode *curr = top; //creates node and points to the top
        while (curr != nullptr) { //while the stack isnt empty
            std::cout << curr-> dataItem << " "; //writes to the termiinal
            curr = curr-> next; //points the node to the next item
        }
    }
    std::cout << std::endl;
}

template<class DataType>
StackLinked<DataType>::StackNode::StackNode(const DataType &nodeData, StackNode *nextPtr) :
        dataItem(nodeData), next(nextPtr) {

}


#endif      //#ifndef STACKARRAY_H