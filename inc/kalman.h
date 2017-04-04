/* kalman.h */

#include <matrix.h>

#ifndef KALMAN_H
#define KALMAN_H

typedef struct Kalman *Kalman_T;

// state transition model (F) should make newVelocity=0, newPosition=oldVelocity*deltaT
// control input model (B) should be a 2x1 vector, like [0 x]^t, and u_k should be a constant related to the
// difference between the motor voltages (this model assumes that the relationship between voltage and
// change in velocity is linear. If this is false, we need to change this model into perhaps a function)
// B should be populated (x should be set) based on empirical data from measuring angular velocity based on 
// voltages
// observationModel (H) is identity matrix
// initErrorCovariance should be zero matrix
Kalman_T Kalman_init(MAT* stateTransModel, VEC* controlInputModel,
	MAT* observationModel, MAT* initErrorCovariance, MAT* processNoiseCovariance,
	MAT* observationNoiseCovariance);

// update the Kalman filter by passing in a measurement and the voltage being passed to the motors
// may need to change input to a vec if very non-linear relationship between motor voltage and speed
// currently, we assume input is the difference between the right/left motor voltage
void Kalman_update(Kalman_T kalman, VEC* measurement, double input, double deltaT);

// return the current value predicted by the filter
double Kalman_get(Kalman_T kalman);

void Kalman_free();

#endif