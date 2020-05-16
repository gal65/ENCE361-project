/* display.c
 * Init, form and display strings on OLED display.
 *
 * FitnessMonGroup8
 * S. Allen, J. Zhu, G. Lay
 * Based on code by C. Moore
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
#include <utils/ustdlib.h>
#include "display.h"
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "readAcc.h"
#include "floatToString.h"

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
void displayRAW(uint32_t steps)
{
    OrbitOledClearBuffer();
    OLEDStringDraw ("Steps:", 0, 0);
    displayUpdate("   ","", steps, "", 2);
    redrawDisplay();
}

// Form strings for GRAVITY display mode
void displayKMeters(uint32_t steps)
{
    // Declare strings for float-to-string conversion
    static char dist_float_km[INT_PLACES+DEC_PLACES+1];

    // Convert raw steps to units of km, as floats
    float dist_float = ((float) steps * KM_CONV) / 1000;

    // Convert float values to strings printable by usnprintf()
    ftos(dist_float, dist_float_km, INT_PLACES, DEC_PLACES);

    // Update the display
    OrbitOledClearBuffer();
    OLEDStringDraw ("Distance:", 0, 0);
    displayUpdateFloatStr("", "", dist_float_km, "km", 2);
    redrawDisplay();
}
// Form strings for MSS display mode
void displayMiles(uint32_t steps)
{
    // Declare strings for float-to-string conversion
    static char dist_float_mi[INT_PLACES + DEC_PLACES + 1];


    // Convert raw steps to units of miles, as floats
    float dist_float = (((float) steps * KM_CONV) * MI_CONV) / 1000;

    // Convert float values to strings printable by usnprintf()
    ftos(dist_float, dist_float_mi, INT_PLACES, DEC_PLACES);

    // Update the display
    OrbitOledClearBuffer();
    OLEDStringDraw ("Distance:", 0, 0);
    displayUpdateFloatStr("", "", dist_float_mi, "mi", 2);
    redrawDisplay();
}


//// Form strings for GRAVITY display mode
//void displayGRAV(vector3_t offset_mean)
//{
//    // Declare strings for float-to-string conversion
//    static char acc_float_x[INT_PLACES+DEC_PLACES+1];
//    static char acc_float_y[INT_PLACES+DEC_PLACES+1];
//    static char acc_float_z[INT_PLACES+DEC_PLACES+1];
//    vector3_float acceleration_floats;
//
//    // Convert raw values to units of gravity, as floats
//    acceleration_floats.x = (float) offset_mean.x / GRAV_CONV;
//    acceleration_floats.y = (float) offset_mean.y / GRAV_CONV;
//    acceleration_floats.z = (float) offset_mean.z / GRAV_CONV;
//
//    // Convert float values to strings printable by usnprintf()
//    ftos(acceleration_floats.x, acc_float_x, INT_PLACES, DEC_PLACES);
//    ftos(acceleration_floats.y, acc_float_y, INT_PLACES, DEC_PLACES);
//    ftos(acceleration_floats.z, acc_float_z, INT_PLACES, DEC_PLACES);
//
//    // Update the display
//    OrbitOledClearBuffer();
//    OLEDStringDraw ("Distance", 0, 0);
//    displayUpdateFloatStr("", "X", acc_float_x, "km", 1);
//    displayUpdateFloatStr("", "Y", acc_float_y, "km", 2);
//    displayUpdateFloatStr("", "Z", acc_float_z, "km", 3);
//    redrawDisplay();
//}
//// Form strings for MSS display mode
//void displayMSS(vector3_t offset_mean)
//{
//    // Declare strings for float-to-string conversion
//    static char acc_float_x[INT_PLACES + DEC_PLACES + 1];
//    static char acc_float_y[INT_PLACES + DEC_PLACES + 1];
//    static char acc_float_z[INT_PLACES + DEC_PLACES + 1];
//    vector3_float acceleration_floats;
//
//    // Convert raw values to units of meters per second, as floats
//    acceleration_floats.x = (float) offset_mean.x / MPS_CONV;
//    acceleration_floats.y = (float) offset_mean.y / MPS_CONV;
//    acceleration_floats.z = (float) offset_mean.z / MPS_CONV;
//
//    // Convert float values to strings printable by usnprintf()
//    ftos(acceleration_floats.x, acc_float_x, INT_PLACES, DEC_PLACES);
//    ftos(acceleration_floats.y, acc_float_y, INT_PLACES, DEC_PLACES);
//    ftos(acceleration_floats.z, acc_float_z, INT_PLACES, DEC_PLACES);
//
//    // Update the display
//    OrbitOledClearBuffer();
//    OLEDStringDraw ("Distance", 0, 0);
//    displayUpdateFloatStr("", "X", acc_float_x, "mi", 1);
//    displayUpdateFloatStr("", "Y", acc_float_y, "mi", 2);
//    displayUpdateFloatStr("", "Z", acc_float_z, "mi", 3);
//    redrawDisplay();
//}

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
}

// Order the refresh of the OLED display - call after each page update.
// only required if OrbitOledSetCharUpdate(0) set on init (which it is)
void redrawDisplay(void)
{
    OrbitOledUpdate();
}
