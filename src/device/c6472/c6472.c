/************************************************************************************
 * FILE PURPOSE: C6472 Device Specific functions
 ************************************************************************************
 * FILE NAME: c6472.c
 *
 * DESCRIPTION: Implements the device specific functions for the IBL
 *
 * @file c6472.c
 *
 * @brief
 *  This file implements the device specific functions for the IBL
 *
 ************************************************************************************/
#include "ibl.h"
#include "device.h"
#include "pllapi.h"
#include "emif31api.h"
#include "pscapi.h"
#include "gpio.h"
#include <string.h>

extern cregister unsigned int DNUM;


/**
 *  @brief Determine if an address is local
 *
 *  @details
 *    Examines an input address to determine if it is a local address
 */
bool address_is_local (Uint32 addr)
{
    /* L2 */
    if ((addr >= 0x00800000) && (addr < 0x00898000))
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
 *  The global address is formed. If the address is not local then
 *  the input address is returned
 */
Uint32 deviceLocalAddrToGlobal (Uint32 addr)
{

    if (address_is_local (addr))
        addr = (1 << 28) | (DNUM << 24) | addr;

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
    if (ibl.ddrConfig.configDdr != 0)
        hwEmif3p1Enable (&ibl.ddrConfig.uEmif.emif3p1);

}
        

/**
 *  @brief Power up a peripheral
 *
 *  @details
 *    Boot peripherals are powered up
 */
int32 devicePowerPeriph (int32 modNum)
{
    /* If the input value is < 0 there is nothing to power up */
    if (modNum < 0)
        return (0);


    if (modNum >= TARGET_PWR_MAX_MOD)
        return (-1);

    return ((int32)pscEnableModule(modNum));
        
}


/**
 *  @brief  Enable the pass through version of the nand controller
 *
 *  @details  On the evm the nand controller is enabled by setting 
 *            gpio 14 high
 */
#ifndef EXCLUDE_NAND
int32 deviceConfigureForNand(void)
{
	hwGpioSetDirection(NAND_MODE_GPIO, GPIO_OUT);
	hwGpioSetOutput(NAND_MODE_GPIO);
    return (0);

}
#endif


/**
 *  @brief
 *    The e-fuse mac address is loaded
 */
void deviceLoadDefaultEthAddress (uint8 *maddr)
{
    uint32 macA, macB;

    /* Read the e-fuse mac address */
    macA = *((uint32 *)0x2a80700);
    macB = *((uint32 *)0x2a80704);

    maddr[0] = (macA >> 24) & 0xff;
    maddr[1] = (macA >> 16) & 0xff;
    maddr[2] = (macA >>  8) & 0xff;
    maddr[3] = (macA >>  0) & 0xff;
    maddr[4] = (macB >> 24) & 0xff;
    maddr[5] = (macB >> 16) & 0xff;
}





