#ifndef SENSOR_H
#define SENSOR_H

/* Motor Dirver */

/**
  * Initialize Sensors
  * Return 1 on Success.
  * Return 0 on Failure
 **/
int Sensor_init(int pifd);

/* Return angle in radians or radians/s. */
double Sensor_getGyro();
double Sensor_getCompass();

/* Calibrates Current angle as "0" */
void Sensor_calCompass();

/* Linear Stuff */
typedef enum {
	kFRONT,
	kBACK,
	kLEFT,
	kRIGHT
} Dir_t;

/* Return distance in cm */
double Sensor_getShort();//Dir_t dir);
double Sensor_getLong(Dir_t dir);

/* Any Cleanup */
void Sensor_free();


#endif
