#include "Sensor.h"

Sensor::Sensor() {
	m_extracost = 0;
	myStringCopy(m_type, " ");
}
Sensor::Sensor(char * type) {
	setType(type);
}

Sensor::Sensor(const Sensor & point) {
		myStringCopy(m_type, point.m_type);
		m_extracost = point.m_extracost;
}

void Sensor::setType(char * type) {
	myStringCopy(m_type, type);
	if (myStringCompare(type, "gps")) {
		setExtraCost(5.0);
	}
	if (myStringCompare(type, "camera")) {
		setExtraCost(10.0);
	}
	if (myStringCompare(type, "lidar")) {
		setExtraCost(15.0);
	}
	if (myStringCompare(type, "radar")) {
		setExtraCost(20.0);
	}
	if (myStringCompare(type, "none")) {
		setExtraCost(0.0);
	}
}

char * Sensor::getType() {
	return m_type;
}

void Sensor::setExtraCost(float extracost) {
	m_extracost = extracost;
}

float Sensor::getExtraCost() {
	return m_extracost;
}

int Sensor::gps_cnt = 0;
int Sensor::lidar_cnt = 0;
int Sensor::camera_cnt = 0;

int Sensor::getGps() {
	return gps_cnt;
}

void Sensor::resetGps() {
	--gps_cnt;
}

int Sensor::getLidar() {
	return lidar_cnt;
}

void Sensor::resetLidar() {
	--lidar_cnt;
}

int Sensor::getCamera() {
	return camera_cnt;
}

void Sensor::resetCamera() {
	--camera_cnt;
}

