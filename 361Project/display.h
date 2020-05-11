/*
 * display.h
 *
 *  Created on: 11/05/2020
 *      Author: simon
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_
#include "readAcc.h"

typedef struct{
    float x;
    float y;
    float z;
} vector3_float;

void initDisplay(void);

void displayRAW(vector3_t offset_mean);

void displayGRAV(vector3_t offset_mean);

void displayMSS(vector3_t offset_mean);

void displayUpdate (char *str1, char *str2, int16_t num, char *str3, uint8_t charLine);

void displayUpdateFloatStr (char *str1, char *str2, char *float_string, char *str3, uint8_t charLine);

void redrawDisplay(void);


#endif /* DISPLAY_H_ */
