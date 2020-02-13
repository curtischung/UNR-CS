#include <iostream>
#include <cstring>
#include "NodeStack.h"
#include "ArrayStack.h"

using std::endl;
using std::cout;
using std::cin;

int main() {
	cout << "\n"
		<<  "//////////////////////////////////////////\n" <<
			 "////////// ArrayStack Testing ///////////\n" <<
			 "/////////////////////////////////////////\n"
			 << endl;

	DataType value(1, 2.001);
	DataType value2(3, 4.223);
	ArrayStack<size_t> a_default;

	cout << value << endl;

	ArrayStack<DataType> a_parametized(5, value);
	cout << a_parametized << endl;

	ArrayStack<DataType> a_copy(a_parametized);
	cout << a_copy << endl;

	ArrayStack<DataType> a_assignment(3, value2);

	ArrayStack<DataType> a_lhs(1, value);
	a_lhs = a_assignment;

	cout << a_lhs << endl;

	a_lhs.push(value);
	cout << a_lhs << endl;
	a_lhs.pop();
	cout << a_lhs << endl;

	cout << "Top: " << a_lhs.top() << endl;
	cout << "Size: " << a_lhs.size() << endl;
	cout << "Empty? " << std::boolalpha << a_lhs.empty() << endl;
	cout << "Full? " << std::boolalpha << a_lhs.full() << endl;

	a_lhs.clear();

	cout << "\n"
		<<  "//////////////////////////////////////////\n" <<
			 "/////////// NodeStack Testing ///////////\n" <<
			 "/////////////////////////////////////////\n"
			 << endl;

	NodeStack<DataType> n_default;

	NodeStack<DataType> n_parametized(5, value);
	cout << "Parametized: " <<n_parametized << endl;

	NodeStack <DataType> n_copy(n_parametized);
	cout << "Copy: " << n_copy << endl;

	NodeStack<DataType> n_assignment(3, value2);

	NodeStack<DataType> n_lhs(1, value);
	n_lhs = n_assignment;

	cout << "Assignment:" << n_assignment << endl;

	n_lhs.push(value);
	cout << "Push: "<<n_lhs << endl;

	cout << "Top: " << n_lhs.top() << endl;

	n_lhs.pop();
	cout << "Pop: " << n_lhs << endl;

	cout << "Top: " << n_lhs.top() << endl;
	cout << "Size: " << n_lhs.size() << endl;
	cout << "Empty? " << std::boolalpha << n_lhs.empty() << endl;

	n_lhs.clear();
}