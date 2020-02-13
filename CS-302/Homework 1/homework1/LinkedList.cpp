/*#include "LinkedList.h"

template<typename T>
void LinkedList<T>::push(const T value) { //
	Node<T> * ptr = new Node<T>;
	ptr -> data = value;
	ptr -> next = front; 
	front = ptr;
	std::cout << "asdasda" << std::endl;
}

template<typename T>
void LinkedList<T>::pop() {
	Node<T> * temp;
	temp = front -> next;
	delete front;
	front = temp;
}

template<typename T>
size_t LinkedList<T>::total() const {
	size_t size;
	Node<T> * ptr;
	ptr = new Node<T>;
	while(ptr != NULL) {
		ptr -> next = new Node<T>;
		ptr = ptr -> next;
		size++;
	}
	ptr = NULL;
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
	Node<T> * curr = front;
	while(curr != NULL) {
		os << curr -> info() << std::endl;
		curr = curr -> next;
	}
}

template<typename T>
std::ostream & operator<<(std::ostream & os, const LinkedList<T> & linkedList) {
	linkedList.serialize(os);
	return os;
}*/