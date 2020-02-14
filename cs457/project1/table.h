/*
	Database Header file
	CS 457, from University of Nevada, Reno
	by Curtis Chung
	Date: 2/6/2020
*/
#ifndef TABLE_H
#define TABLE_H

#include <iostream>
#include <string>

using namespace std;

class Table {
public:
	Table(string name);
	~Table();

	string getName();

private:
	string tableName;

};

#endif