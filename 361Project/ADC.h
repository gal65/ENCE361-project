//*****************************************************************************
//
// ADCdemo1.h - Simple interrupt driven program which samples with AIN0
//
// Mon 11 Group 20
// Authors:  S. Allen, J. Zhu, G. Lay after P.J. Bones	UCECE
// Last modified:	10/03/2020
//
//*****************************************************************************
// Based on the 'convert' series from 2016
//*****************************************************************************

#ifndef ADC_H_
#define ADC_H_

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/adc.h"
#include "driverlib/pwm.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"
#include "utils/ustdlib.h"
#include "circBufT.h"
#include "OrbitOLED/OrbitOLEDInterface.h"


static circBuf_t* inBuffer;        // Buffer of size BUF_SIZE integers (sample values)

//*****************************************************************************
//
// The interrupt handler for the for SysTick interrupt.
//
//*****************************************************************************
void SysTickIntHandler(void);//uint32_t ulSampCnt);

//*****************************************************************************
//
// The handler for the ADC conversion complete interrupt.
// Writes to the circular buffer.
//
//*****************************************************************************
void ADCIntHandler(void);

//*****************************************************************************
// Initialisation functions for the clock (incl. SysTick), ADC, display
//*****************************************************************************
void initClockADC (uint32_t ulSampCnt, uint32_t SAMPLE_RATE_HZ);

void initADC (circBuf_t* inBufferPointer);

void initDisplayADC (void);

//*****************************************************************************
//
// Function to display the mean ADC value (10-bit value, note) and sample count.
//
//*****************************************************************************
void displayMeanVal(uint16_t meanVal, uint32_t count);

#endif /*ADC_H_*/
