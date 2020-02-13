#include <iostream>

#include "MyString.h"

using std::cout;
using std::endl;

int main () {
//(1)
	MyString ms_default;
//(2)
	MyString ms_parametrized("MyString parametrized constructor!");
//(3)
	MyString ms_copy(ms_parametrized);
//(4)
	MyString * ms_Pt = new MyString("MyString to be deleted…");
	delete ms_Pt;
	ms_Pt = NULL;
//(5),(6)
	MyString ms_size_length("Size and length test");
	cout << ms_size_length.size() << endl;
	cout << ms_size_length.length() << endl;
//(7)
	MyString ms_toCstring("C-String equivalent successfully obtained!");
	cout << ms_toCstring.c_str() << endl;
//(8)
	MyString ms_same1("The same"), ms_same2("The same");
	if (ms_same1==ms_same2) {
 		cout << "Same success" << endl;
	}
	MyString ms_different("The same (NOT)");
	if (!(ms_same1==ms_different)) {
 		std::cout << "Different success" << std::endl;
 	}
//(9)
	MyString ms_assign("Before assignment");
	ms_assign = MyString("After performing assignment");
//(10)
	MyString ms_append1("The first part");
	MyString ms_append2(" and the second");
	MyString ms_concat = ms_append1 + ms_append2;
//(11)
	MyString ms_access("Access successful (NOT)");
	ms_access[17] = 0;
//12
	cout << ms_access << endl;
return 0;
}