/* kalman.c */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <kalman.h>
#include <matrix.h>
#include <assert.h>
#include <matrix2.h>

#define TWO_PI (2*3.1415926536)

//------------------

struct Kalman {

	MAT *stateTransM; // F_k
	VEC *controlInputM; // B_k
	MAT *observationM; // H_k
	MAT *errorCov; // P_k,k
	VEC *stateEstimate; // current state estimate
	MAT *processNoiseCov; // Q_k
	MAT *observationNoiseCov; // R_k

};

Kalman_T Kalman_init(MAT* stateTransModel, VEC* controlInputModel,
	MAT* observationModel, MAT* processNoiseCovariance,
	MAT* observationNoiseCovariance) {
	Kalman_T oKalman;
	MAT *initErrorCovariance = m_get(2,2);

	assert((stateTransModel->m == 2)&&(stateTransModel->n == 2));
	assert((observationModel->m == 2)&&(observationModel->n == 2));
	assert((processNoiseCovariance->m == 2)&&(processNoiseCovariance->n == 2));
	assert((observationNoiseCovariance->m == 2)&&(observationNoiseCovariance->n == 2));
	assert(controlInputModel->dim == 2);

	m_zero(initErrorCovariance);

	oKalman = (struct Kalman*) malloc(sizeof(struct Kalman));
	if (oKalman == NULL) return NULL;

	oKalman->stateTransM = stateTransModel;
	oKalman->controlInputM = controlInputModel;
	oKalman->observationM = observationModel;
	oKalman->errorCov = initErrorCovariance;
	oKalman->stateEstimate = v_get(2);
	v_zero(oKalman->stateEstimate); // set stateEstimate to [0; 0]
	oKalman->processNoiseCov = processNoiseCovariance;
	oKalman->observationNoiseCov = observationNoiseCovariance;

	return oKalman;
}

// update the Kalman filter by passing in a measurement and the voltage being passed to the motors
// may need to change input to a vec if very non-linear relationship between motor voltage and speed
// currently, we assume input is the difference between the right/left motor voltage

// Note from Changyan: in order to account for the fact that angles range from 0 to TWO_PI, I have "modded"
// the angle values in aPrioriEstimate and measureResidual below. These may or may not be all the things
// that need to be modded. measurement does not need to be modded because it will be already modded
// in the sensors.c function when read from Gyro/Compass
void Kalman_update(Kalman_T kalman, VEC* measurement, double input, double deltaT) {
	VEC *aPrioriEstimate = v_get(2);
	MAT *aPrioriCov = m_get(2, 2);
	VEC *measureResidual = v_get(2);
	MAT *residualCov = m_get(2,2);
	MAT *kalmanGain = m_get(2,2);

	MAT *pivotM1 = m_get(2, 2);
	MAT *pivotM2 = m_get(2, 2);
	MAT *pivotM3 = m_get(2, 2);
	MAT *identityM = m_get(2, 2);
	VEC *pivotV1 = v_get(2);
	VEC *pivotV2 = v_get(2);
	VEC *pivotV3 = v_get(2);
	double modPivot;

	assert(measurement->dim == 2);

	m_ident(identityM); // set identityM to be an identity matrix

	m_set_val(kalman->stateTransM, 0, 1, deltaT); // Change F to reflect deltaT

	// Predicted (a priori) state estimate  [aPrioriEstimate]
	mv_mlt(kalman->stateTransM, kalman->stateEstimate, pivotV1); // pivotV1 = stateTransM * stateEstimate
	sv_mlt(deltaT, pivotV1, pivotV2); // pivotV2 = pivotV1 * deltaT
	sv_mlt(input, kalman->controlInputM, pivotV3); // pivotV3 = input * controlInputM
	v_add(pivotV2, pivotV3, aPrioriEstimate); // aPrioriEstimate = pivotV2 + pivotV3
	modPivot = v_get_val(aPrioriEstimate, 0);
	while (modPivot < 0) {
		modPivot += TWO_PI;
	}
	while (modPivot >= TWO_PI) {
		modPivot -= TWO_PI;
	}
	v_set_val(aPrioriEstimate, 0, modPivot);
	printf("aPrioriEstimate:"); v_output(aPrioriEstimate);

	// Predicted (a priori) estimate covariance  [aPrioriCov]
	// aPrioriCov = (F_k)(P_k-1,k-1)(F_k^t) + Q_k
	m_mlt(kalman->stateTransM, kalman->errorCov, pivotM1);
	m_transp(kalman->stateTransM, pivotM2);
	m_mlt(pivotM1, pivotM2, pivotM3); // pivotM3 = (F_k)(P_k-1,k-1)(F_k^t)
	m_add(pivotM3, kalman->processNoiseCov, aPrioriCov);
	printf("aPrioriCov:"); m_output(aPrioriCov);

	// Innovation or measurement residual  [measureResidual]
	mv_mlt(kalman->observationM, aPrioriEstimate, pivotV1);
	sv_mlt(-1, pivotV1, pivotV2);
	v_add(pivotV2, measurement, measureResidual);
	modPivot = v_get_val(measureResidual, 0);
	while (modPivot < 0) {
		modPivot += TWO_PI;
	}
	while (modPivot >= TWO_PI) {
		modPivot -= TWO_PI;
	}
	v_set_val(measureResidual, 0, modPivot);
	printf("measureResidual:"); v_output(measureResidual);

	// Innovation (or residual) covariance [residualCov]
	m_mlt(kalman->observationM, aPrioriCov, pivotM1);
	m_transp(kalman->observationM, pivotM2);
	m_mlt(pivotM1, pivotM2, pivotM3); // pivotM3 = (F_k)(P_k-1,k-1)(F_k^t)
	m_add(pivotM3, kalman->observationNoiseCov, residualCov);

	// optimal Kalman gain  [kalmanGain]
	m_transp(kalman->observationM, pivotM1);
	m_mlt(aPrioriCov, pivotM1, pivotM2);
	m_inverse(residualCov, pivotM3);
	m_mlt(pivotM2, pivotM3, kalmanGain);
	printf("kalmanGain:"); m_output(kalmanGain);

	// updated (a posteriori) state estimate
	mv_mlt(kalmanGain, measureResidual, pivotV1);
	v_add(pivotV1, aPrioriEstimate, kalman->stateEstimate);
	printf("kalman->stateEstimate:"); v_output(kalman->stateEstimate);

	// updated (a posteriori) estimate covariance
	m_mlt(kalmanGain, kalman->observationM, pivotM1);
	sm_mlt(-1, pivotM1, pivotM2);
	m_add(pivotM2, identityM, pivotM3);
	m_mlt(pivotM3, aPrioriCov, kalman->errorCov);

	V_FREE(aPrioriEstimate);
	M_FREE(aPrioriCov);
	V_FREE(measureResidual);
	M_FREE(residualCov);
	M_FREE(kalmanGain);

	M_FREE(pivotM1);
	M_FREE(pivotM2);
	M_FREE(pivotM3);
	M_FREE(identityM);
	V_FREE(pivotV1);
	V_FREE(pivotV2);
	V_FREE(pivotV3);
}

double Kalman_get(Kalman_T kalman) {
	return v_get_val(kalman->stateEstimate, 0);
}

void Kalman_free(Kalman_T oKalman) {
	M_FREE(oKalman->stateTransM);
	V_FREE(oKalman->controlInputM);
	M_FREE(oKalman->observationM);
	M_FREE(oKalman->errorCov);
	V_FREE(oKalman->stateEstimate);
	M_FREE(oKalman->processNoiseCov);
	M_FREE(oKalman->observationNoiseCov);
	free(oKalman);
}