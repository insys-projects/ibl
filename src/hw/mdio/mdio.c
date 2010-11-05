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



/**********************************************************************************
 * FILE PURPOSE: A very simple mdio driver
 **********************************************************************************
 * FILE NAME: mdio.c
 *
 * DESCRIPTION: Enables mdio and performs blind access
 *
 *  @file mdio.c
 *
 * @brief
 *	Enables mdio and performs blind access
 *
 **********************************************************************************/
#include "types.h"
#include "mdio.h"
#include "device.h"

/***********************************************************************************
 * FUNCTION PURPOSE: Provide an approximate delay
 ***********************************************************************************
 * DESCRIPTION: A delay in units of CPU cycles is executed
 ***********************************************************************************/
void mdio_delay (uint32 del)
{
    volatile unsigned int i;

    for (i = 0; i < del/8; i++);

}

/***********************************************************************************
 * FUNCTION PURPOSE: Perform blind MDIO access
 ***********************************************************************************
 * DESCRIPTION: Pre-configured mdio access is performed.
 ***********************************************************************************/
void hwMdio (int16 nAccess, uint32 *access, uint16 clkdiv, uint32 delayCpuCycles)
{
    int16 i;

    /* Enable MDIO, set the divider. A divider value of 0 is not allowed */
    if (clkdiv == 0)
        clkdiv = 1;

    MDIOR->CONTROL = (0x40000000 | clkdiv);


    for (i = 0; i < nAccess; i++)  {

        /* Set the Go bit */
        MDIOR->USERACCESS0 = (((uint32)1 << 31) | access[i]);
        while (MDIOR->USERACCESS0 & 0x80000000);

        mdio_delay (delayCpuCycles);

    }

    /* A big delay after the last configure */
    mdio_delay (1000000);

}






