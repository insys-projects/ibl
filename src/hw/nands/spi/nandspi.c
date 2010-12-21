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

#include "types.h"
#include "ibl.h"
#include "iblcfg.h"
#include "nandhwapi.h"
#include "ecc.h"
#include "target.h"
#include "spi_api.h"

nandDevInfo_t *hwSpiDevInfo;  /* Pointer to the NAND configuration */

/**
 *  @brief
 *      Initialize the NAND SPI configuration. The SPI interface
 *      must be initialized seperately at a higher level 
 */
Int32 nandHwSpiDriverInit (int32 cs, nandDevInfo_t *devInfo)
{

    hwSpiDevInfo = devInfo;

    return (0);

}


/**
 *  @brief
 *      Read bytes without ECC correction
 */
Int32 nandHwSpiDriverReadBytes (Uint32 block, Uint32 page, Uint32 byte, Uint32 nbytes, uint8 *data)
{
    Uint32 uAddr;

    uAddr = (block << hwSpiDevInfo->blockOffset) + (page << hwSpiDevInfo->pageOffset) + 
            (byte  << hwSpiDevInfo->columnOffset);

    
    if (hwSpiRead (uAddr, nbytes, data) != 0)
        return (-1);

    return (0);

}


/**
 *  @brief
 *      Read a complete page of data
 */
Int32 nandHwSpiDriverReadPage (Uint32 block, Uint32 page, Uint8 *data)
{
    Int32   i;
    Int32   nSegs;
    Uint8  *blockp;
    Uint8  *eccp;
    Uint8   eccCalc[3];


    /* Read the entire page, including the extra bytes. The array data
     * has been sized to handle the full page */
    if (nandHwSpiDriverReadBytes (block, page, 0, hwSpiDevInfo->pageSizeBytes + hwSpiDevInfo->pageEccBytes, data))
        return (NAND_READ_FAILURE);



    /* Break the page into segments of 256 bytes for ECC correction */
    nSegs = hwSpiDevInfo->pageSizeBytes >> 8;

    for (i = 0; i < nSegs; i++)  {

        blockp = &data[i << 8];
        eccp   = &data[hwSpiDevInfo->pageSizeBytes + hwSpiDevInfo->pageEccBytes - ((nSegs - i) * 3)];
        eccComputeECC (blockp, eccCalc);

        if (eccCorrectData (blockp, eccp, eccCalc) != ECC_SUCCESS)
            return (NAND_ECC_FAILURE);

    }

    return (0);

}

/**
 *  @brief
 *      Close the driver
 */
int32 nandHwSpiDriverClose (void)
{
    return (0);

}





