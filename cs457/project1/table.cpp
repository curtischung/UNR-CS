/*
	Database source code file
	CS 457, from University of Nevada, Reno
	by Curtis Chung
	Date: 2/6/2020
*/
#include <iostream>
#include "table.h"

Table::Table(string name, string parameters, string db) 
{
	tableName = name;
	tableParameters = parameters;
	database = db;
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
	return database;
}

void Table::setParameters(string p)
{
	tableParameters = p;
}