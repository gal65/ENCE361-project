/* switch.c
 * Init, detect and flag interrupts on the switch
 *
 * FitnessMonGroup8
 * J. Zhu, G. Lay, S. Allen
 */

#include "switch.h"

volatile uint8_t switchPos;


// Reads the gpio pin and changes the switch position variable
void checkSwitch (){
    switchPos = GPIOPinRead(SW1_PORT_BASE, SW1_PIN) == SW1_PIN;
}

// Interrupt triggers reads the gpio and sets it (so its not necessary to always read the pin)
void switchIntHandler (void)
{
    checkSwitch ();
    GPIOIntClear(SW1_PORT_BASE, SW1_PIN);
}

//Returns the value stored for the switch position (does not read the pin)
uint8_t getSwitchPos(){
    return switchPos;
}

//Initiates the switch gpio and interrupt which triggers on rising and falling edges
void initSwitch(void) {
    // UP button (active HIGH)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinTypeGPIOInput(SW1_PORT_BASE, SW1_PIN);
    GPIOPadConfigSet(SW1_PORT_BASE, SW1_PIN, GPIO_STRENGTH_2MA,
    GPIO_PIN_TYPE_STD);

    GPIOIntRegister(SW1_PORT_BASE, switchIntHandler);
    GPIOIntTypeSet(SW1_PORT_BASE, SW1_PIN, GPIO_BOTH_EDGES);
    GPIOIntEnable(SW1_PORT_BASE, SW1_PIN);
    checkSwitch ();

}
