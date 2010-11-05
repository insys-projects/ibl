/*
 *
 * Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/ 
 * 
 * 
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions 
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the   
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/



/****************************************************************************************************
 * FILE PURPOSE: Read an I2C eeprom
 ****************************************************************************************************
 * @file i2cRead.c
 *
 * @brief
 *   Reads bytes from an i2c eeprom
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
unsigned int   nbytes        = I2C_SIZE_BYTES;
unsigned int   firstByte     = 0;


#pragma DATA_SECTION(i2cData, ".i2cData")
unsigned char i2cData[I2C_SIZE_BYTES];

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

    hwI2Cinit (deviceFreqMhz,
               DEVICE_I2C_MODULE_DIVISOR,
               25,                              /* Run the bus at 25 kHz */
               10);


    i2cRet = hwI2cMasterRead (firstByte,
                              nbytes,
                              i2cData,
                              busAddress,
                              10 );


    if (i2cRet != I2C_RET_OK) 
        showI2cError (i2cRet);
    else
        printf ("I2C read complete\n");

}
        






