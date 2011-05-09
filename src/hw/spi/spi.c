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

/**********************************************************************************************
 * FILE PURPOSE: The SPI boot driver
 **********************************************************************************************
 * FILE NAME: spi.c
 *
 * DESCRIPTION: Implements an SPI eeprom read.
 *
 **********************************************************************************************/
#include "types.h"
#include "device.h"
#include "spi_api.h"
#include "spi_loc.h"

typedef struct spimcb_s
{
    spiConfig_t spiCfg;

} spimcb_t;

spimcb_t spimcb;


/**********************************************************************************************
 * FUNCTION PURPOSE: Initialize the SPI interface
 **********************************************************************************************
 * DESCRIPTION: The SPI interface is setup. Only format register 0 is configured
 **********************************************************************************************/
SINT16 hwSpiConfig (spiConfig_t *cfg)
{
    UINT32 v;

    if ((cfg->addrWidth != 24) && (cfg->addrWidth != 16))
        return (SPI_INVALID_ADDR_WIDTH);

    if ((cfg->npin != 4) && (cfg->npin != 5))
        return (SPI_INVALID_NPIN);

    /* Store the configuration for subsequent reads */
    spimcb.spiCfg = *cfg;
    

    /* Reset */
    DEVICE_REG32_W (DEVICE_SPI_BASE(cfg->port) + SPI_REG_SPIGCR0, SPI_REG_VAL_SPIGCR0_RESET);

    /* Release Reset */
    DEVICE_REG32_W (DEVICE_SPI_BASE(cfg->port) + SPI_REG_SPIGCR0, SPI_REG_VAL_SPIGCR0_ENABLE);

    /* Master mode */
    if (cfg->npin == 4)  
        DEVICE_REG32_W (DEVICE_SPI_BASE(cfg->port) + SPI_REG_SPIPC0, SPI_REG_VAL_SPIPC0_4PIN);
    else
        DEVICE_REG32_W (DEVICE_SPI_BASE(cfg->port) + SPI_REG_SPIPC0, SPI_REG_VAL_SPIPC0_5PIN);

    /* Format 0 register */
    v = 0;
    if (cfg->clkdiv > 0)
        SPI_REG_SPIFMT_SET_PRESCALE(v, cfg->clkdiv - 1);
    else
        SPI_REG_SPIFMT_SET_PRESCALE(v, 0);

    SPI_REG_SPIFMT_SET_CHARLEN(v, 8);
    SPI_REG_SPIFMT_SET_MODE(v, cfg->mode);
    SPI_REG_SPIFMT_SET_SHIFTDIR(v, SPI_REG_VAL_SPIFMT_SHIFT_MSB_FIRST);
    DEVICE_REG32_W (DEVICE_SPI_BASE(cfg->port) + SPI_REG_SPIFMT(0), v);

    /* Chip select to transmit delay */
    v = 0;
    SPI_REG_SPIDELAY_SET_C2T(v, cfg->c2tdelay);
    DEVICE_REG32_W (DEVICE_SPI_BASE(cfg->port) + SPI_REG_SPIDELAY, v);


    return (0);

} /* hwSpiConfig */


/*************************************************************************************************
 * FUNCTION PURPOSE: Perform an SPI transfer
 *************************************************************************************************
 * DESCRIPTION: A bi-directional transfer is done
 *************************************************************************************************/
SINT16 hw_spi_xfer (UINT32 nbytes, UINT8 *dataOut, UINT8 *dataIn, spiConfig_t *cfg, BOOL terminate)
{
    UINT32 v;
    UINT32 i;
    UINT32 tcount;
    UINT32 timeout;
    UINT32 spidat1;


    /* The SPIDAT1 upper 16 bits */
    spidat1 = 0;
    SPI_REG_SPIDAT1_SET_CSHOLD(spidat1, 1);
    SPI_REG_SPIDAT1_SET_WDELAY(spidat1, 1);
    SPI_REG_SPIDAT1_SET_CSNR(spidat1, cfg->csel);


    /* The timeout is used to prevent traps. 
     *
     * Each bit on the SPI bus will clock in at MOD_DIVIDER * cfg->clkdiv
     * cpu cycles. So the timeout is based on the MOD_DIVIDER * cfg->clkdiv
     * product. The actual number of cycles the loop takes isnt known
     * with too much accuracy since the code is in C, so assume the
     * compiler is spectacular and condenses each loop into just one
     * cpu cycle. In that case it would take MOD_DIVIDER * cfg->clkdiv * 8
     * passes through the loop to get the data byte. Since the compiler
     * is slower then that the timeout value is good. But an extra
     * x20 is thrown in just to be safe. */

    timeout = (cfg->clkdiv + 1) * 8 * 20 * DEVICE_SPI_MOD_DIVIDER;


    /* Clear out any pending read data */
    do  {
        v = DEVICE_REG32_R (DEVICE_SPI_BASE(cfg->port) + SPI_REG_SPIBUF);
        v = DEVICE_REG32_R (DEVICE_SPI_BASE(cfg->port) + SPI_REG_SPIFLG);
    }  while (SPI_REG_SPIFLG_RX_DATA(v));


    /* Perform the transfer */
    for (i = 0; i < nbytes; i++)  {


        /* For the last byte release the hold */
        if ((terminate == TRUE) && (i == (nbytes - 1)))  {
            SPI_REG_SPIDAT1_SET_CSHOLD(spidat1, 0);
        }

        tcount = 0;

        do  {

            v = DEVICE_REG32_R (DEVICE_SPI_BASE(cfg->port) + SPI_REG_SPIFLG);
            tcount += 1;

        } while ( (SPI_REG_SPIFLG_TX_EMPTY(v) == 0) && (tcount < timeout) );

        if (tcount >= timeout)  {

            /* Disable transfer */
            DEVICE_REG32_W (DEVICE_SPI_BASE(cfg->port) + SPI_REG_SPIGCR1, SPI_REG_VAL_SPIGCR1_XFER_DISABLE);

            return (SPI_TIMEOUT);
        }
        
        if (dataOut != NULL)
            v = dataOut[i];
        else
            v = 0;


        /* Send the data */
        SPI_REG_SPIDAT1_SET_DATA(spidat1, v);
        DEVICE_REG32_W (DEVICE_SPI_BASE(cfg->port) + SPI_REG_SPIDAT1, spidat1);

        /* Receive the data */
        tcount = 0;

        do  {

            v = DEVICE_REG32_R (DEVICE_SPI_BASE(cfg->port) + SPI_REG_SPIFLG);
            tcount += 1;

        } while ( (SPI_REG_SPIFLG_RX_DATA(v) == 0) && (tcount < timeout) );

        if (tcount >= timeout)  {

            /* Disable transfer */
            DEVICE_REG32_W (DEVICE_SPI_BASE(cfg->port) + SPI_REG_SPIGCR1, SPI_REG_VAL_SPIGCR1_XFER_DISABLE);

            return (SPI_TIMEOUT);
        }


        v = DEVICE_REG32_R (DEVICE_SPI_BASE(cfg->port) + SPI_REG_SPIBUF);

        if (dataIn != NULL)
            dataIn[i] = v & 0xff;


    }

    return (0);

}




/*************************************************************************************************
 * FUNCTION PURPOSE: Read a block of data
 *************************************************************************************************
 * DESCRIPTION: A single data block of a fixed size is read
 *************************************************************************************************/
SINT16 hwSpiRead (UINT32 addr, UINT32 sizeBytes, UINT8 *data)
{
    UINT32 n;
    UINT8  command[4];
    UINT16 ret;

    /* Do nothing for a read of 0 bytes */
    if (sizeBytes == 0)
        return (0);

    /* Format the read command and address */
    command[0] = SPI_COMMAND_READ;
    if (spimcb.spiCfg.addrWidth == 24)  {
        n = 4;
        command[1] = (addr >> 16) & 0xff;
        command[2] = (addr >>  8) & 0xff;
        command[3] = addr & 0xff;

    }  else if (spimcb.spiCfg.addrWidth == 16)  {
        n = 3;
        command[1] = (addr >> 8) & 0xff;
        command[2] = addr & 0xff;

    }  else  {
        return (SPI_INVALID_ADDR_WIDTH);
    }


    /* Enable the device for transfer */
    DEVICE_REG32_W (DEVICE_SPI_BASE(spimcb.spiCfg.port) + SPI_REG_SPIGCR1, SPI_REG_VAL_SPIGCR1_XFER);

    /* Send the command and address */
    ret = hw_spi_xfer (n, command, NULL, &spimcb.spiCfg, FALSE);
    if (ret != 0)
        return (ret);


    /* Read the data */
    ret = hw_spi_xfer (sizeBytes, NULL, data, &spimcb.spiCfg, TRUE);
    if (ret != 0)
        return (ret);


    DEVICE_REG32_W (DEVICE_SPI_BASE(spimcb.spifg.port) + SPI_REG_SPIGCR1, SPI_REG_VAL_SPIGCR1_XFER_DISABLE);
    return (0);

}


    



        












