//Project 4, calc.c by Curtis Chung, Lab Section 1103
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
int main (void) {

	int command;
	int toggletype;
	int ivalue1, ivalue2;
	double dvalue1, dvalue2;
	double term;
	int isum, isubtract, imultiply, idivide;
	double dsum, dsubtract, dmultiply, ddivide;
	double resultexp, resultlog;

	printf("This program implements a calculator.\n");

	toggletype = 0;
	for(;;){
		printf("Options:\n");
		printf("1 - addition\n");
		printf("2 - subtraction\n");
		printf("3 - multiplication\n");
		printf("4 - division\n");
		printf("5 - exp(x)\n");
		printf("6- log(x)\n");
		printf("7 - toggle calculator type\n");
		printf("8 - exit program\n");
		printf("Please enter your option: ");
		scanf("%d", &command);
			if(command > 8 || command <= 0) {
				printf("Invalid Option.\n");
			}
			switch(command) {
				case 1: {
					if(toggletype == 0){
						printf("Enter first term: ");
						scanf("%lf", &dvalue1);
						printf("Enter second term: ");
						scanf("%lf", &dvalue2);
							dsum = dvalue1 + dvalue2;
								printf("The sum is: %.15lf\n", dsum);
						} else {
							printf("Enter first term: ");
							scanf("%d", &ivalue1);
							printf("Enter second term: ");
							scanf("%d", &ivalue2);
								isum = ivalue1 + ivalue2;
									printf("The sum is: %d\n", isum);
						}
				}
			}
			switch (command) {
				case 2: {
					if (toggletype == 0) {
						printf("Enter first term: ");
						scanf("%lf", &dvalue1);
						printf("Enter second term: ");
						scanf("%lf", &dvalue2);
							dsubtract = dvalue1 - dvalue2;
								printf("The difference is: %.15lf\n", dsubtract);
					} else {
						printf("Enter first term: ");
						scanf("%d", &ivalue1);
						printf("Enter second term: ");
						scanf("%d", &ivalue2);
							isubtract = ivalue1 - ivalue2;
								printf("The difference is: %d\n", isubtract);
						}
				}
			}
			switch (command) {
				case 3: {
					if (toggletype == 0) {
						printf("Enter first term: ");
						scanf("%lf", &dvalue1);
						printf("Enter second term: ");
						scanf("%lf", &dvalue2);
							dmultiply = dvalue1 * dvalue2;
								printf("The product is: %.15lf\n", dmultiply);
					} else {
						printf("Enter first term: ");
						scanf("%d", &ivalue1);
						printf("Enter second term: ");
						scanf("%d", &ivalue2);
							imultiply = ivalue1 * ivalue2;
								printf("The product is: %d\n", imultiply);
						}
				}
			}
			switch (command) {
				case 4: {
					if(toggletype == 0) {
						printf("Enter first term: ");
						scanf("%lf", &dvalue1);
						printf("Enter second term: ");
						scanf("%lf", &dvalue2);
						if (dvalue2 == 0) {
							printf("Cannot divide by zero!\n");
						} else {
							ddivide = dvalue1 / dvalue2;
								printf("The quotient is: %.15lf\n", ddivide);
						}
					} else {
						printf("Enter first term: ");
						scanf("%d", &ivalue1);
						printf("Enter second term: ");
						scanf("%d", &ivalue2);
						if (ivalue2 == 0) {
							printf("Cannot divide by zero!\n");
						} else {
							idivide = ivalue1 / ivalue2;
								printf("The quotient is: %d\n", idivide);
						}						
					}

				}
			}
			switch (command) {
				case 5: {
					if (toggletype == 0) {
						printf("Enter term: ");
						scanf("%lf", &term);
							resultexp = exp(term);
								printf("The result of exp(%.15lf) is: %.15lf\n", term, resultexp);

					} else {
						printf("Cannot calculate with integers.\n");
					}
				}
			}
			switch (command) {
				case 6: {
					if (toggletype == 0) {
						printf("Enter term: ");
						scanf("%lf", &term);
							resultlog = log(term);
								if (term > 0) {
									printf("The result of log(%.15lf) is: %.15lf\n", term, resultlog);
								} else {
									printf("Cannot take the log of that number!\n");
								}

					} else {
						printf("Cannot calculate with integers.\n");
					}
				}
			}			
			switch(command) {
				case 7: {
					if (toggletype == 0) {
						toggletype = 1;
						printf("Calculator now works with integers.\n");
					} else {
						toggletype = 0;
						printf("Calculator now works with doubles.\n");
					}
				}
			}
			switch(command) {
				case 8: {
					return 0;
				}
			}



	}


	return 0;
}