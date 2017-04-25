#ifndef WALL_CTRL_H
#define WALL_CTRL_H

typedef struct WallState_T *WallState;

WallState_T WC_init();

/**
  * @param leftDist: Distance to left wall (or -1 if no signal)
  * @param rightDist: Distance to right wall (or -1 if no signal)
  * @param currentAngle: The current angle, as estimated by the Kalman filter.

  **/

// returns the motor difference that should be sent to motors
double WC_update(WallState_T wc, double leftDist, double rightDist, double currentAngle);

void WC_free(WallState_T wc);

#endif