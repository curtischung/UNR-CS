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
	string delTable = "";

	string db = "";
	string para = "";

	struct stat buf;

	vector<Table> tableObject;
	int systemTracker;
	int errorFlag = 0;


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

		//selects which database folder to use
		else if (command.find("USE") != -1)
		{
			db = command.substr(4, command.length() - 5);
			if (stat(db.c_str(), &buf) == 0)
			{
				systemTracker = system(("cd " + db).c_str());
				cout << "Using database '" << db << "'." << endl;
			}
			else
			{
				cout << "Failed to select database '" << db << "' because it does not exist." << endl;
			}
		}

		//creates a table.txt file, which stores said .txt in the database folder
		else if (command.find("CREATE TABLE") != string::npos)
		{
			tName = command.substr(13, command.find("(") - 14);
			string file = db + "/" + tName + ".txt";

			if(stat(file.c_str(), &buf) == 0)
			{
				cout << "Failed to create table '" << tName << "' because it already exists." << endl;	
			}
			else
			{
				if(db != "")
				{
					systemTracker = system(("touch " + db + "/" + tName + ".txt").c_str()); //creates .txt table
					para = command.substr(command.find("(") + 1, command.length() - 3 - command.find(")")); //sets parameters from command inputted
					Table* T = new Table(tName, para, db); //Creates table instance
					tableObject.push_back(*T);
					cout << "Table '" << tName << "' was successfully created." << endl;
				}
				else
				{
					cout << "Error: No database selected." << endl;
				}
			}
		}
		else if (command.find("DROP TABLE") != string::npos)
		{
			delTable = command.substr(11, command.length() - 12);

			string file = db + "/" + delTable + ".txt";
			if(stat(file.c_str(), &buf) == 0)
			{
				systemTracker = system(("rm " + db + "/" + delTable + ".txt").c_str());
				for(int i = 0; i < tableObject.size(); i++)
				{
					if(delTable == tableObject[i].getName() && db == tableObject[i].getDatabase())
					{
						tableObject.erase(tableObject.begin() + i);
					}
					cout << "Table '" << delTable <<"' successfully deleted." << endl;
				}
			}
			else
			{
				cout << "Failed to delete table '" << delTable << "' because it does not exist." << endl;
			}
		}
		else if (command.find("ALTER TABLE") != string::npos)
		{
			tName = command.substr(12, command.find("ADD")-13);
			string add = command.substr(command.find("ADD") + 4, command.length() - (command.find("ADD") + 5));
			
			string file = db + "/" + tName + ".txt";
			if(stat(file.c_str(), &buf) == 0)
			{
				for(int i = 0; i < tableObject.size(); i++)
				{
					if(tName == tableObject[i].getName() && db == tableObject[i].getDatabase())
					{
						tableObject[i].setParameters(tableObject[i].getParameters() + "," + add);
						cout << "Table modified" << endl;
						break;
					}
				}
			}
			else
			{
				cout << "Failed to modify '" << tName << "' because it does not exist." << endl;
			}

		}
		else
		{
			cout << "Error: Command not recognized." << endl;
		}
	}

	return 0;
}
