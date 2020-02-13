#include <iostream>
#include <fstream>

#include <vector>

#include "VectorRecursion.h"


int main(){

  // Vector creation
  std::vector<int> vecInt;
  
  // Vector population with input values
  std::ifstream fin("RandomText.txt");
  while(fin){
    int fin_in;
    fin >> fin_in;
    if (!fin) {
      break;
    }
    vecInt.push_back( fin_in );
  }
  int size = vecInt.size();
  vector_resort(vecInt, 0, size-1);
  for(int i = 0; i < size; i++) {
    std::cout << vecInt[i] << " ";
  }
  int x;
  std::cout << std::endl << "Please choose a number to search: ";
  std::cin >> x;

  int result = vector_research(vecInt, 0, size, x);
  if(result == -1) {
    std::cout << "Element does not exist in vector!" << std::endl;
  } else {
    std::cout << "Element exists at " << result << std::endl;
  }

  std::vector<int> vecIntCpy;
  std::cout << "Copy of Vector: " << std::endl;
  for(int i = 0; i < size; i++) {
    vecIntCpy.push_back(vecInt[i]);
    std::cout << vecIntCpy[i] << " ";
  }
  std::cout << std::endl;

  // Vector indexing and output ( using operator[] or at() )
  for (size_t i=0; i<vecInt.size(); ++i){
    std::cout << vecInt[i] << " same as " << vecInt.at(i) << std::endl; 
  }
  
  // Vector indexing and output ( using iterator )
  for (std::vector<int>::iterator it=vecInt.begin(); it!=vecInt.end(); ++it){
    std::cout << *it << std::endl; 
  }

  return 0;
}
