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
#include <stdio.h>

// Reverses a string 'str' of length 'len'
void reverse(char* str, int len);

// Converts a given integer x to string str[].
// d is the number of digits required in the output.
// If d is more than the number of digits in x,
// then 0s are added at the beginning.
int intToStr(int x, char str[], int d);

// Converts a floating-point/double number to a string.
void ftoa(float n, char* res, int afterpoint);

#endif /*READACC_H_*/
