//Project 1, CS 202
//by Curtis Chung
//Lab Section 1105

#include <iostream>
#include <fstream>

using namespace std;

const int ENTRY_LIMIT = 10;
const int LETTER_LIMIT = 9;

/*Declaration of my functions, Prototypes*/
void myStringCopy(char destination[], const char source []);
int myStringLength(const char str[]);
int myStringCompare(const char str[], const char str2[]);
void lengthComparer(char a[ENTRY_LIMIT][LETTER_LIMIT], int b[ENTRY_LIMIT]);
void alphabetSorter(char a[ENTRY_LIMIT][LETTER_LIMIT], int b[ENTRY_LIMIT]);
void printFunc(char a[ENTRY_LIMIT][LETTER_LIMIT], int b[ENTRY_LIMIT], char file[]);

int main () 
{
	/*ENTRY_LIMIT is the max number of rows, which is the max number
	of names which can be in the fil, and the LETTER_LIMIT is the max
	number of columns, which is the letter limit.*/

	char names_array[ENTRY_LIMIT][LETTER_LIMIT]; 
	int num_array[ENTRY_LIMIT];

	/*input and output files are 1D character arrays in order to store
	a user inputed file name in which they want to open and output to.*/
	char input_file[50], output_file1[50], output_file2[50];

	cout << "Enter the input file name: ";
	cin.getline(input_file, 50);

	cout << "Enter the Length Output File: ";
	cin.getline(output_file1, 50);

	cout << "Enter the Alphabet Output file name: ";
	cin.getline(output_file2, 50);

	/*fstream opens "input_file" to be read and edited by the user, which will
	allow the user to edit the order of the file names. */
	fstream input_stream;
	input_stream.open(input_file);

	/*This for loop will label and assign each name with a number from 0 - 9.
	and print all the names along with the numbers. This is the original print, where
	nothing has been edited or sorted, coming directly from the file being read.*/
		for (int i = 0; i < ENTRY_LIMIT; i++) {
			input_stream.getline(names_array[i], ENTRY_LIMIT);
			num_array[i] = i;
			cout << num_array[i] << " " << names_array[i] << endl;
		}
		cout << "==============================" << endl;
		
	/*I call the lengthComparer and the alphabetSorter function, which takes the 2 arrays given 		from the input file and runs it through the function. Once it runs through the function, it is 		now run through the print functions,which print the function has its own unique
	output file, which will print the now organized list of names and print them each to their
	own output files, which names are given by the user.*/
		lengthComparer(names_array, num_array);
		printFunc(names_array, num_array, output_file1);

		alphabetSorter(names_array, num_array);
		printFunc(names_array, num_array, output_file2);

	input_stream.close();
	return 0;
}
/*myStringLength computes the length of a string, when the function is called, it will process
the string given, and increment a count value for each character read that is not a NULL character, 
and then return that incremented value, which would be the length of the string.*/
int myStringLength(const char str[]) {
	int length = 0;
	int i = 0;
	while (str[i] != '\0') {
		i++;
		length++;
	}

	return length;
}

/*Compares the value of the letters and determines the alphabetical placement with each character
of 2 given strings. If str[i] comes first, or is larger than str2[i], then the function returns 1, if str2[] comes first, then the function returns -1, if anything else, it will return 0.*/
int myStringCompare(const char str[], const char str2[]) {
	int i = 0;
	while (str[i] != '\0' && str2[i] != '\0') {
		if(str[i] > str2[i]) {
			return 1;
		} else if (str[i] < str2[i]) {
			return -1;
		}
		i++;
	}
	return 0;
}
/*myStringCopy runs 2 strings into the function, one is labeled as the destination, where the source
string will be copied into. It runs it through a for loop, which will replace each individual
character in the destination string from the source string until a NULL character is seen, and it will
end the for loop. if there are leftover characters, the last line will set all of the leftover characters to NULL.*/
void myStringCopy(char destination[], const char source []) {
	int i = 0;
	for (i = 0; source[i] != '\0'; i++) {
		destination[i] = source[i];
	}
	destination[i] = '\0';
}
/*lengthComparer is the function in which it will compare the length of 2 strings, using the "myStringLength" function, in order to determine the length of the array. If the first array
has a larger length value than the second array, the array will run through the for loop and
swap the array and number array placements of both arrays. */
void lengthComparer(char a[ENTRY_LIMIT][LETTER_LIMIT], int b[ENTRY_LIMIT]) {
	char c[LETTER_LIMIT];
	int d;

	for (int j = 0; j < LETTER_LIMIT; j++) {
		for(int i = 0; i < LETTER_LIMIT; i++) {
			if(myStringLength(a[i]) > myStringLength(a[i+1])) {
				myStringCopy(c, a[i]);
				myStringCopy(a[i], a[i+1]);
				myStringCopy(a[i+1], c);
				d = b[i];
				b[i] = b[i+1];
				b[i+1] = d;
			}
		}
	}
 
}
/*alphabetSorter works extremely similarly to the lengthComparer, but uses the myStringCompare instead of myStringLength.*/
void alphabetSorter(char a[ENTRY_LIMIT][LETTER_LIMIT], int b[ENTRY_LIMIT]) {
	char c[LETTER_LIMIT];
	int d;

	for(int j = 0; j < LETTER_LIMIT; j++) {
		for(int i = 0; i < LETTER_LIMIT;i++) {
			if(myStringCompare(a[i], a[i+1]) == 1) {
				myStringCopy(c, a[i]);
				myStringCopy(a[i], 
a[i+1]);
				myStringCopy(a[i+1], c);
				d = b[i];
				b[i] = b[i+1];
				b[i+1] = d;
			}
		}
	}
}
/*printFunc just acts as a function which compressed the main function. It's purpose is to print
the sorted names list, differing on which function was called in the main, whether or not it is 
the alphabet or length sorted. The output file name is also passed through to ensure it is printed
to 2 seperate output files while on the same output stream.*/
void printFunc(char a[ENTRY_LIMIT][LETTER_LIMIT], int b[ENTRY_LIMIT], char file[]) {
	ofstream output_stream;
	output_stream.open(file);
	for (int i = 0; i < ENTRY_LIMIT; i++) {
		cout << b[i] << " " << a[i] << endl;
		output_stream << b[i] << " " << a[i] << endl;
	}
	cout << "==============================" << endl;
	output_stream.close();
}
