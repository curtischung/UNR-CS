/*
	Written in C++
	CS 477 Homework 3 Problem 2, from University of Nevada, Reno
	By Curtis Chung
	Date: February 12, 2020
*/

#include <iostream>

using namespace std;


int MaxArrayElement (int arr[], int left, int right) 
{
	if(right - left == 1)
	{
		return arr[left];
	}

	int mid = (left + right) / 2;

	int u = MaxArrayElement(arr, left, mid);

	int v = MaxArrayElement(arr, mid, right);


	return u>v?u:v;
}

int MaxArrayIndex(int arr[], int max, int n)
{
	cout << "The max element appears at indexes: ";
	for(int i = 0; i < n; i++)
	{
		if(arr[i] == max)
		{
			cout << i << " ";
		}
	}
	cout << endl;
}

int main (int argc, char* argv[]) 
{
	int A[] = {1, 4, 9, 3, 4, 9, 5, 6, 9, 3 ,7};
	int size = sizeof(A)/sizeof(int);
	int result;


	result = MaxArrayElement(A, 0, size - 1);
	MaxArrayIndex(A, result, size);
}