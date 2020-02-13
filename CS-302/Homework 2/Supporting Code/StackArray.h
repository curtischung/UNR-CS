//--------------------------------------------------------------------
//
//  StackLinked.h
// 
//  Class declaration for the linked implementation of the Stack ADT
//
//--------------------------------------------------------------------

#ifndef STACKARRAY_H
#define STACKARRAY_H

#include <stdexcept>
#include <iostream>

using namespace std;

#include "Stack.h"

template <typename DataType> class StackArray;
template <typename DataType> std::ostream & operator<<(std::ostream & os, const StackArray<DataType> & stackArray);
template <typename DataType>
class StackArray : public Stack<DataType> {
friend std::ostream & operator<< <>(std::ostream & os, const StackArray<DataType> & stackArray);
  public:
    StackArray(int maxNumber = Stack<DataType>::MAX_STACK_SIZE);
    StackArray(const StackArray & other);
    StackArray& operator=(const StackArray & rhs);
    ~StackArray() { clear();};

    void push(const DataType& newDataItem) throw (logic_error);
    DataType pop() throw (logic_error);

    void clear();

    bool isEmpty() const;
    bool isFull() const;

    void showStructure() const;

    void serialize(std::ostream & os) const;
  private:
    int maxSize;
    int top;
    DataType* dataItems;
};

//class implementations

template <typename DataType>
StackArray<DataType>::StackArray(int maxNumber)
    : maxSize(maxNumber) {
}

template <typename DataType>
StackArray<DataType>::StackArray(const StackArray & other)
: maxSize(other.maxSize), top(other.top) {
    for(int i = 0; i < other.maxSize; i++) {
        dataItems = other.dataItems;
    }
}

template <typename DataType>
StackArray<DataType>& StackArray<DataType>::operator=(const StackArray & rhs) {
    if(*this != rhs) {
        maxSize = rhs.maxSize;
        top = rhs.top;
        for(int i = 0; i < rhs.maxSize; i++) {
            dataItems[i] = rhs.dataItems[i];
        }
    }
    return *this;
}

template <typename DataType>
bool StackArray<DataType>::isEmpty() const {
    if(top == 0) {
        return true;
    } else{
        return false;
    }
}

template <typename DataType>
bool StackArray<DataType>::isFull() const {
    if(top == maxSize) {
        return true;
    } else {
        return false;
    }
}

template <typename DataType>
void StackArray<DataType>::push(const DataType& newDataItem) throw (logic_error) {
    if(!isEmpty() && top < maxSize) {
        top += 1;
        dataItems[top] = newDataItem;
    }
}

template<typename DataType>
DataType StackArray<DataType>::pop() throw (logic_error) {
    if(!isEmpty()) {
        DataType delValue;
        delValue = dataItems[top];
        top -= 1;
        return delValue;
    }  else {
        std::cout << "Stack is Empty" << std::endl;
        return 0;
    }
}

template<typename DataType>
void StackArray<DataType>::clear() {
    top = 0;
}

template<typename DataType>
void StackArray<DataType>::serialize(std::ostream & os) const {
    for(size_t i = 0; i < top; i++ ) {
      os << dataItems[i]; //iterates through array and writes it to the output stream
    }
  }

template<typename DataType>
void StackArray<DataType>::showStructure() const {
    for(int i = 0; i < maxSize; i++) {
        if(isEmpty()) {
            std::cout << "S0;tack is empty" << std::endl;
        } else {
            std::cout << dataItems[i] << endl;
        }
    }
}
template<typename DataType>
std::ostream & operator<<(std::ostream & os, const StackArray<DataType> & stackArray) {
    stackArray.serialize(os);
    return os;
}

#endif		//#ifndef STACKARRAY_H
