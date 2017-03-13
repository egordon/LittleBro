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

#include <pigpio.h>

static double homeAngle = 0; /* desired angle */
static VEC *angleState;

/* functions to read input from the gyroscope and compass */



/* function to run Kalman filter, given the previous state */
AngleState_T Kalman(AngleState_T prev) {

}

/* function to return the correct voltage that should be
	sent to the right motor, given the current angle*/
int rightMotorVoltage(double currentAngle) {

}
