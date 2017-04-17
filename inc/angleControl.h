#ifndef ANGLE_CTRL_H
#define ANGLE_CTRL_H

typedef struct AngleState *AngleState_T;

AngleState_T AC_init();

/**
  * @param angle: Angle in Radians
  * @param dAngle: Angular Velocity in Radians/s
  * @param dt: Time difference from last update.

  **/

// returns the motor difference that should be sent to motors
double AC_update(AngleState_T ac, double angle, double dAngle, double dt, double inputDiff);

void AC_changeHome(AngleState_T ac, double newHome);

double AC_getAngle(AngleState_T ac);

void AC_free(AngleState_T ac);

#endif