/*
	Database Project Assignment, written in C++
	CS 457, from University of Nevada, Reno
	by Curtis Chung
	Date: 2/6/2020
*/
#define _GLIBCXX_USE_CXX11_ABI 0

#include <iostream>
#include <string>
#include <ctype.h>
#include <bits/stdc++.h>
#include <algorithm>
#include <vector>
#include "table.cpp"

using namespace std;

string convertUppercase (string &str); //broken

/*
	CREATE DATABASE is the only one read atm
*/

int main (int argc, char * argv[]) 
{
	string command;
	string dbName = "";
	string tName = "";
	int systemTracker;


	while (command != ".EXIT;") {
		cout << "> ";
		getline(cin, command);
		//convertUppercase(command); //broken

		transform(command.begin(), command.end(), command.begin(), ::toupper);
		if(command.find(';') == -1)
		{
			cout << "Command not recognized, please insert a ';' after each command." << endl;
		}
		else if (command.find("CREATE DATABASE") != 1) 
		{

			if (command.substr (16, command.length() - 16) != dbName)
			{
				dbName = command.substr (16, command.length() - 16);
				systemTracker = system(("mkdir " + dbName).c_str());
				if(systemTracker == 0)
				{
					cout << "Database '" << dbName << "' was successfully created. "<< endl;
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
		else
		{
			cout << "Error: Command not recognized.";
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
	return c;
}