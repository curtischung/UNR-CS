// Project 3, series.c Lab Section 1103 by Curtis Chung
#include <stdio.h>
#include <stdbool.h>

int main (void)
{
	int value, total, variable;

	printf("Enter an integer number: ");
	scanf("%d", &value);

	total = 0;
	variable = 1;

		for (variable; variable <= value; variable++) {
			total += variable;
		}
		
	printf("The value of the series is: %d\n", total);
	return 0;
}