#include <pigpiod_if2.h>
#include <stdio.h>

int main() {
  int pi = pigpio_start(NULL, NULL);
  int bus = 1;
  int addr = 0x6B;
  int handleA = i2c_open(pi, bus, addr, 0);
  printf("HandleA (in decimal) is %d\n", handleA);

  int CTRL1 = 0x20;
  int CTRL4 = 0x23;
  int LOWODR = 0x39;
  // these init values are taken from the pololu library
  i2c_write_byte_data(pi, handleA, CTRL1, 0x6F);
  i2c_write_byte_data(pi, handleA, CTRL1, 0x00);
  i2c_write_byte_data(pi, handleA, CTRL1, 0x00);




  int regaddr = 0x28;
  int data;
  while (1) {
    int i = 0;

    for(i=0; i<10; i++){
  	data = i2c_read_byte_data(pi, handleA, regaddr+i);
  	printf("A, %x: data is %d = %x\n", regaddr+i, data, regaddr+i);
  	time_sleep(.1);
    }
    printf("\n");
  }
  //handle = i2c_close(pi, bus);

  return 0;
}
