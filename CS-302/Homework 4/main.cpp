#include "BinaryNode.h"
#include "BinarySearchTree.h"

#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <random>

void randomgen(BinarySearchTree<int> & object, const int min, const int max, const int num);

int main () {
	BinarySearchTree<int> object; //class object creation of binary search tree
	int min, max, num; 

	std::cout << "Enter Minimum value of the numbers generated: ";
	std::cin >> min;
	std::cout << std::endl;

	std::cout << "Enter Maximum value of the numbers generated: ";
	std::cin >> max;
	std::cout << std::endl;

	std::cout << "Enter number of values generated: ";
	std::cin >> num;
	std::cout << std::endl;


	randomgen(object, min, max, num);

	std::cout << "Height: " << object.getHeight()<< std::endl;
	std::cout << "PreOrder: ";
	object.preorderTraverse();
	std::cout << std::endl;

	std::cout << "PostOrder: ";
	object.postorderTraverse();
	std::cout << std::endl;

	std::cout << "InOrder: ";
	object.inorderTraverse();
	std::cout << std::endl;

	return 0;
}

void randomgen(BinarySearchTree<int> & object, const int min, const int max, const int num) {
	std::vector<int> randomvalues;

	for(int i = 0; i < max; i++) {
		randomvalues.push_back(i);
	}
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::shuffle(randomvalues.begin(), randomvalues.end(), std::default_random_engine(seed));

	for(int i = 0; i < num; i++) {
		object.add(randomvalues[i]);
	}
}