#ifndef Agency_h
#include <iostream>

class Agency {
public:
	Agency();
	Agency(char * name, int zipcode);
	void setName(char * name);
	char * getName();
	void setZip(int zip);
	int getZip();
	Car & operator[](int number);
	void readData(Agency * a, Car * b);
	void printData(Agency * a, Car * b);
	Car * returnPointer();

private: 
	char m_name[MAX_SIZE];
	int m_zipcode;
	Car m_inventory[CARS];
};

#endif