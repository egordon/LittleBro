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
#include <pid.h>
#include <math.h>

// CHANGE THESE CONSTANTS PLEASE, ETHAN!
#define ANGLECOEFF 0.10
#define WALLCOEFF 0.30
#define GRIDCOEFF 0.50

#define ROBOT_LENGTH (100) // in millimeters
#define GRID_LENGTH (180)
#define PI (3.1415926536)

static double rightOutput = 0;
static double leftOutput = 0;

struct Control {

	int prevSeconds;
	// seconds elapsed since pigpio was initialized
	// as measured during the most recent Control_turnUpdateAngle() or Control_updateAngle() call
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

static double Control_turnRightOutput(Control_T oControl) {
	// during Control_turnUpdateAngle, only this is used to calculate
	// motor voltages
	// during Control_advance, this term is added to the other motor voltage
	// contributors, particularly the PID loop for moving forwards/backwards, and
	// the control for avoiding walls on the left and right.
	return 0.5*oControl->angleOutputDiff * ANGLECOEFF; // + oControl->wallOutput * WALLCOEFF, etc.
}

static double Control_turnLeftOutput(Control_T oControl) {
	return -0.5*oControl->angleOutputDiff * ANGLECOEFF; // + oControl->wallOutput * WALLCOEFF, etc.
}

static double Control_updateAngle(Control_T oControl) {
	double angle, dAngle, dt, inputDiff;
	double secs;

	secs = time_time();
	dt = secs - oControl->prevSeconds;

	oControl->prevSeconds = secs;
	angle = Sensor_getCompass();
	dAngle = Sensor_getGyro();
	inputDiff = Control_turnRightOutput(oControl) - Control_turnLeftOutput(oControl);
	oControl->angleOutputDiff = AC_update(oControl->ac, angle, dAngle, dt, inputDiff);
}

static void Control_turnUpdateAngle(Control_T oControl) {
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

static int Control_estimateNumberOfSquares(double dist) {
	return floor(dist/GRID_LENGTH);
}

static void Control_advanceShort(Control_T oControl) {
	int i, j = 0;
	double temp, dist = 0, num = 0, motorOut;
	PID_T pid = PID_init(0);
	PID_setpoint(pid, 44);
	PID_gains(pid, 63.6, 0, 0);
	PID_clamp(pid, -10, 25);
	Control_updateAngle(oControl);

	for (i = 0; i < 10; i++) {
		temp = Sensor_getShort(kFRONT);
		if (temp != -1) {
			dist += temp;
			num++;
		}
	}
	dist /= num;

	motorOut = PID_update(pid, dist);
	Motor_setLeft(motorOut);
	Motor_setRight(motorOut);
	while((dist > 48)||(dist < 40)) {
		Control_updateAngle(oControl);
		j++;
		dist = 0;
		num = 0;
		for (i = 0; i < 10; i++) {
			temp = Sensor_getShort(kFRONT);
			if (temp != -1) {
				dist += temp;
				num++;
			}
		}
		dist /= num;

		motorOut = PID_update(pid, dist);
		if (j%2 == 0) {
			Motor_setLeft(motorOut + Control_turnLeftOutput(oControl));
			Motor_setRight(motorOut + Control_turnRightOutput(oControl));
		}
		else {
			Motor_setRight(motorOut + Control_turnRightOutput(oControl));
			Motor_setLeft(motorOut + Control_turnLeftOutput(oControl));
		}
	}
	PID_free(pid);
}

// advance one square forward in the maze
// return 0 if success
int Control_advance(Control_T oControl) {
	double longF = 0, longB = 0, shortF = 0; // distance forwards, backwards
	double tempLongF, tempLongB, tempShortF;
	double fDest, bDest, bHome;
	double positionEstimate, motorOut;
	int numLongF = 0, numLongB = 0, numShortF = 0;
	int numSquareF, numSquareB; // number of squares before next forward, back wall
	int i, j = 0;
	PID_T pid = PID_init(0);
	PID_gains(pid, 63.6, 0, 0);
	PID_clamp(pid, -10, 25);

	// get current position
	for (i = 0; i < 25; i++) {
		tempLongF = Sensor_getLong(kFRONT);
		tempLongB = Sensor_getLong(kBACK);
		tempShortF = Sensor_getShort(kFRONT);
		if (i == 1) {
			if ((tempLongF == longF)||(tempLongB == longB)) {
				printf("ERROR: Distance measurement is the same twice in a row.\n");
				printf("tempLongF: %f\nlongF: %f\ntempLongB: %f\nlongB: %f\n",
					tempLongF, longF, tempLongB, longB);
			}
		}
		if (tempLongF != -1) {
			longF += tempLongF;
			numLongF++;
		}
		if (tempLongB != -1) {
			longB += tempLongB;
			numLongB++;
		}
		if (tempShortF != -1) {
			shortF += tempShortF;
			numShortF++;
		}
	}

	Control_updateAngle(oControl);

	longF /= numLongF;
	longB /= numLongB;
	shortF /= numShortF;
	printf("Initial forward distance: %.2f and numLongF = %d", longF, numLongF);
	printf("Initial backward distance: %.2f and numLongB = %d", longB, numLongB);
	printf("Initial forward short distance: %.2f and numShortF = %d", shortF, numShortF);

	// if at least 10 forward short-distance readings were successful and
	// the bot is close to the forward wall, transfer motor control to Control_advanceShort()
	if ((numShortF >= 10)&&(shortF <= 150)) {
		PID_free(pid);
		Control_advanceShort(oControl);
		return 0;
	}

	// use both front and back long-range if we're in no danger of losing the back-sensor
	// while moving forward
	if ((numLongF >= 10)&&(numLongB >= 10)&&(longB <= 1500)) {
		numSquareF = Control_estimateNumberOfSquares(longF);
		numSquareB = Control_estimateNumberOfSquares(longB);
		fDest = GRID_LENGTH*(numSquareF - 1) + 84 - ROBOT_LENGTH/2;
		bHome = GRID_LENGTH * numSquareB + 84 - ROBOT_LENGTH/2;
		PID_setpoint(pid, fDest);

		while ((longF - fDest >= 1) || (longF - fDest <= 1)) {
			Control_updateAngle(oControl);

			j++;

			longF = 0;
			numLongF = 0;
			longB = 0;
			numLongB = 0;
			shortF = 0;
			numShortF = 0;
			for (i = 0; i < 10; i++) {
				tempLongF = Sensor_getLong(kFRONT);
				if (tempLongF != -1) {
					longF += tempLongF;
					numLongF++;
				}
				tempLongB = Sensor_getLong(kBACK);
				if (tempLongB != -1) {
					longB += tempLongB;
					numLongB++;
				}
				tempShortF = Sensor_getShort(kFRONT);
				if (tempShortF != -1) {
					shortF += tempShortF;
					numShortF++;
				}
			}
			longF /= numLongF;
			longB /= numLongB;
			shortF /= numShortF;

			if ((numShortF >= 10)&&(shortF <= 150)) {
				PID_free(pid);
				Control_advanceShort(oControl);
				return 0;
			}

			longF = (longF - longB + bHome)/2; // may be wrong
			// averaged the longF and longB values to estimate current position
			motorOut = PID_update(pid, longF);
			if (j%2 == 0) {
				Motor_setLeft(motorOut + Control_turnLeftOutput(oControl));
				Motor_setRight(motorOut+ Control_turnRightOutput(oControl));
			}
			else {
				Motor_setRight(motorOut + Control_turnRightOutput(oControl));
				Motor_setLeft(motorOut + Control_turnLeftOutput(oControl));
			}
		}
		PID_free(pid);
		return 0;
	}

	else if (numLongF >= 10) {
		numSquareF = Control_estimateNumberOfSquares(longF);
		fDest = GRID_LENGTH*(numSquareF - 1) + 84 - ROBOT_LENGTH/2;
		bDest = GRID_LENGTH*(numSquareB + 1) + 84 - ROBOT_LENGTH/2;
		PID_setpoint(pid, fDest);

		while ((longF - fDest >= 1) || (longF - fDest <= 1)) {
			Control_updateAngle(oControl);

			j++;
			longF = 0;
			numLongF = 0;
			shortF = 0;
			numShortF = 0;
			for (i = 0; i < 10; i++) {
				tempLongF = Sensor_getLong(kFRONT);
				if (tempLongF != -1) {
					longF += tempLongF;
					numLongF++;
				}
				tempShortF = Sensor_getShort(kFRONT);
				if (tempShortF != -1) {
					shortF += tempShortF;
					numShortF++;
				}
			}
			longF /= numLongF;
			shortF /= numShortF;

			if ((numShortF >= 10)&&(shortF <= 150)) {
				PID_free(pid);
				Control_advanceShort(oControl);
				return 0;
			}

			motorOut = PID_update(pid, longF);
			if (j%2 == 0) {
				Motor_setLeft(motorOut + Control_turnLeftOutput(oControl));
				Motor_setRight(motorOut + Control_turnRightOutput(oControl));
			}
			else {
				Motor_setRight(motorOut + Control_turnRightOutput(oControl));
				Motor_setLeft(motorOut + Control_turnLeftOutput(oControl));
			}
		}
		PID_free(pid);
		return 0;
	}

	else if (numLongB >= 10) {
		numSquareB = Control_estimateNumberOfSquares(longB);
		bDest = GRID_LENGTH*(numSquareB + 1) + 84 - ROBOT_LENGTH/2;
		PID_setpoint(pid, -1 * bDest);

		while ((longB - bDest >= 1) || (longB - bDest <= 1)) {
			Control_updateAngle(oControl);

			j++;

			longB = 0;
			numLongB = 0;
			shortF = 0;
			numShortF = 0;
			for (i = 0; i < 10; i++) {
				tempLongB = Sensor_getLong(kBACK);
				if (tempLongB != -1) {
					longB += tempLongB;
					numLongB++;
				}
				tempShortF = Sensor_getShort(kFRONT);
				if (tempShortF != -1) {
					shortF += tempShortF;
					numShortF++;
				}
			}
			longB /= numLongB;
			shortF /= numShortF;

			if ((numShortF >= 10)&&(shortF <= 150)) {
				PID_free(pid);
				Control_advanceShort(oControl);
				return 0;
			}

			motorOut = PID_update(pid, -1 * longB);
			if (j%2 == 0) {
				Motor_setLeft(motorOut + Control_turnLeftOutput(oControl));
				Motor_setRight(motorOut + Control_turnRightOutput(oControl));
			}
			else {
				Motor_setRight(motorOut + Control_turnRightOutput(oControl));
				Motor_setLeft(motorOut + Control_turnLeftOutput(oControl));
			}
		}
		PID_free(pid);
		return 0;
	}

	PID_free(pid);
	return 0;
}

static void Control_changeHomeAngle(Control_T oControl, double newHome) {
	AC_changeHome(oControl->ac, newHome);
}

void Control_free(Control_T oControl) {

}