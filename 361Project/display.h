/* display.h
 *
 * FitnessMonGroup8
 * S. Allen, J. Zhu, G. Lay
 *
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_





#include "readAcc.h"
enum dispModes {STEP = 0, DIST};
enum unitModes {KM = 0, MI};

typedef struct{
    float x;
    float y;
    float z;
} vector3_float;


void initDisplay(void);

uint8_t swap_disp(uint8_t dispmode, uint8_t dir);

void displayRAW(uint32_t offset_mean);

void displayDist(uint32_t steps, uint8_t unitmode);

void displayUpdate (char *str1, char *str2, int16_t num, char *str3, uint8_t charLine);

void displayUpdateFloatStr (char *str1, char *str2, char *float_string, char *str3, uint8_t charLine);

void redrawDisplay(void);

#endif /* DISPLAY_H_ */
