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
#include <stdlib.h>

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

    uint8   blockEraseCommandPre;  /**< The command used to erase a block (sent before the address) */
    uint8   blockEraseCommandPost; /**< The command used to erase a block (sent after the address) */
    bool    blockErasePost;        /**< If TRUE the post command is sent */

    uint8   pageWriteCommandPre;   /**< The command used to program a page (sent before the address) */
    uint8   pageWriteCommandPost;  /**< The command used to program a page (sent after the address) */
    bool    pageWritePost;         /**< If TRUE the post command is sent */

    char    fileName[132];         /**< The name of the file to burn on the flash */

} nandWriterInfo_t;


nandWriterInfo_t nandWriterInfo;



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


    /* Prompt the user to initialize the data block */
    printf ("Initialize the nandWriteInfo structure, press return\n");
    getchar ();


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
    fp = fopen (nandWriterInfo.fileName, "rb");
    if (fp == NULL)  {
      printf ("Failed to open file %s\n", nandWriterInfo.fileName);
      return (-1);
    }

    fseek (fp, 0l, SEEK_END);
    flen = ftel (fp);
    fseek (fp, 0l, SEEK_SET);

    data = malloc ((nandWriterInfo.pageSizeBytes + nandWriterInfo.pageEccBytes) * sizeof (Uint8));
    if (data == NULL)  {
      printf ("Malloc failed on size of %d bytes\n", nandWriterInfo.pageSizeBytes + nandWriterInfo.pageEccBytes);
      fclose (fp);
      return (-1);
    }
    
    /* Initialize the programming interface */
    ret = nandHwDriverInit (&devInfo);
    if (ret != 0)  {
        printf ("nandHwDriverInit failed with error code %d\n", ret);
        free (data);
        fclose (fp);
        return (-1);
    }

    /* Write the flash, verify the results. On read back failure mark
     * the page as bad and try rewriting again */
    do  {

        flashNand (fp, flen, data);

    }  while (flashVerify (fp, flen, data) == FALSE);


    free (data);
    fclose (fp);
    return (0);

}



void flashNand (FILE *fp, Uint32 flen, Uint8 *data)
{
    Uint32 fpos;
    Uint32 rlen;
    Int32  ret;
    Int32  i;

    Int32   eccsPerPage;
    Uint8  *eccData;        /* Points to the 256 bytes of data to compute the ecc */
    Uint8  *eccBytes;       /* Points to the generated ECC data */

    /* Reset to the start of the data file */
    fseek (fp, 0l, SEEK_SET);

    /* The ECC size is fixed at 256 bytes */
    eccsPerPage = nandWriterInfo.pageSizeBytes >> 8;


    /* Program the NAND */
    for (block = -1, page = nandWriterInfo.pagesPerBlock, fpos = 0; fpos < flen; fpos += nandWriterInfo.pageSizeBytes)  {

        /* Advance over a block boundary. Verify block limit and bad block mark */
        if (++page >= nandWriterInfo.pagesPerBlock)  {

            do  {

                block += 1;

            } while ((block < nandWriterInfo.totalBlocks) && (checkBadBlockMark (block) == TRUE));

            page = 0;
        }

        /* Initialize the entire page (with ecc bytes) to 0xff. This will preserve the good block
         * mark at the beginning of pages 0 and 1 */
        memset (data, 0xff, ((nandWriterInfo.pageSizeBytes + nandWriterInfo.eccSizeBytes) * sizeof (Uint8));


        /* Read the data from the file */
        rlen = nandWriterInfo.pageSizeBytes
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

        /* Compute the ECCs. The ECC location size varies by the ECC algorithm */
        for (i = 0; i < eccsPerPage; i++)  {

            eccData  = &data[i*256];
            eccBytes = &data[nandWriterInfo.pageSizeBytes + nandWriterInfo.pageEccBytes - ((eccsPerPage - i) * NUM_ECC_BYTES_PER_PAGE)];
            eccComputeECC (eccData, eccBytes);

        }

        ret = nandHwWritePage (block, page, data);
        if (ret < 0)  {
            printf ("nandHwWritePage returned error code %d writing block %d, page %d\n", ret, block, page);
            free (data);
            fclose (fp);
            return (-1);
        }

    }

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


        /* Read a page of data */
        ret = nandHwDriverReadPage (block, page, nandData);
        if (ret < 0)  {
            printf ("nandHwDriverReadPage returned error cod %d\n", ret);
            free (nandData);
            return (TRUE);      /* return TRUE to allow the program to exit */
        }

        /* Read the data from the file */
        rlen = nandWriterInfo.pageSizeBytes
        if (flen - fpos < nandWriterInfo.pageSizeBytes)  {
            rlen = flen - fpos;
        }
    
        if (fread (data, sizeof (Uint8), rlen, fp) == 0)  {
            printf ("fread unexpectedly returned 0 on read of %d bytes staring at byte location %d.\n", rlen, fpos);
            printf ("  Total file length = %d bytes\n", flen);
            free (nandData);
            return (TRUE);
        }

        if (memcmp (data, nandData, nandWriterInfo.pageSizeBytes)  {
            printf ("Data verification failed on block %d, page %d, file position %d, file size %d\n", block, page, fpos, flen);
            printf ("Marking the block as bad and retrying the flash\n");
            free (nandData);
            return (FALSE);
        }

    }

}



    






