#include <stdio.h>
#include <sensors.h>
#include <pigpiod_if2.h>
#include <sensors.h>

int main(){
  int pi = pigpio_start(NULL, NULL);
  Sensor_init(pi);

  int i=0;
  while(1) {
    int x = Sensor_getShort();
    printf("Dist is %d\n", x);
  }
  
  return 0;
}
