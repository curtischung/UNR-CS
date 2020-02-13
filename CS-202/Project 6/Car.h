#ifndef CAR_H
#define CAR_H

#include <iostream>
#include <strings.h>
#include "Vehicle.h"

class Car : public Vehicle {
private:
	int m_throttle;
	virtual void serialize(ostream & a)const;
public:
	Car();
	Car(float * lla);
	Car(const Car & object);
	virtual ~Car();
	Car & operator=(const Car & object);
	void setThrottle(int throttle);
	int getThrottle();
	virtual void Move(float * location);
	int drive(int value);
};

#endif