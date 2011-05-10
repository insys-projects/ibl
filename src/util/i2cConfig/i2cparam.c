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



/*************************************************************************************
 * FILE PURPOSE: Write the ibl configuration table to the i2c eeprom
 *************************************************************************************
 * @file i2cparam.c
 *
 * @brief
 *	  Creates the ibl configuration table and writes to the i2c
 *
 *************************************************************************************/

#include "types.h" 
#include "ibl.h"
#include "i2c.h"
#include "pllapi.h"
#include "iblcfg.h"
#include "target.h"
#include "string.h"
#include <stdlib.h>
#include <stdio.h>

ibl_t ibl;

/* The configAddress must be programmed. On images which support both endians
 * there can be two seperate configurations, one for big endian, and one for little */
unsigned int configBusAddress = I2C_BUS_ADDR;
unsigned int configAddress = I2C_MAP_ADDR;


/**
 *  @brief
 *      Ones complement addition
 */
inline uint16 onesComplementAdd (uint16 value1, uint16 value2)
{
  uint32 result;

  result = (uint32)value1 + (uint32)value2;

  result = (result >> 16) + (result & 0xFFFF); /* add in carry   */
  result += (result >> 16);                    /* maybe one more */
  return ((uint16)result);
}


/**
 *  @brief
 *      Ones complement checksum computation 
 */
uint16 onesComplementChksum (uint16 * restrict p_data, uint16 len)
{
  uint16 chksum = 0;

  while (len > 0)
  {
    chksum = onesComplementAdd(chksum, *p_data);
    p_data++;
    len--;
  }
  return (chksum);
} 

/**
 *  @brief
 *     Display the error returned by the i2c driver 
 */
void showI2cError (I2C_RET iret, char *stage)
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

    printf ("Error: %s, reported at stage: %s\n", ecode, stage);

}

/**
 * @brief
 *   Group the data into a 64 byte (max) data block aligned on a 64 byte boundary 
 */
Int32 formBlock (UINT8 *base, Int32 dataSize, Int32 *offset, Uint8 *data, Uint32 baseI2cDataAddr)
{
    Uint32 currentAddress;
    Int32  nbytes;
    Int32  i;

    /* The I2C eeprom address is the parameter base address plus the current offset
     * The number of bytes is blocked to end on a 64 byte boundary */
    currentAddress = baseI2cDataAddr + *offset;
    nbytes         = 64 - (currentAddress & (64 - 1));

    /* Only write the bytes in the input array */
    if (*offset + nbytes > dataSize)
        nbytes = dataSize - *offset;

    if (nbytes == 0)
        return (0);

    /* The address is placed first */
    data[0] = (currentAddress >> 8) & 0xff;
    data[1] = (currentAddress >> 0) & 0xff;

    /* The data */
    for (i = 0; i < nbytes; i++)
        data[i+2] = base[*offset + i];

    /* Update the offset */
    *offset += nbytes;

    /* Return the total number of bytes (including address) to go on the bus */
    return (nbytes + 2);

}



void main (void)
{
    I2C_RET  iret;
    UINT8    writeBlock[66];
    char     iline[132];
    Int32    n; 
    Int32    currentOffset;
    uint16   chk; 

    volatile Int32 i;

    if (configAddress == 0)  {
      printf ("Error: The global variable config address must be setup prior to running this program\n");
      printf ("       This is the address in the I2C eeprom where the parameters live. On configurations\n");
      printf ("       which support both big and little endian it is possible to configure the IBL to\n");
      printf ("       usage a different configuration table for each endian, so this program must be run\n");
      printf ("       twice. The value 0 is invalid for configAddress\n");
      return;
    }

    memset(&ibl, 0, sizeof(ibl_t));

    printf ("Run the GEL for for the device to be configured, press return to program the I2C\n");
    getchar ();

    /* Program the main system PLL */
    hwPllSetPll (MAIN_PLL,      
                 ibl.pllConfig[ibl_MAIN_PLL].prediv,         /* Pre-divider  */
                 ibl.pllConfig[ibl_MAIN_PLL].mult,           /* Multiplier   */
                 ibl.pllConfig[ibl_MAIN_PLL].postdiv);       /* Post-divider */


    hwI2Cinit (ibl.pllConfig[ibl_MAIN_PLL].pllOutFreqMhz,  /* The CPU frequency during I2C data load */
               DEVICE_I2C_MODULE_DIVISOR,                  /* The divide down of CPU that drives the i2c */
               IBL_CFG_I2C_CLK_FREQ_KHZ/8,                 /* The I2C data rate used during table load. Slowed for writes */
               IBL_CFG_I2C_OWN_ADDR);                      /* The address used by this device on the i2c bus */


    /* Compute the checksum over the ibl configuration structure */
    ibl.chkSum = 0;
    chk = onesComplementChksum ((uint16 *)&ibl, sizeof(ibl_t)/sizeof(uint16));
    if (ibl.chkSum != 0xffff)
      ibl.chkSum = ~chk;

    

    /* Block the data into 64 byte blocks aligned on 64 byte boundaries for the data write.
     * The block address is prepended to the data block before writing */
    currentOffset = 0;
    do  {

        n = formBlock ((UINT8 *)&ibl, sizeof(ibl_t), &currentOffset, writeBlock, configAddress);

        if (n > 0)  {

            iret = hwI2cMasterWrite (configBusAddress,   /* The I2C bus address of the eeprom */
                                     writeBlock,         /* The data to write */
                                     n,                  /* The number of bytes to write */
                                     I2C_RELEASE_BUS,    /* Release the bus when the write is done */
                                     FALSE );            /* Bus is not owned at start of operation */

            if (iret != I2C_RET_OK)  {
                sprintf (iline, "Block at offset %d\n", currentOffset);
                showI2cError (iret, iline);
                return;
            }

            /* Need some delay to allow the programming to occur */
            for (i = 0; i < 600000; i++);

        }

    } while (n > 0);
            
    

    printf ("I2c table write complete\n");

}

