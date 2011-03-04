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
#include "emif25.h"
#include "emif25_loc.h"
#include "ecc.h"
#include "target.h"

#define NAND_DATA_OFFSET    0x0     /* Data register offset */
#define NAND_ALE_OFFSET     0x2000  /* Address latch enable register offset */
#define NAND_CMD_OFFSET     0x4000  /* Command latch enable register offset */

#define NAND_DELAY          50000 

extern void chipDelay32 (uint32 del);
extern uint32 deviceEmif25MemBase (int32 cs);

int32          gCs;        /* The chip select space */
uint32         memBase;    /* Base address in device memory map */
nandDevInfo_t *hwDevInfo;  /* Pointer to the device configuration */

void 
nandAleSet
(   
    Uint32    addr
)
{
    DEVICE_REG32_W (memBase + NAND_ALE_OFFSET, addr);
}

void 
nandCmdSet
(   
    Uint32    cmd
)
{
    DEVICE_REG32_W (memBase + NAND_CMD_OFFSET, cmd);
}

void 
nandReadDataBytes
(
    Uint32      nbytes,
    Uint8*      data
)
{
    Int32   i;

    if (hwDevInfo->busWidthBits == 8)  
    {
        for (i = 0; i < nbytes; i++)
            data[i] = *(volatile Uint8 *)memBase;

    }  else  {

        for (i = 0; i < (nbytes+1) >> 1; i++)
            data[i] = *(volatile Uint16 *)memBase;
    }
}

/**
 *  @brief
 *      Initialize the Nand emif interface 
 */
Int32 nandHwEmifDriverInit (int32 cs, void *vdevInfo)
{
    gCs       = cs;
    hwDevInfo = (nandDevInfo_t *)vdevInfo;
    memBase   = deviceEmif25MemBase (cs);

    return (0);
}

/**
 *  @brief
 *      Read bytes without ecc correction
 */
Int32 nandHwEmifDriverReadBytes (Uint32 block, Uint32 page, Uint32 byte, Uint32 nbytes, Uint8 *data)
{
    Uint32  addr;
    Uint32  cmd;

    addr = (block << hwDevInfo->blockOffset) | (page << hwDevInfo->pageOffset) | ((byte & 0xff) << hwDevInfo->columnOffset);   

    if (byte < 256)
    {
        cmd = hwDevInfo->readCommandPre;
    }
    else if (byte < 512)
    {
        cmd = hwDevInfo->readCommandPre + 1;
    }
    else
    {
        cmd = 0x50;
    }

    nandCmdSet(cmd); // First cycle send 0

    /* 4 address cycles */
    nandAleSet((addr >> 0) & 0xFF);     /* A0-A7   1st Cycle, column addr       */
    nandAleSet((addr >> 9) & 0xFF);     /* A9-A16  2nd Cycle, page addr & blk   */
    nandAleSet((addr >> 17) & 0xFF);    /* A17-A24 3rd Cycle, block addr        */
    nandAleSet((addr >> 25) & 0x1);     /* A25-A26 4th Cycle, plane addr        */

    chipDelay32 (NAND_DELAY);

    nandReadDataBytes(nbytes, data);

    return (0);
}

/**
 *  @brief
 *      Read a complete page of data
 */
Int32 nandHwEmifDriverReadPage (Uint32 block, Uint32 page, Uint8 *data)
{
    Int32   i, j, nSegs;
    Int32   addr;
    Uint8  *blockp, *pSpareArea;
    Uint8   eccCompute[3];
    Uint8   eccFlash[3];

    addr = (block << hwDevInfo->blockOffset) | (page << hwDevInfo->pageOffset);   

    /* Send the read command */
    nandCmdSet(hwDevInfo->readCommandPre); 
     
    /* 4 address cycles */
    nandAleSet((addr >> 0) & 0xFF);     /* A0-A7   1st Cycle, column addr       */
    nandAleSet((addr >> 9) & 0xFF);     /* A9-A16  2nd Cycle, page addr & blk   */
    nandAleSet((addr >> 17) & 0xFF);    /* A17-A24 3rd Cycle, block addr        */
    nandAleSet((addr >> 25) & 0x1);     /* A25-A26 4th Cycle, plane addr        */

    chipDelay32 (NAND_DELAY);

    /* Read the data */
    nandReadDataBytes(hwDevInfo->pageSizeBytes + hwDevInfo->pageEccBytes, data);
    pSpareArea = &data[hwDevInfo->pageSizeBytes];
     
    /* Break the page into segments of 256 bytes, each with its own ECC */
    nSegs = hwDevInfo->pageSizeBytes >> 8;

    for (i = 0; i < nSegs; i++)  {
        
        blockp = &data[i << 8];

        /* Format the ecc values to match what the software is looking for */
        for (j = 0; j < 3; j++)
        {
            eccFlash[j] = pSpareArea[hwDevInfo->eccBytesIdx[i*3+j]];
        }

        eccComputeECC(blockp, eccCompute);
        if (eccCorrectData (blockp, eccFlash, eccCompute))
        {
            return (NAND_ECC_FAILURE);
        }
    }

    return (0);

}
    
        
        
/**
 *  @brief
 *      Close the low level driver
 */
Int32 nandHwEmifDriverClose (void)
{
    /* Simply read the ECC to clear the ECC calculation */
    DEVICE_REG32_R (DEVICE_EMIF25_BASE + EMIF25_FLASH_ECC_REG(gCs));

    return (0);

}
        




