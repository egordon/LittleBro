#include <stdio.h>
//#include <sensors.h>
#include <adafruit_distance.h>
#include <pigpiod_if2.h>
#include <sensors.h>
#include <unistd.h>

static int handle;
static int pi;

uint32_t ReadByte(uint16_t reg) {
  char dataWrite[2];
  uint8_t dataRead[1];

  dataWrite[0] = (reg >> 8) & 0xFF;
  dataWrite[1] = reg & 0xFF;
  int w = i2c_write_device(pi, handle, dataWrite, 2);
  int r = i2c_read_device(pi, handle, dataRead, 1);

  printf("Write Code: %d\n", w);
  printf("Read Code: %d\n", r);

  return (uint32_t)dataRead[0];
}

int main(){
  pi = pigpio_start(NULL, NULL);
  Sensor_init(pi);

  while(1) {
    int i,j,k;
    i = Sensor_getShort(0);
    j = Sensor_getShort(1);
    k = Sensor_getShort(2);

    printf("i: %d \tj: %d \tk: %d\n", i, j, k);
    time_sleep(.1);
  }




  /*
  double x;
  Sensor_init(pi);

  int i=0;
  while(1) {
    x = Sensor_getCompass();
    printf("Angle is %.2f\n", x);
    time_sleep(.2);
    */

  /*
  unsigned int i2c_address = 0x29;
  unsigned int i2c_bus = 1;
  handle = i2c_open(pi, i2c_bus, i2c_address, 0);
  printf("handle = %d\n", handle); fflush(stdout);
  int addr = 0x000;
  for(addr = 0x000; addr < 0x00F; addr++) {
    uint32_t byte = ReadByte(addr);
    printf("byte %X = %X\n", addr, byte);
  }

  addr = 0x016;
  uint32_t byte = ReadByte(addr);
  printf("byte %X = %X\n", addr, byte);

  i2c_close(pi, handle);

  return 0;

  adafruit_distance_set_pi_handle(pi);
  adafruit_distance_begin(handle); // calls loadSettings

  int i=0;
  while(1) {
    int x = adafruit_distance_readRange(handle);
    printf("Dist is %d\n", x);
    
  }
  */
  
  return 0;
}
