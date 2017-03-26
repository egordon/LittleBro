/* pid.c */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "pid.h"

enum {false, true};

//----------------------------

struct PID {

	double kp, ki, kd;
	double kMax, kMin;
	double integral;
	double setPoint;
	double prevError;
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
	oPID->prevError = 0;
	oPID->setPoint = 0;
	oPID->hasClamp = false;

	return oPID;
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

/* Each iteration, get output from error. */
double PID_update(PID_T pid, double error) {
	double dVal, pidOut;
	pid->integral += error;

	dVal = error - pid->prevError;
	pid->prevError = error;

	pidOut = error * (pid->kp) + (pid->integral) * (pid->ki) + dVal * (pid->kd);

	/* Clamp */
	if (pid->hasClamp) {
		if (pidOut > pid->kMax) pidOut = pid->kMax;
		if (pidOut < pid->kMin) pidOut = pid->kMin;
	}

	return pidOut;
}

void PID_free(PID_T pid) {
	free(pid);
}

/* format of input file should be:
"
	kp ki kd
	setpoint
	kMin kMax
	error0 error1 error2 ...
"
*/
int main() {
	double error;
	double kpIn, kiIn, kdIn, setPointIn, kMinIn, kMaxIn;
	scanf("%lf %lf %lf\n%lf\n%lf %lf", &kpIn, &kiIn, &kdIn, &setPointIn, &kMinIn, &kMaxIn);

	PID_T pid = PID_init();
	PID_gains(pid, kpIn, kiIn, kdIn);
	PID_clamp(pid, kMinIn, kMaxIn);
	printf("PID successfully initialized. kp is %f, ki is %f, kd is %f\n", kpIn, kiIn, kdIn);
	printf("Clamp min is: %f   max is: %f\n", kMinIn, kMaxIn);
	while (scanf(" %lf", &error) == 1 && !feof(stdin)) {
		printf("Output is: %f \n", PID_update(pid, error));
	}

	return 0;
}