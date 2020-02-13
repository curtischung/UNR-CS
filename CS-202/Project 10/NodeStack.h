#ifndef NODESTACK_H
#define NODESTACK_H

#include "DataType.h"

template <class T> class NodeStack;
template <class T>
class Node {
	friend class NodeStack<T>;
public:
	Node(): 
		m_next(NULL) 
	{
	}
	Node(const T & data, Node<T> * next = NULL) :
		m_next(next),
		m_data(data)
	{
	}
	T & data() {
		return m_data;
	}
	const T & data() const {
		return m_data;
	}

private:
	Node * m_next;
	T m_data;
 };

template <class T> class NodeStack;
template <class T> std::ostream & operator<<(std::ostream & os, const NodeStack<T> & nodeStack);
template <class T>
class NodeStack {
 	friend std::ostream & operator<< <T> (std::ostream & os,
 									const NodeStack <T> & nodeStack);
public:
	NodeStack();
	NodeStack(size_t count, const T & value);
	NodeStack(const T * other);
	~NodeStack();

	NodeStack & operator=(const NodeStack & rhs);

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
	Node<T> * m_top;

 };
//default constructor
template <class T>
NodeStack<T> ::NodeStack(){
	m_top = NULL;
}

//parametized constructor
template <class T>
NodeStack<T>::NodeStack(size_t count, const T & value) {
	Node<T> * curr = new Node<T>; //create a new node from node pointer
	m_top = curr; //sets node pointer to m_top
	curr -> m_data = value; // sets m_data of the curr node pointer to the value
	for(size_t i = 0; i < count; i++ ) {
		curr -> m_next = new Node<T>; // creates a new node from the next value of the node
		curr = curr-> m_next;
		curr -> m_data = value;
	}
}

//copy constructor
template <class T>
NodeStack<T>::NodeStack(const T * other) {
	Node<T> * curr = m_top; //creates a node pointer and sets it to m_top
	Node<T> * currObj = other.m_top;  //creates node pointer of other class and sets it to the other's m_top
	while(currObj != NULL) { //while the currObj pointer isnt empty
		if(currObj -> m_next != NULL) { // if there isnt enough space to copy currObj to curr
			curr -> m_next = new Node<T>; //creates new Node for the new memory
		}
		curr -> m_data = currObj -> m_data; //sets data of both pointer equal to one another
		currObj = currObj -> m_next; //increments through the pointers
		curr = curr -> m_next;
	}
}

//assignment operator, which functions the same as copy constructor
template <class T>
NodeStack<T> & NodeStack<T>::operator=(const NodeStack & rhs) {
	if(this != &rhs) {
		Node<T> * curr = m_top;
		Node<T> * currObj = rhs.m_top;
		while(currObj != NULL) {
			if(currObj -> m_next != NULL) {
				curr -> m_next = new Node<T>;
			}
			curr -> m_data = currObj -> m_data;
			currObj = currObj -> m_next;
			curr = curr -> m_next;
		}
	}
	return *this;
}

template <class T>
NodeStack<T>::~NodeStack() {
	Node<T> * curr = new Node<T>;
	while(curr != NULL) {
		Node<T> * temp = curr -> m_next;
		delete curr;
		curr = temp;
	}
}

//top function
template <class T>
T & NodeStack<T>::top() {
	return m_top -> m_data;
}
//const top function
template <class T>
const T & NodeStack<T>::top() const{
	return m_top -> m_data;
}

//push function
template <class T>
void NodeStack<T>::push(const T & value) {
	m_top -> m_data = value;
}

//pop function
template <class T>
void NodeStack<T>::pop() {
	Node<T> * temp;
	temp = m_top -> m_next;
	delete m_top;
	m_top = temp;
}

//serialize
template <class T>
void NodeStack<T>::serialize(std::ostream & os) const {
	Node<T> * curr;
	curr = m_top; //sets node pointer to m_top
	while(curr != NULL) {
		os << curr -> m_data << std::endl; //writes data from pointer to os
		curr = curr -> m_next; //iterates
	}
}

//size function
template<class T>
size_t NodeStack<T>::size() const {
	size_t size = 0;
	Node<T> * ptr; //Node pointer
	ptr = m_top; //sets ptr to m_top
	while(ptr != NULL) { //while loop to iterate through the node
		ptr = ptr -> m_next; //iterates when it isn't equal to NULL
		size++; //increments size tracker
	}
	return size;
}

//empty function
template <class T>
bool NodeStack<T>::empty() const {
	if(m_top == NULL) {
		return true;
	} else {
		return false;
	}
}

//full function, which has no purpose
template <class T>
bool NodeStack<T>::full() const {
	return false;
}

//clear function, pop's every data member until all are NULL
template <class T>
void NodeStack<T>::clear() {
	while(m_top != NULL) {
		pop();
	}
	std::cout << "Queue has been cleared." << std::endl;
}

//friend ostream, which calls serialize to write to the os and returns it
template <class T>
std::ostream & operator<<(std::ostream & os, const NodeStack<T> & nodeStack) {
	nodeStack.serialize(os);
	return os;
}
#endif
