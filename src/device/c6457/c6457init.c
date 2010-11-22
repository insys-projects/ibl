/**
 * @file c6457init.c
 *
 * @brief
 *	c6457 functions used during the initial stage of the ibl load
 */
#include "ibl.h"
#include "device.h"
#include "pllapi.h"

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
