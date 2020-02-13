#ifndef Sensor_h
#include <iostream>

class Sensor {
public:
	Sensor();
	Sensor(char * type);
	Sensor(const Sensor & point); 
	void setType(char * type);
 	char * getType();
 	void setExtraCost(float extracost);
 	float getExtraCost();
 	static int getGps();
 	void resetGps();
	static int getLidar();
	void resetLidar();
	static int getCamera();
	void resetCamera();

private:
	char m_type[MAX_SIZE];
	float m_extracost;
	static int gps_cnt;
	static int lidar_cnt;
	static int camera_cnt;

};
#endif