#include <matrix.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <motors.h>
#include <control.h>
#include <pigpiod_if2.h>

int main(int argc, char** argv) {
	Control_T oControl;
	int pifd = pigpio_start(NULL, NULL);
	
	Sensor_init(pifd);
	oControl = Control_init(pifd);

	while (1) {
		i++;
		Control_updateAngle(oControl);
		printf("Gyro: %f, Compass: %f\n", Sensor_getGyro(), Sensor_getCompass());
		time_sleep(1);
		if (i == 4) {
			// 4 seconds have elapsed
			Control_turnNorth(oControl, 3.0);
		}
		if (i == 8) {
			Control_turnEast(oControl, 3.0);
		}
		if (i == 12) {
			Control_turnSouth(oControl, 2.0);
		}
		if (i == 16) {
			Control_turnWest(oControl, 4.0);
		}

		if (i == 25) {
			break;
		}
	}

	return EXIT_SUCCESS;
}
