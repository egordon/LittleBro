#include <matrix.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <motors.h>
#include <control.h>
#include <pigpiod_if2.h>

int main(int argc, char** argv) {
	Control_T oControl;
	int i;
	sleep(5);
	VEC *x;
	x = v_get(10);
	v_output(x);

	int pifd = pigpio_start(NULL, NULL);
	Sensor_init(pifd);
	Sensor_calCompass();
	oControl = Control_init(pifd);

	while (1) {
		i++;
		Control_updateAngle(oControl);
		printf("Gyro: %f, Compass: %f\n", Sensor_getGyro(), Sensor_getCompass());
		gpioSleep(0, 0, 200000);
		if (i == 20) {
			// 4 seconds have elapsed
			Control_changeHomeAngle(1.5);
		}
		if (i == 40) {
			// 8 secs have passed
			Control_changeHomeAngle(3.0);
		}
	}

	return EXIT_SUCCESS;
}
