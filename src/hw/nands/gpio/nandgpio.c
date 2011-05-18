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

#define NAND_BYTES_PER_PAGE		512
#define ECC_BLOCK_SIZE			256
#define NAND_SPARE_BYTES_PER_PAGE	16
#define ECC_SPARE_OFFSET  (NAND_SPARE_BYTES_PER_PAGE - 3 * (NAND_BYTES_PER_PAGE / ECC_BLOCK_SIZE))

/* NAND address pack macro */
#define PACK_ADDR(col, page, block) \
        ((col & 0x000000ff) | ((page & 0x0000001f) << 9) | ((block & 0x00000fff) << 14))

/* Pointer to the device configuration */
extern volatile cregister Uint32 TSCL;
nandDevInfo_t *hwDevInfo;

/**
 * @brief
 *   Delay for a number of cycles (approximate)
 */

void ndelay(Uint32 uiDelay)
{
	Uint32 t;
	TSCL = 1;

	t = TSCL;
	while(TSCL < (t + uiDelay));
}

Uint32 ptNandWaitRdy(Uint32 in_timeout) 
{
    Uint32 t;

    ndelay(NAND_WAIT_PIN_POLL_ST_DLY);

    TSCL = 1; 
    t = TSCL;
     
    while(!hwGpioReadInput(NAND_BSY_GPIO_PIN))
    {
        if( TSCL > (t + in_timeout) )
        {
            return 0; /* fail */
        }
    }
    return 1; /* success */
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
Int32 nandHwGpioDriverInit (int32 cs, void *vdevInfo)
{
	Uint32 cmd;
	Uint32 ret;
    nandDevInfo_t *devInfo = (nandDevInfo_t *)vdevInfo;

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
		
	ret = ptNandWaitRdy(100000);
	if (ret != 1)
		return -1;

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

Int32 nandHwGpioDriverReadBytes (Uint32 block, Uint32 page, Uint32 byte, Uint32 nbytes, Uint8 *data)
{

	Uint32 addr;
	Uint32 cmd;
	Uint32 ret;
	if (data == NULL)
		return (NAND_NULL_ARG);

    if ( (block >= hwDevInfo->totalBlocks)    ||
         (page  >= hwDevInfo->pagesPerBlock)  )
        return (NAND_INVALID_ADDR);

	ndelay(TARGET_NAND_STD_DELAY*10);
		
	hwGpioClearOutput(NAND_NCE_GPIO_PIN);
	ndelay(TARGET_NAND_STD_DELAY*5);

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

	ptNandCmdSet(cmd); // First cycle send 0

	/* 
	 * Send address of the block + page to be read
	 * Address cycles = 4, Block shift = 14,
	 * Page Shift = 9, Bigblock = 0
	 */
	addr = PACK_ADDR(0x0, page, block);
//	addr = (block << hwDevInfo->blockOffset) | (page <<
//	hwDevInfo->pageOffset) | ((byte & 0xff)// << hwDevInfo->columnOffset); 
	
	ptNandAleSet((addr >>  0u) & 0xFF);   /* A0-A7  1st Cycle;  column addr */
	ndelay(TARGET_NAND_STD_DELAY);
	ptNandAleSet((addr >>  9u) & 0xFF);   /* A9-A16 2nd Cycle;  page addr & blk */
	ndelay(TARGET_NAND_STD_DELAY);
	ptNandAleSet((addr >> 17u) & 0xFF);   /* A17-A24 3rd Cycle; Block addr */
	ndelay(TARGET_NAND_STD_DELAY);
	ptNandAleSet((addr >> 25u) & 0x1);    /* A25    4th Cycle;  Plane addr */
	ndelay(TARGET_NAND_STD_DELAY);

    
	// Wait for Ready Busy Pin to go HIGH  
	ret = ptNandWaitRdy(100000);
#if 0
	if (ret != 1)
		return -1;
#endif
    
	ptNandReadDataBytes(nbytes, data);
//	ptNandReadDataBytes(512, data);
	
	// Set Chip enable
	hwGpioSetOutput(NAND_NCE_GPIO_PIN);	
	ndelay(TARGET_NAND_STD_DELAY*5);

	return (0);
}




/**
 *  @brief
 *     Read a complete page including the extra page bytes
 */  
Int32 nandHwGpioDriverReadPage(Uint32 block, Uint32 page, Uint8 *data)
{
    Int32  ret;
    Int32  i;
    Int32  nblocks;
    Uint8 *blockp;
    Uint8 *eccp;
    Uint8  eccCalc[3];
    int32 iErrors = ECC_SUCCESS;
    Uint8 *SpareAreaBuf = NULL;

    SpareAreaBuf = data + NAND_BYTES_PER_PAGE;
    
    /* Read the page, including the extra bytes */
    ret = nandHwGpioDriverReadBytes (block, page, 0, hwDevInfo->pageSizeBytes + hwDevInfo->pageEccBytes, data);
    if (ret < 0)
        return (ret);

    /* Perform ECC on 256 byte blocks. Three bytes of ecc per 256 byte block are used. The last
     * 3 bytes are used for the last block, the previous three for the block before that, etc */
#if 0
	for(i = 0; i < NAND_BYTES_PER_PAGE / ECC_BLOCK_SIZE; i++)
	{ 
	/* Correct ecc error for each 256 byte blocks */
	eccComputeECC(data + i * ECC_BLOCK_SIZE, eccCalc);
	iErrors = eccCorrectData(data + (i * ECC_BLOCK_SIZE), 
                 (SpareAreaBuf + ECC_SPARE_OFFSET + i * 3), eccCalc);
 
//	if(iErrors != ECC_SUCCESS)
//		return (NAND_ECC_FAILURE);
	}
#endif
    return (0);

}



/**
 *  @brief
 *      Close the driver
 */
int32 nandHwGpioDriverClose (void)
{
    return (0);

}

