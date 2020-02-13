//Sorting header, sorting methods chosen: Bubble and Merge

#ifndef BUBBLESORT_H
#define BUBBLESORT_H

#include <fstream>
#include <iostream>
#include <string>

template<class ItemType>
class BubbleSort {
public:
	BubbleSort(const size_t max);

	void readFile(const std::string filename);
	void writeOutputFile(const std::string filename);

	void bubblesort();

	unsigned long long int getSwap() const;
	unsigned long long int getComp() const;
	void showArr() const;
private:
	double * arr;
	size_t size;
	size_t maxSize;

	unsigned long long int comparisons;
	unsigned long long int swaps;


};
template<class ItemType>
BubbleSort<ItemType>::BubbleSort(const size_t max): size(0), maxSize(max) {
	arr = new double[max]; //dynamically allocate max size to the array
}

template<class ItemType>
void BubbleSort<ItemType>::readFile(const std::string filename) {
	std::ifstream file; //file stream declaration
	file.open(filename);

	for(size_t i = 0; i < maxSize; i++) {
		if(file) { //if file exists
			file >> arr[i]; //write file to array
		} 
		
	}
	file.close();
}
template<class ItemType>
void BubbleSort<ItemType>::writeOutputFile(const std::string filename) {
	std::ofstream out; //file declaration
	out.open(filename);
	for(size_t i = 0; i < maxSize; i++) {
		out << arr[i] << std::endl; //write array to the file
	}
	out.close();

}

template<class ItemType>
void BubbleSort<ItemType>::bubblesort () {
	unsigned long long int comp;
	unsigned long long int sw;
	//trackers for the swaps and comparisions
	comp = 0;
	sw = 0;

	for(size_t i = 0; i < maxSize;i++) {
		for(size_t j = 0; j < maxSize; j++) {
			comp++; //increment comparisons
			if(arr[i] > arr[j]) {  //if the array at integer i is less than j
				std::swap(arr[i], arr[j]); //swap the two values
				sw++;
			}
		}
	}
	swaps = sw;
	comparisons = comp;
} 

template<class ItemType>
void BubbleSort<ItemType>::showArr() const{
	for(size_t i = 0; i < maxSize; i++) {
		std::cout << arr[i] << std::endl; //print array function, for testing purposes
	}
}

template<class ItemType>
unsigned long long int BubbleSort<ItemType>::getSwap() const{
	return swaps; //allows access to the swaps
}

template<class ItemType>
unsigned long long int BubbleSort<ItemType>::getComp() const{
	return comparisons; //allows access to get the value of comparisions
}
#endif