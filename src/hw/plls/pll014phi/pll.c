/*************************************************************************************
 * FILE PURPOSE: Provide PLL control functions
 *************************************************************************************
 * FILE NAME: pll.c
 *
 * DESCRIPTION: Provides functions to control the pll
 *
 *************************************************************************************/
#include "types.h"
#include "ibl.h"
#include "pllloc.h"
#include "pllapi.h"
#include "target.h"

#define DEVICE_REG32_W(x,y)   *(volatile unsigned int *)(x)=(y)
#define DEVICE_REG32_R(x)    (*(volatile unsigned int *)(x))

#define BOOTBITMASK(x,y)      (   (   (  ((UINT32)1 << (((UINT32)x)-((UINT32)y)+(UINT32)1) ) - (UINT32)1 )   )   <<  ((UINT32)y)   )
#define BOOT_READ_BITFIELD(z,x,y)   (((UINT32)z) & BOOTBITMASK(x,y)) >> (y)
#define BOOT_SET_BITFIELD(z,f,x,y)  (((UINT32)z) & ~BOOTBITMASK(x,y)) | ( (((UINT32)f) << (y)) & BOOTBITMASK(x,y) )


typedef enum  {
    HW_PLL_DO_NOT_ENABLE_PLL,
    HW_PLL_ENABLE_PLL
} hwPllEnable_t;

/*********************************************************************************
 * FUNCTION PURPOSE: Provide a delay loop
 *********************************************************************************
 * DESCRIPTION: Generates a delay, units of cycles
 *********************************************************************************/
void hw_pll_delay (UINT32 del)
{
  UINT32 i;
  volatile UINT32 j;

  for (i = j = 0; i < del; i++)
    asm (" nop ");

} /* hw_pll_delay */


/**********************************************************************************
 * FUNCTION PURPOSE: Enables the pll to the specified multiplier
 **********************************************************************************
 * DESCRIPTION: Sets up the pll
 **********************************************************************************/
SINT16 hwPllSetPll (UINT32 pllNum, UINT32 prediv, UINT32 mult, UINT32 postdiv)
{
  UINT32 ctl, reg;
  UINT32 secctl;
  UINT32 status;
  UINT32 alnctl;
  UINT32 pmult;
  UINT32 pdiv;
  UINT32 pllBase;
  UINT32 i;
  SINT16 ret = 0;
  UINT32 pllm_min = 10, plld_min =0, outputdiv = 9;
  UINT32 div2=3, div5=5, div8=64;

  /* Mutliplier/divider values of 0 are invalid */
  if (prediv == 0)
    prediv = 1;

  if (mult == 0)
    mult = 1;

  if (postdiv == 0)
    postdiv = 1;

  /* Get the base address of the pll */
  pllBase = (UINT32) DEVICE_PLL_BASE(pllNum);

 /* Wait for Stabilization time (min 100 us) */
 /* assuming max device speed, 1.4GHz, 1 cycle = 0.714 ns *
  * so, 100 us = 100000 ns = 140056 cycles */
  hw_pll_delay (140056);

  /* Program pllen=0 (pll bypass), pllrst=1 (reset pll), pllsrc = 0 */
  ctl = DEVICE_REG32_R (pllBase + PLL_REG_CTL);

 /* Check if PLL BYPASS is turned off previously */
  secctl = DEVICE_REG32_R (pllBase + PLL_REG_SECCTL);

  if ( (secctl & PLL_REG_SECCTL_FIELD_BYPASS) != 0 ) {
    /* PLL BYPASS is turned on */

	/* Set the ENSAT Bit */
	/* Usage Note 9: For optimal PLL operation, the ENSAT bit in the PLL control
	 * registers for the Main PLL, DDR3 PLL, and PA PLL should be set to 1.
	 * The PLL initialization sequence in the boot ROM sets this bit to 0 and
	 * could lead to non-optimal PLL operation. Software can set the bit to the
	 * optimal value of 1 after boot
	 */
        reg = DEVICE_REG32_R (DEVICE_MAIN_PLL_CTL_1);
        reg = reg | (1 << 6);
        DEVICE_REG32_W (DEVICE_MAIN_PLL_CTL_1, reg);

	/* Clear the PLLENSRC bit */
	ctl = ctl & ~(PLL_REG_CTL_FIELD_PLLENSRC);
	DEVICE_REG32_W (pllBase + PLL_REG_CTL, ctl);

	/* Clear the PLLEN bit */
	ctl = ctl & ~(PLL_REG_CTL_FIELD_PLLEN);
	DEVICE_REG32_W (pllBase + PLL_REG_CTL, ctl);

	
	/* Wait for 4 Ref clocks */
	/* The slowest clock can be at 24MHz, so min:160ns delay */
	hw_pll_delay(225);

	/* Put the PLL in Bypass mode to perform the power down mode */
	secctl = secctl | PLL_REG_SECCTL_FIELD_BYPASS;
	DEVICE_REG32_W (pllBase + PLL_REG_SECCTL, secctl);

	/* Advisory 8: Multiple PLLs May Not Lock After Power-on Reset Issue
         * In order to ensure proper PLL startup, the PLL power_down pin needs to be
         * toggled. This is accomplished by toggling the PLLPWRDN bit in the PLLCTL
         * register. This needs to be done before the main PLL initialization
         * sequence
	 */
	ctl = ctl | 2;
	DEVICE_REG32_W (pllBase + PLL_REG_CTL, ctl);

	/* Stay in a loop such that the bit is set for 5 µs (minimum) and
	 * then clear the bit.
	 */
	hw_pll_delay (14005); /* waiting 10 us */

	/* Power up the PLL */
	ctl = ctl & ~2;
	DEVICE_REG32_W (pllBase + PLL_REG_CTL, ctl);
	
  }

	/* Assert PLL Reset */
	ctl = ctl | (PLL_REG_CTL_FIELD_PLLRST);
	DEVICE_REG32_W (pllBase + PLL_REG_CTL, ctl);

	/* Program the necessary multipliers/dividers and BW adjustments
	 * This routine will subtract 1 from the mult value
	 */
	pmult = chipPllExternalMult(pllNum, mult);
	pmult   = pmult & PLL_REG_PLLM_FIELD_MULTm1;
	DEVICE_REG32_W (pllBase + PLL_REG_PLLM, pmult);

	/* set the BWADJ */
	chipPllExternalBwAdj (pllNum, mult);

	/* Set the PLL Divider */
	chipPllSetExternalPrediv(pllNum, prediv - 1);  

	/* set the output divide */
	secctl = BOOT_SET_BITFIELD(secctl, 1 & 0x000f, 22, 19);
	DEVICE_REG32_W (pllBase + PLL_REG_SECCTL, secctl); 
      	
	/* WAIT FOR THE go STAT BIT HERE (50 us) */
	hw_pll_delay (140056 >> 1);   
	/* wait for the GOSTAT, but don't trap if lock is never read */
	for (i = 0; i < 100; i++)  {
		hw_pll_delay (300);
		status = DEVICE_REG32_R (pllBase + PLL_REG_PLLSTAT);
		if ( (status & PLL_REG_STATUS_FIELD_GOSTAT) == 0 )
			break;
	}

	/* Enable the pll even if the lock failed. Return a warning. */
	if (i == 100)  
		ret = -1;
  
	/* Set PLL dividers if needed */
	reg = 0x8000 | (div2 -1);
	DEVICE_REG32_W (pllBase + PLL_REG_DIV2, reg);

	reg = 0x8000 | (div5 -1);
	DEVICE_REG32_W (pllBase + PLL_REG_DIV5, reg);

	reg = 0x8000 | (div8 -1);
	DEVICE_REG32_W (pllBase + PLL_REG_DIV8, reg);

	/* Program ALNCTLn registers */
	alnctl = DEVICE_REG32_R (pllBase + PLL_REG_ALNCTL);
	alnctl = alnctl | ((1 << 1) | (1 << 4) | (1 << 7));
	DEVICE_REG32_W (pllBase + PLL_REG_ALNCTL, alnctl);

	/* Set GOSET bit in PLLCMD to initiate the GO operation to change the divide *   
	 * values and align the SYSCLKs as programmed                                */
	reg = DEVICE_REG32_R (pllBase + PLL_REG_CMD);
	reg = reg | 1;
	DEVICE_REG32_W (pllBase + PLL_REG_CMD, reg);

	/* wait for the GOSTAT, but don't trap if lock is never read */
	for (i = 0; i < 100; i++)  {
		hw_pll_delay (300);
		status = DEVICE_REG32_R (pllBase + PLL_REG_PLLSTAT);
		if ( (status & PLL_REG_STATUS_FIELD_GOSTAT) == 0 )
			break;
	}

	/* Wait for 7 us, use 10 us*/
	hw_pll_delay (14006);

	/* Release PLL from Reset */
	ctl = ctl & ~(PLL_REG_CTL_FIELD_PLLRST);
	DEVICE_REG32_W (pllBase + PLL_REG_CTL, ctl);

	/* Wait for PLL Lock time (min 50 us) */
	hw_pll_delay (140056 >> 1);  
   
	/* wait for the pll to lock, but don't trap if lock is never read */
	for (i = 0; i < 100; i++)  {
		hw_pll_delay (2000/7);
		status = DEVICE_REG32_R (pllBase + PLL_REG_PLLSTAT);
		if ( (status & PLL_REG_STATUS_FIELD_LOCK) != 0 )
			break;
	}

	/* Enable the pll even if the lock failed. Return a warning. */
	if (i == 100)  
		ret = -1;

	/* Clear the secondary controller bypass bit */
	secctl = secctl & ~PLL_REG_SECCTL_FIELD_BYPASS;
	DEVICE_REG32_W (pllBase + PLL_REG_SECCTL, secctl);


	/* Set pllen to 1 to enable pll mode */
	ctl = ctl | PLL_REG_CTL_FIELD_PLLEN;
	DEVICE_REG32_W (pllBase + PLL_REG_CTL, ctl);
  
	return (ret);
} /* hwPllSetPll */


