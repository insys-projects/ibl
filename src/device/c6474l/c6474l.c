/************************************************************************************
 * FILE PURPOSE: C6474 Device Specific functions
 ************************************************************************************
 * FILE NAME: c6474l.c
 *
 * DESCRIPTION: Implements the device specific functions for the IBL
 *
 * @file c6474l.c
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
 * @brief Configure the PLLs
 *
 * @details
 *   Only the main PLL can be configured here. The DDR pll is enabled by default,
 *   and the network PLL is enabled through serdes configuration.
 *   the multiplier and dividers.
 */
void devicePllConfig (void)
{
    if (ibl.pllConfig[ibl_MAIN_PLL].doEnable == TRUE)
        hwPllSetPll (MAIN_PLL, 
                     ibl.pllConfig[ibl_MAIN_PLL].prediv,
                     ibl.pllConfig[ibl_MAIN_PLL].mult,
                     ibl.pllConfig[ibl_MAIN_PLL].postdiv);

}

/**
 * @brief
 *   Enable the DDR
 *
 * @details
 *   The DDR controller on the c6474 is an emif 3.1. The controller is
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
#if 0
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
    macA = *((uint32 *)0x2880834);
    macB = *((uint32 *)0x2880838);

    maddr[0] = (macB >>  8) & 0xff;
    maddr[1] = (macB >>  0) & 0xff;
    maddr[2] = (macA >> 24) & 0xff;
    maddr[3] = (macA >> 16) & 0xff;
    maddr[4] = (macA >>  8) & 0xff;
    maddr[5] = (macA >>  0) & 0xff;
}





