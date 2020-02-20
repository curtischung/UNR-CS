/*
	Written in C++
	CS 477 Homework 3 Problem 2, from University of Nevada, Reno
	By Curtis Chung
	Date: February 12, 2020
*/

#include <iostream>
#include <vector>

using namespace std;


int MaxArrayElement (int arr[], int left, int right) 
{

	int i, j;
	int mid = (left + right) / 2;
	int maxIndexL;
	int maxIndexR;

	if (right - left == 0)
	{
		return left;
	}
	else if (right - left == 1)
	{
		if(arr[left] > arr[right])
		{
			return left;
		}
		else
		{
			return right;
		}
	}
	else
	{
		maxIndexL = MaxArrayElement(arr, left, mid);
		maxIndexR = MaxArrayElement(arr, mid, right);
		if(arr[maxIndexL] > arr[maxIndexR])
		{
			return maxIndexL;
		}
		else
		{
			return maxIndexR;
		}
	}

}


int main (int argc, char* argv[]) 
{
	int A[] = {1, 4, 9, 3, 4, 9, 5, 6, 9, 3 ,7};
	int size = sizeof(A)/sizeof(int);
	int result;


	result = MaxArrayElement(A, 0, size - 1);
	cout << "Largest number index: " << result << endl;
}