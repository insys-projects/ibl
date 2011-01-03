/********************************************************************************************************
 * FILE PURPOSE: Config level PLL driver
 ********************************************************************************************************
 * FILE NAME: cfgpll.c
 *
 * DESCRIPTION: The boot driver for PLLs that dont have a pll controller, but are controlled
 *				by registers in config space.
 *
 *              This driver differs from cfgpll! In this file the reset is controlled by
 *              bit 13 in register b, not bit 14!.
 *
 *********************************************************************************************************/
#include "types.h"
#include "target.h"
#include "pllapi.h"

#define DEVICE_REG32_W(x,y)   *(volatile unsigned int *)(x)=(y)
#define DEVICE_REG32_R(x)    (*(volatile unsigned int *)(x))

#define BOOTBITMASK(x,y)      (   (   (  ((UINT32)1 << (((UINT32)x)-((UINT32)y)+(UINT32)1) ) - (UINT32)1 )   )   <<  ((UINT32)y)   )
#define BOOT_READ_BITFIELD(z,x,y)   (((UINT32)z) & BOOTBITMASK(x,y)) >> (y)
#define BOOT_SET_BITFIELD(z,f,x,y)  (((UINT32)z) & ~BOOTBITMASK(x,y)) | ( (((UINT32)f) << (y)) & BOOTBITMASK(x,y) )


/*********************************************************************************************************
 * FUNCTION PURPOSE: Configure and enable a pll
 *********************************************************************************************************
 * DESCRIPTION: The PLL is configured. If the existing configuration matches the requested one no
 *              register write is made.
 *********************************************************************************************************/
SINT16 hwPllSetCfg2Pll (UINT32 base, UINT32 prediv, UINT32 mult, UINT32 postdiv, UINT32 chipFreqMhz, UINT32 pllFreqMhz)
{
    UINT32 reg;
    UINT32 regb;
    UINT32 bwAdj;

    UINT16 currentPrediv;
    UINT16 currentMult;
    UINT16 currentPostdiv;
    UINT16 currentBypass;
    UINT16 currentBwAdj;
    UINT16 currentEnable;

    reg =  DEVICE_REG32_R (base);
    regb = DEVICE_REG32_R (base + 4);

    currentPrediv  = 1 + BOOT_READ_BITFIELD (reg,  5,  0);
    currentMult    = 1 + BOOT_READ_BITFIELD (reg, 18,  6);
    currentPostdiv = 1 + BOOT_READ_BITFIELD (reg, 22, 19);
    currentBypass  =     BOOT_READ_BITFIELD (reg, 23, 23);
    currentBwAdj   = 1 + BOOT_READ_BITFIELD (reg, 31, 24) + ((BOOT_READ_BITFIELD (regb, 3, 0)) << 8);
    currentEnable  =     BOOT_READ_BITFIELD(regb, 13, 13);

    /* The PLL is currently enabled and connected if bypass == 0, enable == 1, clkout == 1 */

    if ( (currentBypass  == 0)           &&
         (currentPrediv  == prediv)      &&
         (currentMult    == mult)        &&
         (currentPostdiv == postdiv)     &&
         (currentEnable  == 0)           &&
         (currentBwAdj   == (mult >> 1))  )
        return (0);


    /* bwAdj is based only on the mult value */
    bwAdj = (mult >> 1) - 1;

    /* Multiplier / divider values are input as 1 less then the desired value */
    if (prediv > 0)
        prediv -= 1;

    if (mult > 0)
        mult -= 1;

    if (postdiv > 0)
        postdiv -= 1;

    /* Set bit 13 in register 1 to disable the PLL (assert reset) */
    regb = BOOT_SET_BITFIELD(regb, 1, 13, 13);
    DEVICE_REG32_W (base + 4, regb);

    /* Setup the PLL. Assert bypass */
    reg = BOOT_SET_BITFIELD (reg, prediv,          5,  0);
    reg = BOOT_SET_BITFIELD (reg, mult,           18,  6);
    reg = BOOT_SET_BITFIELD (reg, postdiv,        22, 19);
    reg = BOOT_SET_BITFIELD (reg, 1,              23, 23);   /* Bypass must be enabled */
    reg = BOOT_SET_BITFIELD (reg, (bwAdj & 0xff), 31, 24);

    DEVICE_REG32_W (base, reg);

    /* The 4 MS Bits of bwadj */
    regb = BOOT_SET_BITFIELD (regb, (bwAdj >> 8), 3, 0);
    DEVICE_REG32_W (base + 4, regb);


    /* Reset must be asserted for at least 5us. Give a huge amount of padding here to be safe
     * (the factor of 100) */
    chipDelay32 (5 * chipFreqMhz * 100);


    /* Clear bit 13 in register 1 to re-enable the pll */
    regb = BOOT_SET_BITFIELD(regb, 0, 13, 13);
    DEVICE_REG32_W (base + 4, regb);

    /* Need to wait 100,000 output PLL cycles before releasing bypass and setting 
     * up the clk output */
    chipDelay32 (chipFreqMhz * 100000 / pllFreqMhz);


    /* Disable the bypass */
    reg = BOOT_SET_BITFIELD (reg, 0, 23, 23);   /* The value 0 disables the bypass */
    DEVICE_REG32_W (base, reg);

    return (0);

} /* hwPllSetCfg2Pll */


