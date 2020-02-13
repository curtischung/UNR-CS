// Project 3, sq_root.c by Curtis Chung, Lab Section 1103

#include <stdio.h>
#include <stdbool.h>
#include <math.h>
int main (void)
{
	double value, newguess, oldguess;

	printf("Enter a number: ");
	scanf("%lf", &value);

	oldguess = (1.0);
	newguess = (oldguess + ((value * 1.0)/ oldguess)) / 2.0;
	printf("%.5f\n", oldguess);

	while(fabs(newguess * newguess) - value >= .000001) {
			oldguess = 1.0 / 2;
			printf("%.5f\n", newguess);
			oldguess = newguess;
			newguess = (oldguess +(value * 1.0 / oldguess)) / 2.0;
			

	}
	printf("Estimated square root of %.5f: %.5f\n", value, newguess);
	return 0;
}
