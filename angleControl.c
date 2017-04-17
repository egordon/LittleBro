/***********************************************
Filename: angleControl.c
Author: Changyan
Date: 3/11/17
***********************************************/

#include <matrix.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <angleControl.h>
#include <motors.h>
#include <kalman.h>
#include <pid.h>

enum {false, true};

// CHANGE THESE CONSTANTS PLEASE, ETHAN :)
#define KALMANPARAMFILE "kalmanParam.txt"
#define PID_KP 63.6 // 400 "Volts" range / 2 PI radians range
#define PID_KI 0
#define PID_KD 0
#define PID_CLAMPMIN -200
#define PID_CLAMPMAX 200

struct AngleState {

	PID_T pid;
	Kalman_T stateEstimator;

};
// TODO: fix memory leaks
AngleState_T AC_init() {
	FILE *fp;
	PID_T p;
	Kalman_T k;
	AngleState_T ac = (struct AngleState*) malloc(sizeof(struct AngleState));
	MAT *F = m_get(2,2);
	VEC *B = v_get(2);
	MAT *H = m_get(2,2);
	MAT *Q = m_get(2,2);
	MAT *R = m_get(2,2);

	fp = fopen(KALMANPARAMFILE, "r");

	m_finput(fp, F);
	v_finput(fp, B);
	m_finput(fp, H);
	m_finput(fp, Q);
	m_finput(fp, R);
	k = Kalman_init(F, B, H, Q, R);

	p = PID_init(true);
	PID_setpoint(p, 0);
	PID_gains(p, PID_KP, PID_KI, PID_KD);
	PID_clamp(p, PID_CLAMPMIN, PID_CLAMPMAX);

	ac->pid = p;
	ac->stateEstimator = k;
	return ac;
}

/**
  * @param angle: Angle in Radians
  * @param dAngle: Angular Velocity in Radians/s
  * @param dt: Time difference from last update.
  * @param inputDiff: voltage difference between motors
  **/
double AC_update(AngleState_T ac, double angle, double dAngle, double dt, double inputDiff) {
	VEC *measurement = v_get(2);
	double currentAngle;

	v_set_val(measurement, 0, angle);
	v_set_val(measurement, 1, dAngle);

	Kalman_update(ac->stateEstimator, measurement, inputDiff, dt);
	currentAngle = Kalman_get(ac->stateEstimator);
	return PID_update(ac->pid, PID_getSetpoint(ac->pid) - currentAngle);
	// may need to be multiplied by a constant
}

void AC_changeHome(AngleState_T ac, double newHome) {
	PID_setpoint(ac->pid, newHome);
}

double AC_getAngle(AngleState_T ac) {
	return Kalman_get(ac->stateEstimator);
}

void AC_free(AngleState_T ac) {

}