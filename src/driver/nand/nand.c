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



/************************************************************************************************
 * FILE PURPOSE: The top level NAND driver
 ************************************************************************************************
 * FILE NAME: nand.c
 *
 * DESCRIPTION: Provides the required driver functions which populate the BOOT_MODULE_FXN_TABLE
 *
 * @file nand.c
 *
 * @brief
 *		The top level nand driver
 *
 *************************************************************************************************/
#include "types.h"
#include "ibl.h"
#include "iblloc.h"
#include "device.h"
#include "nand.h" 
#include "nandhwapi.h"
#include <string.h>
#include <stdlib.h>

extern void *iblMalloc (Uint32 size);
extern void iblFree (void *mem);

/**
 *  @brief  The nand master control block which tracks the current nand boot information 
 */
typedef struct nandmcb_s
{
    nandDevInfo_t  devInfo;             /**< Device information */

    Int32   fpos;                       /**< The current logical file position */
    Uint32  currentLogicalBlock;        /**< The logical block number of the page currently stored */
    Uint32  currentPage;                /**< The page number currently stored */

    Uint8  *page;                       /**< The current page */
    Uint16 *logicalToPhysMap;           /**< Maps local block to physical block */
    Uint16 *physToLogicalMap;           /**< Maps a physical block number to a logical block number */

    Uint32  numBadBlocks;               /**< Total number of bad blocks */
    Uint8  *blocks;                     /**< There is one byte per block. A non-zero value indicates
                                             that the block is bad */

    nandCtbl_t *nand_if;                /**< Current low level interface (GPIO, EMIF or SPI) */
    

} nandmcb_t;

nandmcb_t nandmcb;



/**
 *  @b Description
 *  @n
 *
 *  This function sets the current file position, and loads the page corresponding
 *  to that position into the pre-allocated page memory
 */
Int32 nand_seek (Int32 loc, Int32 from)
{
    Int32  desiredPos;
    Uint32 desiredBlock;
    Uint32 desiredPage;

    /* Can't seek from the end of the file, since the end is not known */
    if (from == 0)
        desiredPos = loc;
    else if (from == 1)
        desiredPos = nandmcb.fpos + loc;
    else
        return (-1);

    if (desiredPos < 0)
        return (-1);


    /* Convert the file position (relative to the file start) into a page number */
    desiredPage = desiredPos / nandmcb.devInfo.pageSizeBytes;

    /* Convert the logical page to logical block/page */
    desiredBlock = desiredPage / nandmcb.devInfo.pagesPerBlock;
    desiredPage  = desiredPage % nandmcb.devInfo.pagesPerBlock;

    /* Update the current position */
    nandmcb.fpos = desiredPos;

    /* Nothing to do if the current block/page is already loaded */
    if ((desiredBlock == nandmcb.currentLogicalBlock) && (desiredPage == nandmcb.currentPage))
        return (0);

    /* Otherwise load the desired page */
    if (nandmcb.nand_if->nct_driverReadPage != NULL)  {
        if ((*nandmcb.nand_if->nct_driverReadPage)((Uint32)(nandmcb.logicalToPhysMap[desiredBlock]), desiredPage, nandmcb.page) < 0)
            return (-2);
    }

    /* Update the currently loaded block/page info */
    nandmcb.currentLogicalBlock = desiredBlock;
    nandmcb.currentPage         = desiredPage;

    return (0);

}


/**
 *  @b  Description
 *  @n
 *      Free any memory allocated by the driver
 */
Int32 nand_free_return (Int32 retcode)
{
    if (nandmcb.page != NULL)
        iblFree (nandmcb.page);

    if (nandmcb.logicalToPhysMap != NULL)
        iblFree (nandmcb.logicalToPhysMap);

    if (nandmcb.physToLogicalMap != NULL)
        iblFree (nandmcb.physToLogicalMap);

    if (nandmcb.blocks != NULL)
        iblFree (nandmcb.blocks);

    return (retcode);

}



/**
 *  @b Description
 *  @n
 *
 *  This function initializes the nand control structure and reads the bad block info
 *  from the nand.
 */
Int32 nand_open (void *ptr_driver, void (*asyncComplete)(void *))
{
    iblNand_t *ibln = (iblNand_t *)ptr_driver;
    
    Int32 size;
    Int32 ret;
    Int32 i, j, startBlock;
    Bool  badBlock;

    /* Initialize the control info */
    iblMemset (&nandmcb, 0, sizeof(nandmcb));
    iblMemcpy (&nandmcb.devInfo, &ibln->nandInfo, sizeof(nandDevInfo_t));

    nandmcb.page             = NULL;
    nandmcb.logicalToPhysMap = NULL;
    nandmcb.physToLogicalMap = NULL;
    nandmcb.blocks           = NULL;

    nandmcb.nand_if = deviceGetNandCtbl (ibln->interface);

    if (nandmcb.nand_if != NULL)  {

        ret = (*nandmcb.nand_if->nct_driverInit)(ibln->interface, (void *)&nandmcb.devInfo);
        if (ret < 0)
        {
            nand_free_return (ret);
            return (-1);
        }
    }  
    else  
    {
        return (-1);
    }

    /* allocate memory for the page data and the logical to physical block map */
    size = nandmcb.devInfo.pageSizeBytes + nandmcb.devInfo.pageEccBytes;
    nandmcb.page = iblMalloc (size * sizeof(Uint8));
    if (nandmcb.page == NULL)
    {
        nand_free_return (NAND_MALLOC_PAGE_FAIL);
        return (-1);
    }


    /* Logical to physical map data */
    nandmcb.logicalToPhysMap = iblMalloc (nandmcb.devInfo.totalBlocks * sizeof(Uint16));
    if (nandmcb.logicalToPhysMap == NULL)  
    {
        nand_free_return (NAND_MALLOC_MAP_LTOP_FAIL);
        return (-1);
    }
    

    /* Physical to logical map data */
    nandmcb.physToLogicalMap = iblMalloc (nandmcb.devInfo.totalBlocks * sizeof(Uint16));
    if (nandmcb.physToLogicalMap == NULL)  
    {
        nand_free_return (NAND_MALLOC_MAP_PTOL_FAIL);
        return (-1);
    }

    /* Block info */
    size = nandmcb.devInfo.totalBlocks * sizeof(Uint8);
    nandmcb.blocks = iblMalloc (size);
    if (nandmcb.blocks == NULL)  
    {
        nand_free_return (NAND_MALLOC_BLOCK_INFO_FAIL);
        return (-1);
    }


    /* Bad blocks are identified by reading page 0 and page 1. If the first 
     * byte in these pages is not 0xff then the block is bad */
    if (nandmcb.nand_if == NULL)
        return (-1);

    nandmcb.numBadBlocks = 0;
    startBlock = ibln->bootAddress[iblEndianIdx][iblImageIdx]/(nandmcb.devInfo.pageSizeBytes*nandmcb.devInfo.pagesPerBlock);
    for (i = startBlock; i < nandmcb.devInfo.totalBlocks; i++)  {

        badBlock = FALSE;
        for (j = 0; j < ibl_N_BAD_BLOCK_PAGE; j++)
        {
            if (nandmcb.devInfo.badBlkMarkIdx[j] < nandmcb.devInfo.pageEccBytes)
            {
                ret = (*nandmcb.nand_if->nct_driverReadBytes)(i, 
                    j, 
                    nandmcb.devInfo.pageSizeBytes, 
                    nandmcb.devInfo.pageEccBytes, 
                    nandmcb.page);
                if (ret < 0)
                {
                    nand_free_return (ret);
                    return (-1);
                }
                
                if (nandmcb.page[nandmcb.devInfo.badBlkMarkIdx[j]] != 0xff)
                {
                    badBlock = TRUE;
                    break;
                }
            }
        }
        
        if (badBlock)  {
            nandmcb.blocks[i] = 0xff;
            nandmcb.numBadBlocks += 1;
        } else
            nandmcb.blocks[i] = 0;

    }
    

    /* Construct the logical to physical block array */  
    for (i = startBlock, j = 0; i < nandmcb.devInfo.totalBlocks; i++)  {
        if (nandmcb.blocks[i] != 0xff)
            nandmcb.logicalToPhysMap[j++] = i;
    }

    /* Construct the physical to logical block array */
    for (i = startBlock, j = 0; i < nandmcb.devInfo.totalBlocks; i++)  {
        if (nandmcb.blocks[i] == 0xff)
            nandmcb.physToLogicalMap[i] = 0xff;
        else
            nandmcb.physToLogicalMap[i] = j++;
    }


    /* Seek to the first byte of the file */
    nandmcb.fpos                = -1;      /* Force a read on the first seek */
    nandmcb.currentLogicalBlock = 0xffffffff;
    nandmcb.currentPage         = 0xffffffff;

    nand_seek (0, 0);
            
    return (0);

}


/**
 *  @b  Description
 *  @n
 *      This function performs a reads from the current read point
 *
 */
Int32 nand_read (Uint8 *ptr_buf, Uint32 num_bytes)
{
    Int32  i;
    Int32  pIdx;


    if (nandmcb.nand_if == NULL)
        return (-1);

    /* Convert the global file position to an offset in the currently cached page */
    pIdx = nandmcb.fpos % nandmcb.devInfo.pageSizeBytes;

    for (i = 0; i < num_bytes; i++)  {

        ptr_buf[i]    = nandmcb.page[pIdx++];
        nandmcb.fpos += 1;

        if (pIdx >= nandmcb.devInfo.pageSizeBytes)  {

            pIdx = 0;
            nandmcb.currentPage += 1;

            if (nandmcb.currentPage >= nandmcb.devInfo.pagesPerBlock)  {
                nandmcb.currentPage          = 0;
                nandmcb.currentLogicalBlock += 1;
            }

            
            /* Load the new page */
            if ((*nandmcb.nand_if->nct_driverReadPage)((Uint32)(nandmcb.logicalToPhysMap[nandmcb.currentLogicalBlock]), nandmcb.currentPage, nandmcb.page) < 0)
                return (-2);

        }
    }

  return (0);

}

/**
 *  @b  Description
 *  @n
 *      This function performs a peek from the current read point.
 */
Int32 nand_peek (Uint8 *ptr_buf, Uint32 num_bytes)
{
    Int32  ret;
    Int32  origPos;
    Uint32 origLogicalBlock;
    Uint32 origPage;


    if (nandmcb.nand_if == NULL)
        return (-1);

    origPos          = nandmcb.fpos;
    origLogicalBlock = nandmcb.currentLogicalBlock;
    origPage         = nandmcb.currentPage;

    ret = nand_read (ptr_buf, num_bytes);

    if ( (origLogicalBlock != nandmcb.currentLogicalBlock)  ||
         (origPage         != nandmcb.currentPage)  )   {

            if ((*nandmcb.nand_if->nct_driverReadPage)((Uint32)(nandmcb.logicalToPhysMap[origLogicalBlock]), origPage, nandmcb.page) < 0)
                return (-2);
    }
    
    nandmcb.fpos                = origPos;
    nandmcb.currentLogicalBlock = origLogicalBlock;
    nandmcb.currentPage         = origPage;

    return (ret);

}

/**
 *  @b  Description
 *  @n
 *      This function returns how much data is available for immediate read.
 *      On nand this always returns the page size.
 */
Int32 nand_query (void)
{
    return (nandmcb.devInfo.pageSizeBytes);
}
   

/**
 *  @b  Description
 *  @n
 *      This function closes the nand driver
 */
Int32 nand_close (void)
{
    if (nandmcb.nand_if != NULL)
        (*nandmcb.nand_if->nct_driverClose)();

    nandmcb.nand_if = NULL;

    return (nand_free_return (0));

}



/**
 * @brief  The global nand module function table
 */
BOOT_MODULE_FXN_TABLE nand_boot_module =
{
    nand_open,          /* Open  API */
    nand_close,         /* Close API */
    nand_read,          /* Read  API */
    NULL,               /* Write API */
    nand_peek,          /* Peek  API */
    nand_seek,          /* Seek  API */
    nand_query          /* Query API */

};

