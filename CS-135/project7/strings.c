//Project 7, strings.c by Curtis Chung, Lab Section 1103
#include <stdio.h>
#include <stdbool.h>

#define STR_LENGTH 100
int strLength (char* stringLength); //done
char* strcopy (char* myInput1, char* myInput2);
int strcomp(char *myString1, char* myString2); //compiles, does not compare, bugged
char* strconcat(char *dString, char* sString); 

int main (void) {
	char stringInput1[STR_LENGTH + 1], stringInput2[STR_LENGTH + 1];
	int length1, length2;
	printf("Please enter the first string: ");
		scanf("%s", stringInput1);
	printf("Please enter second string: ");
		scanf("%s", stringInput2);

	length1 = strLength(stringInput1);
		printf("The length of string 1 is: %d\n", length1);

	length2 = strLength(stringInput2);
		printf("The length of string 2 is: %d\n", length2);

	int alpha = strcomp(stringInput1, stringInput2);
		if (alpha == 1) {
			printf("String 1 comes before string 2 alphabetically.\n");
		} else if (alpha == -1) {
			printf("String 2 comes before string 1 alphabetically.\n");
		}
	strconcat(stringInput2, stringInput1);
		printf("String 1 after concatenation: %s\n", stringInput1);
		printf("String 2 after concatenation: %s\n", stringInput2);

	strcopy(stringInput2, stringInput1);
		printf("string 1 after copying: %s\n", stringInput1);
		printf("String 2 after copying: %s\n", stringInput2);

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