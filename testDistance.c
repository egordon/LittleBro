#include <stdio.h>
#include <sensors.h>
#include <pigpiod_if2.h>
#include <sensors.h>

int main(){
  int pi = pigpio_start(NULL, NULL);
  double x;
  Sensor_init(pi);

  int i=0;
  while(1) {
    x = Sensor_getCompass();
    printf("Angle is %.2f\n", x);
    time_sleep(.2);
  }
  
  return 0;
}
