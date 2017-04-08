/* control.h */

#ifndef CONTROL_H
#define CONTROL_H

typedef struct Control *Control_T;

Control_T Control_init();

// updates the AngleState object inside of oControl, by reading the gyro and compass
// then using the AC_update() function from angleControl.c
// then calls Control_getRightOutput() to aggregate the different control setups
// Then, outputs the correct voltage to the motors
// TODO: Fix the dt value being used!!
void Control_updateAngle(Control_T oControl);

// return the current output to the right motor
double Control_getRightOutput(Control_T oControl);

double Control_getLeftOutput(Control_T oControl);

void Control_changeHomeAngle(Control_T oControl, double newHome);

// eventually add things like Control_changeHomeGrid

void Control_free(Control_T oControl);

#endif