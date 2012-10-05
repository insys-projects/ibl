/********************************************************************************************************
 * FILE PURPOSE: Config level PLL driver
 ********************************************************************************************************
 * FILE NAME: cfgpll.c
 *
 * DESCRIPTION: The boot driver for PLLs that dont have a pll controller, but are controlled
 *				by registers in config space.
 *
 *********************************************************************************************************/
#include "types.h"
#include "target.h"
#include "pllapi.h"

#define DEVICE_REG32_W(x,y)   *(volatile unsigned int *)(x)=(y)
#define DEVICE_REG32_R(x)    (*(volatile unsigned int *)(x))

#define BOOTBITMASK(x,y)      (   (   (  ((UINT32)1 << (((UINT32)x)-((UINT32)y)+(UINT32)1) ) - (UINT32)1 )   )   <<  ((UINT32)y)   )    // Sets a bitmask of 1s from [x:y]
#define BOOT_READ_BITFIELD(z,x,y)   (((UINT32)z) & BOOTBITMASK(x,y)) >> (y)                                                             // Reads the value of register z[x:y]
#define BOOT_SET_BITFIELD(z,f,x,y)  (((UINT32)z) & ~BOOTBITMASK(x,y)) | ( (((UINT32)f) << (y)) & BOOTBITMASK(x,y) )                     // Sets register z[x:y] to f

void pass_pll_delay (UINT32 del)
{
  UINT32 i;
  volatile UINT32 j;

  for (i = j = 0; i < del; i++)
    asm (" nop ");

}


/*********************************************************************************************************
 * FUNCTION PURPOSE: Configure and enable a pll
 *********************************************************************************************************
 * DESCRIPTION: The PLL is configured. If the existing configuration matches the requested one no
 *              register write is made.
 *********************************************************************************************************/
SINT16 hwPllSetCfgPll (UINT32 base, UINT32 prediv, UINT32 mult, UINT32 postdiv, UINT32 chipFreqMhz, UINT32 pllFreqMhz)
{
    UINT32 reg;         // PASSPLLCTL0
    UINT32 regb;        // PASSPLLCTL1
    UINT32 bwAdj;

    UINT16 currentPrediv;
    UINT16 currentMult;
    UINT16 currentPostdiv;
    UINT16 currentBypass;
    UINT16 currentBwAdj;
    UINT16 currentEnable;
    UINT16 currentClkOut;

    reg =  DEVICE_REG32_R (base);
    regb = DEVICE_REG32_R (base + 4);

    currentPrediv  = 1 + BOOT_READ_BITFIELD (reg,  5,  0);
    currentMult    = 1 + BOOT_READ_BITFIELD (reg, 18,  6);
    currentPostdiv = 1 + BOOT_READ_BITFIELD (reg, 22, 19);
    currentBypass  =     BOOT_READ_BITFIELD (reg, 23, 23);
    currentBwAdj   = 1 + BOOT_READ_BITFIELD (reg, 31, 24) + ((BOOT_READ_BITFIELD (regb, 3, 0)) << 8);
    currentEnable  =     BOOT_READ_BITFIELD(regb, 14, 14);
    currentClkOut  =     BOOT_READ_BITFIELD(regb, 13, 13);

    /* bwAdj is based only on the mult value */
    bwAdj = (mult >> 1) - 1;

    /* Write to the ENSAT bit */
    regb = BOOT_SET_BITFIELD(regb, 1, 6, 6);
    DEVICE_REG32_W (base + 4, regb);

    /* Setup the PLL. Assert bypass */
    reg = BOOT_SET_BITFIELD (reg, 1, 23, 23);   /* Bypass must be enabled */
    DEVICE_REG32_W (base, reg);

    /* Program PLLM and PLLD */
    reg = BOOT_SET_BITFIELD (reg, prediv - 1, 5, 0);
    reg = BOOT_SET_BITFIELD (reg, mult - 1, 18, 6);
    
    /* Program BWADJ */
    reg = BOOT_SET_BITFIELD (reg, (bwAdj & 0xff), 31, 24);

    DEVICE_REG32_W (base, reg);

    /* The 4 MS Bits of bwadj */
    regb = BOOT_SET_BITFIELD (regb, (bwAdj >> 8), 3, 0);
    DEVICE_REG32_W (base + 4, regb);

    /* Set bit 14 in register 1 to disable the PLL (assert reset) */
    regb = BOOT_SET_BITFIELD(regb, 1, 14, 14);
    DEVICE_REG32_W (base + 4, regb);

    /* Reset must be asserted for at least 5us */
    pass_pll_delay(7000);
    
    /* set bit 13 in register 1 for selecting the output of PASS PLL as the input to PASS */
    regb = BOOT_SET_BITFIELD(regb, 1, 13, 13);
    DEVICE_REG32_W (base + 4, regb);

    /* Clear bit 14 in register 1 to re-enable the pll */
    regb = BOOT_SET_BITFIELD(regb, 0, 14, 14);
    DEVICE_REG32_W (base + 4, regb);

    /* Wait for atleast 500 * REFCLK cycles * (PLLD+1) */
    pass_pll_delay(70000);

    /* Disable the bypass */
    reg = BOOT_SET_BITFIELD (reg, 0, 23, 23);   /* The value 0 disables the bypass */
    DEVICE_REG32_W (base, reg);

    return (0);

} /* hwPllSetCfgPll */




