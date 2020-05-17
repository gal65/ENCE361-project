/* buttons4.c
 * Support for a set of FOUR specific buttons on the Tiva/Orbit.
 * ENCE361 sample code.
 *
 * FitnessMonGroup8
 * S. Allen, J. Zhu, G. Lay
 * Based on code by P.J. Bones UCECE
 *
 * The buttons are:  UP and DOWN (on the Orbit daughterboard) plus
 * LEFT and RIGHT on the Tiva.
 *
 * Note that pin PF0 (the pin for the RIGHT pushbutton - SW2 on
 *  the Tiva board) needs special treatment - See PhilsNotesOnTiva.rtf.
 */

#include "buttons4.h"

// *******************************************************
// Globals to module
// *******************************************************
static bool but_state[NUM_BUTS];    // Corresponds to the electrical state
static uint8_t but_count[NUM_BUTS];
static bool but_flag[NUM_BUTS];
static bool but_normal[NUM_BUTS];   // Corresponds to the electrical state

volatile uint32_t flagU;
volatile uint32_t flagD;
volatile uint32_t flagL;
volatile uint32_t flagR;
static uint8_t cycles = 0; //checks the cycles for the button held down

// *******************************************************

// initButtons: Initialise the variables associated with the set of buttons
// defined by the constants in the buttons2.h header file.
void initButtons(void)
{

    int i;

    // UP button (active HIGH)
    SysCtlPeripheralEnable(UP_BUT_PERIPH);
    GPIOPinTypeGPIOInput(UP_BUT_PORT_BASE, UP_BUT_PIN);
    GPIOPadConfigSet(UP_BUT_PORT_BASE, UP_BUT_PIN, GPIO_STRENGTH_2MA,
    GPIO_PIN_TYPE_STD_WPD);
    but_normal[UP] = UP_BUT_NORMAL;
    // DOWN button (active HIGH)
    SysCtlPeripheralEnable(DOWN_BUT_PERIPH);
    GPIOPinTypeGPIOInput(DOWN_BUT_PORT_BASE, DOWN_BUT_PIN);
    GPIOPadConfigSet(DOWN_BUT_PORT_BASE, DOWN_BUT_PIN, GPIO_STRENGTH_2MA,
    GPIO_PIN_TYPE_STD_WPD);
    but_normal[DOWN] = DOWN_BUT_NORMAL;
    // LEFT button (active LOW)
    SysCtlPeripheralEnable(LEFT_BUT_PERIPH);
    GPIOPinTypeGPIOInput(LEFT_BUT_PORT_BASE, LEFT_BUT_PIN);
    GPIOPadConfigSet(LEFT_BUT_PORT_BASE, LEFT_BUT_PIN, GPIO_STRENGTH_2MA,
    GPIO_PIN_TYPE_STD_WPU);
    but_normal[LEFT] = LEFT_BUT_NORMAL;
    // RIGHT button (active LOW)
    // Note that PF0 is one of a handful of GPIO pins that need to be
    // "unlocked" before they can be reconfigured.  This also requires
    //      #include "inc/tm4c123gh6pm.h"
    SysCtlPeripheralEnable(RIGHT_BUT_PERIPH);
    //---Unlock PF0 for the right button:
    GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;
    GPIO_PORTF_CR_R |= GPIO_PIN_0; //PF0 unlocked
    GPIO_PORTF_LOCK_R = GPIO_LOCK_M;
    GPIOPinTypeGPIOInput(RIGHT_BUT_PORT_BASE, RIGHT_BUT_PIN);
    GPIOPadConfigSet(RIGHT_BUT_PORT_BASE, RIGHT_BUT_PIN, GPIO_STRENGTH_2MA,
    GPIO_PIN_TYPE_STD_WPU);
    but_normal[RIGHT] = RIGHT_BUT_NORMAL;

    for (i = 0; i < NUM_BUTS; i++)
    {
        but_state[i] = but_normal[i];
        //but_count[i] = 0;
        but_flag[i] = false;
    }

    // Adding an interrupt system in the init***
    //GPIOIntRegister(UP_BUT_PORT_BASE, UPButIntHandler);
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

void UPButIntHandler(void)
{
    flagU = 1;
    GPIOIntClear(UP_BUT_PORT_BASE, UP_BUT_PIN);
}

void DOWNButIntHandler(void)
{
    flagD = 1;
    GPIOIntClear(DOWN_BUT_PORT_BASE, DOWN_BUT_PIN);
}

// Left and Right button interrupts are on the same port so they occur at the same time
void LEFTButIntHandler(void)
{
    flagL = 1;
    GPIOIntClear(LEFT_BUT_PORT_BASE, LEFT_BUT_PIN);
    GPIOIntClear(RIGHT_BUT_PORT_BASE, RIGHT_BUT_PIN);
}

void RIGHTButIntHandler(void)
{
    flagR = 1;
    GPIOIntClear(RIGHT_BUT_PORT_BASE, RIGHT_BUT_PIN);
    GPIOIntClear(LEFT_BUT_PORT_BASE, LEFT_BUT_PIN);
}

// *******************************************************
// updateButtons: Function designed to be called regularly. It polls all
// buttons once and updates variables associated with the buttons if
// necessary.  It is efficient enough to be part of an ISR, e.g. from
// a SysTick interrupt.
// Debounce algorithm: A state machine is associated with each button.
// A state change occurs only after NUM_BUT_POLLS consecutive polls have
// read the pin in the opposite condition, before the state changes and
// a flag is set.  Set NUM_BUT_POLLS according to the polling rate.
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

// *******************************************************
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
    }

    if (flagD == 1)
    {
        inputFlags.D = 1;
        flagD = 0;
    }

    if (flagL == 1)
    {
        inputFlags.L = 1;
        flagL = 0;
    }

    if (flagR == 1)
    {
        inputFlags.R = 1;
        flagR = 0;
    }

    return inputFlags;
}

// Function acts as a detector for the a button held down
// Checks how long a button is held down depending on a limit
int detect_hold(uint8_t butName, int lim)
{
    int fin_flag = 0;
    uint8_t butState;
    updateButtons();
    butState = checkButton(butName);
    if (butState == NO_CHANGE)
    {
        cycles++;
    }
    else
    {
        flagD = 0;
        cycles = 0;
    }
    if (cycles == lim)
    {
        fin_flag = 1;
        cycles = 0;
    }

    return fin_flag;
}

// Used to swap the displays depending on the direction input and current disp
uint8_t swap_disp(uint8_t dispmode, uint8_t dir)
{
    uint8_t next_disp;
    switch (dir)
    {
    case 0: // Swap between distance and steps
        if (dispmode == RAW)
        {
            next_disp = MPS;     // edit here for doing things with buttons
        }
        else
        {
            next_disp = RAW;
        }
        break;

    case 1: // Swap between units (only works if it is displaying distance already)
        if (dispmode == MPS)
        {
            next_disp = GRAV;     // edit here for doing things with buttons
        }
        else if (dispmode == GRAV)
        {
            next_disp = MPS;
        }
        break;
    }
    return next_disp;
}

/*
 uint8_t butState;
 int flag = 0;

 // Poll the buttons
 updateButtons();

 // check state of each button
 butState = checkButton(UP);
 switch (butState)
 {
 case PUSHED: // increment dispMode cycler if a change is detected on UP button
 if (dispMode == MPS) {
 dispMode = RAW;
 } else {
 dispMode++;
 }
 break;
 // Do nothing if state is NO_CHANGE or RELEASED
 }
 butState = checkButton(DOWN);
 switch (butState)
 {
 case PUSHED: // set flag to logic high if a change is detected on DOWN button
 flag = 1;
 break;
 // Do nothing if state is NO_CHANGE or RELEASED
 }
 */

/*
 butState = checkButton(LEFT);
 switch (butState)
 {
 case PUSHED:
 displayButtonState("LEFT ", "PUSH", ++leftPushes, 2);
 break;
 case RELEASED:
 displayButtonState("LEFT ", "RELS", leftPushes, 2);
 break;
 // Do nothing if state is NO_CHANGE
 }
 butState = checkButton(RIGHT);
 switch (butState)
 {
 case PUSHED:
 displayButtonState("RIGHT", "PUSH", ++rightPushes, 3);
 break;
 case RELEASED:
 displayButtonState("RIGHT", "RELS", rightPushes, 3);
 break;
 // Do nothing if state is NO_CHANGE
 }
 */

// set each field of the vector_buttons    - Better to do this as they happen?
//    button_flags.dispMode = dispMode;
//    button_flags.D = flag;
