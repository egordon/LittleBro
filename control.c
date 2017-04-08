/* control.c */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <angleControl.h>
#include <motors.h>
#include <sensors.h>

// CHANGE THESE CONSTANTS PLEASE, ETHAN!
#define ANGLECOEFF 1
#define WALLCOEFF 2
#define GRIDCOEFF 4
#define DT 0.1 // REMOVE WHEN WE HAVE A BETTER WAY OF GETTING DT

struct Control {

	AngleState_T ac;
	double angleOutputDiff;
	// eventually add things like:
	// WallState_T wc;
	// double wallOutputDiff;
	// double gridOutput; // the amount of voltage we want to send to move forwards or backwards
};

Control_T Control_init(int pifd) {
	Control_T returnVal = (struct Control*) malloc(sizeof(struct Control));
	AngleState_T ac = AC_init();
	Motor_init(pifd);

	returnVal->ac = ac;
	returnVal->output = 0;
	return returnVal;
}

void Control_updateAngle(Control_T oControl) {
	double angle, dAngle, dt, inputDiff;
	angle = Sensor_getCompass();
	dAngle = Sensor_getGyro();
	dt = DT; // TEMPORARY, IS BAD
	inputDiff = Control_getRightOutput() - Control_getLeftOutput();
	oControl->angleOutputDiff = AC_update(oControl->ac, double angle, double dAngle, double dt, double inputDiff);
	Motor_setRight(Control_getRightOutput());
	Motor_setLeft(Control_getLeftOutput());
}

double Control_getRightOutput(Control_T oControl) {
	return oControl->angleOutputDiff * ANGLECOEFF; // + oControl->wallOutput * WALLCOEFF, etc.
}

double Control_getLeftOutput(Control_T oControl) {
	return -1 * oControl->angleOutputDiff * ANGLECOEFF; // + oControl->wallOutput * WALLCOEFF, etc.
}

void Control_changeHomeAngle(Control_T oControl, double newHome) {
	AC_changeHome(oControl->ac, newHome);
}

void Control_free(Control_T oControl) {

}