#ifndef Car_h
#include <iostream>

class Car{
public: 
	Car();
	Car(char * make, char * model, int year, float baseprice, 
		float m_finalprice, bool available, char * owner);
	Car(const Car & point); 
	void setMake(char * make);
	char * getMake();

	void setModel(char * model);
	char * getModel();

	void setYear(int year);
	int getYear();

	void setBasePrice(float year);
	float getBasePrice();

	void setAvailable(bool available);
	bool getAvailable();

	void setOwner(char * owner);
	char * getOwner();

	float getFinalPrice();
	float updatePrice();

	void print();
	float estimateCost(int days);
	Sensor * getSensor() {
		return m_sensors;
	}
	Sensor * returnPointer();
	Car & operator+(Sensor type);
	Car & operator+(char * owner);

private: 
	char m_make[MAX_SIZE];
	char m_model[MAX_SIZE];
	int m_year;
	Sensor m_sensors[SENSORS];
	float m_baseprice;
	float m_finalprice;
	bool m_available;
	char m_owner[MAX_SIZE];

};

#endif