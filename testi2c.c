#include <pigpiod_if2.h>
#include <stdio.h>
#include <unistd.h>

int main() {
  int temp, i, j;
  int pi = pigpio_start(NULL, NULL);
  int bus = 1;
  int addr = 0x1D; // CHANGED
  int handleA = i2c_open(pi, bus, addr, 0);
  printf("HandleA (in decimal) is %d\n", handleA);

  /*int CTRL1 = 0x20;
  int CTRL4 = 0x23;
  int LOWODR = 0x39;*/

  int CTRL5 = 0x24;
  int CTRL6 = 0x25;
  int CTRL7 = 0x26;
  int t;
  // these init values are taken from the pololu library
  i2c_write_byte_data(pi, handleA, CTRL5, 0x64); // pololu has 0x6f
  t = i2c_read_byte_data(pi, handleA, CTRL5);
  printf("t = %d\n", t);
  i2c_write_byte_data(pi, handleA, CTRL6, 0x20); // highest resolution
  i2c_write_byte_data(pi, handleA, CTRL7, 0x00);

  unsigned char buffer[32];

  int regaddr = 0x08; // CHANGED
  int data;
  while(1) {
      j++;
      buffer[6] = 42; // Sentinel to check whether it's being touched
      // yes, I think it's strange we're writing to the output register
      // i2c_write_byte_data(pi, handleA, regaddr, 0x80);
      temp = i2c_read_i2c_block_data(pi, handleA, regaddr | 0x80, buffer, 6);
      if (temp != 6) {
        printf("ERROR: read_block_data returns %d\n", temp);
      }
      printf("j = %d\n", j);
      time_sleep(0.5);

      for(i=0;i<7;i++){
        printf("%d: \t%d\n", i, buffer[i]);
      }
      printf("\n");
  }
    
    
    /*for(i=0; i<10; i++){
  	data = i2c_read_byte_data(pi, handleA, regaddr+i);
  	printf("A, %x: data is %d = %x\n", regaddr+i, data, regaddr+i);
  	time_sleep(.1);
    }
    printf("\n");*/
    
  //handle = i2c_close(pi, bus);

  return 0;
}
