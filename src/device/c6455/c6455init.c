/************************************************************************************
 * FILE PURPOSE: C6455 Device Specific functions used in the 1st load stage
 ************************************************************************************
 * FILE NAME: c6455init.c
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


/**
 * @brief Configure the PLLs
 *
 * @details
 *   Only the main PLL is enabled. The second pll is used for
 *   emac and DDR, with a fixed multiplier, and is enabled
 *   at power up. The divider is configurable, but not done here.
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
 *  Return the endian status of the device
 *
 * @details
 *  Returns true if the device is executing in little endian mode
 */
extern cregister volatile unsigned int CSR;

bool deviceIsLittleEndian (void)
{
    if ((CSR & (1 << 8)) == 0)    
        return (FALSE);

    return (TRUE);

}

