//Random Number Generator
//By Curtis Chung
/*
#include <random>
#include <string>
#include <iostream>
#include <fstream>

int randomgen(size_t max) {
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_real_distribution<double> dist(1.0, 1000000.0);

	std::ofstream output;
	std::string file = std::to_string(max) + ".txt";

	output.open(file);

	for(size_t i = 0; i < max; i++) {
		output << dist(mt) << std::endl;
	}
	return 0;
}
*/