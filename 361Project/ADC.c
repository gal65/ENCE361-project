
/*
*
* Interrupts and Circular Buffers
*
* Mon 11 Group 20
* Authors; S. Allen, J. Zhu, G. Lay
*
* Based on 'ADCdemo1.c' by P.J. Bones (2018)
*/

#include "ADC.h"
#include "readAcc.h"

//*****************************************************************************
//
// The interrupt handler for the for SysTick interrupt.
//
//*****************************************************************************
void
SysTickIntHandler(void)//uint32_t ulSampCnt)
{
    //
    // Initiate a conversion
    //
    ADCProcessorTrigger(ADC0_BASE, 3); 
    //ulSampCnt++;
}

//*****************************************************************************
// Initialisation functions for the clock (incl. SysTick), ADC, display
//*****************************************************************************
void initClockADC (uint32_t ulSampCnt, uint32_t SAMPLE_RATE_HZ)
{
/*
    // Set the clock rate to 20 MHz
    SysCtlClockSet (SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);
    //
*/
    // Set up the period for the SysTick timer.  The SysTick timer period is
    // set as a function of the system clock.
    SysTickPeriodSet(SysCtlClockGet() / SAMPLE_RATE_HZ);
    //
    // Register the interrupt handler
    SysTickIntRegister(SysTickIntHandler);
    //
    // Enable interrupt and device
    SysTickIntEnable();
    SysTickEnable();
}

void initADC(void)
{
    //static circBuf_t *inBuffer;
    //inBuffer = inBufferPointer;
    //
    // The ADC0 peripheral must be enabled for configuration and use.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    
    // Enable sample sequence 3 with a processor signal trigger.  Sequence 3
    // will do a single sample when the processor sends a signal to start the
    // conversion.
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
  
    //
    // Configure step 0 on sequence 3.  Sample channel 0 (ADC_CTL_CH0) in
    // single-ended mode (default) and configure the interrupt flag
    // (ADC_CTL_IE) to be set when the sample is done.  Tell the ADC logic
    // that this is the last conversion on sequence 3 (ADC_CTL_END).  Sequence
    // 3 has only one programmable step.  Sequence 1 and 2 have 4 steps, and
    // sequence 0 has 8 programmable steps.  Since we are only doing a single
    // conversion using sequence 3 we will only configure step 0.  For more
    // on the ADC sequences and steps, refer to the LM3S1968 datasheet.
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH0 | ADC_CTL_IE |
                             ADC_CTL_END);    
                             
    //
    // Since sample sequence 3 is now configured, it must be enabled.
    ADCSequenceEnable(ADC0_BASE, 3);
  
    //
    // Register the interrupt handler
    ADCIntRegister (ADC0_BASE, 3, ADCIntHandler);
  
    //
    // Enable interrupts for ADC0 sequence 3 (clears any outstanding interrupts)
    ADCIntEnable(ADC0_BASE, 3);
}


// The handler for the ADC conversion complete interrupt.
// Writes to the circular buffer.
void ADCIntHandler(void)
{
    uint32_t ulValue;

    // Get the single sample from ADC0.  ADC_BASE is defined in
    // inc/hw_memmap.h
    ADCSequenceDataGet(ADC0_BASE, 3, &ulValue);
    //
    // Place it in the circular buffer (advancing write index)
    //writeCircBuf(inBuffer, ulValue);
    //
    // Clean up, clearing the interrupt
    ADCIntClear(ADC0_BASE, 3);
}

/* ADC FUNCTIONS ARE DISABLED!
 *
 *
void initDisplayADC (void)
{
    // intialise the Orbit OLED display
    OLEDInitialise ();
}


// Function to display the mean ADC value (10-bit value, note) and sample count.
void displayMeanVal(uint16_t meanVal, uint32_t count)
{
	char string[17];  // 16 characters across the display

    OLEDStringDraw ("ADC demo 1", 0, 0);
	
    // Form a new string for the line.  The maximum width specified for the
    //  number field ensures it is displayed right justified.
    usnprintf (string, sizeof(string), "Mean ADC = %4d", meanVal);
    // Update line on display.
    OLEDStringDraw (string, 0, 1);

    usnprintf (string, sizeof(string), "Sample # %5d", count);
    OLEDStringDraw (string, 0, 3);
}
*
* ADC FUNCTIONS ARE DISABLED!
*/
