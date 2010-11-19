/*
 *
 * Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/ 
 * 
 * 
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions 
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the   
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/



/*************************************************************************************
 * FILE PURPOSE: Provide PLL control functions
 *************************************************************************************
 * FILE NAME: pll.c
 *
 * DESCRIPTION: Provides functions to control the pll
 *
 * @file pll.c
 *
 * @brief
 *  The pllhi1p8 and pllmd1ph driver
 *
 *************************************************************************************/
#include "types.h"
#include "pllloc.h"
#include "pllapi.h"
#include "target.h"


#define DEVICE_REG32_W(x,y)   *(volatile unsigned int *)(x)=(y)
#define DEVICE_REG32_R(x)    (*(volatile unsigned int *)(x))

/**************************************************************************************
 * FUNCTION PURPOSE: Return the last boot type
 **************************************************************************************
 * DESCRIPTION: Returns the last boot mode type
 **************************************************************************************/
uint16 hwPllResetType (uint32 pllNum)
{
  if ( ((DEVICE_REG32_R (DEVICE_PLL_BASE(pllNum) + PLL_REG_RSTYPE)) & PLL_REG_RSTYPE_FIELD_POWER_ON_RESET) != 0 )
    return (pll_POR_RESET);

  return (pll_WARM_RESET);

} /* hwPllResetType */


/*********************************************************************************
 * FUNCTION PURPOSE: Provide a delay loop
 *********************************************************************************
 * DESCRIPTION: Generates a delay, units of cycles
 *********************************************************************************/
void hw_pll_delay (uint32 del)
{
  uint32 i;
  volatile uint32 j;

  for (i = j = 0; i < del; i++)
    j = j + 1;

} /* hw_pll_delay */


/**********************************************************************************
 * FUNCTION PURPOSE: Enables the pll to the specified multiplier
 **********************************************************************************
 * DESCRIPTION: Sets up the pll
 **********************************************************************************/
int16 hwPllSetPll (uint32 pllNum, uint32 prediv, uint32 mult, uint32 postdiv)
{
  uint32 ctl;
  uint32 status;
  uint32 pmult;
  uint32 pdiv;
  uint32 pllBase;
  uint32 i;

  /* Mutliplier/divider values of 0 are invalid */
  if (prediv == 0)
    prediv = 1;

  if (mult == 0)
    mult = 1;

  if (postdiv == 0)
    postdiv = 1;

  /* Get the base address of the pll */
  pllBase = (uint32) DEVICE_PLL_BASE(pllNum);

  /* Set pllensrc = 0 so that the register bit pllen can control the pllen mux */
  ctl = DEVICE_REG32_R (pllBase + PLL_REG_CTL);
  ctl = ctl & ~(PLL_REG_CTL_FIELD_PLLENSRC);
  DEVICE_REG32_W (pllBase + PLL_REG_CTL, ctl);


  /* Program pllen=0 (pll bypass), pllrst=1 (reset pll) */
  ctl = ctl & ~(PLL_REG_CTL_FIELD_PLLEN);
  ctl = ctl | PLL_REG_CTL_FIELD_PLLRST;
  DEVICE_REG32_W (pllBase + PLL_REG_CTL, ctl);


  /* Program prediv, pllm, postdiv */
  pmult   = (uint32) ((mult-1)   & PLL_REG_PLLM_FIELD_MULTm1);
  pdiv    = (uint32) (((prediv-1) & PLL_REG_PREDIV_FIELD_RATIOm1) | PLL_REG_PREDIV_FIELD_ENABLE);

  DEVICE_REG32_W (pllBase + PLL_REG_PLLM,    pmult);
  DEVICE_REG32_W (pllBase + PLL_REG_PREDIV,  pdiv);

  if (postdiv != 1)  {
    postdiv = (uint16) (((postdiv-1) & PLL_REG_POSTDIV_FIELD_RATIO) | PLL_REG_POSTDIV_FIELD_ENABLE);
    DEVICE_REG32_W (pllBase + PLL_REG_POSTDIV, postdiv);
  }  else  {
    DEVICE_REG32_W (pllBase + PLL_REG_POSTDIV, PLL_REG_POSTDIV_VALUE_DISABLED);
  }

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

  /* Do not enable the pll if the lock failed. Return a warning. */
  if (i == 100)  
    return (-1);


  /* Set pllen to 1 to enable pll mode */
  ctl = ctl | PLL_REG_CTL_FIELD_PLLEN;
  DEVICE_REG32_W (pllBase + PLL_REG_CTL, ctl);

  return (0);


} /* hwPllSetPll */

 
/**********************************************************************************
 * FUNCTION PURPOSE: Disable the pll
 **********************************************************************************
 * DESCRIPTION: The pll is put into bypass. Returns 0 on success. 
 **********************************************************************************/
int16 hwPllDisable (uint32 pllNum)
{
  uint32 ctl;
  uint32 pllBase;

  /* Get the base address of the pll */
  pllBase = (uint32)DEVICE_PLL_BASE(pllNum);

  /* Set pllensrc = 0 so that the register bit pllen can control the pllen mux */
  ctl = DEVICE_REG32_R (pllBase + PLL_REG_CTL);
  ctl = ctl & ~(PLL_REG_CTL_FIELD_PLLENSRC);
  DEVICE_REG32_W (pllBase + PLL_REG_CTL, ctl);


  /* Program pllen=0 (pll bypass), pllrst=1 (reset pll) */
  ctl = ctl & ~(PLL_REG_CTL_FIELD_PLLEN);
  ctl = ctl | PLL_REG_CTL_FIELD_PLLRST;
  DEVICE_REG32_W (pllBase + PLL_REG_CTL, ctl);

  return (0);


} /* hwPllDisable */

/***********************************************************************************
 * FUNCTION PURPOSE: Enable the PLL
 ***********************************************************************************
 * DESCRIPTION: The PLL is enabled with no changes to multipliers or dividers
 ***********************************************************************************/
int16 hwPllEnable (uint32 pllNum)
{
  uint32 ctl;
  uint32 pllBase;
  uint32 status;
  uint32 i;

  /* Get the base address of the pll */
  pllBase = (uint32)DEVICE_PLL_BASE(pllNum);

  /* Set pllensrc = 0 so that the register bit pllen can control the pllen mux */
  ctl = DEVICE_REG32_R (pllBase + PLL_REG_CTL);
  ctl = ctl & ~(PLL_REG_CTL_FIELD_PLLENSRC);
  DEVICE_REG32_W (pllBase + PLL_REG_CTL, ctl);

  /* Program pllen=0 (pll bypass), pllrst=1 (reset pll) */
  ctl = ctl & ~(PLL_REG_CTL_FIELD_PLLEN);
  ctl = ctl | PLL_REG_CTL_FIELD_PLLRST;
  DEVICE_REG32_W (pllBase + PLL_REG_CTL, ctl);

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

  /* Return a warning if the lock failed */
  if (i == 100)
    return (-1);

  /* Set pllen to 1 to enable pll mode */
  ctl = ctl | PLL_REG_CTL_FIELD_PLLEN;
  DEVICE_REG32_W (pllBase + PLL_REG_CTL, ctl);

  return (0);

} /* hwPllEnable */





   




