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



/************************************************************************************
 * FILE PURPOSE: Boot a binary blob
 ************************************************************************************
 * FILE NAME: blob.c
 *
 * DESCRIPTION: A binary blob is booted.
 *
 * @file blob.c
 *
 * @brief
 *   This file reads data from the boot device as a binary blob
 *
 ************************************************************************************/
#include "types.h"
#include "ibl.h"
#include "iblloc.h"
#include "iblblob.h"



/**
 * @b Description
 * @n
 *
 * A simple read from the boot device is done until one of the following
 * conditions occurs:
 *    - The complete data block is read
 *    - The read request fails
 */

void iblBootBlob (BOOT_MODULE_FXN_TABLE *bootFxn, Uint32 *entry, void *formatParams)
{
    Int32   dataSize;
    Uint32  remainSize;
    Uint32  dummyVal;
    Uint32  bytesRead    = 0;
    Int32   erVal        = 0;
    Uint8  *datap;

    iblBinBlob_t *blobParams = (iblBinBlob_t *)formatParams;

    datap  = (Uint8 *)blobParams->startAddress;
    *entry = 0;

    for (remainSize = blobParams->sizeBytes; (remainSize > 0) && (erVal == 0);   )  {

        /* If there is any data waiting go ahead and process it */
        dataSize = (*bootFxn->query)();

        /* The query function will return a negative value when the stream has
         * closed */
        if (dataSize < 0)
            break;

        if (dataSize > remainSize)
            dataSize = remainSize;

        if (dataSize > 0)  {

            (*bootFxn->read)(datap, dataSize);
            datap      = datap      + dataSize;
            remainSize = remainSize - dataSize;
            bytesRead  = bytesRead  + dataSize;

        }  else  {

            /* If there is no data waiting peek for more. On error assume
             * that the device has completed the transfer. This is valid
             * since the read includes a timeout. */
            erVal = (*bootFxn->peek)((Uint8 *)&dummyVal, 1);

        }

    }

   /* Assume that if any data was read the boot was successful. 
    * A check could be added to see if the entry address was
    * actually written, but this assumes there was no data already
    * at the entry point */
   if (bytesRead > 0)
      *entry = blobParams->branchAddress;

}


     
    







