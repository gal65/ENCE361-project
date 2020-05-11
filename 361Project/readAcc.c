/**********************************************************
 * readAcc.c
 *
 * Example code which reads acceleration in
 * three dimensions and displays the resulting data on
 * the Orbit OLED display.
 *
 * FitnessMonGroup8
 * S. Allen, J. Zhu, G. Lay
 *
 *    C. P. Moore
 *    11 Feb 2020
 *
 **********************************************************/

#include <readAcc.h>

/**********************************************************
 * Constants
 **********************************************************/
// Systick configuration
//#define SYSTICK_RATE_HZ    100
enum dispModes {RAW = 0, GRAV, MPS};

/***********************************************************
 * Initialisation functions: clock, SysTick, PWM
 ***********************************************************
 * Clock
 ***********************************************************/
/*void initClock (void)
{
    // Set the clock rate to 20 MHz
    SysCtlClockSet (SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);
} */

/*********************************************************
 * initDisplay
 *********************************************************/
/*void initDisplay (void)
{
    // Initialise the Orbit OLED display
    OLEDInitialise ();
} */



/*********************************************************
 * initAccl
 *********************************************************/
void initAccl (void)
{
    char    toAccl[] = {0, 0};  // parameter, value

    /*
     * Enable I2C Peripheral
     */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
    SysCtlPeripheralReset(SYSCTL_PERIPH_I2C0);

    /*
     * Set I2C GPIO pins
     */
    GPIOPinTypeI2C(I2CSDAPort, I2CSDA_PIN);
    GPIOPinTypeI2CSCL(I2CSCLPort, I2CSCL_PIN);
    GPIOPinConfigure(I2CSCL);
    GPIOPinConfigure(I2CSDA);

    /*
     * Setup I2C
     */
    I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), true);

    GPIOPinTypeGPIOInput(ACCL_INT2Port, ACCL_INT2);

    //Initialize ADXL345 Acceleromter

    // set +-2g, 10 bit resolution, active low interrupts
    toAccl[0] = ACCL_DATA_FORMAT;
    toAccl[1] = (ACCL_RANGE_16G | ACCL_FULL_RES);
    I2CGenTransmit(toAccl, 1, WRITE, ACCL_ADDR);

    toAccl[0] = ACCL_PWR_CTL;
    toAccl[1] = ACCL_MEASURE;
    I2CGenTransmit(toAccl, 1, WRITE, ACCL_ADDR);


    toAccl[0] = ACCL_BW_RATE;
    toAccl[1] = ACCL_RATE_100HZ;
    I2CGenTransmit(toAccl, 1, WRITE, ACCL_ADDR);

    toAccl[0] = ACCL_INT;
    toAccl[1] = 0x00;       // Disable interrupts from accelerometer.
    I2CGenTransmit(toAccl, 1, WRITE, ACCL_ADDR);

    toAccl[0] = ACCL_OFFSET_X;
    toAccl[1] = 0x00;
    I2CGenTransmit(toAccl, 1, WRITE, ACCL_ADDR);

    toAccl[0] = ACCL_OFFSET_Y;
    toAccl[1] = 0x00;
    I2CGenTransmit(toAccl, 1, WRITE, ACCL_ADDR);

    toAccl[0] = ACCL_OFFSET_Z;
    toAccl[1] = 0x00;
    I2CGenTransmit(toAccl, 1, WRITE, ACCL_ADDR);
}

/********************************************************
 * Function to read accelerometer
 ********************************************************/
vector3_t getAcclData (void)
{
    char    fromAccl[] = {0, 0, 0, 0, 0, 0, 0}; // starting address, placeholders for data to be read.
    vector3_t acceleration;
    uint8_t bytesToRead = 6;

    fromAccl[0] = ACCL_DATA_X0;
    I2CGenTransmit(fromAccl, bytesToRead, READ, ACCL_ADDR);

    acceleration.x = (fromAccl[2] << 8) | fromAccl[1]; // Return 16-bit acceleration readings.
    acceleration.y = (fromAccl[4] << 8) | fromAccl[3];
    acceleration.z = (fromAccl[6] << 8) | fromAccl[5];

/*    // Convert to desired units, if necessary.
    if (unitMode == GRAV) {
        acceleration.x = acceleration.x / 128; // TODO; enable floats and make this output to two decimal points.
        acceleration.y = acceleration.y / 128;
        acceleration.z = acceleration.z / 128;
    }
    if (unitMode == MPS) {
        acceleration.x = acceleration.x / 26; // TODO; enable floats and output to one decimal point.
        acceleration.y = acceleration.y / 26;
        acceleration.z = acceleration.z / 26;
    }*/


    return acceleration;
}

//*****************************************************************************
// Stores the values obtained by the accelerometer into the circular buffer
//*****************************************************************************
void store_accl(vector3_t acceleration, circBuf_t *buffer_x, circBuf_t *buffer_y, circBuf_t *buffer_z)
{
    writeCircBuf(buffer_x, acceleration.x);
    writeCircBuf(buffer_y, acceleration.y);
    writeCircBuf(buffer_z, acceleration.z);
}

//*****************************************************************************
// Calculates the mean of the circular buffer and returns a 3 vector of x, y and z
//*****************************************************************************
vector3_t calculate_mean(circBuf_t *buffer_x, circBuf_t *buffer_y, circBuf_t *buffer_z, uint8_t buf_size)
{
    vector3_t sum;
    vector3_t average;
    int i;
    sum.x = 0;
    sum.y = 0;
    sum.z = 0;

    // Sum all values in the circular buffer
    for (i = 0; i < buf_size; i++) {
        sum.x = sum.x + readCircBuf (buffer_x);
        sum.y = sum.y + readCircBuf (buffer_y);
        sum.z = sum.z + readCircBuf (buffer_z);
    }
    // Divide all values in the circular buffer to get mean
    average.x = sum.x / buf_size;
    average.y = sum.y / buf_size;
    average.z = sum.z / buf_size;

    return average;
}

// void function to reset the calibration of the accelerometer as displayed
void accCalibrate (void)
{
     //  blank for now TODO; this is where we reset the accelerometers to zero
}


