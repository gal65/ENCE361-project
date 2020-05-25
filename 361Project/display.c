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

#define GRAV_CONV 256   // conversion factor for raw accelerometer data to gravities - change if changing accelerometer settings
#define MPS_CONV 26     // conversion factor for raw accelerometer data to meters per second per second - change if changing accelerometer settings
#define KM_CONV 0.9     // conversion factor for steps to km
#define MI_CONV 0.5592  // conversion factor for km to mi
#define INT_PLACES 2    // number of integer places for float display
#define DEC_PLACES 3    // number of decimal places for float display

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <utils/ustdlib.h>
#include "display.h"
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "readAcc.h"
#include "floatToString.h"
#include <string.h>

// Idea for this part came from tutors, implementation from Zach Preston;
#include <OrbitOLED/lib_OrbitOled/OrbitOledChar.h>
#include <OrbitOLED/lib_OrbitOled/OrbitOled.h>


// Initialise the OLED display - call on startup
void initDisplay(void)
{
    OLEDInitialise();
    OrbitOledSetCharUpdate(0);
}


// Order the refresh of the OLED display - call after each page update.
// only required if OrbitOledSetCharUpdate(0) set on init (which it is)
inline void redrawDisplay(void)
{
    OrbitOledUpdate();
}


// Swap the display mode depending on the current input and the
uint8_t swap_disp(uint8_t dispmode)
{
    uint8_t next_disp;

    // Swap between distance and steps
    if (dispmode == STEP)
    {
        next_disp = DIST;     // edit here for doing things with buttons
    }
    else
    {
        next_disp = STEP;
    }
    return next_disp;
}


// Steps-mode display handler. Form strings, fill display buffer, and refresh
void displayRAW(uint32_t steps)
{
    OrbitOledClearBuffer();
    OLEDStringDraw("Steps:", 0, 0);
    displayUpdate("   ", "", steps, "", 2);
    redrawDisplay();
}


// Distance-mode display handler. Accepts raw steps and unit mode
// Form strings including floats, fill display buffer, and refresh
void displayDist(uint32_t steps, uint8_t unitmode)
{
    // Declare string for float-to-string conversion
    char dist_str[INT_PLACES + DEC_PLACES + 1] = {'\0'}; // init to full of zeros for safety
    char unit_str[2 + 1] = {'\0'};   // units have two letters and null terminator
    //IntMasterDisable();   // dealing with floats, disable ints for speed
    if (unitmode == KM)
    {
        strcpy(unit_str, "km");
        // Convert raw steps to units of km, as floats
        float f_dist_km = (steps * KM_CONV) / 1000;
        // Convert float values to strings printable by usnprintf()
        ftos(f_dist_km, dist_str, INT_PLACES, DEC_PLACES);
    }
    else
    {
        strcpy(unit_str, "mi");
        // Convert raw steps to units of miles, as floats
        float f_dist_mi = (steps * KM_CONV / 1000) * MI_CONV;
        // Convert float values to strings printable by usnprintf()
        ftos(f_dist_mi, dist_str, INT_PLACES, DEC_PLACES);
    }
    // Clear then fill display buffer, then refresh the display
    OrbitOledClearBuffer();
    OLEDStringDraw("Distance:", 0, 0);
    displayUpdateFloatStr("", "", dist_str, unit_str, 2);
    redrawDisplay();
    //IntMasterEnable(); // don't actually need this - floats not messed with
}


// Function to display one line of a changing message on the display.
// The display has 4 rows of 16 characters, with 0, 0 at top left.
void displayUpdate(char *str1, char *str2, int16_t num, char *str3,
                   uint8_t charLine)
{
    char text_buffer[17];  //Display fits 16 characters wide.
    // "Undraw" the previous contents of the line to be updated.
    OLEDStringDraw("                ", 0, charLine);
    // Form a new string for the line.  The maximum width specified for the
    // number field ensures it is displayed right justified.
    usnprintf(text_buffer, sizeof(text_buffer), "%s %s %3d %s", str1, str2, num,
              str3);
    // Send line to display buffer
    OLEDStringDraw(text_buffer, 0, charLine);
}


// Display-one-line function, version for displaying floats
// The display has 4 rows of 16 characters, with 0, 0 at top left.
void displayUpdateFloatStr(char *str1, char *str2, char *float_string,
                           char *str3, uint8_t charLine)
{
    char text_buffer[17];   //Display fits 16 characters wide.

    // "Undraw" the previous contents of the line to be updated.
    OLEDStringDraw("                ", 0, charLine);
    // Form a new string for the line.  The maximum width specified for the
    // number field ensures it is displayed right justified.
    usnprintf(text_buffer, sizeof(text_buffer), "%s %s %s %s", str1, str2,
              float_string, str3);
    // Send line to display buffer
    OLEDStringDraw(text_buffer, 0, charLine);
}
