#ifndef READACC_H_
#define READACC_H_

/*******************************************************
 * acc.h
 * Support for interfacing with the ADXL345 accelerometer
 * on the Orbit BoosterPack.
 *
 * FitnessMonGroup8
 * S. Allen, J. Zhu, G. Lay
 * Based on code by C.P. Moore
 * Last modified:  24/02/2020
 *
*******************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_i2c.h"
#include "driverlib/pin_map.h" //Needed for pin configure
#include "driverlib/systick.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "utils/ustdlib.h"
#include "i2c_driver.h"
#include <circBufT.h>

// Structure definition for 3D vectors
typedef struct{
    int16_t x;
    int16_t y;
    int16_t z;
} vector3_t;

// Buffer of size BUF_SIZE integers (sample values) for each axis x, y and z
//static circBuf_t* inBuffer; // Buffer of size BUF_SIZE integers (sample values)
circBuf_t inBuffer_x;
circBuf_t inBuffer_y;
circBuf_t inBuffer_z;

// Buffer of size BUF_SIZE integers (sample values) for the ADC (CURRENTLY NOT IN USE)
circBuf_t inBuffer;

// Definition of unit modes
#define UNITS_RAW 0;
#define UNITS_GRAV 1;
#define UNITS_MPS 2;

// Accl Interrupt Pins
#define ACCL_INT1Port       GPIO_PORTB_BASE
#define ACCL_INT2Port       GPIO_PORTE_BASE
#define ACCL_INT1           GPIO_PIN_4
#define ACCL_INT2           GPIO_PIN_4

#define ACCL                2
#define ACCL_ADDR           0x1D

#define ACCL_INT            0x2E
#define ACCL_OFFSET_X       0x1E
#define ACCL_OFFSET_Y       0x1F
#define ACCL_OFFSET_Z       0x20
#define ACCL_DATA_X0        0x32
#define ACCL_DATA_X1        0x33

#define ACCL_PWR_CTL        0x2D
// Parameters for ACCL_PWR_CTL:
#define ACCL_MEASURE        0x08

#define ACCL_DATA_FORMAT    0x31
// Parameters for ACCL_DATA_FORMAT:
#define ACCL_RANGE_2G       0x00
#define ACCL_RANGE_4G       0x01
#define ACCL_RANGE_8G       0x02
#define ACCL_RANGE_16G      0x03
#define ACCL_FULL_RES       0x08
#define ACCL_JUSTIFY        0x04

#define ACCL_BW_RATE        0x2C
// Parameters for ACCL_BW_RATE:
#define ACCL_RATE_3200HZ    0x0F
#define ACCL_RATE_1600HZ    0x0E
#define ACCL_RATE_800HZ     0x0D
#define ACCL_RATE_400HZ     0x0C
#define ACCL_RATE_200HZ     0x0B
#define ACCL_RATE_100HZ     0x0A
#define ACCL_RATE_50HZ      0x09
#define ACCL_RATE_25HZ      0x08
#define ACCL_RATE_12_5HZ    0x07
#define ACCL_RATE_6_25HZ    0x06
#define ACCL_RATE_3_13HZ    0x05
#define ACCL_RATE_1_56HZ    0x04
#define ACCL_RATE_0_78HZ    0x03
#define ACCL_RATE_0_39HZ    0x02
#define ACCL_RATE_0_20HZ    0x01
#define ACCL_RATE_0_10HZ    0x00


// Void function to display a changing message on the display.
// The display has 4 rows of 16 characters, with 0, 0 at top left.
void displayUpdate (char *str1, char *str2, int16_t num, char *str3, uint8_t charLine);


// Display function, version for displaying floats
// The display has 4 rows of 16 characters, with 0, 0 at top left.
void displayUpdateFloatStr (char *str1, char *str2, char *float_string, char *str3, uint8_t charLine);


// Void function to initialise accelerometer functions.
void initAccl (void);


// Void function to read accelerometer. Returns a vector3_t struct.
vector3_t getAcclData (void);


// Stores the values obtained by the accelerometer into the circular buffer
void store_accl(vector3_t acceleration, circBuf_t *buffer_x, circBuf_t *buffer_y, circBuf_t *buffer_z);


// Calculates the mean of the circular buffer and returns a 3 vector of x, y and z
vector3_t calculate_mean(circBuf_t *buffer_x, circBuf_t *buffer_y, circBuf_t *buffer_z, uint8_t buf_size);


// void function to reset the calibration of the accelerometer as displayed
void accCalibrate (void);


#endif /*READACC_H_*/
