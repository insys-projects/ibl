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

/**
 *  @file spiWrite.c
 *
 *  @brief
 *      Write to an SPI eeprom
 */

#include "types.h"
#include "spi_api.h"
#include "target.h"
#include "pllapi.h"
#include <stdio.h>
#include <string.h>

#pragma DATA_SECTION(txData, ".tx_data")
#pragma DATA_SECTION(rxData, ".rx_data")

#define MAX_DATA_SIZE         0x40000  /* 256 kb */
uint32  txData[MAX_DATA_SIZE >> 2];
uint32  rxData[MAX_DATA_SIZE >> 2];

#define MAX_BLOCK_SIZE          2048
uint8   scratchBlock[MAX_BLOCK_SIZE];

unsigned int nbytes       = MAX_DATA_SIZE;
unsigned int startAddress = 0;

/* Run time configuration */
unsigned int  deviceFreqMhz = 1000;
unsigned int  prediv        = 1;
unsigned int  mult          = 20;
unsigned int  postdiv       = 2;

unsigned int  spiBlockSize  = 256;
unsigned int  spiSectorSize = 4096;     /* Erase size */

spiConfig_t cfg =  {

   0,               /* port */
   SPI_MODE,        /* mode */
   SPI_ADDR_WIDTH,  /* address width */
   SPI_NPIN,        /* number of pins */
   SPI_CSEL,        /* csel */
   SPI_CLKDIV,      /* clock divider */
   SPI_C2TDEL       /* ct2delay */

};



int main (void)
{
   int result;
   int firstSector;
   int lastSector;
   int nblocks;
   int i, j, k;

   unsigned int addr;

    /* This writer requires that the start address be block aligned */
    if ((startAddress % spiBlockSize) != 0)  {
      printf ("spiWriter requires that the start address be block aligned, exiting\n");
      return (-1);
    }

    /* Check that the block size is within range */
    if (spiBlockSize > MAX_BLOCK_SIZE)  {
      printf ("The block size selected (%d) is greater then the max size of %d.\n", spiBlockSize, MAX_BLOCK_SIZE);
      printf ("Increase the value of MAX_BLOCK_SIZE and recompile the program\n");
      return (-1);
    }

    /* Power up the SPI */
    #ifdef TARGET_PWR_SPI
        devicePowerPeriph (TARGET_PWR_SPI);
    #endif


    /* Program the main system PLL */
    hwPllSetPll (0,          /* Main PLL */
                 prediv,     /* Pre-divider */
                 mult,       /* Multiplier */
                 postdiv);   /* Post-divider */


    result = hwSpiConfig (&cfg);
    if (result != 0)  {
      printf ("hwSpiConfig returned error %d\n", result);
      return (-1);
    }

    /* Erase all sectors that will be used */
    firstSector = startAddress / spiSectorSize;
    lastSector  = (startAddress + nbytes + spiSectorSize - 1) / spiSectorSize;

    printf ("Erasing sector: ");
    for (i = firstSector; i <= lastSector; i++)  {
        printf ("%d ", i);
        eraseSector (&cfg, i * spiSectorSize);
    }

    printf ("done\n");


    /* Program all the data */
    nblocks = (nbytes + spiBlockSize - 1) / spiBlockSize;

    printf ("Writing blocks ");
    for (i = 0, addr = startAddress; i < nblocks; i++, addr += spiBlockSize)  {

        result = writeBlock (&cfg, addr, &txData[(i * spiBlockSize) >> 2], spiBlockSize, scratchBlock);
        if (result != 0)  {
            printf ("\nwriteBlock returned error code %d, exiting\n", result);
            return (-1);
        }

        printf (".");
    }

    printf ("\n");


    /* Read the data as bytes. Translate it to words */
    printf ("Reading blocks ");
    for (i = j = 0, addr = startAddress; i < nblocks; i++, addr += spiBlockSize)  {

        result = readBlock (&cfg, addr, scratchBlock, spiBlockSize);

        if (result != 0)  {
            printf ("\nreadBlock returned error code %d, exiting\n", result);
            return (-1);
        }

        for  (k = 0; k < spiBlockSize; k += 4)
            rxData[j++] = (scratchBlock[k+0] << 24) | (scratchBlock[k+1] << 16) | (scratchBlock[k+2] << 8) | scratchBlock[k+3];

        printf (".");
    }

    printf ("\n");

    /* Compare the results */
    printf ("Checking the read back\n");

    for (i = j = 0; i < nbytes >> 2; i++)  {

        if (txData[i] != rxData[i])  {

            j += 1;
            printf ("failure at word %d: Expected 0x%04x, found 0x%04x\n", i, txData[i], rxData[i]);

        }

        if (j >= 5)  {
            printf ("Too many errors, quitting\n");
            break;
        }
    }

    if (j == 0)
        printf ("Read back verified\n");


    return (0);

}
        





    

