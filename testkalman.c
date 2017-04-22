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

//-----------------

int main() {
	double pos, vel, deltaT, input;
	MAT *F = m_get(2,2);
	VEC *B = v_get(2);
	MAT *H = m_get(2,2);
	MAT *Q = m_get(2,2);
	MAT *R = m_get(2,2);
	VEC *measurement = v_get(2);
	Kalman_T kal;

	m_input(F);
	v_input(B);
	m_input(H);
	m_input(Q);
	m_input(R);
	kal = Kalman_init(F, B, H, Q, R);

	while (scanf("%lf %lf %lf %lf", &pos, &vel, &deltaT, &input) == 4 && !feof(stdin)) {
		v_set_val(measurement, 0, pos);
		v_set_val(measurement, 1, vel);
		Kalman_update(kal, measurement, input, deltaT);
		printf("Input: Position %f, Velocity %f, DeltaTime %f, Input (Motors) %f", pos, vel, deltaT, input);
		printf("The resulting Kalman state estimate is: %f \n", Kalman_get(kal));
	}

}