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



/**************************************************************************************
 * FILE PURPOSE: NAND writer for the IBL
 **************************************************************************************
 * FILE NAME: nandwriter.c
 *
 * DESCRIPTION: A simple nand writer used to program the flash with an image
 *				that the ibl can read.
 *
 * @file nandwriter.c
 *
 * @brief
 *	  A simple nand writer
 *
 ***************************************************************************************/
#include "types.h"
#include "ibl.h"
#include "nandhwapi.h"
#include "ecc.h"
#include "pllapi.h"
#include "device_nb.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


/**
 *  @brief
 *      The input file name is hard coded
 */
char *fileName = "c:\\flash.bin";


/**
 *  @brief
 *      NAND device configuration. This structure should be filled in
 *      by the user before running 
 */
typedef struct nandWriterInfo_s
{
    uint32  busWidthBits;       /**< 8 or 16 bit bus width */
    uint32  pageSizeBytes;      /**< The size of each page */
    uint32  pageEccBytes;       /**< Number of ecc bytes in each page */
    uint32  pagesPerBlock;      /**< The number of pages in each block */
    uint32  totalBlocks;        /**< The total number of blocks in a device */
    
    uint32  addressBytes;       /**< Number of bytes in the address */
    bool    lsbFirst;           /**< Set to true if the LSB is output first, otherwise msb is first */
    uint32  blockOffset;        /**< Address bits which specify the block number */
    uint32  pageOffset;         /**< Address bits which specify the page number */
    uint32  columnOffset;       /**< Address bits which specify the column number */
    
    uint8   resetCommand;       /**< The command to reset the flash */
    uint8   readCommandPre;     /**< The read command sent before the address */
    uint8   readCommandPost;    /**< The read command sent after the address */
    bool    postCommand;        /**< If TRUE the post command is sent */

} nandWriterInfo_t;

nandWriterInfo_t nandWriterInfo;


/**
 *  @brief
 *      Information used only for programming the flash 
 */
nandProgramInfo_t nandProgramInfo;

/**
 *  @brief
 *    System setup information
 */
typedef struct sysSetup_s
{
    Int32 pllPrediv;
    Int32 pllMult;
    Int32 pllPostdiv;

}  sysSetup_t;

sysSetup_t sysSetup;



/**
 * @brief
 *    Checks for the bad block mark. Returns TRUE if a block is marked bad
 */
BOOL checkBadBlockMark (Int32 block)
{
  uint8 data[4];
  Int32 ret;

  /* A block is marked bad if the first ECC byte on page 1 and page 0 of the block
   * is not 0xff. These bytes will not be used for ECC checks */
  ret = nandHwDriverReadBytes (block, 0, nandWriterInfo.pageSizeBytes, 1, &data[0]);
  if (ret < 0)  {
    printf ("Fatal error in nandHwDriverReadBytes, exiting\n");
    exit (-1);
  }

  ret = nandHwDriverReadBytes (block, 1, nandWriterInfo.pageSizeBytes, 1, &data[1]);
  if (ret < 0)  {
    printf ("Fatal error in nandHwDriverReadBytes, exiting\n");
    exit (-1);
  }

  /* Return TRUE if the block has been marked bad */
  if ((data[0] != 0xff) || (data[1] != 0xff))  {
     printf ("Block %d already marked bad, skipping\n", block);
     return (TRUE);
  }


  /* Return FALSE if the block has not been marked bad */
  return (FALSE);

}

/**
 *  @brief
 *      Mark a block as bad. Byte 0 for pages 0 and 1 of the ecc data (extra bytes) are 
 *      written as non-zero. Both pages are programmed
 */
void markBlockBad (Int32 block, Uint8 *pageData)
{

    /* Set all data bytes to 0xff, only zero the bad block marks */
    memset (pageData, 0xff, ((nandWriterInfo.pageSizeBytes + nandWriterInfo.pageEccBytes) * sizeof (Uint8)));
    pageData[nandWriterInfo.pageSizeBytes] = 0;

    /* Write the data to pages 0 and 1 */
    nandHwDriverWritePage (block, 0, pageData, &nandProgramInfo);
    nandHwDriverWritePage (block, 1, pageData, &nandProgramInfo);
}


int flashNand (FILE *fp, Uint32 flen, Uint8 *data)
{
    Uint32 fpos;
    Uint32 rlen;
    Int32  block;
    Int32  page;
    Int32  ret;
    Int32  i, j;

    Int32  eccsPerPage;     /* Number of 256 bytes ecc sections in a page */

    Uint8  *eccBytes;       /* Points to the generated ECC data */

    /* Reset to the start of the data file */
    fseek (fp, 0l, SEEK_SET);

    eccsPerPage = nandWriterInfo.pageSizeBytes / eccBytesPerBlock ();


    /* Program the NAND */
    for (block = -1, page = nandWriterInfo.pagesPerBlock, fpos = 0; fpos < flen; fpos += nandWriterInfo.pageSizeBytes)  {

        /* Advance over a block boundary. Verify block limit and bad block mark */
        if (++page >= nandWriterInfo.pagesPerBlock)  {

            do  {

                block += 1;

            } while ((block < nandWriterInfo.totalBlocks) && (checkBadBlockMark (block) == TRUE));

            nandHwDriverBlockErase (block, &nandProgramInfo);

            page = 0;
        }

        if (block >= nandWriterInfo.totalBlocks)  {
            printf ("Flash failed: End of device reached\n");
            free (data);
            fclose (fp);
            return (-1);
        }

        /* Initialize the entire page (with ecc bytes) to 0xff. This will preserve the good block
         * mark at the beginning of pages 0 and 1 */
        memset (data, 0xff, ((nandWriterInfo.pageSizeBytes + nandWriterInfo.pageEccBytes) * sizeof (Uint8)));


        /* Read the data from the file */
        rlen = nandWriterInfo.pageSizeBytes;
        if (flen - fpos < nandWriterInfo.pageSizeBytes)  {
            rlen = flen - fpos;
        }

        if (fread (data, sizeof (Uint8), rlen, fp) == 0)  {
            printf ("fread unexpectedly returned 0 on read of %d bytes staring at byte location %d.\n", rlen, fpos);
            printf ("  Total file length = %d bytes\n", flen);
            free (data);
            fclose (fp);
            return (-1);
        }

        /* The page is broken into 256 byte blocks, each has its own ecc data */
        for (i = 0; i < eccsPerPage; i++)   {

            /* The eccs values are at the end of the page. The very last bytes correspond
             * to the very last block */
            j = (eccsPerPage - i) * eccNumBytes ();

            eccBytes = &data[nandWriterInfo.pageSizeBytes + nandWriterInfo.pageEccBytes - j];
            eccComputeECC (&data[i * eccBytesPerBlock()], eccBytes);

        }

    
        printf ("Flashing block %d, page %d (%d bytes of %d)\n", block, page, fpos, flen);

        ret = nandHwDriverWritePage (block, page, data, &nandProgramInfo);
        if (ret < 0)  {
            printf ("nandHwWritePage returned error code %d writing block %d, page %d\n", ret, block, page);
            free (data);
            fclose (fp);
            return (-1);
        }

    }


    return (0);

}    


/**
 *  @brief
 *      Read back the data file that was just flashed. On errors mark the block as bad.
 *  Returns TRUE if the image verified correctly.
 *          FALSE if the image verification failed
 */

BOOL flashVerify (FILE *fp, Uint32 flen, Uint8 *data)
{
    Uint8 *nandData;
    Int32  block;
    Int32  page;
    Int32  fpos;
    Int32  ret;
    Int32  rlen;
    Int32  i;
 

    nandData = malloc ((nandWriterInfo.pageSizeBytes + nandWriterInfo.pageEccBytes) * sizeof (Uint8));
    if (nandData == NULL)  {
        printf ("Malloc failed on verification data block of %d bytes\n", nandWriterInfo.pageSizeBytes + nandWriterInfo.pageEccBytes);
        return (TRUE);  /* return true so the program gives up */
    }

    
    /* Reset to the start of the data file */
    fseek (fp, 0l, SEEK_SET);


    for (block = -1, page = nandWriterInfo.pagesPerBlock, fpos = 0; fpos < flen; fpos += nandWriterInfo.pageSizeBytes)  {

        /* Advance over a block boundary. Verify block limit and bad block mark */
        if (++page >= nandWriterInfo.pagesPerBlock)  {

            do  {

                block += 1;

            } while ((block < nandWriterInfo.totalBlocks) && (checkBadBlockMark (block) == TRUE));

            page = 0;
        }

        if (block >= nandWriterInfo.totalBlocks)  {
            printf ("Verify failed: End of flash device reached\n");
            free (nandData);
            return (TRUE);   /* Return TRUE to allow the program to exit */
        }


        /* Read a page of data */
        ret = nandHwDriverReadPage (block, page, nandData);
        if (ret < 0)  {
            printf ("nandHwDriverReadPage returned error code %d\n", ret);
            free (nandData);
            return (TRUE);      /* return TRUE to allow the program to exit */
        }

        /* Read the data from the file */
        rlen = nandWriterInfo.pageSizeBytes;
        if (flen - fpos < nandWriterInfo.pageSizeBytes)  {
            rlen = flen - fpos;
        }
    
        if (fread (data, sizeof (Uint8), rlen, fp) == 0)  {
            printf ("fread unexpectedly returned 0 on read of %d bytes staring at byte location %d.\n", rlen, fpos);
            printf ("  Total file length = %d bytes\n", flen);
            free (nandData);
            return (TRUE);
        }

        printf ("Verifying block %d, page %d (%d bytes of %d)\n", block, page, fpos, flen);

        for (i = 0; i < rlen; i++)  {
            if (data[i] != nandData[i])   {
                printf ("Failure in block %d, page %d, at byte %d, (at byte %d in the data file) expected 0x%08x, read 0x%08x\n", block, page, i, fpos, data[i], nandData[i]);
                printf ("marking block %d as bad, re-flash attempted\n");
                markBlockBad (block, nandData);
                free (nandData);
                return (FALSE);
            }
        }

    }

    free (nandData);
    return (TRUE);

}


/**
 *  @brief
 *      The nandDevInfo structure is a subset of the nandWriteInfo, and is seperated
 *      here to use the existing ibl functions for the writer
 */
nandDevInfo_t devInfo;


int main (void)
{
    Uint8  *data;
    FILE   *fp;
    Uint32  flen;
    Int32   ret;
    Int32   rCount;


    /* Configure the main pll */
    hwPllSetPll (0, (uint16)sysSetup.pllPrediv, (uint16)sysSetup.pllMult, (uint16)sysSetup.pllPostdiv);


    /* Initialize the nand driver */
    devInfo.busWidthBits  = nandWriterInfo.busWidthBits;
    devInfo.pageSizeBytes = nandWriterInfo.pageSizeBytes;
    devInfo.pageEccBytes  = nandWriterInfo.pageEccBytes;
    devInfo.pagesPerBlock = nandWriterInfo.pagesPerBlock;
    devInfo.totalBlocks   = nandWriterInfo.totalBlocks;
    
    devInfo.addressBytes = nandWriterInfo.addressBytes;
    devInfo.lsbFirst     = nandWriterInfo.lsbFirst;
    devInfo.blockOffset  = nandWriterInfo.blockOffset;
    devInfo.pageOffset   = nandWriterInfo.pageOffset;
    devInfo.columnOffset = nandWriterInfo.columnOffset;
    
    devInfo.resetCommand    = nandWriterInfo.resetCommand;
    devInfo.readCommandPre  = nandWriterInfo.readCommandPre;
    devInfo.readCommandPost = nandWriterInfo.readCommandPost;
    devInfo.postCommand     = nandWriterInfo.postCommand;
    


    /* Open and find the length of the data file */
    fp = fopen (fileName, "rb");
    if (fp == NULL)  {
      printf ("Failed to open file %s\n", fileName);
      return (-1);
    }

    fseek (fp, 0l, SEEK_END);
    flen = ftell (fp);
    fseek (fp, 0l, SEEK_SET);

    data = malloc ((nandWriterInfo.pageSizeBytes + nandWriterInfo.pageEccBytes) * sizeof (Uint8));
    if (data == NULL)  {
      printf ("Malloc failed on size of %d bytes\n", nandWriterInfo.pageSizeBytes + nandWriterInfo.pageEccBytes);
      fclose (fp);
      return (-1);
    }


    /* Device specific nand setup */
    deviceConfigureForNand();
    
    /* Initialize the programming interface */
    ret = nandHwDriverInit (&devInfo);
    if (ret != 0)  {
        printf ("nandHwDriverInit failed with error code %d\n", ret);
        free (data);
        fclose (fp);
        return (-1);
    }

    /* Write the flash, verify the results. On read back failure mark
     * the block as bad and try rewriting again */
    rCount = 0;

    do  {

        if (flashNand (fp, flen, data) != 0)  {
            printf ("nand write giving up\n");
            break;
        }

        rCount += 1;

    }  while ((flashVerify (fp, flen, data) == FALSE) && (rCount < 5));

    
    if (rCount >= 5) 
        printf ("nand write failed (maximum retries reached)\n");
    else
        printf ("flash programming completed\n");

    free (data);
    fclose (fp);
    return (0);

}




