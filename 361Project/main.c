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

//*****************************************************************************
// Variables
//*****************************************************************************
#define BUF_SIZE 10
#define SAMPLE_RATE_HZ 100
#define GRAV_CONV 128
#define MPS_CONV 26

//*****************************************************************************
// Stores the values obtained by the accelerometer into the circular buffer
//*****************************************************************************
void store_accl(vector3_t acceleration, circBuf_t *buffer_x, circBuf_t *buffer_y, circBuf_t *buffer_z)
{
    writeCircBuf(buffer_x, acceleration.x);
    writeCircBuf(buffer_y, acceleration.y);
    writeCircBuf(buffer_z, acceleration.z);
}

//*****************************************************************************
// Calculates the mean of the circular buffer and returns a 3 vector of x, y and z
//*****************************************************************************
vector3_t calculate_mean(circBuf_t *buffer_x, circBuf_t *buffer_y, circBuf_t *buffer_z)
{
    vector3_t sum;
    vector3_t average;
    int i;
    sum.x = 0;
    sum.y = 0;
    sum.z = 0;

    // Sum all values in the circular buffer
    for (i = 0; i < BUF_SIZE; i++) {
        sum.x = sum.x + readCircBuf (buffer_x);
        sum.y = sum.y + readCircBuf (buffer_y);
        sum.z = sum.z + readCircBuf (buffer_z);
    }
    // Divide all values in the circular buffer to get mean
    average.x = sum.x / BUF_SIZE;
    average.y = sum.y / BUF_SIZE;
    average.z = sum.z / BUF_SIZE;

    return average;
}

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
    vector3_t mean;

    // TODO; enable changing the displayed units
    uint8_t unitMode = UNITS_RAW;

    OLEDStringDraw ("Accelerometer", 0, 0);

    while (1)
    {
        // Check buttons and update mode if required
        unitMode = pollButtons(unitMode);

        // Loop for accelerometer (TODO; turn into task) // THIS IS CAUSING THE LONG PRESS BUG, when combined with SysCtlDelay elsewhere (ie in buttons4.c)
        SysCtlDelay(SysCtlClockGet() / 48);    // not Approx 2 Hz

        acceleration = getAcclData();
        acceleration = getAcclData(unitMode);
        store_accl(acceleration, &inBuffer_x, &inBuffer_y, &inBuffer_z);
        mean = calculate_mean(&inBuffer_x, &inBuffer_y, &inBuffer_z);
        // TEST MEAN

        switch (unitMode)
        {
        case 0:
            displayUpdate("Accl", "X", acceleration.x, "raw", 1);
            displayUpdate("Accl", "Y", mean.y, "raw", 2);
            displayUpdate("Accl", "Z", acceleration.z, "raw", 3);
            break;

        case 1:
            acceleration_floats.x = acceleration.x / GRAV_CONV;
            acceleration_floats.y = acceleration.y / GRAV_CONV;
            acceleration_floats.z = acceleration.z / GRAV_CONV;
            displayUpdateFloat("Accl", "X", acceleration_floats.x, "G", 1);
            displayUpdateFloat("Accl", "Y", acceleration_floats.y, "G", 2);
            displayUpdateFloat("Accl", "Z", acceleration_floats.z, "G", 3);
            break;

        case 2:
            acceleration_floats.x = acceleration_floats.x / MPS_CONV;
            acceleration_floats.y = acceleration_floats.y / MPS_CONV;
            acceleration_floats.z = acceleration_floats.z / MPS_CONV;
            //displayUpdateFloat("Accl", "X", acceleration_floats.x, "m/s/s", 1);
            displayUpdateFloat("Accl", "X", acceleration_floats.x, "m/s/s", 1);
            displayUpdateFloat("Accl", "Y", acceleration_floats.y, "m/s/s", 2);
            displayUpdateFloat("Accl", "Z", acceleration_floats.z, "m/s/s", 3);
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
