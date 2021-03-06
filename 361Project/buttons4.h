#ifndef BUTTONS_H_
#define BUTTONS_H_

/*
 * buttons4.h
 *
 * Support for a set of FOUR specific buttons on the Tiva/Orbit.
 * ENCE361 sample code.
 * The buttons are:  UP and DOWN (on the Orbit daughterboard) plus
 * LEFT and RIGHT on the Tiva.
 *
 * FitnessMonGroup8
 * J. Zhu, G. Lay, S. Allen
 * Based on code by P.J. Bones UCECE
 */

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "inc/tm4c123gh6pm.h"  // Board specific defines (for PF0)
#include "readAcc.h"
#include "display.h"

// For interrupts
//#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"


// Constants
enum butNames {UP = 0, DOWN, LEFT, RIGHT, NUM_BUTS};
enum butStates {RELEASED = 0, PUSHED, NO_CHANGE};
enum inputMode {NORM = 0, TEST};

// UP button
#define UP_BUT_PERIPH  SYSCTL_PERIPH_GPIOE
#define UP_BUT_PORT_BASE  GPIO_PORTE_BASE
#define UP_BUT_PIN  GPIO_PIN_0
#define UP_BUT_NORMAL  false
// DOWN button
#define DOWN_BUT_PERIPH  SYSCTL_PERIPH_GPIOD
#define DOWN_BUT_PORT_BASE  GPIO_PORTD_BASE
#define DOWN_BUT_PIN  GPIO_PIN_2
#define DOWN_BUT_NORMAL  false
// LEFT button
#define LEFT_BUT_PERIPH  SYSCTL_PERIPH_GPIOF
#define LEFT_BUT_PORT_BASE  GPIO_PORTF_BASE
#define LEFT_BUT_PIN  GPIO_PIN_4
#define LEFT_BUT_NORMAL  true
// RIGHT button
#define RIGHT_BUT_PERIPH  SYSCTL_PERIPH_GPIOF
#define RIGHT_BUT_PORT_BASE  GPIO_PORTF_BASE
#define RIGHT_BUT_PIN  GPIO_PIN_0
#define RIGHT_BUT_NORMAL  true

#define NUM_BUT_POLLS 1 // Effectively disabled - now debouncing with waits

// Structure definition for poll buttons
typedef struct{
    int D;
    int L;
    int R;
    int U;
} vector_inputs;


// initButtons: Initialise the variables associated with the set of buttons
// defined by the constants above.
void initButtons (void);

void UPButIntHandler (void);

void DOWNButIntHandler (void);

void LEFTButIntHandler (void);

void RIGHTButIntHandler (void);

// Function to poll buttons; pushing UP will increment the mode cycle, pushing DOWN will set a flag to logic high.
// Returns a struct containing the mode cycle and state of the flag
vector_inputs readButtonFlags(vector_inputs butflags);

// updateButtons: Function designed to be called regularly. It polls all
// buttons once and updates variables associated with the buttons if
// necessary.  It is efficient enough to be part of an ISR, e.g. from
// a SysTick interrupt.
void updateButtons (void);

// checkButton: Function returns the new button state if the button state
// (PUSHED or RELEASED) has changed since the last call, otherwise returns
// NO_CHANGE.  The argument butName should be one of constants in the
// enumeration butStates, excluding 'NUM_BUTS'. Safe under interrupt.
uint8_t checkButton (uint8_t butName);

int detectHold(uint8_t butName,int lim);

#endif /*BUTTONS_H_*/
