//Main file

#include <iostream>
#include <fstream>
#include <string>
#include <random>
#include <chrono>

#include "BubbleSort.h"
#include "MergeSort.h"

typedef std::chrono::high_resolution_clock Clock;

int randomgen(size_t max);

int main () {
	size_t numberOfItems;


	std::cout << "Generate how many random numbers: ";
	std::cin >> numberOfItems; //asks user input for the amount of values generated
	randomgen(numberOfItems); //sends the user input to the random gen function to generate 'n' amount of values

	std::string filename = std::to_string(numberOfItems) + ".txt"; //file name
	BubbleSort<double> b(numberOfItems); //bubble sort object creation, set with a size of 'n'
	b.readFile(filename); //read 

//	b.showArr();


	auto bsTime1 = Clock::now(); // tracker for time, tracking time starting at the instance of the program
	b.bubblesort();
	auto bsTime2 = Clock::now(); // time which took to process the sortinging

	b.writeOutputFile("sorted_" + filename); //write the sorted array to an output file

	std::chrono::duration<double> btotaltime = (bsTime2 - bsTime1); //getting the accurate time, from start to beginning

	std::cout << "######### BUBBLE SORT #########" << std::endl;
	std::cout << "# of Comparisons: "<< b.getComp() << std::endl;
	std::cout << "# of Swaps: " << b.getSwap() << std::endl;
	std::cout << "Efficiency: " << btotaltime.count() << "Seconds" << std::endl;

	MergeSort<double> m(numberOfItems);
	m.readFile(filename);

//	m.showArr();
	//all function are similar to bubblesort
	auto msTime1 = Clock::now();
	m.mergesort(0, m.getSize() - 1);
	auto msTime2 = Clock::now();

	std::chrono::duration<double> mtotaltime = (msTime2 - msTime1);

	m.writeOutputFile("msorted_" + filename);
	std::cout << "######### MERGE SORT #########" << std::endl;
	std::cout << "# of Comparisons: "<< m.getComp() << std::endl;
	std::cout << "# of Swaps: " << m.getSwap() << std::endl;
	std::cout << "Efficiency: " << mtotaltime.count() << " Seconds" << std::endl;
	return 0;
}


int randomgen(size_t max) {
	std::random_device rd; 
	std::mt19937 mt(rd()); 
	std::uniform_real_distribution<double> dist(1.0, 1000000.0); //limiting numbers of what can be generated

	std::ofstream output;
	std::string file = std::to_string(max) + ".txt";

	output.open(file);

	for(size_t i = 0; i < max; i++) {
		output << dist(mt) << std::endl;
	}
	output.close();
	return 0;
}
