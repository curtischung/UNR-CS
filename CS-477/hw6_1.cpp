#include <iostream>

using namespace std;


int getMin(int a, int b);
int getMax(int a, int b);

int main()
{
	const int n = 4;

	int x[n] = {1, 10, 10, 1};
	int f[n] = {1, 2, 4, 8};
	int A[n];

	for(int i = 0; i < n; i++)
	{
		A[i] = getMin(x[i], f[i]);
		for(int j = 0; j < i; j++)
		{
			A[i] = getMax(getMin(x[i], f[i - j - 1]) + A[j], A[i]);
		}
	}
	cout << "A[n]: ";
	for(int i = 0; i < n; i++)
	{
	cout << A[i] << " ";
	}
	cout << endl << "Number of harmless drones is " << A[n-1] << endl;
	return 0;
}

int getMin(int a, int b)
{
	return (a<b?a:b);
}

int getMax(int a, int b)
{
	return (a>b?a:b);
}

