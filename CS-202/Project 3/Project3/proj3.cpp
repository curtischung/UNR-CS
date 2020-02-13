//Project 3, CS 202
//by Curtis Chung
//Lab Section 1105

#include <iostream>
#include <fstream>

using namespace std;

const int MAX_SIZE = 256;
const int AGENCIES = 3;
const int CARS = 5;

size_t myStringLength(const char *str);
int myStringCompare(const char *str1, const char *str2);
char *myStringCopy(char *destination, const char *source);
char *myStringCat(char *destination, const char *source);


class RentalCar {
	public:
		RentalCar() {
			m_available = 0;
		}
		RentalCar(int * year, char * make, char *model, float * price, bool * available);
	//Set and get declarations
		void setYear(int year);
		int  getYear();
	
		void setMake(char * make);
		char * getMake();

		void setModel(char * model);
		char * getModel();

		void setPrice(float price);
		float getPrice();

		void setAvailable(bool available);
		bool getAvailable();

		float estimateCost(int days);
		void print();

	private:
		int m_year;
		char m_make[MAX_SIZE];
		char m_model[MAX_SIZE];
		float m_price;
		bool m_available;
};
struct RentalAgency {
	char name[MAX_SIZE];
	int zipcode;
	RentalCar inventory[CARS];

};

//Definitions of all set and get functions
void RentalCar::setYear(int year) {
	m_year = year;
}
int RentalCar::getYear() {
	return m_year;
}

void RentalCar::setMake(char * make) {
	myStringCopy(m_make, make);
}
char * RentalCar::getMake(){
	return m_make;
}

void RentalCar::setModel(char * model) {
	myStringCopy(m_model, model);
}

char * RentalCar::getModel(){
	return m_model;
}

void RentalCar::setPrice(float price) {
	m_price = price;
}
float RentalCar::getPrice() {
	return m_price;
}

void RentalCar::setAvailable(bool available) {
	m_available = available;
}
bool RentalCar::getAvailable() {
	return m_available;
}

//Class function which gives a default standard to printing a car's information.
void RentalCar::print() {
	cout << m_year << " " << m_make << " " << m_model << ", $" << m_price << " per day, Available: " << boolalpha << m_available << endl;
}

//Class Function for estimate cost, which takes the private variable of price and multiples
//it by the days passed through the parameters and returns the total amount.
float RentalCar::estimateCost(int days) {
	float total;
	total = m_price * days;
	return total;
}
//Function Declarations
void rentalCost(RentalAgency * a, RentalCar * b);
void inputFile(RentalAgency * a, RentalCar * b, char * file);
void printArr(RentalAgency * a, RentalCar * b);
void expensive(RentalAgency * a, RentalCar *b);
void outputFile(RentalAgency * a, RentalCar * b, char * file);


int main()
{
	char input_file[MAX_SIZE], output_file[MAX_SIZE];


	RentalAgency agency[MAX_SIZE]; //Declaration of struct array
	RentalAgency * agency_ptr = agency; // Creates a struct array pointer and points it to the original struct array
	RentalCar * carsPtr = agency_ptr -> inventory; // Creates a class pointer and points it to the struct array pointer's inventory

	//switch statement, menu starts 
	int option = 0;
	while(option != 6) {
		cout << "========================================================" << endl;
		cout << "1 - Enter and read in an input file name" << endl;
		cout << "2 - Print out Agencies and Cars to the terminal" << endl;
		cout << "3 - Estimate rental cost of a car from any agency" << endl;
		cout << "4 - Print out most expensive car from all agencies" << endl;
		cout << "5 - Enter output file name and print available cars" << endl;
		cout << "6 - Exit" << endl;
		cout << "========================================================" << endl;
		cout << "Please enter an option:";
		cin >> option;
		if (option > 6) {
			cout << "Invalid option, please choose a valid option: " << endl;
		}
		switch (option) {
			case 1: {
				inputFile(agency_ptr, carsPtr, input_file);
			}
			break;
			
			case 2: {
				printArr(agency_ptr, carsPtr);
				agency_ptr = agency;
				carsPtr = agency_ptr -> inventory;
			}
			break;
			case 3: {
				rentalCost(agency_ptr, carsPtr);
			}
			break;
			case 4: {
				expensive(agency_ptr, carsPtr);
			}
			break;
			case 5: {
				outputFile(agency_ptr, carsPtr, output_file);
			}
			break;
		}
	}

	return 0;
}

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
/***************************************/
//Menu Option 1, which asks the user for an input file name, and then
//reads info from the given input file name, and inserts what's read
//into an input_stream, which will write the data read into a struct
//array.
void inputFile(RentalAgency * a, RentalCar * b, char * file) {
	cout << "Please enter an input file name: ";
	cin >> file;


	int year;
	char make[MAX_SIZE];
	char model[MAX_SIZE];
	float price;
	bool available;

	ifstream input_stream;
	input_stream.open(file);
		for(int i = 0; i < AGENCIES; i++ ) {
				input_stream >> a -> name
						     >> a -> zipcode;
			for(int k = 0; k < CARS; k++) {
				input_stream >> year
						>> make
						>> model
						>> price
						>> available;
				b -> setYear(year);
				b -> setMake(make);
				b -> setModel(model);
				b -> setPrice(price);
				b -> setAvailable(available);
				b++;

			}
			a++;
			b = a -> inventory;
		}

	input_stream.close();
}
//Menu Option 3, which allows the user to choose an agency(0 - 2) and 
//a car(0 - 4) from the Agency, and asks how many days they would like to rent
//the car. It will then print an estimate of how much it would cost to
//rent the car for that amount of days.
void rentalCost(RentalAgency * a, RentalCar * b) {
	int agency, car, days;
	float total;

	cout << "Select which agency you would like to rent from: ";
	cin >> agency; 

	cout << "Select which car you would like to rent: ";
	cin >> car;

	cout << "Choose how many days you would like to rent the car: ";
	cin >> days;

	a += agency;
	b = a -> inventory;
	b += car;

	total = b -> estimateCost(days);

	cout << "The estimated cost for your car is: $" << total << endl;

}
//Menu Option 2, which prints all of the car info into the terminal, with all of the
//list of agencies, zipcode, year of the car, etc.
void printArr(RentalAgency * a, RentalCar * b) {
	for (int i = 0; i < AGENCIES; i++) {
		cout << a -> name << " " << a -> zipcode << endl;
		for(int j = 0; j < CARS; j++) {
			b -> print();
			b++;	
			}
		cout << endl;
		a++;
		b = a -> inventory;
		}
}

//Menu Function 4, where it compares all the car's prices, and prints out the highest
//and most expensive car. 
void expensive(RentalAgency *a, RentalCar * b) {

	float highest;
	RentalCar * position = b;

	highest = b -> getPrice();

	for(int i = 0; i < AGENCIES; i++) {
		for(int k = 0; k < CARS; k++) {
			if(highest < (b) -> getPrice()) {

				highest = b -> getPrice();	
				position = b;
			}
			b++;
		}
		a++;
		b = a -> inventory;
	}
	cout << "The most expensive car is ";
        position -> print();

}
//Menu Function 5, which takes the struct arrays of RentalAgency and writes
//them to an output file, writing only the available cars to the file, and getting
//the values from the private class using the "get" functions;
void outputFile(RentalAgency * a, RentalCar * b, char * file) {
	cout << "Please enter an output file name: ";
	cin >> file;

	ofstream output_stream;
	output_stream.open(file);

	for(int i = 0; i < AGENCIES; i++) {
		output_stream << a -> name << " " << a -> zipcode << endl;
		for(int k = 0; k < CARS; k++) {
			if(b -> getAvailable() != 0) {
				output_stream << b -> getYear()
							<< " " << b -> getMake()
							<< " " << b -> getModel()
							<< ", $" << b -> getPrice()
							<< ", Available: " << boolalpha << b -> getAvailable() << endl;
				
			}
			b++;

		}
		output_stream << endl;
		a++;
		b = a -> inventory;
	}
	output_stream.close();
}
