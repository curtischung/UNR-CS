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
#include <algorithm>
#include "table.cpp"

using namespace std;

string convertUppercase (string &str);

int main (int argc, char * argv[]) 
{
	string command;
	string dbName = "default";
	string tName = "default";

	while (command != ".EXIT") {
		cout << "> ";
		cin >> command;
		command = convertUppercase(command);

		if (command.find("CREATE DATABASE") != -1) 
		{
			if (command.substr (16, command.length() - 16) != dbName)
			{
				dbName = command.substr (16, command.length() - 16);
				system(("mkdir" + dbName).c_str());
			}
			else
			{
				cout << "Failed to create database" << dbName <<  "because it already exists.";
			}

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