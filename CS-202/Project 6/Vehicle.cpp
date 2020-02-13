#include "Vehicle.h"

//Default constructor
Vehicle::Vehicle() {
	cout << "Vehicle:Default-ctor" << endl; //prints when called
}

//parametized constructor
Vehicle::Vehicle(float * lla) {
	for(int i = 0; i < 3; i++) {
		m_lla[i] = lla[i]; //sets m_lla equal to the float array passed through
	}
	cout << "Vehicle: Parametized-ctor" << endl; //prints when called
}

//copy constructor
Vehicle::Vehicle(const Vehicle & object) {
	for(int i = 0; i < 3; i++) {
		m_lla[i] = object.m_lla[i]; // sets m_lla equal to the class object m_lla copy
	}
	cout << "Vehicle: Copy-ctor" << endl; //prints when called
}

//destructor
Vehicle::~Vehicle() {
	cout << "Vehicle: Dtor" << endl;
}

//Assignment operator
Vehicle & Vehicle::operator=(const Vehicle & object) {
	//setLla(object.m_lla);
	if(this != &object) {
		for(int i = 0; i < 3; i++) {
			m_lla[i] = object.m_lla[i];
		}
		cout << "Vehicle: Assignment" << endl;
	}
	return *this;
}

void Vehicle::serialize(ostream & a)const {
	a << "Vehicle @";
	for(int i = 0; i < 3; i++) {
		a << m_lla[i] << " "; //writes object class m_throttle to the output stream
	}	 
}

//insertion operator, which rights the m_lla of the object class to the output stream
ostream & operator<<(ostream & os, const Vehicle & object) {
	object.serialize(os);
	return os;
}

//set and get functions for the lla
void Vehicle::SetLLA(float * lla) {
	for(int i = 0; i < 3; i++) {
		m_lla[i] = lla[i];
	}
}

float * Vehicle::getLla() {
	return m_lla;
}
