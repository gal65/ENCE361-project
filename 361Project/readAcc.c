/* readAcc.c
 *
 * Example code which reads acceleration in
 * three dimensions and displays the resulting data on
 * the Orbit OLED display.
 *
 * FitnessMonGroup8
 * S. Allen, J. Zhu, G. Lay
 * Based on code by C. P. Moore
 */

#include <readAcc.h>

// Initiate accelerometer
void initAccl (void)
{
    char    toAccl[] = {0, 0};  // parameter, value

    // Enable I2C Peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
    SysCtlPeripheralReset(SYSCTL_PERIPH_I2C0);

    // Set I2C GPIO pins
    GPIOPinTypeI2C(I2CSDAPort, I2CSDA_PIN);
    GPIOPinTypeI2CSCL(I2CSCLPort, I2CSCL_PIN);
    GPIOPinConfigure(I2CSCL);
    GPIOPinConfigure(I2CSDA);

    // Setup I2C
    I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), true);

    GPIOPinTypeGPIOInput(ACCL_INT2Port, ACCL_INT2);

    //Initialize ADXL345 Accelerometer
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

// function to read accelerometer
vector3_t getAcclData (void)
{
    vector3_t acceleration;
    vector3_t mean_acc;
    char    fromAccl[] = {0, 0, 0, 0, 0, 0, 0}; // starting address, placeholders for data to be read.
    uint8_t bytesToRead = 6;

    fromAccl[0] = ACCL_DATA_X0; // read all accel data
    I2CGenTransmit(fromAccl, bytesToRead, READ, ACCL_ADDR);

    acceleration.x = (fromAccl[2] << 8) | fromAccl[1]; // Return 16-bit acceleration readings.
    acceleration.y = (fromAccl[4] << 8) | fromAccl[3]; // split into axes
    acceleration.z = (fromAccl[6] << 8) | fromAccl[5];

    store_accl(acceleration, &inBuffer_x, &inBuffer_y, &inBuffer_z);
    mean_acc = calculate_mean(&inBuffer_x, &inBuffer_y, &inBuffer_z,BUF_SIZE);

    return mean_acc;
}


// Stores the values obtained by the accelerometer into the circular buffer
void store_accl(vector3_t acceleration, circBuf_t *buffer_x, circBuf_t *buffer_y, circBuf_t *buffer_z)
{
    writeCircBuf(buffer_x, acceleration.x);
    writeCircBuf(buffer_y, acceleration.y);
    writeCircBuf(buffer_z, acceleration.z);
}


// Calculates the mean of the circular buffer and returns a 3 vector of the raw x, y and z values
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




