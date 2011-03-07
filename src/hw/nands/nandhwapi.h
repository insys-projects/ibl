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



#ifndef _NAND_HW_API_H
#define _NAND_HW_API_H
/****************************************************************************************
 * FILE PURPOSE: Define the nand api which applies to all nand drivers
 ****************************************************************************************
 * FILE NAME: nandhwapi.h
 *
 * DESCRIPTION: Defines the API used by all the low level nand drivers
 *
 *  @file nandhwapi.h
 *
 *  @brief
 *		Defines the API used by all the low level nand drivers
 *
 ****************************************************************************************/
 
/* Return values */
#define NAND_INVALID_ADDR_SIZE      -810
#define NAND_NULL_ARG               -811
#define NAND_INVALID_ADDR           -812
#define NAND_ECC_FAILURE            -813
#define NAND_INVALID_CS             -814
#define NAND_READ_FAILURE           -815


/* Information used only for programming flash */
typedef struct nandProgramInfo_s
{
    uint8   blockEraseCommandPre;  /**< The command used to erase a block (sent before the address) */
    uint8   blockEraseCommandPost; /**< The command used to erase a block (sent after the address) */
    uint8   blockEraseNaddrBytes;  /**< The number of address bytes sent */
    bool    blockErasePost;        /**< If TRUE the post command is sent */

    uint8   pageWriteCommandPre;   /**< The command used to program a page (sent before the address) */
    uint8   pageWriteCommandPost;  /**< The command used to program a page (sent after the address) */
    bool    pageWritePost;         /**< If TRUE the post command is sent */
    
}  nandProgramInfo_t; 
    



/* Driver functions, EMIF and GPIO interface */
Int32 nandHwGpioDriverInit (int32 cs, void *devInfo);
Int32 nandHwGpioDriverReadBytes (Uint32 block, Uint32 page, Uint32 byte, Uint32 nbytes, Uint8 *data);
Int32 nandHwGpioDriverReadPage(Uint32 block, Uint32 page, Uint8 *data);
Int32 nandHwGpioDriverClose (void);

Int32 nandHwDriverWritePage  (Uint32 block, Uint32 page, Uint8 *data, nandProgramInfo_t *winfo);
Int32 nandHwDriverBlockErase (Uint32 uiBlockNumber, nandProgramInfo_t *winfo);

/* Driver functions, SPI interface */
Int32 nandHwSpiDriverInit (int32 cs, void *devInfo);
Int32 nandHwSpiDriverReadBytes (Uint32 block, Uint32 page, Uint32 byte, Uint32 nbytes, Uint8 *data);
Int32 nandHwSpiDriverReadPage(Uint32 block, Uint32 page, Uint8 *data);
Int32 nandHwSpiDriverClose (void);




#endif /* _NAND_HW_API_H */
