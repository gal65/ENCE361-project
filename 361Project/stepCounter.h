/*
 * stepCounter.h
 *
 * Created on: 16/05/2020
 * FitnessMonGroup8
 * S. Allen, J. Zhu, G. Lay
 */

#ifndef STEPCOUNTER_H_
#define STEPCOUNTER_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <utils/ustdlib.h>
#include "readAcc.h"
#include <math.h>

// takes x, y and z mean acceleration inputs and returns the norm, raw
int32_t calculate_norm(vector3_t mean_acc);

// toggles flag if current value of the acceleration norm is greater than 1.5G
int less_than_flag (uint32_t current_norm);

// increments total steps if the norm of the accelerations increases from less than to more than 1.5G
int16_t step_increment (uint32_t current_steps, uint8_t current_flag, uint8_t prev_flag);

#endif /* STEPCOUNTER_H_ */
