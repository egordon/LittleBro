#include <matrix.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <motors.h>
#include <control.h>
#include <pigpiod_if2.h>
#include <sensors.h>

int main(int argc, char** argv) {
	int i = 0;
	sleep(5);
	VEC *x;
	x = v_get(10);
	v_output(x);

	int pifd = pigpio_start(NULL, NULL);

	Motor_init(pifd);
	Motor_setLeft(50);
	Motor_setRight(-50);

	gpioSleep(0, 3, 0);

	Motor_setLeft(-50);
	Motor_setRight(50);

	gpioSleep(0, 3, 0);

	Motor_setLeft(50);
	Motor_setRight(50);

	gpioSleep(0, 3, 0);

	Motor_setLeft(0);
	Motor_setRight(0);

	while(1) {
		i++;
		printf("Reading #%d, Gyro: %f, Compass: %f", i, Sensor_getGyro(), Sensor_getCompass());
		gpioSleep(0, 0, 500000);
	}

	pigpio_stop(pifd);

	return EXIT_SUCCESS;
}
