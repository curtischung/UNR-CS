//Homework 1, Linked List 
#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <iostream>

template <typename T> class LinkedList; //forward declaration of class LinkedList
template <typename T> //template typename T
class Node {
	friend class LinkedList<T>;
private:
	T data; //templated data, designed to store all number values
	Node<T> * next; //Node pointer, which designed to point on next value
public:
	Node():next(NULL) {} //default constructor
	Node(const T & a, Node<T> * b = NULL): //parametized constructor
		data(a), next(b) {}
	Node(const Node<T> & other): //copy constructor
		data(other,data), next(other.next) {}
	T & info() { //returns data in node
		return data;
	}
	const T & info() const{ //returns const data in node
		return data;
	}
};
template <typename T> class LinkedList;
template <typename T> std::ostream & operator<<(std::ostream & os, const LinkedList<T> & linkedList);
template<typename T>
class LinkedList{
  friend std::ostream & operator<< <>(std::ostream & os, const LinkedList<T> & linkedList);

public:
	LinkedList(): front(NULL), back(NULL) {} 
	~LinkedList(); 

	void push(const T value);
	void pop(); 
	size_t total() const;
	void serialize(std::ostream & os) const; 
private:
	Node<T> * front;
	Node<T> * back;
}; 

//start of class delcarations
template<typename T>
void LinkedList<T>::push(const T value) {  //push(add function)
	Node<T> * ptr = new Node<T>; //creates new node
	ptr -> data = value; //sets the data of pointer equal to value passed through
	ptr -> next = front; //sets the next thing equal to the header, which moves the front of the list to the 2nd value
	front = ptr; //front is now the node pointer's front(head) value
}

template<typename T>
void LinkedList<T>::pop() { //pop(delete function)
	if(total() == 0) { //checks if the list is empty
		std::cout << "list empty" << std::endl;
	} else {
		Node<T> * temp; //create a node pointer
		temp = front -> next; //sets the temp pointer to the next value in list
		delete front; //deletes first value
		front = temp; //sets pointer of 2nd value to the first value
	}
}


template<typename T>
size_t LinkedList<T>::total() const { //total function
	size_t size = 0; //size_t variable, which will be returned as the total # of items
	Node<T> * ptr; //node pointer
	ptr = front; //sets the node pointer equal to the first value of thelist
	while(ptr != NULL) { //ensures to go through the whole list until the end
		ptr = ptr -> next; //increments through the list
		size++; //increments size
	}
	ptr = NULL; //ensures the pointer knows where the end of the list is
	return size; 
}

template <typename T>
LinkedList<T>::~LinkedList() 	{
	Node<T> * curr = front; //Creates a node pointer pointing to the front value
	while(curr != NULL) { //checks until no memory is stored
		Node<T> * temp = curr -> next; // creates a temporary pointer, storing the value of the next part of the data
		delete curr; //deletes the current data
		curr = temp; //sets the current pointer equal to the next data
	}
}

template<typename T>
void LinkedList<T>::serialize(std::ostream & os) const {
	Node<T> * curr = front; //node pointer pointing to the first element of the node
	while(curr != NULL) { //checks the list isnt empty
		os << curr -> info() << std::endl; //writes the data of the node to the output stream
		curr = curr -> next; //points the node pointer to point to the next item on the list
	}
}

template<typename T>
std::ostream & operator<<(std::ostream & os, const LinkedList<T> & linkedList) {
	linkedList.serialize(os);//calls serialize function to write an output stream
	return os; //returns stream
}

#endif