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
 * FILE PURPOSE: Write the ibl configuration table to the spi eeprom
 *************************************************************************************
 * @file spiparam.c
 *
 * @brief
 *	  Creates the ibl configuration table and writes to the spi 
 *
 *************************************************************************************/

#include "types.h" 
#include "ibl.h"
#include "spi_api.h"
#include "pllapi.h"
#include "iblcfg.h"
#include "target.h"
#include "string.h"
#include "spiWutil.h"
#include <stdlib.h>
#include <stdio.h>

ibl_t ibl;

unsigned int  configAddress = 0;
unsigned int  spiBlockSize  = 256;

#define MAX_SPI_BLOCK_SIZE    2048
uint8 scratch[MAX_SPI_BLOCK_SIZE];

spiConfig_t cfg =  {

   0,               /* port */
   SPI_MODE,        /* mode */
   SPI_ADDR_WIDTH,  /* address width */
   SPI_NPIN,        /* number of pins */
   SPI_CSEL,        /* csel */
   SPI_CLKDIV,      /* clock divider */
   SPI_C2TDEL       /* ct2delay */

};

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


void main (void)
{
    int      result;
    uint16   chk; 
    int      nblocks;
    uint32   addr;
    uint32  *iblUi32;

    volatile Int32 i;

    if (spiBlockSize > MAX_SPI_BLOCK_SIZE)  {
        printf ("Error: spi block size (%d) greater then this program can handle (%d).\n", spiBlockSize, MAX_SPI_BLOCK_SIZE);
        printf ("       Change the definition of MAX_SPI_BLOCK_SIZE and recompile\n");
        return;
    }

    if (configAddress == 0)  {
      printf ("Error: The global variable config address must be setup prior to running this program\n");
      printf ("       This is the address in the I2C eeprom where the parameters live. On configurations\n");
      printf ("       which support both big and little endian it is possible to configure the IBL to\n");
      printf ("       usage a different configuration table for each endian, so this program must be run\n");
      printf ("       twice. The value 0 is invalid for configAddress\n");
      return;
    }

    printf ("Run the GEL for for the device to be configured, press return to program the I2C\n");
    getchar ();

    /* Program the main system PLL */
    hwPllSetPll (MAIN_PLL,      
                 ibl.pllConfig[ibl_MAIN_PLL].prediv,         /* Pre-divider  */
                 ibl.pllConfig[ibl_MAIN_PLL].mult,           /* Multiplier   */
                 ibl.pllConfig[ibl_MAIN_PLL].postdiv);       /* Post-divider */


    result = hwSpiConfig (&cfg);
    if (result != 0)  {
        printf ("hwSpiConfig returned error %d\n", result);
        return;
    }


    /* Compute the checksum over the ibl configuration structure */
    ibl.chkSum = 0;
    chk = onesComplementChksum ((uint16 *)&ibl, sizeof(ibl_t)/sizeof(uint16));
    if (ibl.chkSum != 0xffff)
      ibl.chkSum = ~chk;

    

    /* Write the configuration table out one block at a time */
    nblocks = (sizeof(ibl_t) + spiBlockSize - 1) / spiBlockSize;
    iblUi32 = (uint32 *)&ibl;

    for (i = 0, addr = configAddress; i < nblocks; i++, addr += spiBlockSize)  {

        result = writeBlock (&cfg, addr, &iblUi32[(i * spiBlockSize) >> 2], spiBlockSize, scratch);
        if (result != 0)  {
            printf ("writeBlock returned error code %d, exiting\n", result);
            return;
        }
    }


    printf ("SPI table write complete\n");

}



        







