/*
	Database source code file
	CS 457, from University of Nevada, Reno
	by Curtis Chung
	Date: 2/6/2020
*/
#include <iostream>
#include "table.h"

//parametized constructoir
Table::Table(string name, string parameters, string db) 
{
	tableName = name;
	tableParameters = parameters;
	database = db;
}

//destructor
Table::~Table()
{
	tableName = "";
}

//returns tableName
string Table::getName()
{
	return tableName;
}

//returns table parameters
string Table::getParameters()
{
	return tableParameters;
}

//returns database
string Table::getDatabase()
{
	return database;
}

void Table::setParameters(string p)
{
	tableParameters = p;
}

int Table::getIndex(string column)
{
	int i = 0;

	int pos = tableParameters.find(column); //position of index 
	int commaPos = tableParameters.find(","); //tells where the comma is on given command line

	//counts comma position to tell the index of the column
	while(pos > commaPos && commaPos != -1)
	{
		i++;
		commaPos = tableParameters.find(",", commaPos + 1); 
	}
	return i;
}

//prints current table
void Table::printTable()
{
	string l = tableParameters;
	int pos = l.find(",");
	while (pos > 0)
	{
		l.replace(pos, 2, " | ");
		pos = l.find(',');
	}
	cout << l << endl;

	for(int i = 0; i < vectorData.size(); i++)
	{
		cout << vectorData[i][0] << '|' << vectorData[i][1] << '|' << vectorData[i][2] << endl;
	}
}

void Table::queryTable(int index, string value, vector<int> indices)
{
	string l = tableParameters;
	int pos = l.find(",");

/*	cout << vectorData.size() << endl;
	cout << vectorData[0].size() << endl;*/
	while (pos > 0)
	{
		l.replace(pos, 2, " | ");
		pos = l.find(',');
	}

	l = l.substr(l.find("|") + 2, l.length() - l.find("|") - 1);
	cout << l << endl;

	for(int i = 0; i < vectorData.size(); i++)
	{
		if(vectorData[i][index] != value)
		{
			cout << vectorData[i][indices[0]] << '|' << vectorData[i][indices[1]] << endl;
		}
	}
}

//pushes data from input into vectorData
void Table::insertVector(vector<string> input)
{
	vectorData.push_back(input);
}

int Table::update(int iWhere, string vWhere, int iSet, string vSet)
{
	int c = 0; //counter

	for (int i = 0; i < vectorData.size(); i++)
	{
		if(vectorData[i][iWhere] == vWhere)
		{
			c++;
			vectorData[i][iSet] = vSet; //sets data to new value
		}
	}

	return c;
}

//removes items from tuples
int Table::del(int index, string value, char a)
{
	int c = 0; //counter

	if(a == 'e')
	{
		for(int i = 0; i < vectorData.size(); i++)
		{
			if(vectorData[i][index] == value)
			{
				c++;
				vectorData.erase(vectorData.begin() + i);
			}
		}
	} 
	else if (a == 'g')
	{
		for(int i = 0; i < vectorData.size(); i++)
		{
			if(stoi(vectorData[i][index]) > stoi(value))
			{
				c++;
				vectorData.erase(vectorData.begin() + i);
			}
		}
	}
	else if(a == 'l')
	{
		for(int i = 0; i < vectorData.size(); i++)
		{
			if(stoi(vectorData[i][index]) > stoi(value))
			{
				c++;
				vectorData.erase(vectorData.begin() + i);
			}
		}
	}

	return c;
}