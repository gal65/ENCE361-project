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

    // Declaring vector struct variables
    vector3_t acceleration;
    vector3_float acceleration_floats;
    vector3_t mean_acc;
    vector3_t offset;
    vector3_t offset_mean;

    // Initialising offsets to 0
       offset.x = 0;
       offset.y = 0;
       offset.z = 0;

    // Enable changing the displayed units
       uint8_t unitMode = UNITS_RAW;
       vector_poll pollData;

    OLEDStringDraw ("Accelerometer", 0, 0);

    // Variables used upon startup for the initialisation calibration
    int init_cycles = 0;
    int initialised = 0;

    while (1)
    {
        // Declare strings for float-to-string conversion
        char acc_float_x[5];
        char acc_float_y[5];
        char acc_float_z[5];

        // Check buttons and update mode if required
        pollData = pollButtons(unitMode);
        unitMode = pollData.dispMode;

        // Loop for accelerometer (TODO; turn into task)
        // THIS IS CAUSING THE LONG PRESS BUG, when combined with SysCtlDelay elsewhere (ie in buttons4.c)
        // need to use interrupts properly
        SysCtlDelay(SysCtlClockGet() / 64);    // not Approx 2 Hz

        // Pulls the data from the accelerometer and then collects it in a circular buffer
        // The mean is then calculated and stored in a seperate variable
        acceleration = getAcclData();
        acceleration = getAcclData(unitMode);
        store_accl(acceleration, &inBuffer_x, &inBuffer_y, &inBuffer_z);
        mean_acc = calculate_mean(&inBuffer_x, &inBuffer_y, &inBuffer_z, BUF_SIZE);

        // Upon initialization, the device waits until the buffers have been filled and then uses the mean to set the first offset
        if (initialised == 0) {
            init_cycles++;
            if (init_cycles > BUF_SIZE) {
                initialised = 1;
                offset = mean_acc;
            }
        }

        // Calculates the offset by subtracting the current mean acceleration with the set offset
        offset_mean.x = mean_acc.x - offset.x;
        offset_mean.y = mean_acc.y - offset.y;
        offset_mean.z = mean_acc.z - offset.z;

        if (pollData.flag == 1) {
            offset = mean_acc;
        }

        switch (unitMode)
        {
        case 0: // Raw mode - MAGIC NUMBER - Why does switch not like using enumerated or declared constants?
            displayUpdate("Accl", "X", offset_mean.x, "raw", 1);
            displayUpdate("Accl", "Y", offset_mean.y, "raw", 2);
            displayUpdate("Accl", "Z", offset_mean.z, "raw", 3);
            break;

        case 1: // Gravities mode
            // Convert raw values to units of gravity, as floats
            acceleration_floats.x = (float)offset_mean.x / GRAV_CONV;
            acceleration_floats.y = (float)offset_mean.y / GRAV_CONV;
            acceleration_floats.z = (float)offset_mean.z / GRAV_CONV;
            // Convert float values to strings printable by usnprintf()
            ftos(acceleration_floats.x, acc_float_x, 2);
            ftos(acceleration_floats.y, acc_float_y, 2);
            ftos(acceleration_floats.z, acc_float_z, 2);
            // Update the display
            displayUpdateFloatStr("", "X", acc_float_x, "G", 1);
            displayUpdateFloatStr("", "Y", acc_float_y, "G", 2);
            displayUpdateFloatStr("", "Z", acc_float_z, "G", 3);
            break;

        case 2: // MPS mode
            // Convert raw values to units of meters per second, as floats
            acceleration_floats.x = (float)offset_mean.x / MPS_CONV;
            acceleration_floats.y = (float)offset_mean.y / MPS_CONV;
            acceleration_floats.z = (float)offset_mean.z / MPS_CONV;
            // Convert float values to strings printable by usnprintf()
            ftos(acceleration_floats.x, acc_float_x, 2);
            ftos(acceleration_floats.y, acc_float_y, 2);
            ftos(acceleration_floats.z, acc_float_z, 2);
            // Update the display
            displayUpdateFloatStr("", "X", acc_float_x, "m/s/s", 1);
            displayUpdateFloatStr("", "Y", acc_float_y, "m/s/s", 2);
            displayUpdateFloatStr("", "Z", acc_float_z, "m/s/s", 3);
            break;
        }
    }

}
