/*
 * switch.h
 *
 * Init, detect and flag using interrupts on switch toggle
 * FitnessMonGroup8
 * J. Zhu, G. Lay, S. Allen
 */

#ifndef SWITCH_H_
#define SWITCH_H_

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "inc/tm4c123gh6pm.h"  // Board specific defines (for PF0)

//Defining specific switch ports
#define SW1_PORT_BASE GPIO_PORTA_BASE
#define SW1_PIN GPIO_PIN_7


void checkSwitch (void);

void switchIntHandler (void);

uint8_t getSwitchPos(void);

void initSwitch(void);


#endif /* SWITCH_H_ */
