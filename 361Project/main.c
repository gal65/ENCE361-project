/* main.c
 * Main source file for ENCE361 Project 1, Milestone 1
 *
 * FitnessMonGroup8
 * S. Allen, J. Zhu, G. Lay
 * using material from P. Bones and C. Moore
 * Last modified; 15/05/2020
 */

#include <readAcc.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
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
#include "ADC.h" //
#include "readAcc.h" //
#include "buttons4.h" //
#include "floatToString.h" //
#include "display.h" //
#include "circBufT.h" //
#include "stepCounter.h" //

// Define Variables
#define SAMPLE_RATE_HZ 100 // rate for sampling accelerometers

// Begin main loop
int main(void)
{
    static uint32_t ulSampCnt;    // Counter for the interrupts

    // Initialise components
    initClockADC(ulSampCnt, SAMPLE_RATE_HZ);
    initADC();
    initDisplay();
    initCircBuf(&inBuffer, BUF_SIZE);
    initCircBuf(&inBuffer_x, BUF_SIZE);
    initCircBuf(&inBuffer_y, BUF_SIZE);
    initCircBuf(&inBuffer_z, BUF_SIZE);
    initButtons();
    initAccl();

    FPUEnable(); // enable FPU co-processor
    IntMasterEnable(); // Enable interrupts to the processor.

    // Declaring accel measurement variables and vars used upon startup for the initialisation calibration
    vector3_t mean_acc;
    vector3_t offset;
    vector3_t offset_mean_acc;
    uint32_t accel_norm;
    uint32_t steps; //
    uint8_t acc_buf_filled = 0;
    uint16_t init_cycles = 0;

    // declaring int variables and initialising values to 0
    uint32_t total_steps = 0;
    uint32_t less_than = 0;
    uint32_t prev_less_than = 0;

    // Initialising offsets to 0
    offset.x = 0;
    offset.y = 0;
    offset.z = 0;

    // Enable changing the displayed units
//    uint8_t unitMode = UNITS_RAW;
    vector_inputs inputFlags = {.dispMode=RAW, .D=0, .L=0, .R=0};

    while (1) // TODO: On SYSCLK interrupt raising main_flag, run this while loop.
    {
        //  (TODO; turn this whole loop into a task triggered by an interrupt raising a flag)

        // THIS IS CAUSING THE LONG PRESS BUG, when combined with SysCtlDelay elsewhere (ie in buttons4.c)
        // THIS IS BAD METHOD - need to use interrupts properly.
        SysCtlDelay(SysCtlClockGet() / 128);    // (not) approx 2 Hz

        // Check buttons and update mode if required
        inputFlags = readButtonFlags(inputFlags);
//        unitMode = butFlags.dispMode;

        // Pulls the data from the accelerometer and then collects it in a circular buffer
        // The mean is then calculated and stored in a separate 3D vector variable
        mean_acc = getAcclData();

        // CALIBRATION OF ACCELEROMETERS
        // Upon initialization, the device waits until the buffers have been
        // filled and then uses the mean to set the first offset
        if (acc_buf_filled == 0)
        {
            init_cycles++;
            if (init_cycles > BUF_SIZE)
            {
                acc_buf_filled = 1;
                offset = mean_acc;
            }
        }


        // If recalibrate button pressed, set current mean as the new offset
        if (inputFlags.D == 1)
        {
            offset = mean_acc;
            inputFlags.D = 0;
        }

        // Recalculates the current value by subtracting the current
        // mean acceleration by the last saved offset
        offset_mean_acc.x = mean_acc.x - offset.x;
        offset_mean_acc.y = mean_acc.y - offset.y;
        offset_mean_acc.z = mean_acc.z - offset.z;

        // Calculate the norm of the x, y, and z acceleration readings in G's (could maybe make it a one argument function taking a vector3_t)
        accel_norm = calculate_norm(mean_acc); //
        // Flag is set based on norm
        less_than = less_than_flag(accel_norm);
        // Increment step count based on the 2 flags
        total_steps = step_increment(total_steps, less_than, prev_less_than);
        // Update previous flag
        prev_less_than = less_than;
        steps = total_steps; // won't need this when gui is done

        // Select display logic based on mode
        switch (inputFlags.dispMode)
        {
        case 0: // Raw mode - MAGIC NUMBER? - Why does switch() not like using enumerated or declared constants?
            displayRAW(steps); //changed this to display steps to know it works
            break;

        case 1: // Gravities mode
            displayKMeters(steps);
            break;

        case 2: // MPS mode
            displayMiles(steps);
            break;
        }
    }

}
