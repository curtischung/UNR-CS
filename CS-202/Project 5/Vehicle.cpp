#include "Vehicle.h"
#include <iostream>
#include <strings.h>
using namespace std;

int Vehicle::s_idgen = 0;
//default construct, which intializes m_lla to a default value of zero
Vehicle::Vehicle():m_vin(s_idgen++) {
	cout << "Vehicle #" << m_vin << ": Default-ctor" << endl;
}

//Parametized constructor, which assigns the value of the pointer b to m_lla
//from it call inside the main function
Vehicle::Vehicle(int a, float * b):m_vin(a <= s_idgen ? s_idgen++ : a) {
	for(int i = 0; i < 3; i++) {
		m_lla[i] = b[i];
	}
	cout << "Vehicle #" << m_vin << "Parametized-ctor" << endl;
}

//Copy constructor, which sets the value of m_lla in the constructor passed
//through into the object constructor
Vehicle::Vehicle(const Vehicle & point):m_vin(s_idgen++) {
	for(int i = 0; i < 3; i++) {
		m_lla[i] = point.m_lla[i];
	}
	cout << "Vehicle #" << m_vin << " :Copy-ctor" << endl; 
}

//destructor
Vehicle::~Vehicle() {
	cout << "Vehicle #" << m_vin << ": Dtor" << endl;
}

//Assignment operator, which sets m_lla equal to the object.m_lla
Vehicle & Vehicle::operator=(const Vehicle & object) {
	//setLla(object.m_lla);
	for(int i = 0; i < 3; i++) {
		m_lla[i] = object.m_lla[i];
	}
	cout << "Vehicle #" << object.m_vin << ": Assignment" << endl;
	return *this;
}

//get function for IDGEN
int Vehicle::getIdgen() {
	return s_idgen;
}

//insertion operator, which writes the object passed through to the
//output stream, which allows access to all variables outside of the
//class
ostream & operator<<(ostream & os, const Vehicle & object) {
	os << "Vehicle #" << object.m_vin << " @ ";
	for(int i = 0; i < 3; i++) {
		os << object.m_lla[i] << " ";
	}
	
}

//move operator, which does nothing at this part of the function
void Vehicle::move(float * location) {
	cout << "Vehicle #" << m_vin << " CANNOT MOVE - I DON'T KNOW HOW" << endl;
}

//set function for LLA
float Vehicle::setLla(float * lla) {
	for(int i = 0; i < 3; i++) {
		m_lla[i] = lla[i];
	}
}

//get function for LLA
float * Vehicle::getLla() {
	return m_lla;
}

//get function for VIN
int Vehicle::getVin() {
	return m_vin;
}