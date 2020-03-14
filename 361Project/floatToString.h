#ifndef FLOATTOSTRING_H_
#define FLOATTOSTRING_H_

/*
 * Module converts floats to strings for display on OLED with driver lacking support for printing floats;
 * Mon 11 Group 20
 * S. Allen, J. Zhu, G. Lay
 *
 * Adapted from "C program for implementation of ftoa()"
 * Author; Shubham Singh. Retrieved 14/03/2020 from https://www.geeksforgeeks.org/convert-floating-point-number-string/
 */

#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
//#include "inc/hw_ints.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/debug.h"
#include "driverlib/pin_map.h"
#include "driverlib/fpu.h"
#include "utils/ustdlib.h"
#include <stdio.h>
#include <stdlib.h>
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "ADC.h"
#include "readAcc.h"
#include "buttons4.h"
#include "floatToString.h"

// Reverses a string 'str' of length 'len'
void reverse(char* str, int len);

// Converts a given integer x to string str[].
// d is the number of digits required in the output.
// If d is more than the number of digits in x,
// then 0s are added at the beginning.
int ipartToStr(int x, char str[], int places, uint8_t neg_flag);

// Similar ftos function for fraction part of a float
int fpartToStr(int x, char str[], int places);

// Converts a floating-point/double number to a string.
void ftos(float n, char* res, int decimal_places);

#endif /*READACC_H_*/
