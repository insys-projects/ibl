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
    j = j + 1;

} /* hw_pll_delay */


/**********************************************************************************
 * FUNCTION PURPOSE: Enables the pll to the specified multiplier
 **********************************************************************************
 * DESCRIPTION: Sets up the pll
 **********************************************************************************/
SINT16 hwPllSetPll (UINT32 pllNum, UINT32 prediv, UINT32 mult, UINT32 postdiv)
{
  UINT32 ctl;
  UINT32 secctl;
  UINT32 status;
  UINT32 pmult;
  UINT32 pdiv;
  UINT32 pllBase;
  UINT32 i;
  SINT16 ret = 0;


  /* Mutliplier/divider values of 0 are invalid */
  if (prediv == 0)
    prediv = 1;

  if (mult == 0)
    mult = 1;

  if (postdiv == 0)
    postdiv = 1;

  /* Get the base address of the pll */
  pllBase = (UINT32) DEVICE_PLL_BASE(pllNum);

  /* Program pllen=0 (pll bypass), pllrst=1 (reset pll), pllsrc = 0 */
  ctl = DEVICE_REG32_R (pllBase + PLL_REG_CTL);
  ctl = ctl & ~(PLL_REG_CTL_FIELD_PLLEN | PLL_REG_CTL_FIELD_PLLENSRC | PLL_REG_CTL_FIELD_PLLDIS);
  DEVICE_REG32_W (pllBase + PLL_REG_CTL, ctl);


  /* Enable secondary controller pll bypass */
  secctl = DEVICE_REG32_R (pllBase + PLL_REG_SECCTL);
  secctl = secctl | PLL_REG_SECCTL_FIELD_BYPASS;
  DEVICE_REG32_W (pllBase + PLL_REG_SECCTL, secctl);

  /* Reset the PLL */
  ctl = ctl | PLL_REG_CTL_FIELD_PLLRST;
  DEVICE_REG32_W (pllBase + PLL_REG_CTL, ctl);

  /* Enable the pll divider */
  secctl = DEVICE_REG32_R (pllBase + PLL_REG_SECCTL);
  secctl = PLL_REG_SECCTL_SET_POSTDIV(secctl,postdiv-1);
  secctl = PLL_REG_SECCTL_ENABLE_POSTDIV(secctl);
  DEVICE_REG32_W (pllBase + PLL_REG_SECCTL, secctl);


  /* Some PLLs like the rpll used in Nysh have an external (chip register) PLL predivider */
  if (chipPllExternalPrediv(pllNum) == FALSE)  {
    pdiv    = (UINT32) (((prediv-1) & PLL_REG_PREDIV_FIELD_RATIOm1) | PLL_REG_PREDIV_FIELD_ENABLE);
    DEVICE_REG32_W (pllBase + PLL_REG_PREDIV,  pdiv);
  }  else
    chipPllSetExternalPrediv(pllNum, prediv - 1);

  /* The rpll used in Nysh has both external and internal multiplier components. The external
   * is set first because it modifies the internal. The value returned by chipPllExternalMult
   * will be modified to take into account the value programed by the chip regsiters. This
   * mult value input into chipPllExternalMult is the actual desired multiplier value, not
   * the desired value - 1 */
  pmult = chipPllExternalMult(pllNum, mult);
  pmult   = pmult & PLL_REG_PLLM_FIELD_MULTm1;
  DEVICE_REG32_W (pllBase + PLL_REG_PLLM, pmult);

  /* Some PLLs like the rpll used in Nysh require bandwidth adjustment which is controlled
   * through a chip level register. Devices that don't require this simply define
   * this function to an empty statement */
  chipPllExternalBwAdj (pllNum, mult);


  /* Wait a while for the pll to reset */
  hw_pll_delay (2000/7);

  /* set pllrst to 0 to deassert pll reset */
  ctl = ctl & ~(PLL_REG_CTL_FIELD_PLLRST);
  DEVICE_REG32_W (pllBase + PLL_REG_CTL, ctl);


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


