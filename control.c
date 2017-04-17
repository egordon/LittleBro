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
#define ANGLECOEFF 0.10
#define WALLCOEFF 0.30
#define GRIDCOEFF 0.50

#define PI (3.1415926536)

static double rightOutput = 0;
static double leftOutput = 0;

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
	rightOutput = 0;
	leftOutput = 0;
	return returnVal;
}

double Control_turnRightOutput(Control_T oControl) {
	return 0.5*oControl->angleOutputDiff * ANGLECOEFF; // + oControl->wallOutput * WALLCOEFF, etc.
}

double Control_turnLeftOutput(Control_T oControl) {
	return -0.5*oControl->angleOutputDiff * ANGLECOEFF; // + oControl->wallOutput * WALLCOEFF, etc.
}

void Control_turnUpdateAngle(Control_T oControl) {
	double angle, dAngle, dt, inputDiff;
	double secs;

	secs = time_time();
	dt = secs - oControl->prevSeconds;

	oControl->prevSeconds = secs;
	angle = Sensor_getCompass();
	dAngle = Sensor_getGyro();
	inputDiff = Control_turnRightOutput(oControl) - Control_turnLeftOutput(oControl);
	oControl->angleOutputDiff = AC_update(oControl->ac, angle, dAngle, dt, inputDiff);
	Motor_setRight(Control_turnRightOutput(oControl));
	Motor_setLeft(Control_turnLeftOutput(oControl));
}

// essentially, turn in place for secs seconds
void Control_turnNorth(Control_T oControl, double secs) {
	double time1, time2;

	time2 = time_time();
	Control_changeHomeAngle(oControl, 0);
	// for secs seconds, use angle control only (ignore all other forms of control)
	for (time1 = time_time(); time2 - time1 < secs; time2 = time_time()) {
		Control_turnUpdateAngle(oControl);
	}
}
void Control_turnEast(Control_T oControl, double secs) {
	double time1, time2;
	time2 = time_time();
	Control_changeHomeAngle(oControl, PI/2);
	for (time1 = time_time(); time2 - time1 < secs; time2 = time_time()) {
		Control_turnUpdateAngle(oControl);
	}
}
void Control_turnSouth(Control_T oControl, double secs) {
	double time1, time2;
	time2 = time_time();
	Control_changeHomeAngle(oControl, PI);
	for (time1 = time_time(); time2 - time1 < secs; time2 = time_time()) {
		Control_turnUpdateAngle(oControl);
	}
}
void Control_turnWest(Control_T oControl, double secs) {
	double time1, time2;
	time2 = time_time();
	Control_changeHomeAngle(oControl, PI*(1.5));
	for (time1 = time_time(); time2 - time1 < secs; time2 = time_time()) {
		Control_turnUpdateAngle(oControl);
	}
}

void Control_advance(Control_T oControl) {

}

void Control_changeHomeAngle(Control_T oControl, double newHome) {
	AC_changeHome(oControl->ac, newHome);
}

void Control_free(Control_T oControl) {

}