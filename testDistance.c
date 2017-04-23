#include <stdio.h>
//#include <sensors.h>
#include <adafruit_distance.h>
#include <pigpiod_if2.h>
#include <sensors.h>

int main(){
  int pi = pigpio_start(NULL, NULL);
  int i2c_address = 0x29;
  int i2c_bus = 1;
  int handle = i2c_open(pi, i2c_bus, i2c_address, 0);
  
  adafuit_distance_set_pi_handle(pi);
  adafruit_distance_begin(handle); // calls loadSetings

  int i=0;
  while(1) {
    int x = adafruit_distance_readRange(handle);
    printf("Dist is %d\n", x);
  }
  
  return 0;
}
