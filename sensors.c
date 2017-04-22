#include <math.h>
#include <sensors.h>
#include <pigpiod_if2.h>
#include <adafruit_distance.h>
#include <stdio.h>

// const int SDA_A = 1000
// const int SCL_A = 1000
// const int SDA_B = 1000
// const int SCL_B = 1000

/* Some quick bus info
0x6B gyro
0x1E Magnetometer (if that's wrong, try 1f)
0x29 Short distance
0x52 Long distance


BUS 0 (Pi pins 27 SDA, 28 SCL)
Gyro/a/m, long, short

BUS 1 (Pi pins 3 SDA, 5 SCL)
long, short, short
*/

#define TWO_PI (2*3.1415926536)

const int BUS = 0;
const int GYRO_ADDR = 0x6B; // 7 bit 1101011;
const int GYRO_BUS = 0; // check http://abyz.co.uk/rpi/pigpio/python.html#i2c_open
const int COMPASS_ADDR = 0x1E; // if it's wrong, try 1F
const int COMPASS_BUS = 0;
const int SHORT_ADDR_A = -1; // The original address
const int SHORT_ADDR_B = -1;
const int SHORT_ADDR_C = -1;
const int LONG_ADDR_A = -1;  // The original address
const int LONG_ADDR_B = -1;

const int SHORT_SHUTDOWN_A = -1;
const int SHORT_SHUTDOWN_B = -1;
const int LONG_SHUTDOWN_A = -1;


// The datasheet gives 8.75mdps/digit for default sensitivity
const double RPS_PER_DIGIT = 0.00875*360/TWO_PI;

enum {     
  GYRO_REGISTER_OUT_X_L             = 0x28,   //            r
  GYRO_REGISTER_OUT_X_H             = 0x29,   //            r
  GYRO_REGISTER_OUT_Y_L             = 0x2A,   //            r
  GYRO_REGISTER_OUT_Y_H             = 0x2B,   //            r
  GYRO_REGISTER_OUT_Z_L             = 0x2C,   //            r
  GYRO_REGISTER_OUT_Z_H             = 0x2D,   //            r

  ACC_REGISTER_OUT_X_L_A         = 0x28,
  ACC_REGISTER_OUT_X_H_A         = 0x29,
  ACC_REGISTER_OUT_Y_L_A         = 0x2A,
  ACC_REGISTER_OUT_Y_H_A         = 0x2B,
  ACC_REGISTER_OUT_Z_L_A         = 0x2C,
  ACC_REGISTER_OUT_Z_H_A         = 0x2D,

  COMPASS_REGISTER_OUT_X_H_M     = 0x03, // HEY! if these values are wrong, try adding 5 to each
  COMPASS_REGISTER_OUT_X_L_M     = 0x04, // There's another version of this chip with those addresses
  COMPASS_REGISTER_OUT_Y_H_M     = 0x05,
  COMPASS_REGISTER_OUT_Y_L_M     = 0x06,
  COMPASS_REGISTER_OUT_Z_H_M     = 0x07,
  COMPASS_REGISTER_OUT_Z_L_M     = 0x08,
};

static double compassOffset = 0;

static int gyro_handle = 0;
static int compass_handle = 0;
static int short_A_handle = 0;
static int short_B_handle = 0;
static int short_C_handle = 0;
static int long_A_handle = 0;
static int long_B_handle = 0;

static int pi;

/* Local Functions */
static double getCompassRaw(){
  int16_t xRaw = i2c_read_word_data(pi, compass_handle, COMPASS_REGISTER_OUT_X_L_M);
  int16_t yRaw = i2c_read_word_data(pi, compass_handle, COMPASS_REGISTER_OUT_Y_L_M);

  double angle = atan2(yRaw, xRaw);
  printf("yRaw: %d", yRaw);
  return xRaw;
}

/**
  * Initialize Sensors
  * Return 1 on Success.
  * Return 0 on Failure
 **/
int Sensor_init(int pifd){
  pi = pifd;
  adafruit_distance_set_pi_handle(pi);
  
  /*short_A_handle = i2c_open(pi, BUS, SHORT_ADDR_A, 0);
  short_B_handle = i2c_open(pi, BUS, SHORT_ADDR_B, 0);
  short_C_handle = i2c_open(pi, BUS, SHORT_ADDR_C, 0);
  
  // change address of short range
  gpio_write(pi, SHORT_SHUTDOWN_A, 1); // TODO is 1 alive or dead?
  gpio_write(pi, SHORT_SHUTDOWN_B, 1);
  adafruit_distance_begin(short_A_handle);
  adafruit_distance_change_address(short_A_handle, SHORT_ADDR_C);
  
  gpio_write(pi, SHORT_SHUTDOWN_B, 0);
  adafruit_distance_begin(short_A_handle);
  adafruit_distance_change_address(short_A_handle, SHORT_ADDR_B);
  
  gpio_write(pi, SHORT_SHUTDOWN_A, 0);
  adafruit_distance_begin(short_A_handle);*/
  
  gyro_handle = i2c_open(pi, GYRO_BUS, GYRO_ADDR, 0);
  //acc_handle = i2c_open(ACC_BUS, ACC_ADDRESS);
  //compass_handle = i2c_open(pi, COMPASS_BUS, COMPASS_ADDR, 0);
  //Sensors_calCompass();
  return 1; //success
}

/* Return angle in radians or radians/s. */

double Sensor_getGyro(){
	// TODO: Choose an axis by changing this letter:                    V
	int16_t raw = i2c_read_word_data(pi, gyro_handle, GYRO_REGISTER_OUT_X_L);
  printf("raw is %d\n", raw);
	// needs to be 16 bits signed so the signs work out correctly

	return raw * RPS_PER_DIGIT;
}

double Sensor_getCompass(){
	double raw = getCompassRaw();
	return fmod(((raw - compassOffset) + TWO_PI), TWO_PI);
}

/* Calibrates Current angle as "0" */
void Sensor_calCompass(){
        // TODO: consider averaging over multiple readings
	compassOffset = getCompassRaw();
}

/* Return distance in cm */
double Sensor_getShort(enum Dir_t dir);
double Sensor_getLong(enum Dir_t dir);

/* Any Cleanup */
void Sensor_free(){
  i2c_close(pi, gyro_handle);
  i2c_close(pi, compass_handle);

  // TODO: not sure whether it's a good idea to reset addresses before shutting down
  i2c_close(pi, short_A_handle);
  i2c_close(pi, short_B_handle);
  i2c_close(pi, short_B_handle);
}
