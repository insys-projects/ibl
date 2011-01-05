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
 *  @file noremif25.c
 *
 *	@brief
 *		This file implements a translation layer from the nor boot
 *		to the emif driver. This is used to present a common api
 *		for the nor reads for the SPI and EMIF access
 */

#include "types.h"
#include "ibl.h"
#include "iblcfg.h"
#include "nor_api.h"
#include "emif25.h"
#include "emif25_loc.h"
#include "target.h"


uint32 nmemBase; /* The base address of the device in the memory map  */

/**
 *  @brief
 *      Initialize the interface. 
 */
Int32 norHwEmifDriverInit (int32 cs)
{

    nmemBase = deviceEmif25MemBase (cs);
    
    return (0);

}


/**
 *  @brief
 *      Read bytes. Not using memcpy to avoid a second copy (along with iblinit).
 */
Int32 norHwEmifDriverReadBytes (Uint8 *data, Uint32 nbytes, Uint32 address)
{

    Int32 i;
    Uint8 * restrict src;

    src = (Uint8 *)(nmemBase + address);


    for (i = 0; i < nbytes; i++)
        data[i] = src[i];

    return (0);

}

/**
 *  @brief
 *      Close the driver
 */
Int32 norHwEmifDriverClose (void)
{
    return (0);

}
