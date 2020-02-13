//MergeSort Header File

#ifndef MERGESORT_H
#define MERGESORT_H

#include <iostream>
#include <fstream>
#include <string>

template <class ItemType>
class MergeSort {
public:
	MergeSort(const size_t max);

	void readFile(const std::string filename);
	void writeOutputFile(const std::string filename);

	void mergesort(int first, int last);
	void merge(int first, int mid, int last);

	double * getArr() const;
	int getSize() const;
	int getComp() const;
	int getSwap() const;
	void showArr() const;

private:
	double * arr;
	size_t maxSize;
	long long int swaps;
	long long int comparisons;
};

template <class ItemType>
MergeSort<ItemType>::MergeSort(const size_t max): maxSize(max) {
	arr = new double[max];
}

template<class ItemType>
void MergeSort<ItemType>::readFile(const std::string filename) {
	std::ifstream file;
	file.open(filename);

	for(size_t i = 0; i < maxSize; i++) {
		if(file) {
			file >> arr[i];
		} 
		
	}
	file.close();
}

template<class ItemType>
void MergeSort<ItemType>::writeOutputFile(const std::string filename) {
	std::ofstream out;
	out.open(filename);
	for(size_t i = 0; i < maxSize; i++) {
		out << arr[i] << std::endl;
	}
	out.close();

}

template <class ItemType>
void MergeSort<ItemType>::merge(int first, int mid, int last) {
	long long int comp = 0;
	long long int swap = 0;
	double tempArray[maxSize]; // Temporary array
		// Initialize the local indices to indicate the subarrays
	int first1 = first; // Beginning of first subarray
	int last1 = mid; // End of first subarray
	int first2 = mid+1; // Beginning of second subarray
	int last2 = last; // End of second subarray
		// While both subarrays are not empty, copy the smaller item into the
		// temporary array
	int index = first1; // next available location in tempArray
	while ((first1<=last1) && (first2 <=last2)){
		// At this point, tempArray[first..index-1] is in order
		if (arr[first1] <= arr[first2]) {
			tempArray[index] = arr[first1];
			first1++;
			comp++;
		} else {
			tempArray[index] = arr[first2];
			first2++;
			comp++;
		} // end if
		index++;
	} // end while
	while (first1<=last1) {
// at this point, tempArray[first..index-1] is in order
		tempArray[index] = arr[first1];
		first1++;
		index++;
		comp++;
	} // end while
// Finish off the second subarray, if necessary
	while (first2<=last2) {
// at this point, tempArray[first..index-1] is in order
		tempArray[index] = arr[first2];
		first2++;
		index++;
		comp++;
	} // end for
// Copy the result back into the original array
	for (index=first; index<=last; index++) {
		arr[index] = tempArray[index];
	}
	 // end merge

/*	size_t i, j , k;
	size_t n1;
	size_t n2;

	n1 = mid - first + 1;
	n2 = last - mid;

	double L[n1];
	double R[n2];

	 std::cout << n1 << std::endl;
	 std::cout << n2 << std::endl;
	for(i = 0; i < n1; i++) {
		L[i] = arr[first + i];
		swap++;
				std::cout << L[i] << std::endl;
		std::cout << "tes1" << std::endl;
	}
	for(j = 0; j < n2; j++) {
		R[j] = arr[mid + 1 + j];
		swap++;
					std::cout << R[j] << std::endl;
		std::cout << "tes2" << std::endl;
	}

	i = 0;
	j = 0;
	k = first;

	while(i < n1 && j < n2) {
		comp++;
		if(L[i] <= R[j]) {
			arr[k] = L[i];
			i++;
			swap++;
			std::cout << L[i] << std::endl;
			std::cout << "test" << std::endl;
		} else {
			arr[k] = R[j];
			j++;
			swap++;
			std::cout << R[j] << std::endl;
			std::cout << "tes4" << std::endl;
		}
		k++;
	}
	while(i < n1) {
		L[i] = arr[k];
		i++;
		k++;
		comp++;
		swap++;
		std::cout << "test2" << std::endl;
	}
	while(j < n2) {
		R[j] = arr[k];
		j++;
		k++;
		comp++;
		swap++;
		std::cout << "test3" << std::endl;
	}

	for(i = first; i < maxSize; i++) {
		arr[i] = L[i];
		arr[mid + i] = R[i];
	}*/
	comparisons = comp;
	swaps = swap;

}

template<class ItemType>
void MergeSort<ItemType>::mergesort(int first, int last) {
	if(first < last) {
		int mid = first + (last - first) / 2;
		mergesort(first, mid);
		mergesort(mid + 1, last);
		merge(first, mid, last);
	}
}

template<class ItemType>
double * MergeSort<ItemType>::getArr() const{
	return arr;
}

template<class ItemType>
int MergeSort<ItemType>::getSize() const{
	return maxSize;
}

template<class ItemType>
int MergeSort<ItemType>::getSwap() const{
	return swaps;
}

template<class ItemType>
int MergeSort<ItemType>::getComp() const{
	return comparisons;
}

template<class ItemType>
void MergeSort<ItemType>::showArr() const{
	for(size_t i = 0; i < maxSize; i++) {
		std::cout << arr[i] << std::endl;
	}
}

#endif