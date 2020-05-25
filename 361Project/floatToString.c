/*
 * Module converts floats to strings for display on OLED with driver lacking support for printing floats;
 *
 * FitnessMonGroup8
 * S. Allen, J. Zhu, G. Lay
 *
 * Heavily adapted from "C program for implementation of ftoa()"
 * Author; Shubham Singh. Retrieved 14/03/2020 from https://www.geeksforgeeks.org/convert-floating-point-number-string/
 */

#include "floatToString.h"

// Reverses a string 'str' of length 'len'
void reverse(char* str, int len)
{
    int i = 0, j = len - 1, temp;
    while (i < j) {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}


// Converts a given integer x to string str[].
// d is the number of digits required in the output, NOT INCLUDING the negative sign.
// If d is more than the number of digits in x, then zeros are added at the beginning.
int ipart_to_str(int x, char str[], int integerPlaces, uint8_t negFlag)
{
    int i = 0;

    while (x) {
        str[i++] = (x % 10) + '0';
        x = x / 10;
    }

    // If number of digits required is more, then
    // add 0s at the beginning
    while (i < integerPlaces) {
        str[i++] = '0';
    }

    if (negFlag != 0) { // add a sign if required
        str[i++] = '-';
    } else {
        str[i++] = ' ';
    }

    reverse(str, i);
    str[i] = '\0';
    return i;
}


// Converts a given fraction part (in fixed point) x to string str[].
// Version for fraction part.
int fpart_to_str(int x, char str[], int decimalPlaces)
{
    int i = 0;

    while (x)
    {
        str[i++] = (x % 10) + '0';
        x = x / 10;
    }

    // If number of digits required is more, then add 0s at the beginning
    while (i < decimalPlaces)
    {
        str[i++] = '0';
    }

    reverse(str, i);
    str[i] = '\0';
    return i;
}


// Converts a floating-point/double number to a string.
void f_to_s(float n, char* res, int integerPlaces, int decimalPlaces)
{
    uint8_t negFlag = 0;

    // Extract integer part
    int ipart = (int)n;

    // Extract floating part and convert to fixed point
    float fpart = n - (float)ipart;
    fpart = fpart * pow(10, decimalPlaces);

    // If negative, set neg flag high and take absolute values for both parts
    // Note; abs() does not support floats; an integer is returned.
    if (n < 0)
    {
        negFlag = 1;
        ipart = abs(ipart);
        fpart = abs(fpart);
    }

    // convert integer part to string
    int i = ipart_to_str(ipart, res, integerPlaces, negFlag);
    negFlag = 0;

    // check for display option after point
    if (decimalPlaces != 0) {
        res[i] = '.'; // add dot

        // Get the value of fraction part up to given no.
        // of points after dot. The third parameter
        // is needed to handle cases like 233.007
        fpart_to_str((int) fpart, res + i + 1, decimalPlaces);
    }
}
