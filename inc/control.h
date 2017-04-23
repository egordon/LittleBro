/* control.h */

#ifndef CONTROL_H
#define CONTROL_H

typedef struct Control *Control_T;

Control_T Control_init(int pifd);

// updates the AngleState object inside of oControl, by reading the gyro and compass
// then using the AC_update() function from angleControl.c
void Control_updateAngle(Control_T oControl);

// return the current output to the right motor
double Control_getRightOutput(Control_T oControl);

double Control_getLeftOutput(Control_T oControl);

// this is absolute turning, not relative turning
void Control_changeHomeAngle(Control_T oControl, double newHome);

// turn for secs seconds
// these are absolute turns, not relative turns
// I can write relative turns fairly easily, I think. IDK if they are useful, though.
void Control_turnNorth(Control_T oControl, double secs);
void Control_turnEast(Control_T oControl, double secs);
void Control_turnSouth(Control_T oControl, double secs);
void Control_turnWest(Control_T oControl, double secs);

// eventually add things like Control_changeHomeGrid

void Control_free(Control_T oControl);

#endif