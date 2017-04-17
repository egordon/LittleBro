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
	//sleep(5);

	int pifd = pigpio_start(NULL, NULL);
	Sensor_init(pifd);
	
	printf("test 1\n");
	Motor_init(pifd);
	Motor_setLeft(50);
	Motor_setRight(-50);

	time_sleep(3.0);

	printf("test2\n");
	Motor_setLeft(-25);
	Motor_setRight(25);

	time_sleep(3.0);

	Motor_setLeft(10);
	Motor_setRight(10);

	time_sleep(6.0);

	Motor_setLeft(0);
	Motor_setRight(0);

	while(1) {
		i++;
		printf("Reading #%d, Gyro: %f, Compass: \n", i, Sensor_getGyro());
		time_sleep(0.5);
	}

	pigpio_stop(pifd);

	return EXIT_SUCCESS;
}
