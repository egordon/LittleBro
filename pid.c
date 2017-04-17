/* pid.c */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <pid.h>

#define TWO_PI (2*3.1415926536)

enum {false, true};

//----------------------------

struct PID {

	double kp, ki, kd;
	double kMax, kMin;
	double integral;
	double setPoint;
	double prevMeasure;
	int hasClamp;
	int isAngle; // 0 if this is normal PID, 1 if this is referring to angle

};

PID_T PID_init(int angleOrNot) {
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
	oPID->isAngle = angleOrNot;

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
	double absError = pid->setPoint - measurement;
	double error;

	// this accounts for the case where the PID is controlling angle
	if (pid->isAngle == false) {
		error = absError;
	} else {
		if (absError < (TWO_PI/2)) {
			error = absError;
		} else {
			error = absError - TWO_PI;
		}
	}

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