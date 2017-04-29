/* testkalman.c */

/* kalman.c */

/*

F is the 2x2 state transition model matrix
B is the 2x1 control input model vector (because input is a scalar)
H is the 2x2 observation model (probably identity matrix)
Q is the process noise covariance
R is the observation noise covariance

the initial state is assumed to be [0; 0]
input file format should be:

Matrix: 2 by 2
row 1: F00 F01
row 2: F10 F11
Vector: dim: 2
B0 B1
Matrix: 2 by 2
row 1: H00 H01
row 2: H10 H11
Matrix: 2 by 2
row 1: Q00 Q01
row 2: Q10 Q11
Matrix: 2 by 2
row 1: R00 R01
row 2: R10 R11

positionMeasurement1 velocityMeasurement1 deltaTime1 input1
positionMeasurement2 velocityMeasurement2 deltaTime2 input2
positionMeasurement3 velocityMeasurement3 deltaTime3 input3
positionMeasurement4 velocityMeasurement4 deltaTime4 input4
...


a

*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <kalman.h>
#include <matrix.h>
#include <sensors.h>
#include <motors.h>
#include <pigpiod_if2.h>

//-----------------

static void setMatVals(MAT *m, double v00, double v01, double v10, double v11) {
	m_set_val(m, 0, 0, v00);
	m_set_val(m, 0, 1, v01);
	m_set_val(m, 1, 0, v10);
	m_set_val(m, 1, 1, v11);
}

static void setVecVals(VEC *v, double v0, double v1) {
	v_set_val(v, 0, v0);
	v_set_val(v, 1, v1);
}

int main() {
	int pi = pigpio_start(NULL, NULL);
	double pos, vel, deltaT, input;
	double prevTime = time_time(), currentTime;
	MAT *F = m_get(2,2);
	VEC *B = v_get(2);
	MAT *H = m_get(2,2);
	MAT *Q = m_get(2,2);
	MAT *R = m_get(2,2);
	VEC *measurement = v_get(2);
	Kalman_T kal;

	Sensor_init(pi);

	setMatVals(F, 1, 0.1, 0, 1);
	setVecVals(B, 0, 3.944);
	setMatVals(H, 1, 0, 0, 1);
	setMatVals(Q, 0.01, 0, 0, 0.2);
	setMatVals(R, 0.05, 0, 0, 0.2);
	kal = Kalman_init(F, B, H, Q, R);

	while(1) {
		pos = Sensor_getCompass();
		vel = Sensor_getGyro();
		v_set_val(measurement, 0, pos);
		v_set_val(measurement, 1, vel);
		currentTime = time_time();
		deltaT = currentTime - prevTime;
		prevTime = currentTime;
		input = 0;
		Kalman_update(kal, measurement, input, deltaT);
		printf("Input: Position %.2f, Velocity %.2f, DeltaTime %.2f, Input (Motors) %.2f\n", pos, vel, deltaT, input);
		printf("The resulting Kalman state estimate is: %f \n\n", Kalman_get(kal));
		time_sleep(1);
	}

	M_FREE(F); V_FREE(B); M_FREE(H); M_FREE(Q); M_FREE(R); V_FREE(measurement); Kalman_free(kal);

}