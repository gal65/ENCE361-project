/*
 * stepCounter.c
 *
 * Module that contains functions to increment the step count
 *
 * FitnessMonGroup8
 * S. Allen, J. Zhu, G. Lay
*/

#include <stepCounter.h>
#include "readAcc.h"

// Define Variables
#define THRESHOLD_ACCEL 384 // the raw value equivalent for 1.5G

// takes x, y and z mean acceleration inputs and returns the norm, in raw value
int32_t calculate_norm(vector3_t mean_acc)
{
    int32_t accel_norm = sqrt(mean_acc.x*mean_acc.x + mean_acc.y*mean_acc.y + mean_acc.z*mean_acc.z);
    return accel_norm;
}


// toggles flag if current value of the acceleration norm is greater than 1.5G
int less_than_flag (uint32_t current_norm)
{
    int is_flagged;
    if (current_norm > THRESHOLD_ACCEL)
    {
        is_flagged = 1;
    } else {
        is_flagged = 0;
    }
    return is_flagged;
}


// increments total steps if the norm of the accelerations increases from less than to more than 1.5G
int16_t step_increment (uint32_t current_steps, uint8_t current_flag, uint8_t prev_flag)
{
    if (current_flag == 1 && prev_flag == 0)
    {
        current_steps++;
    }
    return current_steps;
}
