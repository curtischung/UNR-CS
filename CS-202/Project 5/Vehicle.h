#ifndef VEHICLE_H
#define VEHICLE_H

#include <string.h>

#include <iostream>
#include <cstring>

using namespace std;

class Vehicle {
protected:
	float m_lla[3]; //3 stands for Latitude, Longitude, and Altitude
	const int m_vin; //Vehicle Identification Number
private:
	static int s_idgen; // 
public:
	Vehicle(); //default constructor
	Vehicle(int a, float * b); // parametized constructor
	Vehicle(const Vehicle & point); //copy constructor
	~Vehicle(); //destructor
	Vehicle & operator=(const Vehicle & object); //Assignment Operator
	float setLla(float * lla); //Set function
	float * getLla(); // getfunction
	int getVin(); //getfunction
	static int getIdgen(); //static get function
	friend::ostream & operator<<(ostream & os, const Vehicle & object); //insertion operator
	void move(float * location); //move function
};
#endif