#ifndef CAR_H
#define CAR_H

#include "Vehicle.h"
#include <string.h>
#include <iostream>
using namespace std;

class Car : public Vehicle {
private:
	char m_plates[256];
	int m_throttle;
public:
	Car(); //default constructor
	Car(char * plates, int vin, float * lla); //parametized constructor
	Car(const Car & point); // copy constructor
	~Car(); //destructor
	Car & operator=(const Car & object); //assignment operator
	char setPlates(char * plates); // set function for plates
	char * getPlates(); // getfunction for plates
	int setThrottle(int throttle); // set function for throttle
	int getThrottle(); // get function for throttle
	friend::ostream & operator<<(ostream & os, const Car & object); // insertion operator
	void drive(int value); // drive function
	void move(float * location); // move function
};

#endif
