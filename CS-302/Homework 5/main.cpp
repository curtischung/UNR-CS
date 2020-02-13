//Project 5, UNR CS302, by Curtis Chung

#include <iostream>
#include <fstream>
#include <random> //random generator
#include <string> 
#include <algorithm> //for the heap functions

const int MAX_NUM = 100;

void randomGen(std::vector<int>& v);
int meanOfRand(std::vector<int>& v);
void printVec(std::vector<int>& v, std::string fileName);

int main() {

//1. Make a heap consisting of 100 random integers.
	std::vector<int> vec;

	//filling the vector with random generation of values
	randomGen(vec);
	std::cout << std::endl << "Random Vector is printed to RandomVec.txt in it's original state." << std::endl;
	std::cout << "All parts are printed to text files, each labeled with part number of the homework 5 pdf." << std::endl;
	//Printing the vector to an output file
	printVec(vec, "RandomVec.txt");


	//creation of the heap
	std::make_heap(vec.begin(), vec.end());

	std::cout << std::endl << "Vector has been made into a heap." << std::endl << std::endl;

	printVec(vec, "1_HeapVec.txt");
//2. Add mean to the heap
	int mean = meanOfRand(vec);
	std::cout << "The mean of the Vector is: " << mean << std::endl << std::endl;
	//pushing the mean to to the back of the vector
	vec.push_back(mean); 
	std::push_heap(vec.begin(), vec.end());

	printVec(vec, "2_MeanVec.txt");
//3. Delete maximum element
	std::pop_heap(vec.begin(), vec.end());
	vec.pop_back();

	printVec(vec, "3_PoppedVec.txt");

//4. Resort heap
	std::make_heap(vec.begin(), vec.end());

	printVec(vec, "4_ResortedHeap.txt");

	return 0;
}
void randomGen(std::vector<int>& v) {
	std::random_device rd; 
	std::mt19937 rng(rd()); 
	std::uniform_real_distribution<> dist(1, 500); 

	//random numbers pushed onto the vector
	for(size_t i = 0; i < MAX_NUM; i++) {
		v.push_back(dist(rng));
	}
}


int meanOfRand(std::vector<int>& v) {
	int mean = 0;

	//adding each number continually until the end of the vector(or the size)
	for(size_t i = 0; i < v.size(); i++) {
		mean += v[i];
	}

	//divide the numvber found by the total number of elements and return it
	mean = mean / v.size();

	return mean;
}


void printVec(std::vector<int>& v, std::string fileName) {

	std::ofstream output;
	std::string file = fileName;

	output.open(file);

	//vector being written to the desired output file name
	for(size_t i = 0; i < v.size(); i++) {
		output << v[i] << std::endl;
	}

	output.close();

}