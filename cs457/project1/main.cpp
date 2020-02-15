/*
	Database Project Assignment, written in C++
	CS 457, from University of Nevada, Reno
	by Curtis Chung
	Date: 2/6/2020
*/


#include <iostream>
#include <string>
#include <ctype.h>
#include <algorithm>
#include "table.cpp"

using namespace std;

string convertUppercase (string &str); //broken

/*
THINGS TO DO:
	convertUppercase broken, doesnt return the capitalized string, but does
	the process correctly

	CREATE DATABASE is the only one read atm
*/

int main (int argc, char * argv[]) 
{
	string command;
	string dbName = "default";
	string tName = "default";
	int systemTracker;

//	convertUppercase(dbName); //testing statements
//	cout << dbName;

	while (command != ".EXIT") {
		cout << "> ";
		getline(cin, command);
		convertUppercase(command); //broken

		if (command.find("CREATE DATABASE") != 1) 
		{

			if (command.substr (16, command.length() - 16) != dbName)
			{
				dbName = command.substr (16, command.length() - 16);
				systemTracker = system(("mkdir " + dbName).c_str());
				cout << "help";
				if(systemTracker == 0)
				{
					cout << "database created"	<< endl;
				}
			} 
			else if (command.substr (16, command.length() - 16) == dbName)
			{
				cout << "Failed to create database '" <<
				 dbName << "' because it already exists." << endl;
			}

		}
		else if (command.find("DROP DATABASE") != 1)
		{
			dbName = command.substr (14, command.length() - 14);
			systemTracker = system(("rmdir " + dbName).c_str());
			cout << "Database '" << dbName << "' was successfully deleted." << endl;
		}
		
	}

	return 0;
}

string convertUppercase (string &str)
{
	string c;
	for(int i = 0; i < str.size(); i++)
	{
		c += toupper(str[i]);
	}
	return string(c);
}