/* Main.c
 *
 * Main source file for ENCE361 Fitness Tracker project.
 *
 * FitnessMonGroup8
 * J. Zhu, G. Lay, S. Allen
 * using material from P. Bones and C. Moore
 *
 * Last modified; 17/05/2020
 */

#include <readAcc.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
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
#include "ADC.h"   //
#include "readAcc.h"   //
#include "buttons4.h"   //
#include "floatToString.h"   //
#include "display.h"   //
#include "circBufT.h"   //
#include "stepCounter.h"   //
#include "switch.h"   //

// Define Constants
#define SYS_DELAY_DIV 128   // divisor for main loop timing
#define SAMPLE_RATE_HZ 100   // rate for sampling accelerometers
#define LONG_PRESS 50   // amount of cycles the button needs to be held down to confirm
#define SHORT_PRESS 4   // number of cycles to debounce a button
#define STEP_INCR 100   // Increment steps in test mode
#define STEP_DECR 500   // Decrement steps in test mode


int main(void)
{

    static uint32_t ulSampCnt;   // Counter for the interrupts
    // Declare accel measurement vars
    vector3_t mean_acc;
    uint32_t accel_norm;

    // Declare uint32_t variables and initialising values to 0
    uint32_t total_steps = 0;
    uint32_t less_than = 0;
    uint32_t prev_less_than = 0;

    // Declare vars for UI state machine
    vector_inputs inputFlags = { .D = 0, .L = 0, .R = 0, .U = 0 };
    uint8_t dispMode = STEP;
    uint8_t unitMode = KM;
    uint8_t inputMode = NORM;
    uint8_t held = 0;


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
    initSwitch();

    FPUEnable();   // enable FPU co-processor
    IntMasterEnable();   // Enable interrupts to the processor.


    while (1) // TODO: On SYSCLK interrupt raising main_flag, run this while loop.
    {
        // THIS IS CAUSING THE LONG PRESS BUG, when combined with SysCtlDelay elsewhere (ie in buttons4.c)
        // THIS IS A BAD METHOD - TODO; use SysTick interrupt instead.
        SysCtlDelay(SysCtlClockGet() / SYS_DELAY_DIV);    // Divisor is system rate in Hz


        // Pull the data from the accelerometer and then collect it in a circular buffer
        // The mean is then calculated and stored in a separate 3D vector variable
        mean_acc = getAcclData();
        // Check buttons and switches and update mode if required
        inputFlags = readButtonFlags(inputFlags);
        inputMode = getSwitchPos();


        // CALIBRATION OF ACCELEROMETERS - disabled.
        // Unnecessary and makes for bad RMS magnitude if offset is large
//        vector3_t offset;
//        vector3_t offset_mean_acc;
//        uint8_t acc_buf_filled = 0;
//        uint16_t init_cycles = 0;
//
//        // Upon initialization, the device waits until the buffers have been
//        // filled and then uses the mean to set the first offset
//        if (acc_buf_filled == 0)
//        {
//            init_cycles++;
//            if (init_cycles > BUF_SIZE)
//            {
//                acc_buf_filled = 1;
//                offset = mean_acc;
//            }
//        }
//        // Recalculate the current value by subtracting the current
//        // mean acceleration by the last saved offset
//        offset_mean_acc.x = mean_acc.x - offset.x;
//        offset_mean_acc.y = mean_acc.y - offset.y;
//        offset_mean_acc.z = mean_acc.z - offset.z;


        // Calculate the norm of the x, y, and z acceleration readings in raw value, taking the mean acceleration as an argument
        accel_norm = calculate_norm(mean_acc);
        // Flag is set based on value of the norm
        less_than = less_than_flag(accel_norm);
        // Increment step count based on the 2 flags
        total_steps = step_increment(total_steps, less_than, prev_less_than);
        // Update previous flag
        prev_less_than = less_than;


        // User interface state machine
        // Input flags are handled here
        if (inputFlags.D == 1)   // DOWN button
        {
            if (inputMode == NORM)
            {
                held = detect_hold(DOWN, LONG_PRESS);   // If held down for an amount of time,

                if (held == 1)
                {
                    total_steps = 0;   // the steps reset
                    inputFlags.D = 0;
                }
                else if (held == 2)   // not held long enough
                {
                    inputFlags.D = 0;
                }
            }
            else
            {
                held = detect_hold(DOWN, SHORT_PRESS);   // debounce using short press method
                if (held == 1)
                {
                    if (total_steps >= STEP_DECR) {
                    total_steps = total_steps - STEP_DECR;
                    } else {
                        total_steps = 0;    // Unsigned int must not wrap around
                    }

                    inputFlags.D = 0;
                }
                else if (held == 2)   //not held long enough
                {
                    inputFlags.D = 0;
                }
            }
        }

        //swaps units in distance mode on up button press only if the distance is displayed
        if (inputFlags.U == 1) // UP button
        {
            held = detect_hold(UP, SHORT_PRESS);
            if (held == 1)
            {
                if (inputMode == NORM)
                {
                    if (dispMode != STEP) {
                    unitMode = swap_units(unitMode);
                    }
                }
                else
                {
                    total_steps = total_steps + STEP_INCR;
                }
                inputFlags.U = 0;
            }
            else if (held == 2)   //not held long enough
            {
                inputFlags.U = 0;
            }
        }

        // Swap from step and distance modes on left button presses
        if (inputFlags.L == 1)   // LEFT button
        {
            held = detect_hold(LEFT, SHORT_PRESS);
            if (held == 1)
            {
                dispMode = swap_disp(dispMode);
                inputFlags.L = 0;
            }
            else if (held == 2)   //not held long enough
            {
                inputFlags.L = 0;
            }
        }

        // Swap from step and distance modes on right button presses
        if (inputFlags.R == 1)   // RIGHT BUTTON
        {
            held = detect_hold(RIGHT, SHORT_PRESS);
            if (held == 1)
            {
                dispMode = swap_disp(dispMode);
                inputFlags.R = 0;
            }
            else if (held == 2)   //not held long enough
            {
                inputFlags.R = 0;
            }
        }


        // Select and run display logic based on display mode
        switch (dispMode)
        {
        case STEP:   // Raw steps mode
            displayRAW(total_steps);
            break;

        case DIST:   // Distance mode
            displayDist(total_steps, unitMode);
            break;
        }
    }
}
