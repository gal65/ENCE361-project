/**
 * main.c
 * Main source file for ENCE361 Project 1, Milestone 1
 *
 * Mon 11 Group 20
 * S. Allen, J. Zhu, G. Lay
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
#include <math.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
//#include "inc/hw_ints.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/debug.h"
#include "driverlib/pin_map.h"
#include "driverlib/fpu.h"
#include "utils/ustdlib.h"
#include <stdio.h>
#include <stdlib.h>
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "ADC.h"
#include "readAcc.h"
#include "buttons4.h"
#include "floatToString.h"

//*****************************************************************************
// Variables
//*****************************************************************************
#define BUF_SIZE 10
#define SAMPLE_RATE_HZ 100
#define GRAV_CONV 256
#define MPS_CONV 26


/********************************************************
 * main
 ********************************************************/
int main(void)
{
    // Buffer of size BUF_SIZE integers (sample values) for each axis x, y and z
    static circBuf_t inBuffer_x;
    static circBuf_t inBuffer_y;
    static circBuf_t inBuffer_z;

    // Buffer of size BUF_SIZE integers (sample values) for the ADC (CURRENTLY NOT IN USE)
    static circBuf_t inBuffer;

    static uint32_t ulSampCnt;    // Counter for the interrupts


    // Initialise components
    initClockADC (ulSampCnt, SAMPLE_RATE_HZ);
    initADC (&inBuffer);
    OLEDInitialise ();
    initCircBuf (&inBuffer, BUF_SIZE);
    initCircBuf (&inBuffer_x, BUF_SIZE);
    initCircBuf (&inBuffer_y, BUF_SIZE);
    initCircBuf (&inBuffer_z, BUF_SIZE);
    initButtons();
    initAccl();

    FPUEnable(); // enable FPU co-processor

    IntMasterEnable(); // Enable interrupts to the processor.

    // Acceleration code (ripped from readAcc main function)
    vector3_t acceleration;
    vector3_float acceleration_floats;
    vector3_t mean_acc;

    // TODO; enable changing the displayed units
    uint8_t unitMode = UNITS_RAW;

    OLEDStringDraw ("Accelerometer", 0, 0);

    while (1)
    {
        char acc_float_x[5];
        char acc_float_y[5];
        char acc_float_z[5];


        // Check buttons and update mode if required
        unitMode = pollButtons(unitMode);

        // Loop for accelerometer (TODO; turn into task)
        // THIS IS CAUSING THE LONG PRESS BUG, when combined with SysCtlDelay elsewhere (ie in buttons4.c)
        // need to use interrupts properly
        SysCtlDelay(SysCtlClockGet() / 64);    // not Approx 2 Hz

        acceleration = getAcclData();
        acceleration = getAcclData(unitMode);
        store_accl(acceleration, &inBuffer_x, &inBuffer_y, &inBuffer_z);
        mean_acc = calculate_mean(&inBuffer_x, &inBuffer_y, &inBuffer_z, BUF_SIZE);

        switch (unitMode)
        {
        case 0:
            displayUpdate("Accl", "X", mean_acc.x, "raw", 1);
            displayUpdate("Accl", "Y", mean_acc.y, "raw", 2);
            displayUpdate("Accl", "Z", mean_acc.z, "raw", 3);
            break;

        case 1:
            acceleration_floats.x = (float)mean_acc.x / GRAV_CONV;
            acceleration_floats.y = (float)mean_acc.y / GRAV_CONV;
            acceleration_floats.z = (float)mean_acc.z / GRAV_CONV;
            ftos(acceleration_floats.x, acc_float_x, 2);
            ftos(acceleration_floats.y, acc_float_y, 2);
            ftos(acceleration_floats.z, acc_float_z, 2);
            displayUpdateFloatStr("", "X", acc_float_x, "G", 1);
            displayUpdateFloatStr("", "Y", acc_float_y, "G", 2);
            displayUpdateFloatStr("", "Z", acc_float_z, "G", 3);
            break;

        case 2:
            acceleration_floats.x = (float)mean_acc.x / MPS_CONV;
            acceleration_floats.y = (float)mean_acc.y / MPS_CONV;
            acceleration_floats.z = (float)mean_acc.z / MPS_CONV;
            ftos(acceleration_floats.x, acc_float_x, 2);
            ftos(acceleration_floats.y, acc_float_y, 2);
            ftos(acceleration_floats.z, acc_float_z, 2);
            displayUpdateFloatStr("", "X", acc_float_x, "m/s/s", 1);
            displayUpdateFloatStr("", "Y", acc_float_y, "m/s/s", 2);
            displayUpdateFloatStr("", "Z", acc_float_z, "m/s/s", 3);
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
