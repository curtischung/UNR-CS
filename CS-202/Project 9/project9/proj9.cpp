#include <iostream>
#include <cstring>
#include "NodeQueue.h"
#include "ArrayQueue.h"

using std::endl;
using std::cout;
using std::cin;

int main () {
	cout << "\n"
		<<  "//////////////////////////////////////////\n" <<
			 "////////// ArrayQueue Testing ///////////\n" <<
			 "/////////////////////////////////////////\n"
			 << endl;
	size_t size;
	int num;
	double num2;

	cout << "Please enter an array size(size): ";
	cin >> size;
	cout << "Please enter an integer and double(int double): ";
	cin >> num >> num2;



	DataType value(num, num2);

	ArrayQueue a_default;

	ArrayQueue a_parametized(size, value);
	cout << "Parametized Constructor: " << a_parametized << endl;

	ArrayQueue a_copy(a_parametized);
	cout << "Copy Constructor: " << a_copy << endl;

	ArrayQueue a_assignment(size, value);

	a_default = a_assignment;

	cout << "Assignment Operator: " << a_default << endl;

	cout << "Front(): " << a_default.front() << endl;
	cout << "Back(): " << a_default.back() << endl;

	cout << "Size of " << a_default.size() << endl;
	cout << "Empty: " << std::boolalpha << a_default.empty() << endl;
	cout << "Full: " << std::boolalpha << a_default.full() << endl;
	a_default.clear();

	DataType value2(11, 15.98);
	DataType value3(12, 27.17);
	ArrayQueue pp(3, value2);

	cout << pp << endl;
	pp.push(value3);
	cout << pp << endl;

	pp.pop();
	cout << pp << endl;

	cout << "Size of " << pp.size() << endl;
	cout << "Empty: " << pp.empty() << endl;
	cout << "Full: " << pp.full() << endl;
	pp.clear();


	cout << "\n"
	<<  "//////////////////////////////////////////\n" <<
		 "////////// NodeQueue Testing ////////////\n" <<
		 "/////////////////////////////////////////\n"
		 << endl;

	cout << "Please enter a NEW array size(size): ";
	cin >> size;
	cout << "Please enter a NEW integer and double(int double): ";
	cin >> num >> num2;

	DataType nvalue(num, num2);
	NodeQueue n_default;

	NodeQueue n_parametized(size, nvalue);
	cout << "Parametized Constructor: " << n_parametized << endl;

	NodeQueue n_copy(n_parametized);
	cout << "Copy Constructor: " << n_copy << endl;

	NodeQueue n_assignment(size, nvalue);
	n_default = n_assignment;
	cout << "Assignment Operator: " << n_default << endl;

	DataType value4(12, 21.00);
	DataType value5(1, 1.19);
	ArrayQueue npp(3, value4);

	cout << npp << endl;
	npp.push(value5);
	cout << npp << endl;

	npp.pop();
	cout << npp << endl;

	cout << "Front(): " << npp.front() << endl;
	cout << "Back(): " << npp.back() << endl;

	cout << "Size of " << npp.size() << endl;
	cout << npp.empty() << endl;
	cout << npp.full() << endl;
	npp.clear();
}