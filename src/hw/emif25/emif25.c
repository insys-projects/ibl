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
 *  @file emif25.c
 *
 *  @brief
 *		Emif25 driver
 */

#include "types.h"
#include "target.h"
#include "emif25.h"
#include "emif25_loc.h"


/**
 *  @brief
 *      Initialize the interface. 
 */
Int32 hwEmif25Init (int32 cs, int32 busWidth, bool wait, bool nand)
{
    Uint32 reg;
    Uint32 v;

    /* Bound check the chip select */
    if ((cs < 2) || (cs > 5))
        return (EMIF25_INVALID_CS);

    /* Check for a valid bus width */
    if (busWidth == 8) 
        v = 0;
    else if (busWidth == 16)
        v = 1;
    else if (busWidth == 32)
        v = 2;
    else
        return (EMIF25_INVALID_BUS_WIDTH);
    
    /* Setup the bus width. The macro uses the actual chip select value, 2-5 */
    reg = DEVICE_REG32_R (DEVICE_EMIF25_BASE + EMIF25_ASYNC_CFG_REG(cs-2));
    reg = EMIF25_SET_ASYNC_WID(reg, v);

    /* Enable extended wait if requested */
    if (wait)
        v = 1;
    else
        v = 0;

    reg = EMIF25_SET_ASYNC_WAIT(reg, v);

    DEVICE_REG32_W (DEVICE_EMIF25_BASE + EMIF25_ASYNC_CFG_REG(cs-2), reg);

    /* Nand enable */
    if (nand)
        v = 1;
    else
        v = 0;

    reg = DEVICE_REG32_R (DEVICE_EMIF25_BASE + EMIF25_FLASH_CTL_REG);
    reg = EMIF25_SET_FLASH_CTL_NAND_ENABLE(reg, v, cs);
    DEVICE_REG32_W (DEVICE_EMIF25_BASE + EMIF25_FLASH_CTL_REG, reg);

    return (0);

}


