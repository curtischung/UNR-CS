#include <iostream>
using namespace std;
/*void twist(int a, int b) {
	int c;
	c = a + 2;
	c = 1 * a + c;
	b = c + 2*a;
}

int main() {
	int r = 2;
	int s = 3;
	int t = 3;
	twist(t, s);
	cout << r << " " << s << " " << t << endl;

}*/

/*int main() {
	int * ptr;
	int temp;
	int x;

	ptr = new int;
	*ptr = 4;
	temp = *ptr;
	cout << *ptr << temp;

	x = 9;
	temp = x;
	cout << *ptr << temp; 
	ptr = new int;
	*ptr = 5; 
	cout << *ptr << temp;
	return 0;
}

*/

int main() {
	int ** arr;
	int row = 3;
	int col = 5;
	arr = new int* [row];
	for(int i = 0; i < row; i++) {
		arr[i] = new int[col];
	}
	for(int i = 0; i < row; i++) {
		for(int j = 0; j < col; j++) {
			std::cout << arr[i][j] << std::endl;
		}
		std::cout << std::endl;
	}
}