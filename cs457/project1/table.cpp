/*
	Database source code file
	CS 457, from University of Nevada, Reno
	by Curtis Chung
	Date: 2/6/2020
*/
#include <iostream>
#include "table.h"

Table::Table(string name, string parameters) 
{
	tableName = name;
	tableParameters = parameters;
}

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
	return db;
}
void Table::setParameters(string p)
{
	tableParameters = p;
}