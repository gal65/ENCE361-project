/* Module converts floats to strings for display on OLED with driver lacking support for printing floats;
 *
 * Mon 11 Group 20
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
// d is the number of digits required in the output.
// If d is more than the number of digits in x, then zeros are added at the beginning.
int ipartToStr(int x, char str[], int places, uint8_t neg_flag)
{
    int i = 0;

    while (x) {
        str[i++] = (x % 10) + '0';
        x = x / 10;
    }

    // If number of digits required is more, then
    // add 0s at the beginning
    while (i < places) {
        str[i++] = '0';
    }

    if (neg_flag != 0) { // add a sign if required
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
int fpartToStr(int x, char str[], int places)
{
    int i = 0;

    while (x)
    {
        str[i++] = (x % 10) + '0';
        x = x / 10;
    }

    // If number of digits required is more, then add 0s at the beginning
    while (i < places)
    {
        str[i++] = '0';
    }

    reverse(str, i);
    str[i] = '\0';
    return i;
}

// Converts a floating-point/double number to a string.
void ftos(float n, char* res, int decimal_places)
{
    uint8_t neg_flag = 0;

    // Extract integer part
    int ipart = (int)n;

    // Extract floating part and convert to fixed point
    float fpart = n - (float)ipart;
    fpart = fpart * pow(10, decimal_places);

    // If negative, set neg flag high and take absolute values for both parts
    // Note; abs() does not support floats; an integer is returned.
    if (n < 0)
    {
        neg_flag = 1;
        ipart = abs(ipart);
        fpart = abs(fpart);
    }

    // convert integer part to string
    int i = ipartToStr(ipart, res, 2, neg_flag);
    neg_flag = 0;

    // check for display option after point
    if (decimal_places != 0) {
        res[i] = '.'; // add dot

        // Get the value of fraction part upto given no.
        // of points after dot. The third parameter
        // is needed to handle cases like 233.007

        fpartToStr((int) fpart, res + i + 1, decimal_places);
    }
}
