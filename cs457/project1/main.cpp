/*
	Database Project Assignment, written in C++
	CS 457, from University of Nevada, Reno
	by Curtis Chung
	Date: 2/6/2020
*/
#include <iostream>
#include <string>
#include "database.cpp"

using namespace std;

int main (int argc, char * argv[]) 
{
	string line;
	int switch = 0;

	while (line != ".exit") {
		cout << "> ";
		cin >> line;

		if (line.find("CREATE DATABASE") != -1) {
			switch = 1;
		}
	}
	Database test;
	test.createDatabase(line);
	return 0;
}