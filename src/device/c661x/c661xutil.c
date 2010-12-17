/**
 * @file c661xutil.c
 *
 * @brief
 *	  c661x functions used by both the ibl and utility programs
 */

#include "device.h"
#include "pllapi.h"


/**
 *  @brief
 *     Configure the predivider for the main PLL, which resides outside the PLL controller
 */
SINT16 chipPllSetExternalPrediv(UINT16 pllNum, UINT32 predivRegVal)
{
    UINT32 reg;

    reg = DEVICE_REG32_R (DEVICE_MAIN_PLL_CTL_0);
    reg = BOOT_SET_BITFIELD(reg, predivRegVal, 5, 0);
    DEVICE_REG32_W (DEVICE_MAIN_PLL_CTL_0, reg);


    return (0);

} /* chipPllSetExternalPrediv */


/**
 *  @brief
 *      Configure the bandwidth adjustment for the main PLL, which resides outside the PLL controller
 */
SINT16 chipPllExternalBwAdj (UINT16 pllNum, UINT16 mult)
{
    UINT32 reg;
    UINT32 bwAdj;

    bwAdj = (mult >> 1) - 1;

    reg = DEVICE_REG32_R (DEVICE_MAIN_PLL_CTL_0);
    reg = BOOT_SET_BITFIELD(reg, bwAdj & 0x00ff, 31, 24);
    DEVICE_REG32_W (DEVICE_MAIN_PLL_CTL_0, reg);

    reg = DEVICE_REG32_R (DEVICE_MAIN_PLL_CTL_1);
    reg = BOOT_SET_BITFIELD(reg, bwAdj >> 8, 3, 0);
    DEVICE_REG32_W (DEVICE_MAIN_PLL_CTL_1, reg);

    return (0);

} /* chipPllExternalBwAdj */


/**
 *  @brief
 *      Configure the multiplier fields for the main PLL which reside outside the PLL controller
 */
UINT32 chipPllExternalMult (UINT16 pllNum, UINT16 mult)
{
    UINT32 pmult;
    UINT32 reg;
    UINT32 v;


    pmult = mult-1; 

    v = BOOT_READ_BITFIELD(pmult, 12, 6);
   

    reg = DEVICE_REG32_R (DEVICE_MAIN_PLL_CTL_0);
    reg = BOOT_SET_BITFIELD(reg, v, 18, 12);
    DEVICE_REG32_W (DEVICE_MAIN_PLL_CTL_0, reg);


    v = BOOT_READ_BITFIELD(pmult, 5, 0);

    return (v);

} /* chipPllExternalMult */

