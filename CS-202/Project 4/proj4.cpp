//Project 4, CS 202
//by Curtis Chung
//Lab Section 1105

#include <iostream>
#include <fstream>

#include "strings.cpp"

const int MAX_SIZE = 256;
const int SENSORS = 3;
const int CARS = 5;

using namespace std;

//inclusions of my class cpp's
#include "Sensor.cpp"
#include "Car.cpp"
#include "Agency.cpp"


int main () {
	int option = 0;
	Agency agency[MAX_SIZE];
	Agency * agencyPtr = agency;
	Car * carPtr = agencyPtr -> returnPointer();

	//switch statement	
	while(option != 5) {
		cout << "========================================================" << endl;
		cout << "1 - Enter and read in an input file name" << endl;
		cout << "2 - Print out ALL data to the terminal" << endl;
		cout << "3 - Print the total number of sensors" << endl;
		cout << "4 - Print out most expensive available car from all agencies" << endl;
		cout << "5 - Exit" << endl;
		cout << "========================================================" << endl;
		cout << "Please enter an option:";
		cin >> option;
		if (option > 5) {
			cout << "Invalid option, please choose a valid option: " << endl;
		}
		switch(option) {
			case 1: { // call of agency ptr read, passes through agency and car ptr
				agencyPtr -> readData(agencyPtr, carPtr);
				carPtr = agencyPtr -> returnPointer();
			}
			break;
			case 2: { // prints data read to the pointers
				agencyPtr -> printData(agencyPtr, carPtr);
			}
			break;
		}
	}
}
//equals operator, checks if 2 sensors are the same
bool operator==(Sensor sensor1, Sensor sensor2) {
	myStringCompare(sensor1.getType(), sensor2.getType());
	if (sensor1.getType() == sensor2.getType()) {
		return true;
	} else {
		return false;
	}
}