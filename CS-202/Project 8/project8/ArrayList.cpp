#include "ArrayList.h"

ArrayList::ArrayList(): m_array(NULL) {}

void ArrayList::resize(size_t count) {
	if(m_array != NULL) {
		delete [] m_array;
		m_size = 0;
	}
	m_array = new DataType[count-1];
	m_size = count;
	m_maxsize = m_size;
}

ArrayList::ArrayList(size_t count, const DataType & value)
	:m_size(count), m_maxsize(count) {
		resize(count);
		for(size_t i = 0; i < count; i++) {
			m_array[i] = value;
		}
	}
ArrayList::ArrayList(const ArrayList & other) 
	: m_size(other.m_size), m_maxsize(other.m_maxsize) {
		resize(m_size);
		m_array = other.m_array;
}
ArrayList & ArrayList::operator= (const ArrayList & rhs) {
	if(this != &rhs) {
			m_array = rhs.m_array;
	}
	return *this;
}

DataType * ArrayList::front() {
	if(m_array != NULL) {
		return &m_array[0];
	}
	return NULL;
}

DataType * ArrayList::back() {
	if(m_array != NULL) {
		return &m_array[m_size - 1];
	}
	return NULL;
}

/*DataType * ArrayList::find(const DataType & target, DataType * & previous) {
	DataType * temp = previous;
	temp = new DataType[m_size];
	for(size_t i = 0; i < m_size; i++) {
		if(temp[i] != NULL) {
			temp = previous;
		}
	}
}*/

size_t ArrayList::size() const {
	return m_size;
}									
bool ArrayList::empty() const {
	if(m_array == NULL) {
		return true;
	}
	return false;
}