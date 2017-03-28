/* motors.c */

#include <motors.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <pigpio.h>

//-------------------

#define LEFT_MOTOR_PIN 12 // PWM channel 0
#define RIGHT_MOTOR_PIN 13 // PWM channel 1
#define LEFT_FORWARD 17
#define LEFT_BACKWARD 27
#define RIGHT_FORWARD 23
#define	RIGHT_BACKWARD 24
#define MOTOR_RANGE 255

//-------------------

double leftPercent = 0;
double rightPercent = 0;

int Motor_init() {
	leftVoltage = 0;
	rightVoltage = 0;
	gpioWrite(LEFT_FORWARD, 0);
	gpioWrite(LEFT_BACKWARD, 0);
	gpioWrite(RIGHT_FORWARD, 0);
	gpioWrite(RIGHT_BACKWARD, 0);
	gpioWrite(LEFT_MOTOR_PIN, 0);
	gpioWrite(RIGHT_MOTOR_PIN, 0);
	gpioSetPWMrange(LEFT_MOTOR_PIN, MOTOR_RANGE);
	gpioSetPWMrange(RIGHT_MOTOR_PIN, MOTOR_RANGE);
	return 0;
}

double Motor_getLeft() {
	return leftPercent;
}

double Motor_getRight() {
	return rightPercent;
}


int Motor_setLeft(double left) {
	leftPercent = left;
	if (left >= 0) {
		gpioWrite(LEFT_BACKWARD, 0);
		gpioWrite(LEFT_FORWARD, 1);
		gpioPWM(LEFT_MOTOR_PIN, (left * MOTOR_RANGE / 100.0));
	}
	else {
		gpioWrite(LEFT_FORWARD, 0);
		gpioWrite(LEFT_BACKWARD, 1);
		gpioPWM(LEFT_MOTOR_PIN, (left * MOTOR_RANGE / -100.0));
	}
	return 0;
}

int Motor_setRight(double right) {
	rightPercent = right;
	if (right >= 0) {
		gpioWrite(RIGHT_BACKWARD, 0);
		gpioWrite(RIGHT_FORWARD, 1);
		gpioPWM(RIGHT_MOTOR_PIN, (right * MOTOR_RANGE / 100.0));
	}
	else {
		gpioWrite(RIGHT_FORWARD, 0);
		gpioWrite(RIGHT_BACKWARD, 1);
		gpioPWM(RIGHT_MOTOR_PIN, (right * MOTOR_RANGE / -100.0));
	}
	return 0;
}

/* Utility Function, to input array */
int Motor_set(int* motors) {
	int ret = Motor_setLeft(motors[0]);
	ret |= Motor_setRight(motors[1]);
	return ret;
}

void Motor_free() {

}