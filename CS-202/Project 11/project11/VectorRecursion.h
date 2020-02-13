#include <vector>

//simple swap function
void swap(int *a, int *b) {
	int t = *a;
	*a = *b;
	*b = t;
}
//partition function
int partition(std::vector<int> & some, int start, int end) {
	int pivot = some[end]; //creates a pivot that points to the last element of vector
	int i = (start -1); //creates a variable which points to the start

	for(int j = start; j <= end - 1; j++) {
		if(some[j] <= pivot) { //if vector at point j is smaller than pivot
			i++; //increment i
			swap(&some[i], &some[j]); //swap the two values
		}
	}
	swap(&some[i+1], &some[end]); //ensure to swap the last values which have not been checked
	return (i + 1);

}

//vector resort function
void vector_resort(std::vector<int> & some, int start, int end) {
	if(start < end) { //if the first element compare is smaller than the last
		int pi = partition(some, start, end); //calll the partition
		vector_resort(some, start, pi - 1); //recursion
		vector_resort(some, pi + 1, end);
	}
}

//vector binary research
int vector_research(std::vector<int> & some, int start, int end, int x) {
	if(start < end) { //if the first element is less than the end
		int mid = (start + (end - 1)) / 2; //midpoint
		if(some[mid] == x) { //if the mid is equal to the target, then return it
			return mid;
		}else if (some[start] == x) {
			return start;
		}else if(some[end] == x) {
			return end;
		} else if(some[mid] > x) { //if mid point is greater than target, subtract midpoint by 1
			return vector_research(some, start, mid - 1, x);
		} else if (some[mid] < x) {
			return vector_research(some, mid + 1, end, x);//if condition fails, increments mid by 1
		}
	}

	return -1; //return -1 if fails

}