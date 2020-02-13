#include "MyString.h"

//default contructor
MyString::MyString(){
	m_size = 0; 
	m_buffer = NULL;
}

//buffer deallocate private function, which deletes all memory to m_buffer
void MyString::buffer_deallocate() {
	if(m_buffer != NULL) { //if m_buffer has memory
		delete[] m_buffer; //deallocates memory
	}
	m_size = 0; //resets size of array
}			

//buffer allocate function, which allocates a new memory size to the array			    
void MyString::buffer_allocate(size_t size) {
	m_buffer = new char[size]; //creates a dynamic memory allocation for the array of char size
	m_size = size; //updates m_size
}

//parametized constructor
MyString::MyString(const char * str){
	m_size = strlen(str) + 1; //sets m_size to the length of the string + null charatcer
	buffer_allocate(m_size); //allocates the new size
	strcpy(m_buffer, str); //copies the new string to m_buffer
}

//copy constructor
MyString::MyString(const MyString & other_myStr) {
	buffer_allocate(other_myStr.m_size); //calls buffer_allocate and allocates a new size
}

//destructor
MyString::~MyString() {
}						                  

size_t MyString::size() const{
	return m_size;
}	

size_t MyString::length() const {
	return strlen(m_buffer);
}					              
const char * MyString::c_str() const {
	char * ch_arr; 
	ch_arr = m_buffer;
	return ch_arr;
}					          
    
bool MyString::operator== (const MyString & other_myStr) const {
	if (!strcmp(this -> m_buffer, other_myStr.m_buffer)) { //if strcompare succeeds, returns true
		return true;
	} else {
		return false; // returns false if strcompare fails
	}
}
MyString & MyString::operator= (const MyString & other_myStr) {
	if(this != &other_myStr) { //checks if this is the same as the rhs object
		buffer_allocate(other_myStr.m_size); //allocates a new size value 
		strcpy(m_buffer, other_myStr.m_buffer); //copies rhs m_buffer to m_buffer
		m_size = other_myStr.m_size; //sets a new size
	}
	return *this;

}
MyString MyString::operator+ (const MyString & other_myStr) const {
	strcat(this -> m_buffer, other_myStr.m_buffer); //concats m_buffer to the rhs object m_buffer
	return *this;
}
char & MyString::operator[] (size_t index) {
	return m_buffer[index]; 
}					    
const char & MyString::operator[] (size_t index) const {
	return m_buffer[index];
}

std::ostream & operator<<(std::ostream & os, const MyString & myStr) {
	os << myStr.m_buffer << endl;
}


