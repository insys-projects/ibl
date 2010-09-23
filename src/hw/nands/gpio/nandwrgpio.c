#include "types.h"
#include "ibl.h"
#include "nandhwapi.h"
#include "gpio.h"
#include "target.h"
#include "nandgpioloc.h"

/* Pointer to the device configuration */
extern nandDevInfo_t *hwDevInfo;



Int32 ptNandWriteDataBytes(Uint32 numBytes, Uint8 *data)
{
	Uint32 i;
	
	if(NULL == data)
		return (-1);
		
	hwGpioClearOutput(NAND_ALE_GPIO_PIN);
	hwGpioClearOutput(NAND_CLE_GPIO_PIN);
		
	// 8-bit NAND
	for (i = 0; i < numBytes; i++)
	{
		// NANDRead done directly without checking for nand width
		ptNandWriteDataByte( (Uint8)*data);
	    data++;
	}

	return (0);
}



// Routine to write a page from NAND	
Int32 nandHwDriverWritePage (Uint32 block, Uint32 page, Uint8 *data, nandProgramInfo_t *winfo) 
{
	Uint32 addr;
	
	if (data == NULL)
		return (-1);

    if ( (block >= hwDevInfo->totalBlocks)    ||
         (page  >= hwDevInfo->pagesPerBlock)  )
        return (NAND_INVALID_ADDR);
	

	// Set Data Bus direction as OUT
	hwGpioSetDataBusDirection(GPIO_OUT);
	
	ndelay(TARGET_NAND_STD_DELAY);
	hwGpioClearOutput(NAND_NCE_GPIO_PIN);
	ndelay(TARGET_NAND_STD_DELAY*7);
	ptNandCmdSet(winfo->pageWriteCommandPre);

	ndelay(TARGET_NAND_STD_DELAY);

	// Send address of the block + page to be read
	// Address cycles = 4, Block shift = 22, Page Shift = 16, Bigblock = 0
    addr = (block << hwDevInfo->blockOffset)  +
           (page  << hwDevInfo->pageOffset);

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


    ptNandWriteDataBytes (hwDevInfo->pageSizeBytes + hwDevInfo->pageEccBytes, data);

	ndelay(TARGET_NAND_STD_DELAY*10);

    if (winfo->pageWritePost == TRUE) 
	    ptNandCmdSet(winfo->pageWriteCommandPost);

	ptNandWaitRdy();
	ptNandWaitRdy();
	
	hwGpioSetOutput(NAND_NCE_GPIO_PIN);
	
	
	return (0);
}

Int32 nandHwDriverBlockErase (Uint32 block, nandProgramInfo_t *winfo)
{
	Uint32 addr = 0;
	
	if(block >= hwDevInfo->totalBlocks)
		return (NAND_INVALID_ADDR);

	ndelay(TARGET_NAND_STD_DELAY*10);
	hwGpioClearOutput(NAND_NCE_GPIO_PIN);

	ndelay(TARGET_NAND_STD_DELAY*7);
	ptNandCmdSet(winfo->blockEraseCommandPre); // Block erase command


	ndelay(TARGET_NAND_STD_DELAY);


	// Send address of the block + page to be erased 
    addr = (block << hwDevInfo->blockOffset);

    if (hwDevInfo->lsbFirst == FALSE) 
        addr = swapBytes (addr);


    /* In write mode, truncate the ls bytes for limited address writes */

    if (winfo->blockEraseNaddrBytes >= 4)  {
	    ptNandAleSet(addr & 0xFF);				// BIT0-7  1rst Cycle
	    ndelay(TARGET_NAND_STD_DELAY);
    }


    if (winfo->blockEraseNaddrBytes >= 3)  {
	    ptNandAleSet((addr >> 8u) & 0x0F);   	// Bits8-11 2nd Cycle
	    ndelay(TARGET_NAND_STD_DELAY);
    }

	if (winfo->blockEraseNaddrBytes >= 2)  {
	    ptNandAleSet((addr >> 16u) & 0xFF);   	// Bits16-23
	    ndelay(TARGET_NAND_STD_DELAY);
    }

    if (winfo->blockEraseNaddrBytes >= 1)  {
	    ptNandAleSet((addr >> 24u) & 0xFF);   	// Bits24-31
	    ndelay(TARGET_NAND_STD_DELAY);
    }


    if (winfo->blockEraseCommandPost != FALSE)
	    ptNandCmdSet(winfo->blockEraseCommandPost); // Erase confirm
	
	// Wait for erase operation to finish: 2 msec
	ndelay(900 * 1000); 
	ndelay(900 * 1000);
	
	hwGpioSetOutput(NAND_NCE_GPIO_PIN);	

	return (0);
}
