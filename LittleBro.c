#include <matrix.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <motors.h>
#include <pigpiod_if2.h>

int main(int argc, char** argv) {
	sleep(5);
	VEC *x;
	x = v_get(10);
	v_output(x);

	int pifd = pigpio_start(NULL, NULL);

	Motor_init(pifd);
	Motor_setLeft(50);
	Motor_setRight(-50);

	sleep(6);

	Motor_setLeft(50);
	Motor_setRight(-50);

	sleep(6);

	Motor_setLeft(50);
	Motor_setRight(50);

	sleep(3);

	Motor_setLeft(0);
	Motor_setRight(0);

	pigpio_stop(pifd);

	return EXIT_SUCCESS;
}
