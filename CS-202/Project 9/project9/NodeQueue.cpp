#include "NodeQueue.h"


NodeQueue::NodeQueue():m_front(0), m_back(0) {}
NodeQueue::NodeQueue(size_t size, const DataType & value) { //parametized constructor
	Node * curr; //Node pointer
	curr = new Node; //creates new node
	m_front = curr; //sets node pointer = to m_front
	curr -> m_data = value; //sets m_data of ndoe pointer equal to the value
	for(size_t i = 0; i < size; i++ ) {
		curr -> m_next = new Node; // creates a new node from the next value of the node
		curr = curr-> m_next;
		curr -> m_data = value;
	}
	m_back = curr; // sets last value of curr = m_back
}

NodeQueue::NodeQueue(const NodeQueue & other) { //copy constructor
	m_front = new Node;
	Node * curr = m_front; // creates a pointers and sets it to the front
	Node * currObj = other.m_front; // creates a pointer and sets it to the other front
	while(currObj != NULL) {
		if(currObj -> m_next != NULL) {
			curr -> m_next = new Node; //creates a new node from the next element
		}
			curr -> m_data = currObj -> m_data;
			currObj = currObj -> m_next; //increments through currObj to the next
			curr = curr -> m_next; //increments through curr to the next
	}
	m_back = curr; //sets m_back to the curr after everything has been done
}

NodeQueue::~NodeQueue() 	{
	Node * curr = m_front;
	while(curr != NULL) {
		Node * temp = curr -> m_next;
		delete curr;
		curr = temp;
	}
}

NodeQueue & NodeQueue::operator= (const NodeQueue & rhs) { //similar to copy constructo, but uses rhs class object
	if(this != &rhs) {
		m_front = new Node;
		Node * curr = m_front; 
		Node * currObj = rhs.m_front;
		while(currObj != NULL) {
			if(currObj -> m_next != NULL) {
				curr -> m_next = new Node;
			}
			curr -> m_data = currObj -> m_data;
			currObj = currObj -> m_next;
			curr = curr -> m_next;
		}
		m_back = curr;
	}
	return *this;
}

DataType & NodeQueue::front() { //returns m_data at m_front
	return m_front -> m_data;
}

const DataType & NodeQueue::front() const {
	return m_front -> m_data;
}

DataType & NodeQueue::back() { //returns m_data at m_back
	return m_back -> m_data;
}

const DataType & NodeQueue::back() const {
	return m_back -> m_data;
}

void NodeQueue::push (const DataType & value) { //
	m_back += 1;
	m_back -> m_data = value;
}

void NodeQueue::pop() {
	Node * temp;
	temp = m_front -> m_next;
	delete m_front;
	m_front = temp;
}

size_t NodeQueue::size() const {
	size_t size;
	Node * ptr;
	ptr = new Node;
	while(ptr != NULL) {
		ptr -> m_next = new Node;
		ptr = ptr -> m_next;
		size++;
	}
	ptr = NULL;
	return size;
}

bool NodeQueue::empty() const {
	if(m_front == NULL) {
		return true;
	}
	return false;
}

bool NodeQueue::full() const {
	return false;
}

void NodeQueue::clear() {
	while(m_front != NULL) {
		pop();
	}
}

void NodeQueue::serialize(std::ostream & os) const {
	Node * curr = m_front;
	curr = m_front;
	while(curr != NULL) {
		os << curr -> m_data << std::endl;
		curr = curr -> m_next;
	}
}

std::ostream & operator<<(std::ostream & os, const NodeQueue & nodeQueue) {
	nodeQueue.serialize(os);
	return os;
}