#include "Car.h"
#include <iostream>
#include <strings.h>
using namespace std;
//default constructor, intializing only throttle to a default value
Car::Car() {
	m_throttle = 0;
	cout << "Car #" << getVin() << ": Default-ctor" << endl;
}

//parametized constructor, setting m_plates to the string pass through
//setting the vin number passed through, and using setLLA from Vehicle
//to set the lla passed through;
Car::Car(char * plates, int vin, float * lla):Vehicle(vin, lla){
	strcpy(m_plates, plates);
	cout << "Car #" << vin << ": Parametized-Ctor" << endl;
}

//copy constructor, copying all vales into the object
Car::Car(const Car & point) {
	strcpy(m_plates, point.m_plates);
	m_throttle = point.m_throttle;
	for(int i = 0; i < 3; i++) {
		m_lla[i] = point.m_lla[i];
	}
	cout << "Car # " << m_vin << ": Copy-ctor" << endl;
}

Car::~Car() {
	cout << "Car #" << getVin() << " : Dtor" << endl;
}

//insertion operator, which inserts all of the object parts into the output stream
ostream & operator<<(ostream & os, const Car & object) {
	os << "Car #" << object.m_vin
	   << " Plates " << object.m_plates
	   << " "  << ", Throttle: " << object.m_throttle << " @ ";
	for(int j = 0; j < 3; j++) {
		os << object.m_lla[j] << " ";
	}			 
}

//assignment operator, which works the same as the copy constructor but takes in a constructor
//created in the main as a left hand operand and sets it equal to the object values
Car & Car::operator=(const Car & object) {
	strcpy(m_plates,object.m_plates);
	m_throttle = object.m_throttle;
	for(int i = 0; i < 3; i++) {
		m_lla[i] = object.m_lla[i];
	}
	cout << "Car #" << object.m_vin << " Assignment" << endl;
	return *this;
}

//Drive function, which passes in a int value and sets it equal to the thorttle
void Car::drive(int value) {
	setThrottle(value);
}

//move function, which changes the location of LLA to the passed through location
//and then passes through 75 to the drive function
void Car::move(float * location) {
	for(int i = 0; i < 3; i++) {
		m_lla[i] = location[i];
	}
	cout << "Car #" << m_vin << "DRIVE to destination [" << m_lla[0] << " " 
	<< m_lla[1] << " " << m_lla[2] << "] , with throttle @ 75" << endl;
	drive(75);
}

//set and get functions for the plates
char Car::setPlates(char * plates) {
	strcpy(m_plates, plates);
}
char * Car::getPlates() {
	return m_plates;
}

//set and get functions for the throttle
int Car::setThrottle(int throttle) {
	m_throttle = throttle;
}
int Car::getThrottle() {
	return m_throttle;
}