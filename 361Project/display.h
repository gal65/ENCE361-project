#ifndef DISPLAY_H_
#define DISPLAY_H_

/* Display.h
 *
 * Contains functions to initiate OLED display,
 * form strings, send lines to display, and refresh.
 *
 * FitnessMonGroup8
 * J. Zhu, G. Lay, S. Allen
 * Based on code by C. Moore
 *
 * Last modified; 16/05/2020
 */

#include "readAcc.h"

enum dispModes {STEP = 0, DIST};
enum unitModes {KM = 0, MI};

typedef struct{
    float x;
    float y;
    float z;
} vector3_float;


// Initialise the OLED display - call on startup
void initDisplay(void);

// Swap the display mode depending on the current input and the
uint8_t swapDisp(uint8_t dispmode);

// Steps-mode display handler. Form strings, fill display buffer, and refresh
void displayRAW(uint32_t offset_mean);

// Distance-mode display handler. Accepts raw steps and unit mode
// Form strings including floats, fill display buffer, and refresh
void displayDist(uint32_t steps, uint8_t unitmode);

// Function to display one line of a changing message on the display.
// The display has 4 rows of 16 characters, with 0, 0 at top left.
void displayUpdate (char *str1, char *str2, int16_t num, char *str3, uint8_t charLine);

// Display-one-line function, version for displaying floats
// The display has 4 rows of 16 characters, with 0, 0 at top left.
void displayUpdateFloatStr (char *str1, char *str2, char *float_string, char *str3, uint8_t charLine);

uint8_t swapUnits(uint8_t unitMode);

#endif /* DISPLAY_H_ */
