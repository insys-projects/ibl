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



/********************************************************************************
 * FILE PURPOSE: Hardware nand driver for 8 bit hwGpio driven nand
 ********************************************************************************
 * FILE NAME: nandhwGpio.c
 *
 * DESCRIPTION: The low level nand driver which accesses the nand through hwGpio
 *
 * @file nandhwGpio.c
 *
 * @brief
 *	  The low level nand driver which accesses the nand through hwGpio
 *
 ********************************************************************************/
#include "types.h"
#include "ibl.h"
#include "iblcfg.h"
#include "nandhwapi.h"
#include "gpio.h"
#include "ecc.h"
#include "target.h"
#include "nandgpioloc.h"

/* Pointer to the device configuration */
nandDevInfo_t *hwDevInfo;


/**
 * @brief
 *   Delay for a number of cycles (approximate)
 */

void ndelay(Uint32 uiDelay)
{
    volatile Uint32 i;

    for (i = 0; i < uiDelay>>3; i++);
}



/**
 * @brief
 *   a 100us delay
 */
void ptNandWaitRdy(void) 
{
	ndelay(100 * 1000); // 100 usec
}


/**
 * @brief
 *   Write to the flash (command for the boot loader) 
 */
void ptNandWriteDataByte(Uint8 data)
{
	// Data is written to the data register on the rising edge of WE# when
	//	• CE#, CLE, and ALE are LOW, and
	//	• the device is not busy.
	hwGpioClearOutput(NAND_NWE_GPIO_PIN);
	ndelay(TARGET_NAND_STD_DELAY);
	hwGpioClearDataBus(GPIO_DATAMASK);
	hwGpioWriteDataBus(data);
	ndelay(TARGET_NAND_STD_DELAY);
	hwGpioSetOutput(NAND_NWE_GPIO_PIN);   // At posedge clock, make WE# = 1.
	ndelay(TARGET_NAND_STD_DELAY);
}


/**
 *  @brief
 *      Set the address latch
 */
void ptNandAleSet(Uint32 addr)
{
	// ALE HIGH indicates address cycle occurring
	hwGpioSetOutput(NAND_ALE_GPIO_PIN);
	ptNandWriteDataByte(addr);
	hwGpioClearOutput(NAND_ALE_GPIO_PIN);
}

/**
 *  @brief
 *  Write a command to the nand flash
 */
void ptNandCmdSet(Uint32 cmd)
{
	// Commands are written to the command register on the rising edge of WE# when
	//	• CE# and ALE are LOW, and
	//	• CLE is HIGH, and : CLE HIGH indicates command cycle occurring
	//	• the device is not busy
	hwGpioSetOutput(NAND_CLE_GPIO_PIN);
	ptNandWriteDataByte(cmd);
	hwGpioClearOutput(NAND_CLE_GPIO_PIN);
}


/**
 *  @brief
 *  Setup hwGpio to drive the NAND interface
 */
void ptNandConfig (void) 
{	
	// Set direction of control signals as OUT
	hwGpioSetDirection(NAND_CLE_GPIO_PIN, GPIO_OUT);
	hwGpioSetDirection(NAND_NCE_GPIO_PIN, GPIO_OUT);
	hwGpioSetDirection(NAND_NWE_GPIO_PIN, GPIO_OUT);
	hwGpioSetDirection(NAND_ALE_GPIO_PIN, GPIO_OUT);
	hwGpioSetDirection(NAND_NRE_GPIO_PIN, GPIO_OUT);
	
	// Set Data Bus direction as OUT
   	hwGpioSetDataBusDirection(GPIO_OUT);
	
	hwGpioSetOutput(NAND_NCE_GPIO_PIN);   	// Chip Enable = 1
    hwGpioClearOutput(NAND_CLE_GPIO_PIN);  	// Command Latch enable = 0
    hwGpioClearOutput(NAND_ALE_GPIO_PIN);		// Address latch Enable = 0
	hwGpioSetOutput(NAND_NWE_GPIO_PIN);		// Write Enable = 1
	hwGpioSetOutput(NAND_NRE_GPIO_PIN);		// Read Enable = 1
}

/**
 *  @brief Initialize the driver
 *
 */
Int32 nandHwDriverInit (nandDevInfo_t *devInfo)
{
	Uint32 cmd;

    hwDevInfo = devInfo;

    if (devInfo->addressBytes > 4)
        return (NAND_INVALID_ADDR_SIZE);

	// Initialize NAND interface
	ptNandConfig();
	ndelay(TARGET_NAND_STD_DELAY*10);	
	
	cmd = hwDevInfo->resetCommand;
	// Send reset command to NAND
	hwGpioClearOutput(NAND_NCE_GPIO_PIN);   	// Chip EN = 0
	ptNandCmdSet(cmd);
	hwGpioSetOutput(NAND_NCE_GPIO_PIN);
		
	ptNandWaitRdy();

    return (0);
}	 

/**
 *  @brief
 *      Read a single data byte
 */
void ptNandReadDataByte(Uint8* puchValue)
{
	// Set Data Bus direction as IN
   	hwGpioSetDataBusDirection(GPIO_IN);
   	
	hwGpioClearOutput(NAND_NRE_GPIO_PIN);
	ndelay(TARGET_NAND_STD_DELAY);
	
	*puchValue = hwGpioReadDataBus();
	hwGpioSetOutput(NAND_NRE_GPIO_PIN);
	ndelay(TARGET_NAND_STD_DELAY);
	
	// Set Data Bus direction as OUT
   	hwGpioSetDataBusDirection(GPIO_OUT);
   	ndelay(TARGET_NAND_STD_DELAY);
}

/**
 *  @brief
 *      Read multiple bytes
 */
Uint32 ptNandReadDataBytes(Uint32 numBytes, Uint8 *destAddr)
{
	Uint32 i;
	
	// 8-bit NAND
    for (i = 0; i < numBytes; i++)
    {
		// NANDRead done directly without checking for nand width
		ptNandReadDataByte((Uint8 *)destAddr);
    	destAddr++;
    }
	return SUCCESS;
}

/**
 *  @brief
 *      Swap the bytes in a 4 byte field
 */
Uint32 swapBytes (Uint32 v)
{
    Uint32 w;

    w = (((v >> 24) & 0xff) <<  0)  |
        (((v >> 16) & 0xff) <<  8)  |
        (((v >>  8) & 0xff) << 16)  |
        (((v >>  0) & 0xff) << 24)  ;

    return (w);

}


/**
 *  @brief
 *     Read a complete page including the extra page bytes.
 */  

Int32 nandHwDriverReadBytes (Uint32 block, Uint32 page, Uint32 byte, Uint32 nbytes, Uint8 *data)
{

	Uint32 addr;
	
	if (data == NULL)
		return (NAND_NULL_ARG);

    if ( (block >= hwDevInfo->totalBlocks)    ||
         (page  >= hwDevInfo->pagesPerBlock)  )
        return (NAND_INVALID_ADDR);

	ndelay(TARGET_NAND_STD_DELAY*10);
		
	hwGpioClearOutput(NAND_NCE_GPIO_PIN);
	ndelay(TARGET_NAND_STD_DELAY*5);
	
	ptNandCmdSet(hwDevInfo->readCommandPre); // First cycle send 0
	
	// Send address of the block + page to be read
	// Address cycles = 4, Block shift = 22, Page Shift = 16, Bigblock = 0
    addr = (block << hwDevInfo->blockOffset)  +
           (page  << hwDevInfo->pageOffset)   +
           (byte  << hwDevInfo->columnOffset);

    if (hwDevInfo->lsbFirst == FALSE) 
        addr = swapBytes (addr);

	ptNandAleSet(addr & 0xFF);				// BIT0-7  1rst Cycle
	ndelay(TARGET_NAND_STD_DELAY);

    if (hwDevInfo->addressBytes >= 2)  {
	    ptNandAleSet((addr >> 8u) & 0x0F);   	// Bits8-11 2nd Cycle
	    ndelay(TARGET_NAND_STD_DELAY);
    }

	if (hwDevInfo->addressBytes >= 3)  {
	    ptNandAleSet((addr >> 16u) & 0xFF);   	// Bits16-23
	    ndelay(TARGET_NAND_STD_DELAY);
    }

    if (hwDevInfo->addressBytes >= 4)  {
	    ptNandAleSet((addr >> 24u) & 0xFF);   	// Bits24-31
	    ndelay(TARGET_NAND_STD_DELAY);
    }

    if (hwDevInfo->postCommand == TRUE)
		ptNandCmdSet(hwDevInfo->readCommandPost);

	// Wait for Ready Busy Pin to go HIGH  
	ptNandWaitRdy();

    
	ptNandReadDataBytes(nbytes, data);

	
	// Set Chip enable
	hwGpioSetOutput(NAND_NCE_GPIO_PIN);	
	ndelay(TARGET_NAND_STD_DELAY*5);

	return (0);
}




/**
 *  @brief
 *     Read a complete page including the extra page bytes
 */  
Int32 nandHwDriverReadPage(Uint32 block, Uint32 page, Uint8 *data)
{
    Int32  ret;
    Int32  i;
    Int32  nblocks;
    Uint8 *blockp;
    Uint8 *eccp;
    Uint8  eccCalc[3];

    /* Read the page, including the extra bytes */
    ret = nandHwDriverReadBytes (block, page, 0, hwDevInfo->pageSizeBytes + hwDevInfo->pageEccBytes, data);
    if (ret < 0)
        return (ret);

    /* Perform ECC on 256 byte blocks. Three bytes of ecc per 256 byte block are used. The last
     * 3 bytes are used for the last block, the previous three for the block before that, etc */
    nblocks = hwDevInfo->pageSizeBytes >> 8;

    for (i = 0; i < nblocks; i++)  {

        blockp = &data[i * 256];
        eccp   = &data[hwDevInfo->pageSizeBytes + hwDevInfo->pageEccBytes - ((nblocks - i) * 3)];

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
int32 nandHwDriverClose (void)
{
    return (0);

}

