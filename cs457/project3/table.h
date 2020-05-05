/*
	Database Header file
	CS 457, from University of Nevada, Reno
	by Curtis Chung
	Date: 2/6/2020
*/
#ifndef TABLE_H
#define TABLE_H

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

//class for storage of table information
class Table {
public:
	Table(string name, string parameters, string db);
	~Table();

	string getName();
	string getParameters();
	string getDatabase();
	string getValue(int row, int col);
	int getIndex(string column);

	void setParameters(string p);

	void printTable();
//	void fileWrite();
	void queryTable(int whereIndex, string whereVal, vector<int> indices);
	int update(int iWhere, string vWhere, int iSet, string vSet);
	int del(int index, string value, char a);
	void insertVector(vector<string> input);

private:
	string tableName;
	string tableParameters;
	string database;
	vector <vector<string>> vectorData;
};

#endif