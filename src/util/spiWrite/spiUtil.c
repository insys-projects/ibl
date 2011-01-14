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
 * @file spiUtil.c
 *
 *  @brief
 *    I2C access functions 
 */
#include "types.h"
#include "spi_api.h"
#include "target.h"
#include "spi_loc.h"
#include "spiWutil.h"


/**
 *  @brief Wait until the Write In Progress bit is clear
 */

void waitWip (spiConfig_t *cfg)
{
    uint8  cmdBuf[4];
    uint8  rBuf[4];

    /* Loop until the status register write in progress is clear */
    hwSpiEnableXfer (cfg->port);
    do  {
        
        cmdBuf[0] = SPI_COMMAND_READ_STATUS;

        hw_spi_xfer (1, cmdBuf, NULL, cfg, FALSE);
        hw_spi_xfer (1, NULL, rBuf, cfg, TRUE);

    }  while (rBuf[0] & 0x1);

    hwSpiDisableXfer (cfg->port);

}
    


/**
 *  @brief Erase a sector, wait for completion
 */
int eraseSector (spiConfig_t *cfg, unsigned address)
{
    int    ret;
    uint8  cmdBuf[4];
    UINT16 n;

    hwSpiEnableXfer (cfg->port);

    /* Enable write */
    cmdBuf[0] = SPI_COMMAND_WRITE_ENABLE;
    ret = hw_spi_xfer (1, cmdBuf, NULL, cfg, TRUE);
    if (ret != 0)
        return (ret);

    /* Send the erase */
    cmdBuf[0] = SPI_COMMAND_ERASE_SECTOR;

    if (cfg->addrWidth == 24)  {
        cmdBuf[1] = (address >> 16) & 0xff;
        cmdBuf[2] = (address >>  8) & 0xff;
        cmdBuf[3] = (address >>  0) & 0xff;
        n = 4;

    }  else  {
        cmdBuf[1] = (address >> 8) & 0xff;
        cmdBuf[2] = (address >> 0) & 0xff;
        n = 3;
    }


    ret = hw_spi_xfer (n, cmdBuf, NULL, cfg, TRUE);

    waitWip (cfg);

    return (0);

}


/**
 *  @brief Write data to the SPI
 */

int writeBlock (spiConfig_t *cfg, unsigned int address, uint32 *data, int blockSize, uint8 *scratch)
{
    uint8  cmdBuf[4];
    UINT16 n;
    int i, j;
    int ret;

    /* Convert the data to a byte stream */
    for (i = j = 0; j < blockSize; i++, j+=4)  {
        scratch[j+0] = (data[i] >> 24) & 0xff;
        scratch[j+1] = (data[i] >> 16) & 0xff;
        scratch[j+2] = (data[i] >>  8) & 0xff;
        scratch[j+3] = (data[i] >>  0) & 0xff;
    }

    
    /* Enable write */
    hwSpiEnableXfer (cfg->port);
    cmdBuf[0] = SPI_COMMAND_WRITE_ENABLE;
    ret = hw_spi_xfer (1, cmdBuf, NULL, cfg, TRUE);
    if (ret != 0)
        return (ret);

    /* Send the write command */
    cmdBuf[0] = SPI_COMMAND_WRITE;

    if (cfg->addrWidth == 24)  {
        cmdBuf[1] = (address >> 16) & 0xff;
        cmdBuf[2] = (address >>  8) & 0xff;
        cmdBuf[3] = (address >>  0) & 0xff;
        n = 4;

    }  else  {
        cmdBuf[1] = (address >> 8) & 0xff;
        cmdBuf[2] = (address >> 0) & 0xff;
        n = 3;
    }

    /* Send the command and address */
    ret = hw_spi_xfer (n, cmdBuf, NULL, cfg, FALSE);
    if (ret != 0)
        return (ret);

    /* Send the data */
    ret = hw_spi_xfer (blockSize, scratch, NULL, cfg, TRUE);
    if (ret != 0)
        return (ret);

    waitWip (cfg);

    return (0);

}
    

/*
 *  @brief Read a block of data
 */
int readBlock (spiConfig_t *cfg, unsigned int address, uint8 *data, int blockSize)
{
    uint8  cmdBuf[4];
    UINT16 n;
    int    ret;
    
    /* Send the read command */
    cmdBuf[0] = SPI_COMMAND_READ;

    if (cfg->addrWidth == 24)  {
        cmdBuf[1] = (address >> 16) & 0xff;
        cmdBuf[2] = (address >>  8) & 0xff;
        cmdBuf[3] = (address >>  0) & 0xff;
        n = 4;

    }  else  {
        cmdBuf[1] = (address >> 8) & 0xff;
        cmdBuf[2] = (address >> 0) & 0xff;
        n = 3;
    }
     


    /* Send the command and address */
    hwSpiEnableXfer (cfg->port);
    ret = hw_spi_xfer (n, cmdBuf, NULL, cfg, FALSE);
    if (ret != 0)
        return (ret);

    /* Send the data */
    ret = hw_spi_xfer (blockSize, NULL, data, cfg, TRUE);
    if (ret != 0)
        return (ret);

    hwSpiDisableXfer (cfg->port);

    return (0);

}



