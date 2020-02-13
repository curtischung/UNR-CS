
#include <iostream>
#include <string.h>
#include <random>
#include <chrono>
#include <vector>
#include <algorithm>

#include "LeftLeaningRedBlack.h"
const int MAX_NUM = 10;

void randomGen(std::vector<int> & v)
;
int main() {
	std::vector<int> v;

	int pKeys[MAX_NUM];
	int vRef;
	randomGen(v);
	LeftLeaningRedBlack tree;

	for(int i = 0; i < MAX_NUM; i++) {
		pKeys[i] = v[i];
	}

	VoidRef_t ref;
	ref.pContext = &vRef;
	tree.FreeAll();

//printing the parent nodes of the insertion of the tree
	std::cout << "Parent at Insertion: " << std::endl;
	std::cout << "===============================" << std::endl;

	for(int i = 0; i < MAX_NUM; i++) {
		ref.Key = pKeys[i];
		std::cout << ref.Key << std::endl;

		tree.Insert(ref);

		tree.printParent(ref.Key);
		std::cout << std::endl;
	}
	std::cout << "===============================" << std::endl;

	for(int i = 0; i < MAX_NUM; i++) {
		tree.LookUp(pKeys[i]);
	}

//Traversal of the llrbt
	std::cout << "Traversal" << std::endl;
	std::cout << "===============================" << std::endl;

	tree.Traverse();
	std::cout << "===============================" << std::endl;
		std::cout << "Removing fourth element" << std::endl;
	std::cout << "===============================" << std::endl;

 	tree.Delete(pKeys[3]);

 	tree.Traverse();

//Tree stored earlier values, added delete function to clean the tree after runtime
    for (int i = 0; i < MAX_NUM; i++){
        
        tree.Delete(pKeys[i]);
    }

 	tree.FreeAll();

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