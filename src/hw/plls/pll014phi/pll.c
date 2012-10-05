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

 /* 1. Wait for Stabilization time (min 100 us)             *
  * assuming max device speed, 1.4GHz, 1 cycle = 0.714 ns   *
  * so, 100 us = 100000 ns = 140056 cycles                  */
  hw_pll_delay (140056);

  /* Get the value of PLLCTL */
  ctl = DEVICE_REG32_R (pllBase + PLL_REG_CTL);

 /* 2. Check the status of BYPASS bit in SECCTL register,                   *
  *    execute the following steps if                                       *
  *    BYPASS == 1 (if bypass enabled), if BYPASS==0 then Jump to Step 3    */
  secctl = DEVICE_REG32_R (pllBase + PLL_REG_SECCTL);

  if ( (secctl & PLL_REG_SECCTL_FIELD_BYPASS) != 0 ) {
    /* PLL BYPASS is turned on */

	/* 2a. Set the ENSAT Bit */
	/* Usage Note 9: For optimal PLL operation, the ENSAT bit in the PLL control
	 * registers for the Main PLL, DDR3 PLL, and PA PLL should be set to 1.
	 * The PLL initialization sequence in the silicon sets this bit to 0 and
	 * could lead to non-optimal PLL operation. Software can set the bit to the
	 * optimal value of 1 after boot
     * |31...7   |6     |5 4       |3...0      |
     * |Reserved |ENSAT |Reserved  |BWADJ[11:8]|
	 */
    reg = DEVICE_REG32_R (DEVICE_MAIN_PLL_CTL_1);   // Read MAINPLLCTL
    reg = reg | (1 << 6);                           // Set bit 6 (ENSAT)
    DEVICE_REG32_W (DEVICE_MAIN_PLL_CTL_1, reg);    // Write to MAINPLLCTL
    
    /* 2b. Clear the PLLEN bit */
	ctl = ctl & ~(PLL_REG_CTL_FIELD_PLLEN);
	DEVICE_REG32_W (pllBase + PLL_REG_CTL, ctl);

	/* 2c. Clear the PLLENSRC bit */
	ctl = ctl & ~(PLL_REG_CTL_FIELD_PLLENSRC);
	DEVICE_REG32_W (pllBase + PLL_REG_CTL, ctl);

	
	/* 2d. Wait for 4 Ref clocks */
	/* The slowest clock can be at 25MHz, so min:160ns delay */
	hw_pll_delay(225);

	/* 2e. Put the PLL in Bypass mode to perform the power down mode */
	secctl = secctl | PLL_REG_SECCTL_FIELD_BYPASS;
	DEVICE_REG32_W (pllBase + PLL_REG_SECCTL, secctl);

	/* 2f. Advisory 8: Multiple PLLs May Not Lock After Power-on Reset Issue
         * In order to ensure proper PLL startup, the PLL power_down pin needs to be
         * toggled. This is accomplished by toggling the PLLPWRDN bit in the PLLCTL
         * register. This needs to be done before the main PLL initialization
         * sequence
	 */
	ctl = ctl | 0x00000002;
	DEVICE_REG32_W (pllBase + PLL_REG_CTL, ctl);

	/* 2g. Stay in a loop such that the bit is set for 5 µs (minimum) and
	 * then clear the bit.
	 */
	hw_pll_delay (14005); /* waiting 10 us */

	/* 2h. Power up the PLL */
	ctl = ctl & ~(0x00000002);
	DEVICE_REG32_W (pllBase + PLL_REG_CTL, ctl);
	
  }
  else
  {
    /* 3. Enable BYPASS in the PLL controller */
    
    /* 3a. Clear PLLEN bit (bypass enabled in PLL controller mux) */
    ctl = ctl & ~(PLL_REG_CTL_FIELD_PLLEN);
	DEVICE_REG32_W (pllBase + PLL_REG_CTL, ctl);
    
    /* 3b. Clear PLLENSRC bit (enable PLLEN to control PLL controller mux) */
    ctl = ctl & ~(PLL_REG_CTL_FIELD_PLLENSRC);
	DEVICE_REG32_W (pllBase + PLL_REG_CTL, ctl);
    
    /* 3c. Wait for 4 RefClks (to make sure the PLL controller *
     * mux switches properly to bypass)                        *
     * Assuming slowest Ref clock of 25MHz, min: 160 ns delay  */
     hw_pll_delay(225);
  }

	/* Program the necessary multipliers/dividers and BW adjustments
	 * This routine will subtract 1 from the mult value
	 */
    /* 4. Program Multipliers */
	pmult = chipPllExternalMult(pllNum, mult);
	pmult   = pmult & PLL_REG_PLLM_FIELD_MULTm1;
	DEVICE_REG32_W (pllBase + PLL_REG_PLLM, pmult);

	/* 5. set the BWADJ */
	chipPllExternalBwAdj (pllNum, mult);

	/* 6. Set the PLL Divider */
	chipPllSetExternalPrediv(pllNum, prediv - 1);  

	/* 7. set the output divide */
	secctl = BOOT_SET_BITFIELD(secctl, 1 & 0x000f, 22, 19);
	DEVICE_REG32_W (pllBase + PLL_REG_SECCTL, secctl);

    /* 8. Program PLLDIVn */
  
	/* part of 8, wait for the GOSTAT, but don't trap if lock is never read */
	for (i = 0; i < 100; i++)  {
		hw_pll_delay (300);
		status = DEVICE_REG32_R (pllBase + PLL_REG_PLLSTAT);
		if ( (status & PLL_REG_STATUS_FIELD_GOSTAT) == 0 )
			break;
	}

	/* Enable the pll even if the lock failed. Return a warning. */
	if (i == 100)  
		ret = -1;
  
	/* part of 8, Set PLL dividers if needed */
	reg = 0x8000 | (div2 -1);
	DEVICE_REG32_W (pllBase + PLL_REG_DIV2, reg);

	reg = 0x8000 | (div5 -1);
	DEVICE_REG32_W (pllBase + PLL_REG_DIV5, reg);

	reg = 0x8000 | (div8 -1);
	DEVICE_REG32_W (pllBase + PLL_REG_DIV8, reg);

	/* part of 8, Program ALNCTLn registers */
	alnctl = DEVICE_REG32_R (pllBase + PLL_REG_ALNCTL);
	alnctl = alnctl | ((1 << 1) | (1 << 4) | (1 << 7));
	DEVICE_REG32_W (pllBase + PLL_REG_ALNCTL, alnctl);

	/* part of 8, Set GOSET bit in PLLCMD to initiate the GO operation to change the divide *   
	 * values and align the SYSCLKs as programmed                                           */
	reg = DEVICE_REG32_R (pllBase + PLL_REG_CMD);
	reg = reg | 1;
	DEVICE_REG32_W (pllBase + PLL_REG_CMD, reg);

	/* part of 8, wait for the GOSTAT, but don't trap if lock is never read */
	for (i = 0; i < 100; i++)  {
		hw_pll_delay (300);
		status = DEVICE_REG32_R (pllBase + PLL_REG_PLLSTAT);
		if ( (status & PLL_REG_STATUS_FIELD_GOSTAT) == 0 )
			break;
	}

	if (i == 100)  
		ret = -1;
        
    /* 9. Assert PLL Reset */
	ctl = ctl | (PLL_REG_CTL_FIELD_PLLRST);
	DEVICE_REG32_W (pllBase + PLL_REG_CTL, ctl);

	/* 10. Wait for a minimum of 7 us*/
	hw_pll_delay (14006);

	/* 11. Release PLL from Reset */
	ctl = ctl & ~(PLL_REG_CTL_FIELD_PLLRST);
	DEVICE_REG32_W (pllBase + PLL_REG_CTL, ctl);

	/* 12. Wait for PLL Lock time (min 50 us) */
	hw_pll_delay (140056 >> 1);

	/* 13. Clear the secondary controller bypass bit */
	secctl = secctl & ~PLL_REG_SECCTL_FIELD_BYPASS;
	DEVICE_REG32_W (pllBase + PLL_REG_SECCTL, secctl);


	/* 14. Set pllen to 1 to enable pll mode */
	ctl = ctl | PLL_REG_CTL_FIELD_PLLEN;
	DEVICE_REG32_W (pllBase + PLL_REG_CTL, ctl);
    
    /* 15. The PLL and PLL Controller are now initialized in PLL mode - Complete */
  
	return (ret);
} /* hwPllSetPll */


