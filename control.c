/* control.c */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <angleControl.h>
#include <motors.h>
#include <sensors.h>
#include <control.h>
#include <pigpiod_if2.h>

// CHANGE THESE CONSTANTS PLEASE, ETHAN!
#define ANGLECOEFF 0.20
#define WALLCOEFF 0.30
#define GRIDCOEFF 0.50

struct Control {

	int prevSeconds;
	// seconds elapsed since pigpio was initialized
	// as measured during the most recent Control_updateAngle() call
	AngleState_T ac;
	double angleOutputDiff;
	// eventually add things like:
	// WallState_T wc;
	// double wallOutputDiff;
	// double gridOutput; // the voltage we want to send to move forwards or backwards
};

Control_T Control_init(int pifd) {
	Control_T returnVal = (struct Control*) malloc(sizeof(struct Control));
	AngleState_T ac = AC_init();
	Motor_init(pifd);

	returnVal->prevSeconds = time_time();
	returnVal->ac = ac;
	returnVal->angleOutputDiff = 0;
	return returnVal;
}

void Control_updateAngle(Control_T oControl) {
	double angle, dAngle, dt, inputDiff;
	double secs;

	// Hi Changyan! I found this bug but I don't want to fix it because I don't quite understand everything you are doing
	// Anyway, use time_time() instead of gpioTime(int,*int,*int), which just returns a double with the current time in it
	// It should actually make things easier since you only need to store one prevSeconds now (which is a double) instead of separate seconds and microseconds
	secs = time_time();
	dt = secs - oControl->prevSeconds;

	oControl->prevSeconds = secs;
	angle = Sensor_getCompass();
	dAngle = Sensor_getGyro();
	inputDiff = Control_getRightOutput(oControl) - Control_getLeftOutput(oControl);
	oControl->angleOutputDiff = AC_update(oControl->ac, angle, dAngle, dt, inputDiff);
	Motor_setRight(Control_getRightOutput(oControl));
	Motor_setLeft(Control_getLeftOutput(oControl));
}

double Control_getRightOutput(Control_T oControl) {
	return 0.5*oControl->angleOutputDiff * ANGLECOEFF; // + oControl->wallOutput * WALLCOEFF, etc.
}

double Control_getLeftOutput(Control_T oControl) {
	return -0.5*oControl->angleOutputDiff * ANGLECOEFF; // + oControl->wallOutput * WALLCOEFF, etc.
}

void Control_changeHomeAngle(Control_T oControl, double newHome) {
	AC_changeHome(oControl->ac, newHome);
}

void Control_free(Control_T oControl) {

}
