

//string function that compares the value of integers/characters
int myStringCompare(const char *str1, const char *str2) {
	while (str1 != '\0' && str2 != '\0') {
		if (*str1 > *str2) {
			return 1;
		} else if (*str1 < *str2) {
			return -1;
		}
		str1++;
		str2++;
	}
	return 0;
}
//string function which copies "source" string into "destination" string
char *myStringCopy(char *destination, const char *source) {
	while (*source != '\0') {
		*destination = *source;
		destination++;
		source++;
	}
	destination = '\0';
	return destination;
}
//string function which places "source" string into the first whitespace after the "destination" string ends.
char *myStringCat(char *destination, const char *source) {
	while(*destination != '\0') {
		destination++;
	}
	while(*source != '\0') {
		*destination = *source;
		destination++;
		source++;
	}
	destination = '\0';
	return destination;
}
//string function which counts the length of a string
size_t myStringLength(const char *str) {
	size_t length = 0;
	while(*str != '\0') {
		length++;
		str++;
	}
	return length;
}