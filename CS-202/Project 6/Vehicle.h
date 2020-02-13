#ifndef VEHICLE_H
#define VEHICLE_H

#include <iostream>
#include <strings.h>
using namespace std;

class Vehicle {
protected: 
	float m_lla[3];
private:
	virtual void serialize(ostream & a)const;
public: 
	Vehicle();
	Vehicle(float * lla);
	Vehicle(const Vehicle & object);
	virtual ~Vehicle();
	Vehicle & operator=(const Vehicle & object);
	void SetLLA(float * lla);
	float * getLla();
	virtual void Move(float * location) = 0;
	friend::ostream & operator<<(ostream & os, const Vehicle & object);
};

#endif