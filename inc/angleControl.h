#ifndef ANGLE_CTRL_H
#define ANGLE_CTRL_H

typedef struct AngleState {} AngleState_T;

int AC_init();

/**
  * @param angle: Angle in Radians
  * @param dAngle: Angular Velocity in Radians/s
  * @param dt: Time difference from last update.
  **/
double AC_update(double angle, double dAngle, double dt);

void AC_free();

#endif