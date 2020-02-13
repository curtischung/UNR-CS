#include "Car.h"

//default constructor
Car::Car() {
	m_throttle = 0;
	cout << "Car: Default-ctor" << endl;
}

//parametized constructor
Car::Car(float * lla) {
	for(int i = 0; i < 3; i++) {
		m_lla[i] = lla[i];
	}
	cout << "Car: Parametized-ctor" << endl;
}

//copy constructor
Car::Car(const Car & object) {
	for(int i = 0; i < 3; i++) {
		m_lla[i] = object.m_lla[i];
	}
	cout << "Car: Copy-ctor" << endl;
}

//destructor
Car::~Car() {
	cout << "Car: Dtor" << endl;
}

//assignment operator
Car & Car::operator=(const Car & object) {
	m_throttle = object.m_throttle; //sets m_throttle to the object class m_throttle
	cout << "Car: Assignment" << endl;
	return *this;
}

//set and get functions for the throttle
void Car::setThrottle(int throttle) {
	m_throttle = throttle;
}

int Car::getThrottle() {
	return m_throttle;
}

//drive function
int Car::drive(int value) {
	setThrottle(value); //cals setThrottle and passes through the location value to the set function
}

void Car::serialize(ostream & a)const {
	a << "Car: @";
	for(int i = 0; i < 3; i++) {
		a << m_lla[i] << " "; //writes object class m_throttle to the output stream
	}	 	 
}

//move function
void Car::Move(float * location) {
	for(int i = 0; i < 3; i++) {
		m_lla[i] = location[i]; //sets m_lla to the location passed through
	}
	cout << "Car: " << "DRIVE to destination [" << m_lla[0] << ", " 
	<< m_lla[1] << ", " << m_lla[2] << "] , with throttle @ 75" << endl;
	drive(75);
}
