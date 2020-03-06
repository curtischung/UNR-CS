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

string Table::getName()
{
	return tableName;
}

string Table::getParameters()
{
	return tableParameters;
}

string Table::getDatabase()
{
	return database;
}

void Table::setParameters(string p)
{
	tableParameters = p;
}

void Table::printTable()
{
	string l = tableParameters;
	int pos = l.find(',');
	while (pos > 0)
	{
		l.replace(pos, 2, " | ");
		pos = l.find(',');
	}
	cout << l << endl;
}