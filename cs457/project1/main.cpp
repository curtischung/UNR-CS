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
#include <bits/stdc++.h> //transform()
#include <algorithm>
#include <sys/stat.h> //stat
#include <vector>
#include "table.cpp"

using namespace std;


/*
	CREATE DATABASE is the only one read atm
*/

int main (int argc, char * argv[]) 
{
	string command;
	string dbName = "";
	string tName = "";

	string delFile = "";
	struct stat buf;

	vector<Table> tableObject;
	int systemTracker;


	while (command != ".EXIT") {
		cout << "> ";
		getline(cin, command);

		transform(command.begin(), command.end(), command.begin(), ::toupper); //converts all commands to uppercase
		
		//ensures commands end with a ';' or start with '.'
		if(command.find(';') == -1 && command.find('.') != 0)
		{
			cout << "Command not recognized, please insert a ';' after each command." << endl;
		}
		else if (command == ".EXIT") //ensures exit command (probably unnneccesary)
		{
			break;
		}

		//creates file directory database
		else if (command.find("CREATE DATABASE") != string::npos) 
		{
			dbName = command.substr (16, command.length() - 17); //sets dbName as name given by user after command input
			if (stat(dbName.c_str(), &buf) != 0) //checks if file exists
			{
				systemTracker = system(("mkdir " + dbName).c_str());
				cout << "Database '" << dbName << "' was successfully created. "<< endl;
			} 
			else 
			{
				cout << "Failed to create database '" <<
				 dbName << "' because it already exists." << endl;
			}

		}

		//deletes file directory database
		else if (command.find("DROP DATABASE") != string::npos)
		{
			delFile = command.substr (14, command.length() - 15); //sets delFile to name
			if (stat(delFile.c_str(), &buf) == 0) //checks if file exists
			{
				systemTracker = system(("rmdir " + delFile).c_str());
				cout << "Database '" << delFile << "' was successfully deleted." << endl;
			}
			else
			{
				cout << "Failed to delete '" <<
				 delFile << "' because it does not exist." << endl;
			}
		}
		else if (command.find("CREATE TABLE") != string::npos)
		{
			tName = command.substr(13, command.length() - 14);
		}
		else
		{
			cout << "Error: Command not recognized." << endl;
		}
	}

	return 0;
}
