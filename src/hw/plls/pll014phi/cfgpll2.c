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

#define CHIP_LEVEL_REG		0x02620000
#define DDR3PLLCTL0		*(volatile unsigned int*)(CHIP_LEVEL_REG + 0x0330)
#define DDR3PLLCTL1		*(volatile unsigned int*)(CHIP_LEVEL_REG + 0x0334)



void ddr3_pll_delay (UINT32 del)
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

    /* bwAdj is based only on the mult value */
    bwAdj = (mult >> 1) - 1;

    /* Write to the ENSAT bit */
    regb |= (1 << 6);
    DEVICE_REG32_W (base + 4, regb);

    /* Bypass must be enabled */
    reg |= (1 << 23);
    DEVICE_REG32_W (base, reg);

    /* Configure PLLM, PPLD BWADJ */
    reg = BOOT_SET_BITFIELD (reg, prediv - 1, 5, 0);
    reg = BOOT_SET_BITFIELD (reg, mult - 1, 18, 6);
    reg = BOOT_SET_BITFIELD (reg, (bwAdj & 0xff), 31, 24);

    DEVICE_REG32_W (base, reg);

    /* The 4 MS Bits of BWADJ */
    regb = BOOT_SET_BITFIELD (regb, (bwAdj >> 8), 3, 0);
    DEVICE_REG32_W (base + 4, regb);

    /* Set bit 13 in register 1 to disable the PLL (assert reset) */
    regb |= (1 << 13);
    DEVICE_REG32_W (base + 4, regb);

    /* Reset must be asserted for at least 5us */
    ddr3_pll_delay(7000);

    /* Clear bit 13 in register 1 to re-enable the pll */
    regb &= ~(1 << 13);
    DEVICE_REG32_W (base + 4, regb);

    /* Wait for atleast 500 * REFCLK cycles * (PLLD+1) */
    ddr3_pll_delay(70000);

    /* Disable the bypass */
    reg &= ~(1 << 23);   /* The value 0 disables the bypass */
    DEVICE_REG32_W (base, reg);

    return (0);

} /* hwPllSetCfg2Pll */


