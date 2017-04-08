#ifndef ANGLE_CTRL_H
#define ANGLE_CTRL_H

typedef struct AngleState *AngleState_T;

AngleState_T AC_init();

/**
  * @param angle: Angle in Radians
  * @param dAngle: Angular Velocity in Radians/s
  * @param dt: Time difference from last update.

  **/
double AC_update(AngleState_T ac, double angle, double dAngle, double dt, double inputDiff);

void AC_change(AngleState_T ac, double newHome);

void AC_free();

#endif