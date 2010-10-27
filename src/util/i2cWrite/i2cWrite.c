/****************************************************************************************************
 * FILE PURPOSE: Write an I2C eeprom
 ****************************************************************************************************
 * @file i2cWrite.c
 *
 * @brief
 *   Writes bytes to an i2c eeprom. The bytes are stored in word format, and written out
 *   in big endian format, regardless of the endianness of the device.
 *
 ****************************************************************************************************/

#include "types.h"
#include "i2c.h"
#include "target.h"
#include <stdio.h>

#define I2C_SIZE_BYTES  0x10000

/* Run time configuration */
unsigned int   deviceFreqMhz = 1000;
unsigned short busAddress    = 0x50;
unsigned int   i2cBlockSize  = 64;
unsigned int   nbytes        = I2C_SIZE_BYTES;
unsigned int   dataAddress   = 0;


#pragma DATA_SECTION(i2cData, ".i2cData")
unsigned int i2cData[I2C_SIZE_BYTES >> 2];

#define I2C_MAX_BLOCK_SIZE_BYTES    256
unsigned char i2cBlock[I2C_MAX_BLOCK_SIZE_BYTES+4];  /* need 2 bytes for the address */

/** 
 *  @brief
 *      Form a block of data to write to the i2c. The block is 
 *      created as a byte stream from the 4 byte stream in which
 *      the MSB is always sent first.
 */
int formBlock (unsigned int addr, int byteIndex, int n)
{
    int p;
    int i;

    /* Must start on a word aligned boundary */
    if ((n & 0x3) != 0)  {
        printf ("Error: Invalid block base offset specified\n");
        return (-1);
    }

    /* The 1st two bytes are the address */
    i2cBlock[0] = (addr >> 8) & 0xff;
    i2cBlock[1] = (addr >> 0) & 0xff;

    p = byteIndex >> 2;


    for (i = 0; i < i2cBlockSize; i += 4, p++)  {

        i2cBlock[i+2+0] = (i2cData[p] >> 24) & 0xff;
        i2cBlock[i+2+1] = (i2cData[p] >> 16) & 0xff;
        i2cBlock[i+2+2] = (i2cData[p] >>  8) & 0xff;
        i2cBlock[i+2+3] = (i2cData[p] >>  0) & 0xff;

    }

    return (n+2);

}
    

/**
 *  @brief
 *     Display the error returned by the i2c driver 
 */
void showI2cError (I2C_RET iret)
{
    char *ecode;

    switch (iret)  {
        case I2C_RET_LOST_ARB:        ecode = "I2C master lost an arbitration battle";
                                      break;

        case I2C_RET_NO_ACK:          ecode = "I2C master detected no ack from slave";
                                      break;

        case I2C_RET_IDLE_TIMEOUT:    ecode = "I2C timed out";
                                      break;

        case I2C_RET_BAD_REQUEST:     ecode = "I2C driver detected a bad data request";
                                      break;

        case I2C_RET_CLOCK_STUCK_LOW: ecode = "I2C driver found the bus stuck low";
                                      break;

        case I2C_RET_GEN_ERROR:       ecode = "I2C driver reported a general error";
                                      break;

    }

    printf ("I2C reported error: %s\n", ecode);

}


void main (void)
{
    I2C_RET i2cRet;
    int     n;
    int     remain;
    int     progBytes;

    volatile int i;

    hwI2Cinit (deviceFreqMhz,
               DEVICE_I2C_MODULE_DIVISOR,
               25,                              /* Run the bus at 25 kHz */
               10);



    for (n = 0; n < nbytes; n += i2cBlockSize)  {

        remain = nbytes - n;
        if (remain > i2cBlockSize)
            remain = i2cBlockSize;

        /* formBlock sets up the address as well as the data */
        progBytes = formBlock (dataAddress + n, n, remain);

        if (progBytes < 0)
            return;

        /* Write the block */
        i2cRet = hwI2cMasterWrite (busAddress,
                                   i2cBlock,
                                   progBytes,
                                   I2C_RELEASE_BUS,
                                   FALSE);
        

        if (i2cRet != I2C_RET_OK)  {
            showI2cError (i2cRet);
            return;
        }



        /* Some delay */
        for (i = 0; i < 600000; i++);

    }


    printf ("I2C write complete\n");
}
        






