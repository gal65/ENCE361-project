/*
 * buttons4.c
 * Support for a set of FOUR specific buttons on the Tiva/Orbit.
 * ENCE361 sample code.
 *
 * FitnessMonGroup8
 * J. Zhu, G. Lay, S. Allen
 * Based on code by P.J. Bones UCECE
 *
 * The buttons are:  UP and DOWN (on the Orbit daughterboard) plus
 * LEFT and RIGHT on the Tiva.
 *
 * Note that pin PF0 (the pin for the RIGHT pushbutton - SW2 on
 *  the Tiva board) needs special treatment - See PhilsNotesOnTiva.rtf.
 */

#include "buttons4.h"

static bool but_state[NUM_BUTS];    // Corresponds to the electrical state
static uint8_t but_count[NUM_BUTS];
static bool but_flag[NUM_BUTS];
static bool but_normal[NUM_BUTS];   // Corresponds to the electrical state

volatile uint32_t flagU;
volatile uint32_t flagD;
volatile uint32_t flagL;
volatile uint32_t flagR;
static uint8_t cycles = 0;          //checks the cycles for the button held down

static void initButton(enum butNames butId, uint32_t peripheral, uint32_t base,
                       uint8_t pin, bool logic)
{
    SysCtlPeripheralEnable(peripheral);
    GPIOPinTypeGPIOInput(base, pin);
    GPIOPadConfigSet(base, pin, GPIO_STRENGTH_2MA,
                     logic ? GPIO_PIN_TYPE_STD_WPU : GPIO_PIN_TYPE_STD_WPD); // ternary operator for the lols
    but_normal[butId] = logic;
}


// initButtons: Initialise the variables and interrupts associated with the set of buttons
// defined by the constants in the buttons2.h header file.
void initButtons(void)
{
    int i;

    // UP button (active HIGH)
    initButton(UP, UP_BUT_PERIPH, UP_BUT_PORT_BASE, UP_BUT_PIN, UP_BUT_NORMAL);

    // DOWN button (active HIGH)
    initButton(DOWN, DOWN_BUT_PERIPH, DOWN_BUT_PORT_BASE, DOWN_BUT_PIN, DOWN_BUT_NORMAL);

    // LEFT button (active LOW)
    initButton(LEFT, LEFT_BUT_PERIPH, LEFT_BUT_PORT_BASE, LEFT_BUT_PIN, LEFT_BUT_NORMAL);

    // RIGHT button (active LOW)
    // Note that PF0 is one of a handful of GPIO pins that need to be
    // "unlocked" before they can be reconfigured.  This also requires
    // the #include "inc/tm4c123gh6pm.h"
    SysCtlPeripheralEnable(RIGHT_BUT_PERIPH); // must enable before unlocking
    //---Unlock PF0 for the right button:
    GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;
    GPIO_PORTF_CR_R |= GPIO_PIN_0; //PF0 unlocked
    GPIO_PORTF_LOCK_R = GPIO_LOCK_M;
    // Now init
    initButton(RIGHT, RIGHT_BUT_PERIPH, RIGHT_BUT_PORT_BASE, RIGHT_BUT_PIN, RIGHT_BUT_NORMAL);

    for (i = 0; i < NUM_BUTS; i++)   // What is this doing here?
    {
        but_state[i] = but_normal[i];
        //but_count[i] = 0;
        but_flag[i] = false;
    }

    // Adding interrupts to each button system in the init
    GPIOIntRegister(UP_BUT_PORT_BASE, UPButIntHandler);
    GPIOIntRegister(DOWN_BUT_PORT_BASE, DOWNButIntHandler);
    GPIOIntRegister(LEFT_BUT_PORT_BASE, LEFTButIntHandler);
    GPIOIntRegister(RIGHT_BUT_PORT_BASE, RIGHTButIntHandler);

    GPIOIntTypeSet(UP_BUT_PORT_BASE, UP_BUT_PIN, GPIO_RISING_EDGE);
    GPIOIntTypeSet(DOWN_BUT_PORT_BASE, DOWN_BUT_PIN, GPIO_RISING_EDGE);
    GPIOIntTypeSet(LEFT_BUT_PORT_BASE, LEFT_BUT_PIN, GPIO_FALLING_EDGE);
    GPIOIntTypeSet(RIGHT_BUT_PORT_BASE, RIGHT_BUT_PIN, GPIO_FALLING_EDGE);

    GPIOIntEnable(UP_BUT_PORT_BASE, UP_BUT_PIN);
    GPIOIntEnable(DOWN_BUT_PORT_BASE, DOWN_BUT_PIN);
    GPIOIntEnable(LEFT_BUT_PORT_BASE, LEFT_BUT_PIN);
    GPIOIntEnable(RIGHT_BUT_PORT_BASE, RIGHT_BUT_PIN);
}


// Interrupt handlers for each of the button inputs
void UPButIntHandler(void)
{
    flagU = 1;
    GPIOIntClear(UP_BUT_PORT_BASE, UP_BUT_PIN);
    GPIOIntDisable(UP_BUT_PORT_BASE, UP_BUT_PIN);   // prevent a double button interrupt
}


void DOWNButIntHandler(void)
{
    flagD = 1;
    GPIOIntClear(DOWN_BUT_PORT_BASE, DOWN_BUT_PIN);
    GPIOIntDisable(DOWN_BUT_PORT_BASE, DOWN_BUT_PIN);
}


// Left and Right button interrupts are on the same port, so they occur at the same time
void LEFTButIntHandler(void)
{
    flagL = 1;
    GPIOIntClear(LEFT_BUT_PORT_BASE, LEFT_BUT_PIN);
    GPIOIntClear(RIGHT_BUT_PORT_BASE, RIGHT_BUT_PIN);
    GPIOIntDisable(LEFT_BUT_PORT_BASE, LEFT_BUT_PIN);
}


void RIGHTButIntHandler(void)
{
    flagR = 1;
    GPIOIntClear(RIGHT_BUT_PORT_BASE, RIGHT_BUT_PIN);
    GPIOIntClear(LEFT_BUT_PORT_BASE, LEFT_BUT_PIN);
    GPIOIntDisable(RIGHT_BUT_PORT_BASE, RIGHT_BUT_PIN);
}


// updateButtons: Function designed to be called regularly. It polls all
// buttons once and updates variables associated with the buttons if
// necessary.  It is efficient enough to be part of an ISR, e.g. from
// a SysTick interrupt.
// Debounce algorithm disabled by setting NUM_BUT_POLLS to 1. Debounce
// now handled in detectHold()
void updateButtons(void)
{
    bool but_value[NUM_BUTS];
    int i;

    // Read the pins; true means HIGH, false means LOW
    but_value[UP] = (GPIOPinRead(UP_BUT_PORT_BASE, UP_BUT_PIN) == UP_BUT_PIN);
    but_value[DOWN] = (GPIOPinRead(DOWN_BUT_PORT_BASE, DOWN_BUT_PIN)
            == DOWN_BUT_PIN);
    but_value[LEFT] = (GPIOPinRead(LEFT_BUT_PORT_BASE, LEFT_BUT_PIN)
            == LEFT_BUT_PIN);
    but_value[RIGHT] = (GPIOPinRead(RIGHT_BUT_PORT_BASE, RIGHT_BUT_PIN)
            == RIGHT_BUT_PIN);

    // Iterate through the buttons, updating button variables as required
    for (i = 0; i < NUM_BUTS; i++)
    {
        if (but_value[i] != but_state[i])
        {
            but_count[i]++;
            if (but_count[i] >= NUM_BUT_POLLS)
            {
                but_state[i] = but_value[i];
                but_flag[i] = true;    // Reset by call to checkButton()
                but_count[i] = 0;
            }
        }
        else
            but_count[i] = 0;
    }
}


// checkButton: Function returns the new button logical state if the button
// logical state (PUSHED or RELEASED) has changed since the last call,
// otherwise returns NO_CHANGE.
uint8_t checkButton(uint8_t butName)
{
    if (but_flag[butName])
    {
        but_flag[butName] = false;
        if (but_state[butName] == but_normal[butName])
            return RELEASED;
        else
            return PUSHED;
    }
    return NO_CHANGE;
}


// Function to poll buttons; pushing UP will increment the mode cycle, pushing DOWN will set a flag to logic high
// Only polls the flags set by the interrupts and does not require updating the buttons to do so
vector_inputs readButtonFlags(vector_inputs inputFlags)
{
    if (flagU == 1)
    {
        inputFlags.U = 1;
        flagU = 0;
        GPIOIntEnable(UP_BUT_PORT_BASE, UP_BUT_PIN);    // re-enable button ints
    }

    if (flagD == 1)
    {
        inputFlags.D = 1;
        flagD = 0;
        GPIOIntEnable(DOWN_BUT_PORT_BASE, DOWN_BUT_PIN);
    }

    if (flagL == 1)
    {
        inputFlags.L = 1;
        flagL = 0;
        GPIOIntEnable(LEFT_BUT_PORT_BASE, LEFT_BUT_PIN);
    }

    if (flagR == 1)
    {
        inputFlags.R = 1;
        flagR = 0;
        GPIOIntEnable(RIGHT_BUT_PORT_BASE, RIGHT_BUT_PIN);
    }

    return inputFlags;
}


// Function acts as a detector for the a button held down
// Checks how long a button is held down depending on a limit
int detectHold(uint8_t butName, int lim)
{
    uint8_t fin_flag = 0;
    uint8_t butState;
    updateButtons();
    butState = checkButton(butName);
    if (butState == NO_CHANGE)
    {
        cycles++;
    }
    else if (butState == RELEASED)
    { //Condition if button released too early
        fin_flag = 2;
    }
    else
    {
        cycles = 0;
    }
    if (cycles == lim)
    {
        fin_flag = 1;
        cycles = 0;
    }

    return fin_flag;
}
