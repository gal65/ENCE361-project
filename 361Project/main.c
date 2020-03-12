/**
 * main.c
 * Main source file for ENCE361 Project 1, Milestone 1
 *
 * Mon 11 Group 20
 *
 * Authors; S. Allen, J. Zhu, G. Lay
 * using material from P. Bones and C. Moore
 * Created; 10/03/2020
 */

// TODO; implement circular buffers and averaging
// tidy everything up
// confirm modularity

#include <readAcc.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
//#include "inc/hw_ints.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/debug.h"
#include "driverlib/pin_map.h"
#include "utils/ustdlib.h"
#include "stdio.h"
#include "stdlib.h"
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "ADC.h"
#include "readAcc.h"
#include "buttons4.h"

//*****************************************************************************
// Variables
//*****************************************************************************
#define BUF_SIZE 10
#define SAMPLE_RATE_HZ 100

/********************************************************
 * main
 ********************************************************/
int main(void)
{
    static circBuf_t inBuffer;        // Buffer of size BUF_SIZE integers (sample values)
    static uint32_t ulSampCnt;    // Counter for the interrupts

    // Initialise components
    initClockADC (ulSampCnt, SAMPLE_RATE_HZ);
    initADC (&inBuffer);
    OLEDInitialise ();
    initCircBuf (&inBuffer, BUF_SIZE);
    initButtons();
    initAccl();

    IntMasterEnable(); // Enable interrupts to the processor.

    // Acceleration code (ripped from readAcc main function)
    vector3_t acceleration;

    // TODO; enable changing the displayed units
    uint8_t unitMode = UNITS_RAW;

    OLEDStringDraw ("Accelerometer", 0, 0);

    while (1)
    {

        // Check buttons and update mode if required
        unitMode = pollButtons(unitMode);

        // Loop for accelerometer (TODO; turn into task) // THIS IS CAUSING THE LONG PRESS BUG, when combined with SysCtlDelay elsewhere (ie in buttons4.c)
        SysCtlDelay(SysCtlClockGet() / 24);    // not Approx 2 Hz

        acceleration = getAcclData(unitMode);

        switch (unitMode)
        {
        case 0:
            displayUpdate("Accl", "X", acceleration.x, "raw", 1);
            displayUpdate("Accl", "Y", acceleration.y, "raw", 2);
            displayUpdate("Accl", "Z", acceleration.z, "raw", 3);
            break;

        case 1:
            displayUpdate("Accl", "X", acceleration.x, "G", 1);
            displayUpdate("Accl", "Y", acceleration.y, "G", 2);
            displayUpdate("Accl", "Z", acceleration.z, "G", 3);
            break;

        case 2:
            displayUpdate("Accl", "X", acceleration.x, "m/s/s", 1);
            displayUpdate("Accl", "Y", acceleration.y, "m/s/s", 2);
            displayUpdate("Accl", "Z", acceleration.z, "m/s/s", 3);
            break;
        }
    }
/*
    // Loop for display (TODO; turn into task)
    while (1)
    {
        //
        // Background task: calculate the (approximate) mean of the values in the
        // circular buffer and display it, together with the sample number.
        sum = 0;
        for (i = 0; i < BUF_SIZE; i++)
            sum = sum + readCircBuf (&inBuffer);
        // Calculate and display the rounded mean of the buffer contents
        displayMeanVal ((2 * sum + BUF_SIZE) / 2 / BUF_SIZE, ulSampCnt);

        SysCtlDelay (SysCtlClockGet() / 6);  // Update display at ~ 2 Hz
    }
*/
}
