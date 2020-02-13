//Project 8, Problem 2, dynamic_strings.c Lab Section 1103

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
int strLength (char* stringLength); 
char* strcopy (char* myInput1, char* myInput2);
int strcomp(char *myString1, char* myString2);
char* strconcat(char *dString, char* sString); 

int main (int argc, char* argv[]) {
	int numOfStrings = atoi(argv[1]);
	int i, command, lengthOfString, input1, input2;
	char* string[numOfStrings];
	char* output;
	int stringLength, stringCompare;
	char stringConcat, stringCopy;



	for (i = 0; i < numOfStrings; i++) {
		++i;
		printf("Enter the length of string %d: ", i);
			scanf("%d", &lengthOfString);
		string[i] = malloc(sizeof(char) * (lengthOfString)); 
		printf("Please enter %d string: ", i);
			scanf("%s", string[i]);
			--i;
	}

	for (;;) {
		printf("Your strings are: \n");
		for (i = 0; i < numOfStrings; i++) {
			++i;
			printf("String number %d - \"%s\"\n", i, string[i]);
			--i;
		}
		printf("Options: \n");
			printf("1 - Find string length\n");
			printf("2 - Compare strings\n");
			printf("3 - Copy strings\n");
			printf("4 - Concatenate Strings\n");
			printf("5 - Quit\n");
			printf("Please enter your option: ");
				scanf("%d", &command);
				if (command > 5) {
					printf("Invalid Option.\n");
				}

			switch(command) {
				case 1: {
					printf("Please enter a string: ");
						scanf("%d", &input1);
						stringLength = strLength(string[input1]);
					printf("The length of string %d is: %d\n", input1, stringLength);
				}
			}
			switch(command) {
				case 2: {
					printf("Enter the number of the first string: ");
						scanf("%d", &input1);
					printf("Enter the number of the second string: ");
						scanf("%d", &input2);
						stringCompare = strcomp(string[input1], string[input2]);
					if (stringCompare == 1) {
						printf("String %d comes before string %d alphabetically.\n", input2, input1);
					} else if (stringCompare == -1) {
						printf("String %d comes before string %d alphabetically.\n", input1, input2);
					}
				}
			}
			switch(command) {
				case 3: {
					printf("Enter the number of the source string: ");
						scanf("%d", &input1);
					printf("Enter the number of the destination string: ");
						scanf("%d", &input2);
					strcopy(string[input2], string[input1]);
				}
			}
			switch(command) {
				case 4: {
					printf("Enter the number of the source string: ");
						scanf("%d", &input1);
					printf("Enter the number of the destination string: ");
						scanf("%d", &input2);
					strconcat(string[input2], string[input1]);
				}
			}
			switch(command) {
				case 5: {
					return 0;
				}
			}


	}


	return 0;
}

int strLength (char* stringLength) {
	int i, length = 0;
		while (*stringLength != '\0') {
			length++;
			stringLength++;
		}
	return length;
}

int strcomp(char *myString1, char* myString2) {
	int compare1, compare2;
	while (myString1 != '\0' && myString2 != '\0') {
		if(*myString1 == '\0' && *myString2 == '\0') {
			return 0;
		} else if(*myString1 > *myString2) {
			return 1;
		} else {
			return -1;
		}
			myString1++;
			myString2++;
	}
}


char* strcopy (char* destination, char* source) {
	char *starter;
	*starter = *source;

	while (*source != '\0') {
		*destination = *source;
		destination++;
		source++;
	}


	*destination = '\0';
	return starter;
}

char* strconcat(char* destination, char* source) {
	char *starter;
	*starter = *destination;

	while (*destination != '\0') {
		destination++;
	}
	while (*source != '\0') {
	*destination = *source;
		destination++;
		source++;
	}


	*destination = '\0';
	return starter;
}