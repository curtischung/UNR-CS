/*
	Written in C++
	CS 477 Homework 3 Problem 2, from University of Nevada, Reno
	By Curtis Chung
	Date: February 12, 2020
*/

#include <iostream>

#define SIZE 11

using namespace std;


int MaxArrayIndex (int arr[], int left, int right) {
	int mid;
	int maxLeft = 0;
	int maxRight = 0;

	if (arr[left] >= arr[right])
	{
		return left;
	}
	else
	{
		mid = left + (right - left) / 2;
		cout << maxLeft << " ";
		maxLeft = MaxArrayIndex(arr, left, mid);
		cout << endl;
		cout << maxRight << " ";
		maxRight = MaxArrayIndex(arr, mid + 1, right);

		if (arr[maxLeft] >= arr[maxRight])
		{
			return maxLeft;
		}
		else
		{
			return maxRight;
		}

	}
}
void printArray(int arr[]);

int main (int argc, char* argv[]) 
{
	int A[] = {1, 4, 9, 3, 4, 9, 5, 6, 9, 3 ,7};
	int result;


	result = MaxArrayIndex(A, 0, SIZE);
	cout << "Largest number position: " << result << endl;
}


void printArray(int arr[]) {
	for(int i = 0; i < SIZE; i++) {
		cout << arr[i] << " ";
	}
	cout << endl;
}