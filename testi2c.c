#include <pigpiod_if2.h>
#include <stdio.h>

int main() {
  int pi = pigpio_start(NULL, NULL);
  int bus = 0;
  int addr = 0x6B;
  int handle = i2c_open(pi, bus, addr, 0);
  printf("Handle (in decimal) is %d\n", handle);

  int regaddr = 0x28;
  int data = i2c_read_byte_data(pi, handle, regaddr);
  printf("data is %d\n", data);

  return 0;
}
