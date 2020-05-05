/*
	Database Project Assignment, written in C++
	CS 457, from University of Nevada, Reno
	by Curtis Chung
	Date: 2/6/2020
*/
#define _GLIBCXX_USE_CXX11_ABI 0

#include <iostream>
#include <fstream>
#include <string>
#include <ctype.h>
#include <sstream>
#include <algorithm>
#include <sys/stat.h> //stat
#include <vector>
#include "table.cpp"


using namespace std;

void convertCase(string &input);
void loadVector(string input, vector<string> &inputVector);
void fileWrite(string para, string table, string database);

int main (int argc, char * argv[]) 
{
	string command;

	//names for all the specific areas
	string dbName = "";
	string tName = "";
	string tName2 = "";
	string delFile = "";
	string delTable = "";

	//strings to hold database names and parameters
	string db = "";
	string para = "";

	struct stat buf;

	vector<Table> tableObject;
	int errorFlag = 0;


	while (command != ".EXIT") {
		cout << "> ";
		getline(cin, command);

		convertCase(command);
		//debug
		//cout << command << endl;

		
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
				system(("mkdir " + dbName).c_str()); //system call to make directory
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
				system(("rmdir " + delFile).c_str()); //system call to delete file
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
			if (stat(db.c_str(), &buf) == 0) //checks if file/database exists
			{
				system(("cd " + db).c_str()); //system call to go into specific directory
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
			string file = db + "/" + tName + ".txt"; //sets file name 

			if(stat(file.c_str(), &buf) == 0) //checks if table exists
			{
				cout << "Failed to create table '" << tName << "' because it already exists." << endl;	
			}
			else
			{
				if(db != "")
				{
					system(("touch " + db + "/" + tName + ".txt").c_str()); //creates .txt table
					para = command.substr(command.find("(") + 1, command.length() - 3 - command.find("(")); //sets parameters from command inputted
					Table* T = new Table(tName, para, db); //Creates table instance
					tableObject.push_back(*T);
					
/*					ofstream output;
					output.open(db + "/" + tName + ".txt");

					output << para << endl;
					output.close();*/
					fileWrite(para, tName, db);
					cout << "Table '" << tName << "' was successfully created." << endl;
				}
				else
				{
					cout << "Error: No database selected." << endl;
				}
			}
		}

		//Deletes table from selected database
		else if (command.find("DROP TABLE") != string::npos)
		{
			delTable = command.substr(11, command.length() - 12);

			string file = db + "/" + delTable + ".txt"; //ensures we check in the database folder
			if(stat(file.c_str(), &buf) == 0)
			{
				system(("rm " + db + "/" + delTable + ".txt").c_str());
				for(int i = 0; i < tableObject.size(); i++)
				{
					if(delTable == tableObject[i].getName() && db == tableObject[i].getDatabase())
					{
						tableObject.erase(tableObject.begin() + i); //erases table parameters within the vector class
					}
					cout << "Table '" << delTable <<"' successfully deleted." << endl;
				}
			}
			else
			{
				cout << "Failed to delete table '" << delTable << "' because it does not exist." << endl;
			}
		}
		
		//Alter table contents, add parameters to the table vecor that has already been created
		else if (command.find("ALTER TABLE") != string::npos)
		{
			tName = command.substr(12, command.find("ADD")-13);
			string add = command.substr(command.find("ADD") + 3, command.length() - (command.find("ADD") + 4));
			
			string file = db + "/" + tName + ".txt"; //ensures we check inside selected database
			if(stat(file.c_str(), &buf) == 0)
			{
				for(int i = 0; i < tableObject.size(); i++)
				{
					if(tName == tableObject[i].getName() && db == tableObject[i].getDatabase())
					{
						tableObject[i].setParameters(tableObject[i].getParameters() + "," + add); //sets new parameters to already existing parameters
						cout << "Table '" << tName << "' has been modified." << endl;
						break;
					}
				}
			}
			else
			{
				cout << "Failed to modify '" << tName << "' because it does not exist." << endl;
			}
		}	

		// Join handler for Left Outer, inner, and natural join
		else if(command.find("SELECT *") != string::npos && (command.find("WHERE") != string::npos || command.find("ON") != string::npos))
		{
			if(command.find("WHERE") != string::npos)
			{
				tName = command.substr(command.find("FROM") + 5, command.find(',') - command.find("FROM") - 7);
				tName2 = command.substr(command.find(',') + 2, command.find("WHERE") - command.find(',') - 5);

				string tElement1 = command.substr(command.find('.') + 1, command.find('=') - command.find('.') - 2);
				string tElement2 = command.substr(command.find_last_of('.') + 1, command.find(';') - command.find('=') - 4);

				int tIndex1 = 0;
				int tIndex2 = 0;
				int eIndex1 = 0;
				int eIndex2 = 0;

				string temp1, temp2;
				int k = 0;

				//sets tIndex's to eIndexs if the database and table name are correct
				for(tIndex1 = 0; tIndex1 < tableObject.size(); tIndex1++)
				{
					if(tName == tableObject[tIndex1].getName() && db == tableObject[tIndex1].getDatabase())
					{
						eIndex1 = tableObject[tIndex1].getIndex(tElement1);
						break;
					}
				}

				for(tIndex2 = 0; tIndex2 < tableObject.size(); tIndex2++)
				{
					if(tName2 == tableObject[tIndex2].getName() && db == tableObject[tIndex2].getDatabase())
					{
						eIndex2 = tableObject[tIndex2].getIndex(tElement2);
						break;
					}
				}

				//strings that hold parameters of the table
				string l = tableObject[tIndex1].getParameters();
				string l2 = tableObject[tIndex2].getParameters();

				//position to track commas on thre entries
				int pos = l.find(',');
				int pos2 = l2.find(',');

				//both while loops replace ',' with '|'
				while(pos > 0)
				{
					l.replace(pos, 2, " | ");
					pos = l.find(',');
				}

				while (pos2 > 0)
				{
					l2.replace(pos2, 2, " | ");
					pos2 = l2.find(',');
				}

				cout << l << " | " << l2 << endl;

				//sets temp strings to values at certain indexes of the table
				for(int i = 0; i < 3; i++)
				{
					temp1 = tableObject[tIndex1].getValue(i, eIndex1);
					for(int j = 0; j < 3; j++)
					{
						temp2 = tableObject[tIndex2].getValue(j, eIndex2);

						if(temp1 == temp2)
						{
							for(k = 0; k < 2; k++)
							{
								cout << tableObject[tIndex1].getValue(i, k) << '|';

							}

							for(k = 0; k < 2; k++)
							{
								cout << tableObject[tIndex2].getValue(j, k) << '|';
							}
							cout << endl;
						}
					}
				}

			}
			else if(command.find("INNER JOIN") != string::npos)
			{
				cout << "ON" << endl;
				tName = command.substr(command.find("FROM") + 5, command.find("INNER") - command.find("FROM") - 8);
				tName2 = command.substr(command.find("JOIN") + 5, command.find("ON") - command.find("JOIN") - 8);
				
				string tElement1 = command.substr(command.find('.') + 1, command.find('=') - command.find('.') - 2);
				string tElement2 = command.substr(command.find_last_of('.') + 1, command.find(';') - command.find('=') - 4);

				int tIndex1 = 0;
				int tIndex2 = 0;
				int eIndex1 = 0;
				int eIndex2 = 0;

				string temp1, temp2;
				int k = 0;		

				for(tIndex1 = 0; tIndex1 < tableObject.size(); tIndex1++)
				{
					if(tName == tableObject[tIndex1].getName() && db == tableObject[tIndex1].getDatabase())
					{
						eIndex1 = tableObject[tIndex1].getIndex(tElement1);
						break;
					}
				}

				for(tIndex2 = 0; tIndex2 < tableObject.size(); tIndex2++)
				{
					if(tName2 == tableObject[tIndex2].getName() && db == tableObject[tIndex2].getDatabase())
					{
						eIndex2 = tableObject[tIndex2].getIndex(tElement2);
						break;
					}
				}		
				string l = tableObject[tIndex1].getParameters();
				string l2 = tableObject[tIndex2].getParameters();

				int pos = l.find(',');
				int pos2 = l2.find(',');

				while(pos > 0)
				{
					l.replace(pos, 2, " | ");
					pos = l.find(',');
				}

				while (pos2 > 0)
				{
					l2.replace(pos2, 2, " | ");
					pos2 = l2.find(',');
				}

				cout << l << " | " << l2 << endl;

				for(int i = 0; i < 3; i++)
				{
					temp1 = tableObject[tIndex1].getValue(i, eIndex1);
					for(int j = 0; j < 3; j++)
					{
						temp2 = tableObject[tIndex2].getValue(j, eIndex2);

						if(temp1 == temp2)
						{
							for(k = 0; k < 2; k++)
							{
								cout << tableObject[tIndex1].getValue(i, k) << '|';
							}

							for(k = 0; k < 2; k++)
							{
								cout << tableObject[tIndex2].getValue(j, k) << '|';
							}
							cout << endl;
						}
					}
				}
			}

			//left outer join handler
			else if (command.find("LEFT OUTER JOIN") != string::npos)
		{
			tName = command.substr(command.find("FROM") + 5, command.find("LEFT") - command.find("FROM") - 8);
			tName2 = command.substr(command.find("JOIN") + 5, command.find("ON") - command.find("JOIN") - 8);

			string tElement1 = command.substr(command.find('.') + 1, command.find('=') - command.find("JOIN") - 2);
			string tElement2 = command.substr(command.find_last_of('.') + 1, command.find(';') - command.find('=') - 4);

			int tIndex1 = 0;
			int tIndex2 = 0;
			int eIndex1 = 0;
			int eIndex2 = 0;

			string temp1, temp2;
			int k = 0;
			bool flag = 0;

			//checks the name and database
			for(tIndex1 = 0; tIndex1 < tableObject.size(); tIndex1++)
			{
				if(tName == tableObject[tIndex1].getName() && db == tableObject[tIndex1].getDatabase())
				{
					eIndex1 = tableObject[tIndex1].getIndex(tElement1);	//sets eIndex1 to the table object index of Index1
					break;
				}
			}

			for(tIndex2 = 0; tIndex2 < tableObject.size(); tIndex2++)
			{
				if(tName2 == tableObject[tIndex2].getName() && db == tableObject[tIndex2].getDatabase())
				{
					eIndex2 = tableObject[tIndex2].getIndex(tElement2);
					break;
				}
			}

			//string of the table parameters
			string l = tableObject[tIndex1].getParameters();
			string l2 = tableObject[tIndex2].getParameters();

			//position of commas
			int pos = l.find(',');
			int pos2 = l2.find(',');

			//iterates through strings l and l2 and replaces commas with lines
			while(pos > 0)
			{
				l.replace(pos, 2, " | ");
				pos = l.find(',');
			}

			while(pos2 > 0)
			{
				l2.replace(pos2, 2, " | ");
				pos2 = l2.find(',');
			}

			cout << l << " | " << l2 << endl;

			for(int i = 0; i < 3; i++)
			{
				flag = 0;
				temp1 = tableObject[tIndex1].getValue(i, eIndex1); //sets string to table value at tIndex1
				for(int j = 0; j < 3; j++)
				{
					temp2 = tableObject[tIndex2].getValue(j, eIndex2); //sets string to table value at tIndex1

					//handles if the strings are the same  and prints it to console
					if(temp1 == temp2)
					{
						flag = 1;
						for(k = 0; k < 2; k++)
						{
							cout << tableObject[tIndex1].getValue(i, k) << '|';
						}

						for(k = 0; k < 2; k++)
						{
							cout << tableObject[tIndex2].getValue(j, k) << '|';
						}
						cout << endl;
					}				
				}
				//if strings are not the same, flag is 0
				if(flag == 0)
				{
					for(k = 0; k < 2; k++)
					{
						cout << tableObject[tIndex1].getValue(i, k) << '|';
					}
					cout << endl;
				}
			}
		}
	}
		
		//Select allows us to select the table and prints it
		else if (command.find("SELECT *") != string::npos)
		{
			tName = command.substr(14, command.length() - 15);
			convertCase(tName);
			string file = db + "/" + tName + ".txt"; //ensures we check the table within the selected database
			if(stat(file.c_str(), &buf) == 0)
			{
				for(int i = 0; i < tableObject.size(); i++)
				{
					if(tName == tableObject[i].getName() && db == tableObject[i].getDatabase())
					{
						tableObject[i].printTable();
						break;
					}
				}
			}
			else
			{
				cout << "Failed to select table '" << tName << "' because it does not exist." << endl;
			}
		}

		//Allows user to insert new data into the table
		else if (command.find("INSERT INTO") != string::npos)
		{
			int i = 0;

			tName = command.substr(12, command.find("values") - 13);

			for (i = 0; i < tableObject.size(); i++)
			{
				if(tableObject[i].getName() == tName && tableObject[i].getDatabase() == db)
				{
					break; //break out of loop if name and database match
				}
			}

			if(i != tableObject.size()) //checks if index is not the size of the table object vector, which checks if it exists
			{
				vector<string> newItems;

				int pos = command.find("("); //finds where new parameters start
				para = command.substr(pos + 1, command.length() - 3 - pos); //sets the parameter string to the parameter inputted

				para.erase(remove_if(para.begin(), para.end(), ::isspace), para.end()); //removes whitespace
				loadVector(para, newItems); //loads vector in table with new itmes
				tableObject[i].insertVector(newItems); //inserts new items

/*				ofstream output;
				output.open(db + "/" + tName + ".txt", fstream::app);

				output << para << endl;
				output.close();*/
				fileWrite(para, tName, db);
				cout << "1 new record inserted." << endl;

			} 
			else
			{
				cout << "Error: Table selected is invalid/doesn't exist" << endl;
			}
		}

		//updates tuples with new parameters given by the user
		else if (command.find("UPDATE") != string::npos)
		{
			int i = 0;

			tName = command.substr(7, command.find("SET") - 8);

			for(i = 0; i < tableObject.size(); i++)
			{
				if(tableObject[i].getName() == tName && tableObject[i].getDatabase() == db)
				{
					break;
				}
			}

			if(i != tableObject.size()) //checks indexs to size of table object to see if it exists
			{
				string findWhere = command.substr(command.find("WHERE") + 6, command.find_last_of("=") - 6 - command.find("WHERE")); //finds area where updated data needs to be
				string findSet = command.substr(command.find("SET") + 4, command.find("=") - 5 - command.find("SET")); //finds indicies of where to set new value
	
				string setWhere = command.substr(command.find_last_of("=") + 2, command.find(";") - 2 - command.find_last_of("=")); //sets position
				string setSet = command.substr(command.find("=") + 2, command.find("WHERE") - 3 - command.find("=")); //sets value

				int whereIndex = tableObject[i].getIndex(findWhere); //gets index in table
				int setIndex = tableObject[i].getIndex(findSet);

				cout << tableObject[i].update(whereIndex, setWhere, setIndex, setSet) << " record(s) modified." << endl;
			}
			else
			{
				cout << "Error: Table selected is invalid/doesn't exist" << endl;
			}
		}
		/*
		Most functions below have the same funcionality as the lines above, just with different purposes.
		*/


		//deletes data from the tuples
		else if(command.find("DELETE") != string::npos)
		{
			int i = 0;

			tName = command.substr(12, command.find("WHERE") - command.find("FROM") - 6);

			for(i = 0; i < tableObject.size();i++)
			{
				if(tableObject[i].getName() == tName && tableObject[i].getDatabase() == db)
				{
					break;
				}
			}

			if(i != tableObject.size())
			{
				string findWhere = "";
				string setWhere = "";

				//handles when user gives command with equals as the operator
				if(command.find("=") != string::npos)
				{
					findWhere = command.substr(command.find("WHERE") + 6, command.find_last_of("=") - 7 - command.find("WHERE"));
					setWhere = command.substr(command.find_last_of("=") + 2, command.find(";") - 2 - command.find_last_of("="));
					int whereIndex = tableObject[i].getIndex(findWhere);
					char a = 'e'; //equal to
					cout << tableObject[i].del(whereIndex, setWhere, a) << " record(s) deleted." << endl; 
				}
				//handles when user gives command with less than 
				else if(command.find("<") != string::npos)
				{
					findWhere = command.substr(command.find("WHERE") + 6, command.find_last_of("<") - 7 - command.find("WHERE"));
					setWhere = command.substr(command.find_last_of("<") + 2, command.find(";") - 2 - command.find_last_of("<"));
					int whereIndex = tableObject[i].getIndex(findWhere);
					char a = 'l'; //less than
					cout << tableObject[i].del(whereIndex, setWhere, a) << " record(s) deleted." << endl; 
				}

				//handles when user gives command with greater than 
				else if(command.find(">") != string::npos)
				{
					findWhere = command.substr(command.find("WHERE") + 6, command.find_last_of(">") - 7 - command.find("WHERE"));
					setWhere = command.substr(command.find_last_of(">") + 2, command.find(";") - 2 - command.find_last_of(">"));
					int whereIndex = tableObject[i].getIndex(findWhere);
					char a = 'g'; //greater than
					cout << tableObject[i].del(whereIndex, setWhere, a) << " record(s) deleted." << endl; 
				}
				else
				{
					cout << "Error: Operating used is invalid." << endl;
				}
			}
			else
			{
				cout << "Error: Table selected is invalid/doesn't exist" << endl;
			}

		}

		//handles selecting specific data in table
		else if (command.find("SELECT") != string::npos)
		{
			int i = 0;
			tName = command.substr(command.find("FROM") + 5, command.find("WHERE") - command.find("FROM") - 6);

			for(i = 0; i < tableObject.size();i++)
			{
				if(tableObject[i].getName() == tName && tableObject[i].getDatabase() == db)
				{
					break;
				}
			}

			if (i != tableObject.size())
			{
				if(command.find("!=") != string::npos)
				{
					string findWhere = command.substr(command.find("WHERE") + 6, command.find_last_of("!=") - 8 - command.find("WHERE"));
					string setWhere = command.substr(command.find_last_of("!=") + 2, command.find(";") - 2 - command.find_last_of("!="));
					int whereIndex = tableObject[i].getIndex(findWhere);

					vector<int> j;

					string column = command.substr(7, command.find("FROM") - 7);
					j.push_back(tableObject[i].getIndex(column.substr(0, column.find(",") - 1)));
					j.push_back(tableObject[i].getIndex(column.substr(column.find(",") + 1, column.length() - column.find(",") - 1)));
					tableObject[i].queryTable(whereIndex, setWhere, j);

				}
			}
		}

		//if no command is found that matches the above, then will return an unrecognized command.
		else
		{
			cout << "Error: Command not recognized." << endl;
		}
	}
/*	for(int i = 0; i < tableObject.size(); i++)
	{
		output << tableObject[i];
	}*/

	return 0;
}

void convertCase(string &input) 
{
	string line = input;
	string newString = "";
	string dictionary[] = {"FROM","CREATE","UPDATE","ALTER","DROP","INSERT",
							"INTO","SET","WHERE","SELECT","VALUES","USE",
							"DATABASE","TABLE","DELETE",".EXIT", "PRODUCT", "Product",
							"ON", "LEFT", "OUTER", "JOIN", "INNER"};
	string check;

	vector<string> tokens;
	string token;
	istringstream tokenStream(line);

	size_t dictionaryItems = sizeof(dictionary) / sizeof(dictionary[0]); //amount of items within dictionary
	int dictionarySize = static_cast<int> (dictionaryItems);  //static cast to ensure the size does not change

	//get string tokens
	while(getline(tokenStream, token, ' '))
	{
		tokens.push_back(token);
	}

	for (int i = 0; i < tokens.size(); i++)
	{
		check = tokens[i];
		for(int j = 0; j < check.size(); j++)
		{
			check[j] = toupper(check[j]); //converts check elements to uppercase
		}

		for(int k = 0; k < dictionarySize; k++)
		{
			if(check == dictionary[k])
			{
				tokens[i] = check; //sets tokens to the check
				break;
			}
		}

		if(i == (tokens.size() - 1))
		{
			newString = newString + tokens[i];
		}
		else
		{
			newString = newString + tokens[i] + ' ';
		}
	}
	input = newString;

}

void loadVector(string input, vector<string> &inputVector)
{
	int position = input.find(','); //finds where added items are
	inputVector.push_back(input.substr(0, position)); //pushes new itmes to vector

	int newPosition = input.find(',', position + 1);

	while (newPosition > 0) //while the new position of the comma is not 0
	{
		inputVector.push_back(input.substr(position + 1, newPosition - position - 1)); //pushes items onto the input vector
		position = newPosition;// sets new position to find the next ','
		newPosition = input.find(',', position + 1);
	}
	inputVector.push_back(input.substr(position + 1, input.length() - position));
}

void fileWrite(string para, string table, string database)
{
	ofstream output;
	output.clear();
	output.open(database + '/' + table + ".txt", fstream::app);

	string l = para;
	int pos = l.find(',');

	while (pos > 0)
	{
		l.replace(pos, 1, " | ");
		pos = l.find(',');
	}

	output << l << endl;
	output.close();
}

