/* display.c
 * Init, form and display strings on OLED display.
 *
 * FitnessMonGroup8
 * S. Allen, J. Zhu, G. Lay
 * Based on code by C. Moore
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <utils/ustdlib.h>
#include "display.h"
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "readAcc.h"
#include "floatToString.h"

#define GRAV_CONV 256   // conversion factor for raw accelerometer data to gravities - change if changing accelerometer settings
#define MPS_CONV 26     // conversion factor for raw accelerometer data to meters per second per second - change if changing accelerometer settings
#define INT_PLACES 2    // number of integer places for float display
#define DEC_PLACES 2    // number of decimal places for float display

// Idea for this part came from tutors, implementation from Zach Preston;
// include for the OrbitOledSetCharUpdate() function
#include <OrbitOLED/lib_OrbitOled/OrbitOledChar.h>
//#include "../../OrbitOLED/lib_OrbitOled/OrbitOledChar.h"
// include for the OrbitOledUpdate() function
#include <OrbitOLED/lib_OrbitOled/OrbitOled.h>
//#include "../../OrbitOLED/lib_OrbitOled/OrbitOled.h"

// Initialise the OLED display - call on startup
void initDisplay(void)
{
    OLEDInitialise ();
    OrbitOledSetCharUpdate(0);
}

// Form strings for RAW display mode
void displayRAW(vector3_t offset_mean)
{
    OLEDStringDraw ("Accelerometer", 0, 0);
    displayUpdate("Accl", "X", offset_mean.x, "raw", 1);
    displayUpdate("Accl", "Y", offset_mean.y, "raw", 2);
    displayUpdate("Accl", "Z", offset_mean.z, "raw", 3);
}

// Form strings for GRAVITY display mode
void displayGRAV(vector3_t offset_mean)
{
    // Declare strings for float-to-string conversion
    static char acc_float_x[INT_PLACES+DEC_PLACES+1];
    static char acc_float_y[INT_PLACES+DEC_PLACES+1];
    static char acc_float_z[INT_PLACES+DEC_PLACES+1];
    vector3_float acceleration_floats;

    // Convert raw values to units of gravity, as floats
    acceleration_floats.x = (float) offset_mean.x / GRAV_CONV;
    acceleration_floats.y = (float) offset_mean.y / GRAV_CONV;
    acceleration_floats.z = (float) offset_mean.z / GRAV_CONV;

    // Convert float values to strings printable by usnprintf()
    ftos(acceleration_floats.x, acc_float_x, INT_PLACES, DEC_PLACES);
    ftos(acceleration_floats.y, acc_float_y, INT_PLACES, DEC_PLACES);
    ftos(acceleration_floats.z, acc_float_z, INT_PLACES, DEC_PLACES);

    // Update the display
    OLEDStringDraw ("Accelerometer", 0, 0);
    displayUpdateFloatStr("", "X", acc_float_x, "G", 1);
    displayUpdateFloatStr("", "Y", acc_float_y, "G", 2);
    displayUpdateFloatStr("", "Z", acc_float_z, "G", 3);
}
// Form strings for MSS display mode
void displayMSS(vector3_t offset_mean)
{
    // Declare strings for float-to-string conversion
    static char acc_float_x[INT_PLACES + DEC_PLACES + 1];
    static char acc_float_y[INT_PLACES + DEC_PLACES + 1];
    static char acc_float_z[INT_PLACES + DEC_PLACES + 1];
    vector3_float acceleration_floats;

    // Convert raw values to units of meters per second, as floats
    acceleration_floats.x = (float) offset_mean.x / MPS_CONV;
    acceleration_floats.y = (float) offset_mean.y / MPS_CONV;
    acceleration_floats.z = (float) offset_mean.z / MPS_CONV;

    // Convert float values to strings printable by usnprintf()
    ftos(acceleration_floats.x, acc_float_x, INT_PLACES, DEC_PLACES);
    ftos(acceleration_floats.y, acc_float_y, INT_PLACES, DEC_PLACES);
    ftos(acceleration_floats.z, acc_float_z, INT_PLACES, DEC_PLACES);

    // Update the display
    OLEDStringDraw ("Accelerometer", 0, 0);
    displayUpdateFloatStr("", "X", acc_float_x, "m/s/s", 1);
    displayUpdateFloatStr("", "Y", acc_float_y, "m/s/s", 2);
    displayUpdateFloatStr("", "Z", acc_float_z, "m/s/s", 3);
}

//*****************************************************************************
// Function to display a changing message on the display.
// The display has 4 rows of 16 characters, with 0, 0 at top left.
//*****************************************************************************
void displayUpdate (char *str1, char *str2, int16_t num, char *str3, uint8_t charLine)
{
    char text_buffer[17];           //Display fits 16 characters wide.
    // "Undraw" the previous contents of the line to be updated.
    OLEDStringDraw ("                ", 0, charLine);
    // Form a new string for the line.  The maximum width specified for the
    //  number field ensures it is displayed right justified.
    usnprintf(text_buffer, sizeof(text_buffer), "%s %s %3d %s", str1, str2, num, str3);
    // Update line on display.
    OLEDStringDraw (text_buffer, 0, charLine);
    redrawDisplay();
}

//*****************************************************************************
// Display function, version for displaying floats
// The display has 4 rows of 16 characters, with 0, 0 at top left.
//*****************************************************************************
void displayUpdateFloatStr (char *str1, char *str2, char *float_string, char *str3, uint8_t charLine)
{
    char text_buffer[17];           //Display fits 16 characters wide.

    // "Undraw" the previous contents of the line to be updated.
    OLEDStringDraw ("                ", 0, charLine);
    // Form a new string for the line.  The maximum width specified for the
    // number field ensures it is displayed right justified.
    usnprintf(text_buffer, sizeof(text_buffer), "%s %s %s %s", str1, str2, float_string, str3);
    // Update line on display.
    OLEDStringDraw (text_buffer, 0, charLine);
    redrawDisplay();
}

// Order the refresh of the OLED display - call after each page update.
// only required if OrbitOledSetCharUpdate(0) set on init.
void redrawDisplay(void)
{
    OrbitOledUpdate();
}
