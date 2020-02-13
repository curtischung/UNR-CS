#include "Agency.h"

Agency::Agency() {
	myStringCopy(m_name, " ");
	m_zipcode = 0;
}
void Agency::setName(char * name) {
	myStringCopy(m_name, name);
}
char * Agency::getName() {
	return m_name;
}
void Agency::setZip(int zipcode) {
	m_zipcode = zipcode;
}
int Agency::getZip() {
	return m_zipcode;
}
Car & Agency::operator[](int number) {
	return m_inventory[number];
}

void Agency::readData(Agency * a, Car * b) {
	char file[MAX_SIZE];
//	Agency m[MAX_SIZE];
//	Agency * a = m;

//	Car * b = a -> m_inventory;
//	Car d[MAX_SIZE];



	cout << "Please enter a file name to read from: ";
	cin >> file;

	char name[MAX_SIZE];
	int zipcode;

	int year;
	char make[MAX_SIZE];
	char model[MAX_SIZE];
	float price;
	char sensorsTemp[MAX_SIZE];
	bool available;
	char owner[MAX_SIZE];
	char bracket = '}';

	char gps[MAX_SIZE] = "gps";
//	char camera[MAX_SIZE] = "camera";
//	char lidar[MAX_SIZE] = "lidar";
//	char radar[MAX_SIZE] = "radar";
//	char none[MAX_SIZE] = " ";

	char k;

	ifstream input_stream;
	input_stream.open(file);


//		for(int i = 0; i < 2; i++) {
				input_stream >> name
							 >> zipcode; 
				a -> setName(name);
				a -> setZip(zipcode);
			for(int i = 0; i < CARS; i++) {
				input_stream >> year
							 >> make
							 >> model
							 >> price
							 >> sensorsTemp
							 >> available;
				if(available != true) {
					input_stream >> owner;
					b -> setOwner(owner);
				}
				b -> setYear(year);
				b -> setMake(make);
				b -> setModel(model);
				b -> setBasePrice(price);
				input_stream >> k;

				while (k != bracket) {
					if (k == 'g') {
						//Sensor gps(gps);
						//*b = (*b + Sensor(gps));
						myStringCopy(sensorsTemp, gps);
						input_stream >> k;
						input_stream >> k;
					} else if (k == 'c') {
						//*b = (*b + Sensor(camera));
						//myStringCopy(sensorsTemp, camera);
						//Sensor(camera);
						input_stream >> k;
						input_stream >> k;
						input_stream >> k;
						input_stream >> k;
						input_stream >> k;
					} else if (k == 'l') {
						//*b = (*b + Sensor(lidar));
						//myStringCopy(sensorsTemp, lidar);
						//Sensor(lidar);
						input_stream >> k;
						input_stream >> k;
						input_stream >> k;
						input_stream >> k;
					} else if (k == 'r') {
						//*b = (*b + Sensor(radar));
						//myStringCopy(sensorsTemp, radar);
						//Sensor(radar);
						input_stream >> k;
						input_stream >> k;
						input_stream >> k;
						input_stream >> k;
					}	else if (k == 'n') {
						//*b = (*b + Sensor(none));
						//myStringCopy(sensorsTemp, none);
						//Sensor(none);
						input_stream >> k;
						input_stream >> k;
						input_stream >> k;
						input_stream >> k;
					}
					input_stream >> k;	
					k++;			
				}
				b -> setAvailable(available);
				if (available != true) {
					b -> setOwner(owner);
				}
				b++;
			}
			a++;
			b = a -> m_inventory;

		input_stream.close();

}

void Agency::printData(Agency * a, Car * b) {
//	for(int i = 0; i < 2; i++) {
		cout << a -> m_name <<" " << a -> m_zipcode << endl;
		for(int j = 0; j < CARS; j++) {
			b -> print();
			b++;
		}
		cout << endl;
		b = a -> m_inventory;
	//}
}

Car * Agency::returnPointer() {
	Car * carPointer = m_inventory;
	return carPointer;
}
//char * getInventory(char * getInventory)