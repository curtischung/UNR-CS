#include "Car.h"
Car::Car() {
	myStringCopy(m_make, " ");
	myStringCopy(m_model, " ");
	m_year = 0;
	m_baseprice = 0;
	m_available = 0;
}

Car::Car(const Car & point) {
	myStringCopy(m_make, point.m_make);
	myStringCopy(m_model, point.m_model);
	m_year = point.m_year;
	m_baseprice = point.m_baseprice;
	m_available = point.m_available;

}

//Set and Get for the m_make
void Car::setMake(char * make) {
	myStringCopy(m_make,make);
}

char * Car::getMake() {
	return m_make;
}

//set and get for m_model
void Car::setModel(char * model) {
	myStringCopy(m_model, model);
}

char * Car::getModel() {
	return m_model;
}

//set and get for year
void Car::setYear(int year) {
	m_year = year;
}

int Car::getYear() {
	return m_year;
}

//set and get for BasePrice
void Car::setBasePrice(float baseprice) {
	m_baseprice = baseprice;
}

float Car::getBasePrice() {
	return m_baseprice;
}

//Set and get for m_available
void Car::setAvailable(bool available) {
	m_available = available;
}

bool Car::getAvailable() {
	return m_available;
}

//set and get for m_owner
void Car::setOwner(char * owner) {
	myStringCopy(m_owner, owner);
}

char * Car::getOwner() {
	return m_owner;
}

float Car::getFinalPrice() {
	m_finalprice = m_baseprice; //* m_sensors;
	return m_finalprice;
}

//definition of the print
void Car::print() {
	cout << m_year << " " << m_make << " " << m_model << ", $" 
	<< m_baseprice << " per day, Sensors: " << m_sensors << 
	"Available: " << boolalpha << m_available << endl;
}


float Car::estimateCost(int days) {
	float total;
	total = m_baseprice * days;
	m_finalprice = total;
	return m_finalprice;
}


Car & Car::operator+(Sensor type) {
	Sensor * pointer = m_sensors;
	for (int i = 0; i < SENSORS;i++) {
		if(myStringCompare(pointer -> getType(), " ") == 0) {
			pointer -> setType(type.getType());
		}
		pointer++;
	}
	return *this;
}

Car & Car::operator+(char * owner) {
	if (m_available == 1) {
		myStringCopy(m_owner, owner);
		m_available = 0;
	}
	return *this;
}

Sensor * Car::returnPointer() {
	Sensor * sensorPointer = m_sensors;
	return sensorPointer;
}