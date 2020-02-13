/*
	Written in C++
	CS 477 Homework 3 Problem 2, from University of Nevada, Reno
	By Curtis Chung
	Date: February 12, 2020
*/

#include <iostream>

#define SIZE 11

using namespace std;

void merge(int arr[], int left, int mid, int right);
void mergeSort(int arr[], int left, int right);
void printArray(int arr[]);

int main (int argc, char* argv[]) 
{
	int A[] = {1, 4, 9, 3, 4, 9, 5, 6, 9, 3 ,7};

	cout << "Before merge sort: ";
	printArray(A);
	cout << endl;
	mergeSort(A, 0, SIZE);
	cout << "After merge sort: ";
	printArray(A);
}

void merge(int arr[], int left, int mid, int right) 
{
	int i, j, k;
	int n1 = mid - left + 1;
	int n2 = right - mid;

	int L[n1], R[n2];

	for (i = 0; i < n1; i++) {
		L[i] = arr[left + i];
	}
	for (j = 0; j < n2; j++) {
		R[j] = arr[mid + 1 + j];
	}

	i = 0;
	j = 0;
	k = left;

	while (i < n1 && j < n2)
	{
		if (L[i] <= R[j]) 
		{
			arr[k] = L[i];
			i++;
		} 
		else 
		{
			arr[k] = R[j];
			j++;
		}
		k++;
	}

	while (i < n1) 
	{
		arr[k] = L[i];
		i++;
		k++;
	}

	while (j < n2)
	{
		arr[k] = R[j];
		j++;
		k++;
	}
}

void mergeSort(int arr[], int left, int right) 
{ 
    if (left < right) 
    { 
        int mid = left+(right-left) / 2; 
  
      
        mergeSort(arr, left, mid); 
        mergeSort(arr, mid + 1, right); 
  
        merge(arr, left, mid, right); 
    } 
}
void printArray(int arr[]) {
	for(int i = 0; i < SIZE; i++) {
		cout << arr[i] << " ";
	}
	cout << endl;
}