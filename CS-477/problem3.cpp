/*
	Written in C++
	CS 477 Homework 3 Problem 3, from University of Nevada, Reno
	By Curtis Chung
	Date: February 12, 2020
*/

#include <iostream>
#include <bits/stdc++.h>

#define SIZE 11

using namespace std;

void sortNegNum(int arr[], int n);
void printArray(int arr[]);

int main (int argc, char* argv[]) 
{
	int A[] = {4, -3, 9, 8, 7, -4, -2, -1, 0, 6, -5};

	printArray(A);
	sortNegNum(A, SIZE);
	printArray(A);

}

void sortNegNum(int arr[], int n) 
{
    int j = 0; 
    for (int i = 0; i < n; i++) { 
        if (arr[i] < 0) { 
            if (i != j) 
                swap(arr[i], arr[j]); 
            j++; 
        } 
    }
}

void printArray(int arr[]) {
	for(int i = 0; i < SIZE; i++) {
		cout << arr[i] << " ";
	}
	cout << endl;
}