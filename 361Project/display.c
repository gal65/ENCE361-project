/*
 * display.c
 *
 *  Created on: 11/05/2020
 *      Author: S. Allen
 *      based on code by C. Moore
 */

//TivaWare Standard Type Definitions:
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "display.h"
#include "OrbitOLED/OrbitOLEDInterface.h"
// Idea for this came from tutors, implementation from Zach Preston;
// include for the OrbitOledSetCharUpdate() function
#include <OrbitOLED/lib_OrbitOled/OrbitOledChar.h>
//#include "../../OrbitOLED/lib_OrbitOled/OrbitOledChar.h"
// include for the OrbitOledUpdate() function
#include <OrbitOLED/lib_OrbitOled/OrbitOled.h>
//#include "../../OrbitOLED/lib_OrbitOled/OrbitOled.h"

void initDisplay(void)
{
    OLEDInitialise ();
    OrbitOledSetCharUpdate(0);
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
    //  number field ensures it is displayed right justified.
    usnprintf(text_buffer, sizeof(text_buffer), "%s %s %s %s", str1, str2, float_string, str3);
    // Update line on display.
    OLEDStringDraw (text_buffer, 0, charLine);
    redrawDisplay();
}

void redrawDisplay(void)
{
    OrbitOledUpdate();
}
