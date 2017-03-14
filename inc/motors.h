#ifndef MOTOR_H
#define MOTOR_H

/* Motor Dirver */

/**
  * Initialize Motors
  * Return 1 on Success.
  * Return 0 on Failutr
 **/
int Motor_init();

double Motor_getLeft();
double Motor_getRight();

int Motor_setLeft(double left);
int Motor_setRight(double right);

/* Utility Function, to input array */
int Motor_set(int* motors) {
	int ret = Motor_setLeft(motors[0]);
	ret &= Motor_setRight(motors[1]);
	return ret;
}

/* Any Cleanup */
void Motor_free();


#endif