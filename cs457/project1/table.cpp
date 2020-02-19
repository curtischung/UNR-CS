/*
	Database source code file
	CS 457, from University of Nevada, Reno
	by Curtis Chung
	Date: 2/6/2020
*/
#include <iostream>
#include "table.h"

Table::Table(string name) 
{
	tableName = name;
}

Table::~Table()
{
	tableName = "";
}

string Table::getName()
{
	return tableName;
}