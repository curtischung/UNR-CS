#include "ArrayQueue.h"

ArrayQueue::ArrayQueue() : m_front(0), m_back(0) {}
ArrayQueue::ArrayQueue(size_t count, const DataType & value) {
	m_front = 0; //declares first value of array
	m_back = count - 1; //puts m_back to the back of the size given
	m_size = count;
	for(size_t i = 0; i < count; i++) {
		m_array[i] = value; // sets the value of the array = to the value passed
	}
}

ArrayQueue::ArrayQueue(const ArrayQueue & other) {
	m_front = other.m_front; //sets m_front to m_front of other class object
	m_back = other.m_back; //sets m_back to m_back of other class object
	m_size = other.m_size;

	for(size_t i = 0; i < other.m_size; i++) { //itterates from new size of copied class m_size
		m_array[i] = other.m_array[i]; // sets each value = to the other class object value in m_array
	}

}

ArrayQueue & ArrayQueue::operator=(const ArrayQueue & rhs) {
	if(this != &rhs) {
		this -> m_front = rhs.m_front; //sets m_front and m_back to the rhs's values.
		this -> m_back = rhs.m_back;
		this -> m_size = rhs.m_size;

		for(size_t i = 0; i < rhs.m_size; i++) {
			m_array[i] = rhs.m_array[i]; //sets m_array equal to m_array of rhs
		}
	}
	return *this;
}

ArrayQueue::~ArrayQueue() {}

DataType & ArrayQueue::front() {
		return m_array[m_front];
}
const DataType & ArrayQueue::front() const {
	return m_array[m_front];
}

DataType & ArrayQueue::back() {
	return m_array[m_back];
}
const DataType & ArrayQueue::back() const {
	return m_array[m_back];
}

void ArrayQueue::push (const DataType & value) {
	if(m_size < ARRAY_MAX) { //checks if m_size exceeds the max array size
		m_back = (m_back + 1) % 1001;
		m_array[m_back] = value; //pushes the last value back by 1 and sets it equal to value
		 // increments where m_back is in the array by 1
	}
	m_size += 1;
}

void ArrayQueue::pop() {
	if (m_size != 0) { //checks if m_size makes m_front and m_back equal to one another, in which the only instance is if the size of array is 1
		m_front = (m_front + 1) % 1001; // moves m_front up by 1 in the array
	}
	m_size -= 1; //decrements the size of array by 1
}

size_t ArrayQueue::size() const { //returns size of array
	return m_size;
}

bool ArrayQueue::empty() const { // if size is 0, or if the array is empty, returns true
	if(m_size == 0) {
		return true;
	}
	return false; 
}

bool ArrayQueue::full() const { //if size is full, or equal to ARRAY_MAX, returns true
	if(m_size == ARRAY_MAX) {
		return true;
	}
	return false;
}

void ArrayQueue::clear() { //sets an empty array
	m_front = 0;
	m_back = 0;
	m_size = 0;
	std::cout << "Queue has been cleared.\n" << std::endl;
}

void ArrayQueue::serialize(std::ostream & os) const { //writes m_array at each segement to the outputstream
	size_t j = m_front;
	for(size_t i = 0; i < m_size ; i++) {
		os << m_array[j];
		j++;
	}
}

std::ostream & operator<<(std::ostream & os, const ArrayQueue & arrayQueue) {
	arrayQueue.serialize(os);
	return os;
}