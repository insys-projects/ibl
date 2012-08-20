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
#include "uart.h"

#define NAND_DATA_OFFSET    0x0     /* Data register offset */
#define NAND_ALE_OFFSET     0x2000  /* Address latch enable register offset */
#define NAND_CMD_OFFSET     0x4000  /* Command latch enable register offset */

#define NAND_DELAY          50000
#define EMIF16_NAND_PROG_TIMEOUT           (100000)

#define DEVICE_REG8_W(x,y)  *(volatile Uint8 *)(x)=(y)
#define DEVICE_REG8_R(x)    (*(volatile Uint8 *)(x))

#define DEVICE_REG16_W(x,y) *(volatile Uint16 *)(x)=(y)
#define DEVICE_REG16_R(x)   (*(volatile Uint16 *)(x))

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
    DEVICE_REG8_W (memBase + NAND_ALE_OFFSET, addr);
}

void 
nandCmdSet
(   
    Uint32    cmd
)
{
    DEVICE_REG8_W (memBase + NAND_CMD_OFFSET, cmd);
}


/* Read raw ECC code after writing to NAND. */
static void
NandRead4bitECC
(
    Uint32    *code
)
{
    Uint32    mask = 0x03ff03ff;
    if((*(int*)0x80000000) == 0x464C457F)
      uart_write_string("DDR OK",0);

    code[0] = DEVICE_REG32_R (DEVICE_EMIF25_BASE + EMIF25_FLASH_ECC_REG(0)) & mask;
    code[1] = DEVICE_REG32_R (DEVICE_EMIF25_BASE + EMIF25_FLASH_ECC_REG(1)) & mask;
    code[2] = DEVICE_REG32_R (DEVICE_EMIF25_BASE + EMIF25_FLASH_ECC_REG(2)) & mask;
    code[3] = DEVICE_REG32_R (DEVICE_EMIF25_BASE + EMIF25_FLASH_ECC_REG(3)) & mask;
}


/* Correct up to 4 bits in data we just read, using state left in the
* hardware plus the ecc_code computed when it was first written.
*/
static Int32
NandCorrect4bitECC
(
    Uint8     *data,
    Uint8     *ecc_code
)
{
    Int32     i;
    Uint16    ecc10[8];
    Uint16    *ecc16;
    Uint32    syndrome[4];
    Uint32    num_errors, corrected, v;

    /* All bytes 0xff?  It's an erased page; ignore its ECC. */
    for (i = 0; i < 10; i++) {
        if (ecc_code[i] != 0xff)
            goto compare;
    }
    return 0;

compare:
/* Unpack ten bytes into eight 10 bit values.  We know we're
* little-endian, and use type punning for less shifting/masking.
    */
    ecc16 = (Uint16 *)ecc_code;

    ecc10[0] =  (ecc16[0] >>  0) & 0x3ff;
    ecc10[1] = ((ecc16[0] >> 10) & 0x3f) | ((ecc16[1] << 6) & 0x3c0);
    ecc10[2] =  (ecc16[1] >>  4) & 0x3ff;
    ecc10[3] = ((ecc16[1] >> 14) & 0x3)  | ((ecc16[2] << 2) & 0x3fc);
    ecc10[4] =  (ecc16[2] >>  8)         | ((ecc16[3] << 8) & 0x300);
    ecc10[5] =  (ecc16[3] >>  2) & 0x3ff;
    ecc10[6] = ((ecc16[3] >> 12) & 0xf)  | ((ecc16[4] << 4) & 0x3f0);
    ecc10[7] =  (ecc16[4] >>  6) & 0x3ff;

    /* Tell ECC controller about the expected ECC codes. */
    for (i = 7; i >= 0; i--)
    {
        DEVICE_REG32_W (DEVICE_EMIF25_BASE + EMIF25_FLASH_ECC_LOAD_REG, ecc10[i]);
    }

    /* Allow time for syndrome calculation ... then read it.
     * A syndrome of all zeroes 0 means no detected errors.
     */
    v = DEVICE_REG32_R (DEVICE_EMIF25_BASE + EMIF25_FLASH_STATUS_REG);
    NandRead4bitECC(syndrome);
    if (!(syndrome[0] | syndrome[1] | syndrome[2] | syndrome[3]))
        return 0;

   /*
    * Clear any previous address calculation by doing a dummy read of an
    * error address register.
    */
    v = DEVICE_REG32_R (DEVICE_EMIF25_BASE + EMIF25_FLASH_ERR_ADDR_REG(0));

    /* Start address calculation, and wait for it to complete.
    * We _could_ start reading more data while this is working,
    * to speed up the overall page read.
    */
    v = DEVICE_REG32_R (DEVICE_EMIF25_BASE + EMIF25_FLASH_CTL_REG);
    DEVICE_REG32_W (DEVICE_EMIF25_BASE + EMIF25_FLASH_CTL_REG, v | (1<<13));
    for (;;) {
        Uint32    fsr = DEVICE_REG32_R (DEVICE_EMIF25_BASE + EMIF25_FLASH_STATUS_REG);

        switch ((fsr >> 8) & 0x0f) {
        case 0:     /* no error, should not happen */
            v = DEVICE_REG32_R (DEVICE_EMIF25_BASE + EMIF25_FLASH_ERR_VALUE_REG(0));
            return 0;
        case 1:     /* five or more errors detected */
            v = DEVICE_REG32_R (DEVICE_EMIF25_BASE + EMIF25_FLASH_ERR_VALUE_REG(0));
            return -1;
        case 2:     /* error addresses computed */
        case 3:
            num_errors = 1 + ((fsr >> 16) & 0x03);
            goto correct;
        default:    /* still working on it */
            chipDelay32 (NAND_DELAY);
            continue;
        }
    }

correct:
    /* correct each error */
    for (i = 0, corrected = 0; i < num_errors; i++) {
        int error_address, error_value;

        if (i > 1) {
            error_address = DEVICE_REG32_R (DEVICE_EMIF25_BASE + EMIF25_FLASH_ERR_ADDR_REG(1));
            error_value = DEVICE_REG32_R (DEVICE_EMIF25_BASE + EMIF25_FLASH_ERR_VALUE_REG(1));
        } else {
            error_address = DEVICE_REG32_R (DEVICE_EMIF25_BASE + EMIF25_FLASH_ERR_ADDR_REG(0));
            error_value = DEVICE_REG32_R (DEVICE_EMIF25_BASE + EMIF25_FLASH_ERR_VALUE_REG(0));
        }

        if (i & 1) {
            error_address >>= 16;
            error_value >>= 16;
        }
        error_address &= 0x3ff;
        error_address = (512 + 7) - error_address;

        if (error_address < 512) {
            data[error_address] ^= error_value;
            corrected++;
        }
    }

    return corrected;
}

void 
nandReadDataBytes
(
    Uint32      nbytes,
    Uint8*      data
)
{
    Int32   i;
    Uint16  *data16;

    if (hwDevInfo->busWidthBits == 8)  
    {
        for (i = 0; i < nbytes; i++)
            data[i] = DEVICE_REG8_R(memBase);

    }  
    else  
    {
        data16 = (Uint16 *)data;

        for (i = 0; i < (nbytes+1) >> 1; i++)
            data16[i] = DEVICE_REG16_R(memBase);
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

    nandCmdSet(hwDevInfo->resetCommand);
    chipDelay32 (NAND_DELAY);

    return (0);
}



/**
 *  @brief
 *      Read bytes without ecc correction
 */
Int32 nandHwEmifDriverReadBytes (Uint32 block, Uint32 page, Uint32 byte, Uint32 nbytes, Uint8 *data)
{
    Uint32  addr;Uint32 v;
#ifndef NAND_TYPE_LARGE
    Uint32  cmd;
#endif

    addr = ((block & 0x000003ff) << hwDevInfo->blockOffset) | ((page & 0x0000003f) << hwDevInfo->pageOffset) | ((byte & 0xfff) << hwDevInfo->columnOffset);

#ifdef NAND_TYPE_LARGE
    nandCmdSet(hwDevInfo->readCommandPre); // First cycle send pre command

    /* 4 address cycles */
    nandAleSet((addr >>  0) & 0xFF);    /* A0-A7   1st Cycle, column addr       */
    nandAleSet((addr >>  8) & 0x0F);    /* A8-A15  2nd Cycle, page addr & blk   */
    nandAleSet((addr >> 16) & 0xFF);    /* A16-A23 3rd Cycle, block addr        */
    nandAleSet((addr >> 24) & 0xFF);    /* A24-A31 4th Cycle, plane addr        */

    if(hwDevInfo->postCommand)
      nandCmdSet(hwDevInfo->readCommandPost); // Second cycle send post command
#else
    if (byte < hwDevInfo->pageSizeBytes) 
    {
        /* Read page data */
        cmd = hwDevInfo->readCommandPre;
    }
    else
    {
        /* Read spare area data */
        cmd = 0x50;
    }
    nandCmdSet(cmd); // First cycle send 0

    /* 4 address cycles */
    nandAleSet((addr >> 0) & 0xFF);     /* A0-A7   1st Cycle, column addr       */
    nandAleSet((addr >> 9) & 0xFF);     /* A9-A16  2nd Cycle, page addr & blk   */
    nandAleSet((addr >> 17) & 0xFF);    /* A17-A24 3rd Cycle, block addr        */
    nandAleSet((addr >> 25) & 0x1);     /* A25-A26 4th Cycle, plane addr        */
#endif

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
    Uint8   i;
    Int32   addr;
    Uint8  *pSpareArea;
    Uint8  *eccFlash_temp;
    Uint8   eccFlash[ibl_N_ECC_BYTES];
    Uint32  v;
    Uint32  byte = 0;
    Uint8 iIteration;
    
#ifdef NAND_TYPE_LARGE
    /* Read the spare area data */
    pSpareArea = &data[hwDevInfo->pageSizeBytes];
    nandHwEmifDriverReadBytes(block, page, hwDevInfo->pageSizeBytes, hwDevInfo->pageEccBytes, pSpareArea);

    for(iIteration = 0;iIteration < (hwDevInfo->pageSizeBytes/512);iIteration++) {
    
    addr = ((block & 0x000003ff) << hwDevInfo->blockOffset) | ((page & 0x0000003f) << hwDevInfo->pageOffset) | ((byte & 0xfff) << hwDevInfo->columnOffset);
    
    /* Send the read command */
    nandCmdSet(hwDevInfo->readCommandPre);

    /* 4 address cycles */
    nandAleSet((addr >>  0) & 0xFF);    /* A0-A7   1st Cycle, column addr       */
    nandAleSet((addr >>  8) & 0x0F);    /* A8-A15  2nd Cycle, page addr & blk   */
    nandAleSet((addr >> 16) & 0xFF);    /* A16-A23 3rd Cycle, block addr        */
    nandAleSet((addr >> 24) & 0xFF);    /* A24-A31 4th Cycle, plane addr        */

    if(hwDevInfo->postCommand)
          nandCmdSet(hwDevInfo->readCommandPost); // Second cycle send post command
    
    chipDelay32 (NAND_DELAY);
    /* Start 4-bit ECC calculation */
    v = DEVICE_REG32_R (DEVICE_EMIF25_BASE + EMIF25_FLASH_CTL_REG);
    DEVICE_REG32_W (DEVICE_EMIF25_BASE + EMIF25_FLASH_CTL_REG, v | (1<<12));

    /* Read the page data */
    nandReadDataBytes(512, data);
    chipDelay32 (NAND_DELAY);

   /* After a read, terminate ECC calculation by a dummy read
    * of some 4-bit ECC register.
    */
    v = DEVICE_REG32_R (DEVICE_EMIF25_BASE + EMIF25_FLASH_ECC_REG(0));

    /* Get the ECC bytes from the spare area data */
    for (i = 0; i < 10; i++)
    {
    	hwDevInfo->eccBytesIdx[i] = (iIteration*10) + 7 + i;

        eccFlash[i] = pSpareArea[hwDevInfo->eccBytesIdx[i]];
    }
      if (NandCorrect4bitECC(data, eccFlash) < 0) {
    
        return (NAND_ECC_FAILURE);
    }
	data += 512;
	byte += 512;

	chipDelay32 (NAND_DELAY);
   }

#else
    addr = (block << hwDevInfo->blockOffset) | (page << hwDevInfo->pageOffset);
    /* Send the read command */
    nandCmdSet(hwDevInfo->readCommandPre);
     
    /* 4 address cycles */
    nandAleSet((addr >> 0) & 0xFF);     /* A0-A7   1st Cycle, column addr       */
    nandAleSet((addr >> 9) & 0xFF);     /* A9-A16  2nd Cycle, page addr & blk   */
    nandAleSet((addr >> 17) & 0xFF);    /* A17-A24 3rd Cycle, block addr        */
    nandAleSet((addr >> 25) & 0x1);     /* A25-A26 4th Cycle, plane addr        */
    chipDelay32 (NAND_DELAY);
    /* Start 4-bit ECC calculation */
    v = DEVICE_REG32_R (DEVICE_EMIF25_BASE + EMIF25_FLASH_CTL_REG);
    DEVICE_REG32_W (DEVICE_EMIF25_BASE + EMIF25_FLASH_CTL_REG, v | (1<<12));

    /* Read the page data */
    nandReadDataBytes(hwDevInfo->pageSizeBytes, data);

   /* After a read, terminate ECC calculation by a dummy read
    * of some 4-bit ECC register.
    */
    v = DEVICE_REG32_R (DEVICE_EMIF25_BASE + EMIF25_FLASH_ECC_REG(0));

    /* Read the spare area data */
    pSpareArea = &data[hwDevInfo->pageSizeBytes];
    nandReadDataBytes(hwDevInfo->pageEccBytes, pSpareArea);
    
    /* Get the ECC bytes from the spare area data */
    for (i = 0; i < ibl_N_ECC_BYTES; i++)
    {
        eccFlash[i] = pSpareArea[hwDevInfo->eccBytesIdx[i]];
    }
    
    if (NandCorrect4bitECC(data, eccFlash) < 0)
    {
        return (NAND_ECC_FAILURE);
    }
#endif
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
