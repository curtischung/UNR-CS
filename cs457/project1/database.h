/*
	Database Header file
	CS 457, from University of Nevada, Reno
	by Curtis Chung
	Date: 2/6/2020
*/
#ifndef DATABASE_H
#define DATABASE_H

#include <iostream>
#include <string>

using namespace std;

class Database {
public:
	Database();
	Database(string name);

	void createDatabase(string line);

private:


};

#endif