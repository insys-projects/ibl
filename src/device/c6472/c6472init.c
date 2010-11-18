/**
 * @file c6472init.c
 *
 * @brief
 *		c6472 functions used during the initial stage of the ibl load
 *
 */
#include "ibl.h"
#include "device.h"
#include "pllapi.h"

/**
 * @brief Configure the PLLs
 *
 * @details
 *   The three PLLs are enabled. Only the main PLL has the ability to configure
 *   the multiplier and dividers.
 */
void devicePllConfig (void)
{
    if (ibl.pllConfig[ibl_MAIN_PLL].doEnable == TRUE)
        hwPllSetPll (MAIN_PLL, 
                     ibl.pllConfig[ibl_MAIN_PLL].prediv,
                     ibl.pllConfig[ibl_MAIN_PLL].mult,
                     ibl.pllConfig[ibl_MAIN_PLL].postdiv);

    if (ibl.pllConfig[ibl_DDR_PLL].doEnable == TRUE)
        hwPllEnable (DDR_PLL);

    if (ibl.pllConfig[ibl_NET_PLL].doEnable == TRUE)
        hwPllEnable (NET_PLL);


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



