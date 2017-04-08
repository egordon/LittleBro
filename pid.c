/* pid.c */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <pid.h>

enum {false, true};

//----------------------------

struct PID {

	double kp, ki, kd;
	double kMax, kMin;
	double integral;
	double setPoint;
	double prevMeasure;
	int hasClamp;

};

PID_T PID_init() {
	PID_T oPID;

	oPID = (struct PID*) malloc(sizeof(struct PID));
	if (oPID == NULL) return NULL;

	oPID->kp = 0;
	oPID->ki = 0;
	oPID->kd = 0;
	oPID->integral = 0;
	oPID->prevMeasure = 0;
	oPID->setPoint = 0;
	oPID->hasClamp = false;

	return oPID;
}

void PID_setpoint(PID_T pid, double setPoint) {
	pid->setPoint = setPoint;
	pid->prevMeasure = 0;
	pid->integral = 0;
}

void PID_gains(PID_T pid, double kP, double kI, double kD) {
	pid->kp = kP;
	pid->ki = kI;
	pid->kd = kD;
}

void PID_clamp(PID_T pid, double min, double max) {
	pid->hasClamp = true;
	pid->kMin = min;
	pid->kMax = max;
}

/* Clear stored D and I state. */
void PID_clear(PID_T pid) {
	pid->kd = 0;
	pid->ki = 0;
}

/* Each iteration, get output from measurement. */
double PID_update(PID_T pid, double measurement) {
	double dVal, pidOut;
	double error = pid->setPoint - measurement;
	pid->integral += error;

	dVal = measurement - pid->prevMeasure;
	pid->prevMeasure = measurement;

	pidOut = error * (pid->kp) + (pid->integral) * (pid->ki) + dVal * (pid->kd);

	/* Clamp */
	if (pid->hasClamp) {
		if (pidOut > pid->kMax) pidOut = pid->kMax;
		if (pidOut < pid->kMin) pidOut = pid->kMin;
	}

	return pidOut;
}

double PID_getSetpoint(PID_T pid) {
	return pid->setPoint;
}

void PID_free(PID_T pid) {
	free(pid);
}