#ifndef ARRAYSTACK_H
#define ARRAYSTACK_H

#include "DataType.h"
#include <iostream>
#include <cstring>

const size_t MAX_STACKSIZE = 1000;
template <class T> class ArrayStack;
template <class T> std::ostream & operator<<(std::ostream & os, const ArrayStack<T> & arrayStack);
template <class T>
class ArrayStack {

friend std::ostream & operator<< <T> (std::ostream & os
                                ,const ArrayStack <T> & arrayStack);

public:
  ArrayStack();
  ArrayStack(size_t count, const T & value);
  ArrayStack(const T & other);
  ~ArrayStack(){}

  ArrayStack & operator=(const T & rhs);

  T & top();
  const T & top() const;

  void push(const T & value);
  void pop();

  size_t size() const;
  bool empty() const;
  bool full() const;
  void clear();
  void serialize(std::ostream & os) const;

private:
  T m_container[MAX_STACKSIZE];
  size_t m_top;
};

template <class T>
ArrayStack<T>::ArrayStack(): m_top(0) {}

//template parametized constructor
template <class T>
ArrayStack<T>::ArrayStack(size_t count, const T & value) {
  m_top = count; //intializes top as the count size of the array
  for(size_t i = 0; i < count; i++) {
    m_container[i] = value; //loops and sets each element of array count to value
  }
}

//templated copy constructor
template <class T>
ArrayStack<T>::ArrayStack(const T & other) { //template a class object
  m_top = other.m_top; //sets m_top equal to other m_top
  for(size_t i = 0; i < other.m_top; i++) {
    m_container[i] = other.m_container[i]; // loops and sets each elements of the array equal 
  }
}

//templated assignment operator, functions the same as copy
template <class T>
ArrayStack<T> & ArrayStack<T>::operator=(const T & rhs) {
  if(this != rhs) {
    m_top = rhs.m_top;
    for(size_t i = 0; i < rhs.m_top; i++) {
      m_container[i] = rhs.m_container[i];
    }
  }
  return *this;
}

//push function
template <class T>
void ArrayStack<T>::push(const T & value) {
  if(m_top < MAX_STACKSIZE) { //checks if m_top is larger than max array size
    m_top = m_top + 1; //increments top by 1
    m_container[m_top] = value; //pushes new value to new m_top
  }
} 

//pop function
template <class T>
void ArrayStack<T>::pop() {
  if(m_top != -1) { //checks if array is empty
    m_top = m_top - 1; //if it isn't, pops last element
  }  
}

//templated serialize
template<class T>
void ArrayStack<T>::serialize(std::ostream & os) const {
    for(size_t i = 0; i < m_top; i++ ) {
      os << m_container[i]; //iterates through array and writes it to the output stream
    }
  }

//templated ostream friend function
template <class T>
std::ostream & operator<<(std::ostream & os, const ArrayStack <T> & arrayStack) {
  arrayStack.serialize(os); //calls serialize to write to ostream and returns it
  return os;
}

//top function
template <class T>
T & ArrayStack<T>::top() {
  return m_container[m_top]; //returns top element of array
}
//const top function
template <class T>
const T & ArrayStack<T>::top() const {
  return m_container[m_top];
}

//size, which returns the size of array
template <class T>
size_t ArrayStack<T>::size() const {
  return m_top;
}

//checks if array is empty
template <class T>
bool ArrayStack<T>::empty() const {
  if(m_top == 0) {
    return true; //if empty, returns true
  } else {
    return false; //if not, returns flase
  }
}

//checks if array is full
template <class T> 
bool ArrayStack<T>::full() const {
  if(m_top != MAX_STACKSIZE) {
    return false; //if array isn't full, return false
  } else {
    return true; //if it is full, returns true
  }
}

template <class T>
void ArrayStack<T>::clear() {
  m_top = 0;
  std::cout << "Queue has been cleared." << std::endl;
}


#endif