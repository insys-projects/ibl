/**
 * @file c661xinit.c
 *
 * @brief
 *		c661x functions used during the initial stage of the ibl load
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
        hwPllSetCfg2Pll (DEVICE_PLL_BASE(DDR_PLL),
                         ibl.pllConfig[ibl_DDR_PLL].prediv,
                         ibl.pllConfig[ibl_DDR_PLL].mult,
                         ibl.pllConfig[ibl_DDR_PLL].postdiv,
                         ibl.pllConfig[ibl_MAIN_PLL].pllOutFreqMhz,
                         ibl.pllConfig[ibl_DDR_PLL].pllOutFreqMhz);

    if (ibl.pllConfig[ibl_NET_PLL].doEnable == TRUE)
        hwPllSetCfgPll (DEVICE_PLL_BASE(NET_PLL),
                        ibl.pllConfig[ibl_NET_PLL].prediv,
                        ibl.pllConfig[ibl_NET_PLL].mult,
                        ibl.pllConfig[ibl_NET_PLL].postdiv,
                        ibl.pllConfig[ibl_MAIN_PLL].pllOutFreqMhz,
                        ibl.pllConfig[ibl_NET_PLL].pllOutFreqMhz);


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


