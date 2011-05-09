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

/** 
 *  @file nor.c
 *
 *  @brief The nor boot driver
 */

#include "types.h"
#include "ibl.h"
#include "iblloc.h"
#include "device.h"
#include "nor.h"
#include <string.h>
#include <stdlib.h>


/**
 *  @brief  The nor master control block which tracks the current nor boot information 
 */
typedef struct normcb_s
{
    uint32 fpos;        /**<  Current file position. This is an absolute address, not relative to startPos */
    uint32 startPos;    /**<  Initial file position */

    norCtbl_t *nor_if;  /**<  Low level interface driver */

} normcb_t;

normcb_t normcb;

/**
 *  @brief 
 *      Set the current file position
 */
Int32 nor_seek (Int32 loc, Int32 from)
{
    /* Can't seek from the end of the file, since the end is not known */
    if (from == 0)
        normcb.fpos = normcb.startPos + loc;
    else if (from == 1)
        normcb.fpos += loc;
    else
        return (-1);

    if (normcb.fpos < normcb.startPos)
        normcb.fpos = normcb.startPos;

    return (0);
}
    

/**
 *  @brief
 *      Initialize the control structure. Note that the interface value was
 *      previously verified in the top level nor boot control.
 */
Int32 nor_open (void *ptr_driver, void (*asyncComplete)(void *))
{
    iblNor_t *ibln = (iblNor_t *)ptr_driver;

    normcb.startPos = normcb.fpos = ibln->bootAddress[iblEndianIdx][iblImageIdx];
    normcb.nor_if   = deviceGetNorCtbl (ibln->interface);

    return (0);

}

/**
 *  @brief
 *      Read data from the current address. This function is used 
 *      for peek as well as read.
 */
Int32 nor_read (Uint8 *ptr_buf, Uint32 num_bytes)
{
    Int32 ret;

    if (normcb.nor_if == NULL)
        return (-1);

    
    ret = (*normcb.nor_if->nct_driverReadBytes)(ptr_buf, num_bytes, normcb.fpos);

    if (ret == 0)
        normcb.fpos += num_bytes;

    return (ret);

}

/**
 *  @brief
 *      Return the number of bytes available for current read. 
 *      Always return 1k
 */
Int32 nor_query (void)
{
    return (0x400);

}

/**
 *  @brief
 *      Close the nor driver
 */
Int32 nor_close (void)
{
    if (normcb.nor_if != NULL)
        (*normcb.nor_if->nct_driverClose)();

    normcb.nor_if = NULL;
    return (0);

}

/**
 *  @brief
 *      The global nor module function table
 */
BOOT_MODULE_FXN_TABLE nor_boot_module =
{
    nor_open,       /* Open  API */
    nor_close,      /* Close API */
    nor_read,       /* Read  API */
    NULL,           /* Write API */
    nor_read,       /* Peek  API */
    nor_seek,       /* Seek  API */
    nor_query       /* Query API */

};



