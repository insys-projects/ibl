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
 * FILE PURPOSE: C6455 Device Specific functions
 ************************************************************************************
 * FILE NAME: c6455.c
 *
 * DESCRIPTION: Implements the device specific functions for the IBL
 *
 * @file c6455.c
 *
 * @brief
 *  This file implements the device specific functions for the IBL
 *
 ************************************************************************************/
#include "ibl.h"
#include "device.h"
#include "pllapi.h"
#include "emif31api.h"
#include "nandhwapi.h"


/**
 *  @brief Determine if an address is local
 *
 *  @details
 *    Examines an input address to determine if it is a local address
 */
bool address_is_local (Uint32 addr)
{
    /* L2 */
    if ((addr >= 0x00800000) && (addr < 0x00a00000))
        return (TRUE);

    /* L1P */
    if ((addr >= 0x00e00000) && (addr < 0x00e08000))
        return (TRUE);

    /* L2D */
    if ((addr >= 0x00f00000) && (addr < 0x00f08000))
        return (TRUE);

    return (FALSE);

}


/**
 * @brief  Convert a local l1d, l1p or l2 address to a global address
 *
 * @details
 *   On the c6455 all local addresses are also global
 *  
 */
Uint32 deviceLocalAddrToGlobal (Uint32 addr)
{

    return (addr);

}


/**
 * @brief
 *   Enable the DDR
 *
 * @details
 *   The DDR controller on the c6472 is an emif 3.1. The controller is
 *   initialized directly with the supplied values
 */
void deviceDdrConfig (void)
{

    if (ibl.ddrConfig.configDdr != 0)  {
        devicePowerPeriph (TARGET_PWR_DDR);
        hwEmif3p1Enable (&ibl.ddrConfig.uEmif.emif3p1);
    }

}


/**
 *  @brief Power up a peripheral
 *
 *  @details
 *    Boot peripherals are powered up. The c6455 uses a unique control and mapping
 *    to enable peripherals, so it is implemented directly here.
 */
int32 devicePowerPeriph (int32 modNum)
{
    uint32   reg;
    volatile int32 i;


    /* Enable ethernet */
    if (modNum == TARGET_PWR_ETH(0))  {

        reg = *((volatile uint32 *)0x2ac0008);   /* Read the peripheral config 0 register */
        reg = reg | (1 << 4);                    /* Set the emac enable bit */

        *((volatile uint32 *)0x2ac0004) = 0x0f0a0b00;  /* Unlock the peripheral config register */
        *((volatile uint32 *)0x2ac0008) = reg;         /* Power up the emac */

        do   {

            reg = *((volatile uint32 *)0x2ac0014);     /* Read the peripheral status register */

        }  while ((reg & 0x01c0) != 0x0040);           /* Wait for the power up to complete */

    }


    if (modNum == TARGET_PWR_TIMER_0)  {

        reg = *((volatile uint32 *)0x2ac0008);   /* Read the peripheral config 0 register */
        reg = reg | (1 << 6);                    /* Set the timer enable bit */

        *((volatile uint32 *)0x2ac0004) = 0x0f0a0b00;  /* Unlock the peripheral config register */
        *((volatile uint32 *)0x2ac0008) = reg;         /* Power up the timer */

        do   {

            reg = *((volatile uint32 *)0x2ac0014);     /* Read the peripheral status register */

        }  while ((reg & 0x0e00) != 0x0200);           /* Wait for the power up to complete */
    }



    /* Enable DDR */
    if (modNum == TARGET_PWR_DDR)  {
        reg = *((volatile uint32 *)0x2ac002c);   /* Read the peripheral config 1 register */
        reg = reg | (1 << 1);                   /* Set the DDR enable bit */

        *((volatile uint32 *)0x2ac002c) = reg;  /* Enable the DDR */

        /* There is no status register for emac enable, must wait for 16 sysclock 3 cycles
         * before configuring ddr. sysclock 3 is 1/6 cpu clock */
        for (i = 0; i < 16*3; i++);   /* Overkill, but will do it */

    }


    /* For all other modules there was nothing to power up */
    return (0);
    
        
}

unsigned int get_device_switch_setting()
{
	return (0);
}

/**
 *  @brief  Enable the pass through version of the nand controller
 *
 *  @details  
 */
#ifndef EXCLUDE_NAND_GPIO
int32 deviceConfigureForNand(void)
{
    return (0);
}
#endif

/**
 *  @brief
 *    There is no mac address in e-fuse, so 
 *    0 is returned.
 */
void deviceLoadDefaultEthAddress (uint8 *maddr)
{
    maddr[0] = 0;
    maddr[1] = 0;
    maddr[2] = 0;
    maddr[3] = 0;
    maddr[4] = 0;
    maddr[5] = 0;
}

/**
 *  @brief  Return the NAND interface call table. Only GPIO is supported on c6455
 */

#ifndef EXCLUDE_NAND_GPIO
nandCtbl_t nandCtbl =  {

    nandHwGpioDriverInit,
    nandHwGpioDriverReadBytes,
    nandHwGpioDriverReadPage,
    nandHwGpioDriverClose

};

nandCtbl_t *deviceGetNandCtbl (int32 interface)
{
    return (&nandCtbl);
}
#endif

