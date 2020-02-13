//Project 8, Problem 1: combine_strings.c

#include <stdio.h>
#include <stdlib.h>
int strcomp(char *myString1, char* myString2);
int strLength (char* stringLength);
char *intersperse(char *string1, char *string2);
char *widen_stars(char *string1, char *string2);

int main (int argc, char *argv[]) {
	char str1[30], str2[30];
	char *output;

	printf("Please enter a string of maximum thirty characters: ");
		fgets(str1, 100, stdin);
	printf("Please enter a string of maximum thirty characters: ");
		fgets(str2, 100, stdin);

	switch(argv[1][1]) {
		case 'i':
			output = intersperse(str1, str2);
			break;
		case 'w':
			output = widen_stars (str1, str2);
			break;
	}

	printf("The combined string is: %s", output);

	free(output);

	return 0;
}

char *intersperse(char *string1, char *string2) {
	char *result, *pointer;
	int length1, length2;

	length1 = strLength(string1);
	length2 = strLength(string2);

	result = malloc(length1 + length2 + 1);

	pointer = result;
	while (*string1 != '\0' || *string2 != '\0') {
		if (*string1 != '\0') {
			*pointer++ = *string1++;
		}
		if (*string2 != '\0'){
			*pointer++ = *string2++;
		}
	}

	*pointer = '\0';

	return result;
}

char *widen_stars(char *string1, char *string2) {
	char *result, *pointer;
	int length1, length2;

	length1 = strLength(string1);
	length2 = strLength(string2);

	result = malloc((length1 + length2) * 2 + 1);

	pointer = result;
		while(*string1 != '\0') {
			*pointer++ = *string1++;
			*pointer++ = '*';
		}

		while(*string2 != '\0') {
			*pointer++ = *string2++;
			*pointer++ = '*';
		}
		*--pointer = '\0';
	return result;
}

int strLength (char* stringLength) {
	int i, length = 0;
		while (*stringLength != '\0') {
			length++;
			stringLength++;
		}
	return length;
}