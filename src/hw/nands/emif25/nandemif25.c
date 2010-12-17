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
#include "nandemif25_loc.h"
#include "ecc.h"
#include "target.h"

int32          gCs;        /* The chip select space */
uint32         memBase;    /* Base address in device memory map */
nandDevInfo_t *hwDevInfo;  /* Pointer to the device configuraiton */

/**
 *  @brief
 *      Initialize the Nand emif interface 
 */
Int32 nandHwDriverInit (int32 cs, nandDevInfo_t *devInfo)
{
    /* Bound check the chip select */
    if ((cs < 2) || (cs > 5))
        return (NAND_INVALID_CS);

    /* Check the bus width */
    if ((hwDevInfo->busWidthBits != 8) && (hwDevInfo->busWidthBits != 16))
        return (NAND_INVALID_ADDR_SIZE);

    gCs       = cs;
    hwDevInfo = devInfo;

    memBase = deviceNandMemBase (cs);

    /* Enable NAND on the specified chip select, all other bits set to 0 */
    DEVICE_REG32_W (DEVICE_EMIF25_BASE + NAND_FLASH_CTL_REG, (1 << (cs - 2)));

    return (0);

}

/**
 *  @brief
 *      Read bytes without ecc correction
 */

Int32 nandHwDriverReadBytes (Uint32 block, Uint32 page, Uint32 byte, Uint32 nbytes, Uint8 *data)
{
    Int32   i;
    Uint32  uAddr;
    Uint8  *v8Addr;
    Uint16 *v16Addr;
    Uint16 *vData;

    /* Form the base address */
    uAddr = memBase + (block << hwDevInfo->blockOffset)  + (page << hwDevInfo->pageOffset) +
                      (byte  << hwDevInfo->columnOffset);

    if (hwDevInfo->busWidthBits == 8)  {

        v8Addr = (Uint8 *)uAddr;
        for (i = 0; i < nbytes; i++)
            data[i] = v8Addr[i];

    }  else  {

        v16Addr = (Uint16 *)uAddr;
        vData   = (Uint16 *)data;

        for (i = 0; i < (nbytes+1) >> 1; i++)
            vData[i] = v16Addr[i];

    }

    return (0);

}

/**
 *  @brief
 *      Convert the 32 bit ecc format used by the emif25 into the 3 byte values used by the software
 *      ecc algorithm
 */
void nand_format_ecc (uint32 v32, Uint8 *v8)
{
    /* An intrinsic is used for devices that support the shfl instruction */
    v32 = TARGET_SHFL(v32);

    v8[0] = (v32 >>  0) & 0xff;
    v8[1] = (v32 >>  8) & 0xff;
    v8[1] = (v32 >> 16) & 0x3f;   /* p2048o and p2048e are unused and must be masked out */

}

/**
 *  @brief
 *      Read a complete page of data
 */
Int32 nandHwDriverReadPage (Uint32 block, Uint32 page, Uint8 *data)
{
    Int32   i;
    Int32   nSegs;
    Uint32  v;
    Uint8  *blockp;
    Uint32  eccv;
    Uint8   eccHw[3];
    Uint8   eccFlash[3];

    /* Break the page into segments of 256 bytes, each with its own ECC */
    nSegs = hwDevInfo->pageSizeBytes >> 8;

    for (i = 0; i < nSegs; i++)  {
        
        blockp = &data[i << 8];


        /* Read the ecc bytes stored in the extra page data */
        nandHwDriverReadBytes (block, page, hwDevInfo->pageSizeBytes + hwDevInfo->pageEccBytes - ((nSegs - i) * 4), 4, (Uint8 *)eccv);


        /* Reset the hardware ECC correction by reading the ECC status register */
        v = DEVICE_REG32_R (DEVICE_EMIF25_BASE + NAND_FLASH_ECC_REG(gCs));

        /* Enable ECC */
        v = DEVICE_REG32_R (DEVICE_EMIF25_BASE + NAND_FLASH_CTL_REG);
        v = v | (1 << (gCs + 8 - 2));
        DEVICE_REG32_W (DEVICE_EMIF25_BASE + NAND_FLASH_CTL_REG, v);

        nandHwDriverReadBytes (block, page, i << 8, 256, data);

        /* Read the ECC value computed by the hardware */
        v = DEVICE_REG32_R (DEVICE_EMIF25_BASE + NAND_FLASH_ECC_REG(gCs));

        /* Format the ecc values to match what the software is looking for */
        nand_format_ecc (eccv, eccFlash);
        nand_format_ecc (v,    eccHw);

        if (eccCorrectData (blockp, eccFlash, eccHw))
            return (NAND_ECC_FAILURE);

    }

    return (0);

}
    
        
        
/**
 *  @brief
 *      Close the low level driver
 */
Int32 nandHwDriverClose (void)
{
    int32 v;

    /* Simply read the ECC to clear the ECC calculation */
    v = DEVICE_REG32_R (DEVICE_EMIF25_BASE + NAND_FLASH_ECC_REG(gCs));

    return (0);

}
        






