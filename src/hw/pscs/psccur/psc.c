/***********************************************************************************
 * FILE PURPOSE: Driver for the PSC module
 ***********************************************************************************
 * FILE NAME: psc.c
 *
 * DESCRIPTION: The boot loader PSC driver
 *
 * Copyright (C) 2006, Texas Instruments, Inc.
 * @file psc.c
 * 
 * @brief
 *   The PSC 2.0.x driver
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
#include "types.h"
#include "pscapi.h"
#include "pscloc.h"
#include "target.h"


#define DEVICE_REG32_W(x,y)   *(volatile unsigned int *)(x)=(y)
#define DEVICE_REG32_R(x)    (*(volatile unsigned int *)(x))

/***********************************************************************************
 * FUNCTION PURPOSE: Wait!
 ***********************************************************************************
 * DESCRIPTION: This function doesn't do didly. It loops around for a bit
 *              to give transitions some time to occur.
 *              This function always returns 1.
 ***********************************************************************************/
int16 pscDelay (void)
{
  volatile uint32 i;
  for (i = 0; i < 10000; i++);

  return (1);

} /* pscDelay */


/***********************************************************************************
 * FUNCTION PURPOSE: Wait for end of transitional state
 ***********************************************************************************
 * DESCRIPTION: Polls pstat for the selected domain and waits for transitions
 *              to be complete. 
 *
 *              Since this is boot loader code it is *ASSUMED* that interrupts
 *              are disabled and no other core is mucking around with the psc
 *              at the same time.
 *
 *              Returns 0 when the domain is free. Returns -1 if a timeout
 *              occurred waiting for the completion.
 ***********************************************************************************/
int16 pscWait (uint32 domainNum)
{
  uint32 retry;
  uint32 ptstat;

  /* Do nothing if the power domain is in transition. This should never
   * happen since the boot code is the only software accesses psc. 
   * It's still remotely possible that the hardware state machines initiate transitions. 
   * Don't trap if the domain (or a module in this domain) is 
   * stuck in transition.  */
  retry = 0;

  do  {

    ptstat = DEVICE_REG32_R (DEVICE_PSC_BASE + PSC_REG_PSTAT);
    ptstat = ptstat & (1 << domainNum);

  } while ((ptstat != 0) &&  ((retry += pscDelay ()) < PSC_PTSTAT_TIMEOUT_LIMIT));

  if (retry >= PSC_PTSTAT_TIMEOUT_LIMIT)
    return (-1);

  return (0);

} /* pscWait */


/***********************************************************************************
 * FUNCTION PURPOSE: Return the PSC state
 ***********************************************************************************
 * DESCRIPTION: Checks the power state of the module
 ***********************************************************************************/
BOOL pscModuleIsEnabled (uint32 modNum)
{
#if 0
      	uint32 domainNum;
  uint32 mdstat;

  /* Get the power domain associated with the module number */
  domainNum = DEVICE_REG32_R (DEVICE_PSC_BASE + PSC_REG_MDCFG(modNum));
  domainNum = PSC_REG_MDCFG_GET_PD (domainNum);

  /* Wait for the status of the domain/module to be non-transitional,
   * but don't trap if stuck in a transitional state. */ 
  pscWait (domainNum);

  mdstat = DEVICE_REG32_R (DEVICE_PSC_BASE + PSC_REG_MDSTAT(modNum));
  mdstat = PSC_REG_MDSTAT_GET_STATUS (mdstat);

  if ((mdstat == PSC_REG_VAL_MDSTAT_STATE_ON) || 
      (mdstat == PSC_REG_VAL_MDSTAT_STATE_ENABLE_IN_PROG))
      return (TRUE);

  else  

    return (FALSE);
#endif

  return (TRUE);
} /* pscModuleIsEnabled */




/***********************************************************************************
 * FUNCTION PURPOSE: Power up/down a module
 ***********************************************************************************
 * DESCRIPTION: Powers up/down the requested module and the associated power domain
 *              if required. No action is taken it the module is already 
 *              powered up/down.
 *
 *              This only controls modules. The domain in which the module 
 *              resides will be left in the power on state. Multiple modules
 *              can exist in a power domain, so powering down the domain based
 *              on a single module is not done.
 *
 *              Returns 0 on success, -1 if the module can't be powered up, or
 *              if there is a timeout waiting for the transition.
 ***********************************************************************************/
int16 pscSetState (uint32 modNum, uint32 state)
{
#if 0
  uint32 domainNum;
  uint32 pdctl;
  uint32 mdctl;
  uint32 ptcmd;
  uint32 resetIso;
  uint32 v;

  /* Get the power domain associated with the module number, and reset
   * isolation functionality */
  v         = DEVICE_REG32_R (DEVICE_PSC_BASE + PSC_REG_MDCFG(modNum));
  domainNum = PSC_REG_MDCFG_GET_PD (v);
  resetIso  = PSC_REG_MDCFG_GET_RESET_ISO(v);


  /* Wait for the status of the domain/module to be non-transitional */ 
  if (pscWait (domainNum) != 0)
    return (-1);


  /* Perform configuration even if the current status matches the existing state */


  /* Set the next state of the power domain to on. It's OK if the domain
   * is always on. This code will not ever power down a domain, so no change
   * is made if the new state is power down. */
  if (state == PSC_REG_VAL_MDCTL_NEXT_ON)  {
      pdctl = DEVICE_REG32_R (DEVICE_PSC_BASE + PSC_REG_PDCTL(domainNum));
      pdctl = PSC_REG_PDCTL_SET_NEXT(pdctl, PSC_REG_VAL_PDCTL_NEXT_ON);
      DEVICE_REG32_W (DEVICE_PSC_BASE + PSC_REG_PDCTL(domainNum), pdctl);
  }


  /* Set the next state for the module to enabled/disabled */
  mdctl = DEVICE_REG32_R (DEVICE_PSC_BASE + PSC_REG_MDCTL(modNum));
  mdctl = PSC_REG_MDCTL_SET_NEXT (mdctl, state);
  mdctl = PSC_REG_MDCTL_SET_RESET_ISO (mdctl, resetIso);
  DEVICE_REG32_W (DEVICE_PSC_BASE + PSC_REG_MDCTL(modNum), mdctl);

  /* Trigger the enable */
  ptcmd = DEVICE_REG32_R (DEVICE_PSC_BASE + PSC_REG_PTCMD);
  ptcmd |= (uint32)(1<<domainNum);
  DEVICE_REG32_W (DEVICE_PSC_BASE + PSC_REG_PTCMD, ptcmd);


  /* Wait on the complete */
  return (pscWait (domainNum));
#endif
return 0;
} /* pscSetState*/


/***********************************************************************************
 * FUNCTION PURPOSE: Power up a module
 ***********************************************************************************
 * DESCRIPTION: Powers up the requested module and the associated power domain
 *              if required. No action is taken it the module is already 
 *              powered up.
 *
 *              Returns 0 on success, -1 if the module can't be powered up, or
 *              if there is a timeout waiting for the transition.
 ***********************************************************************************/
int16 pscEnableModule (uint32 modNum)
{

//  return (pscSetState (modNum, PSC_REG_VAL_MDCTL_NEXT_ON));
 return 0;
} /* pscEnableModule */

/************************************************************************************
 * FUNCTION PURPOSE: Power down a module
 ************************************************************************************
 * DESCRIPTION: Powers down the requested module.
 *  
 *              Returns 0 on success, -1 on failure or timeout.
 ************************************************************************************/
int16 pscDisableModule (uint32 modNum)
{
#if 0
      	uint32 mdctl;

  /* Set the bit to apply reset */
  mdctl = DEVICE_REG32_R (DEVICE_PSC_BASE + PSC_REG_MDCTL(modNum));
  mdctl = PSC_REG_MDCTL_SET_LRSTZ(mdctl,0);
  DEVICE_REG32_W (DEVICE_PSC_BASE + PSC_REG_MDCTL(modNum), mdctl);

  return (pscSetState (modNum, PSC_REG_VAL_MDCTL_NEXT_SWRSTDISABLE));
#endif
  return 0;
} /* pscDisableModule */


/************************************************************************************
 * FUNCTION PURPOSE: Set the reset isolation bit in mdctl
 ************************************************************************************
 * DESCRIPTION: The reset isolation enable bit is set. The state of the module
 *              is not changed. Returns 0 if the module config showed that
 *              reset isolation is supported. Returns 1 otherwise. This is not
 *              an error, but setting the bit in mdctl has no effect.
 ************************************************************************************/
int16 pscSetResetIso (uint32 modNum)
{
#if 0
      	uint32 v;
  uint32 mdctl;

  /* Set the reset isolation bit */
  mdctl = DEVICE_REG32_R (DEVICE_PSC_BASE + PSC_REG_MDCTL(modNum));
  mdctl = PSC_REG_MDCTL_SET_RESET_ISO (mdctl, 1);
  DEVICE_REG32_W (DEVICE_PSC_BASE + PSC_REG_MDCTL(modNum), mdctl);
  
  v = DEVICE_REG32_R (DEVICE_PSC_BASE + PSC_REG_MDCFG(modNum));
  if (PSC_REG_MDCFG_GET_RESET_ISO(v) == 1)
    return (0);

  return (1);
#endif

  return (0);
} /* pscSetResetIso */

/*************************************************************************************
 * FUNCTION PURPOSE: Disable a power domain
 *************************************************************************************
 * DESCRIPTION: The power domain is disabled
 *************************************************************************************/
int16 pscDisableDomain (uint32 domainNum)
{
#if 0	
  uint32 pdctl;
  uint32 ptcmd;

  pdctl = DEVICE_REG32_R (DEVICE_PSC_BASE + PSC_REG_PDCTL(domainNum));
  pdctl = PSC_REG_PDCTL_SET_NEXT (pdctl, PSC_REG_VAL_PDCTL_NEXT_OFF);
  pdctl = PSC_REG_PDCTL_SET_PDMODE (pdctl, PSC_REG_VAL_PDCTL_PDMODE_SLEEP);
  DEVICE_REG32_W (DEVICE_PSC_BASE + PSC_REG_PDCTL(domainNum), pdctl);

  ptcmd = DEVICE_REG32_R (DEVICE_PSC_BASE + PSC_REG_PTCMD);
  ptcmd |= (uint32)(1<<domainNum);
  DEVICE_REG32_W (DEVICE_PSC_BASE + PSC_REG_PTCMD, ptcmd);
#endif
  return (0);

} /* pscDisableDomain */


/*************************************************************************************
 * FUNCTION PURPOSE: Power down a domain
 *************************************************************************************
 * DESCRIPTION: The specified power domain is set to the disabled state
 *************************************************************************************/
void pscDisableModAndDomain (uint32 modNum, uint32 domainNum)
{
#if 0
   uint32 pdctl;
  uint32 ptcmd;
  uint32 mdctl;


  mdctl = DEVICE_REG32_R (DEVICE_PSC_BASE + PSC_REG_MDCTL(modNum));
  mdctl = PSC_REG_MDCTL_SET_NEXT(mdctl, PSC_REG_VAL_MDCTL_NEXT_SWRSTDISABLE);
  mdctl = PSC_REG_MDCTL_SET_LRSTZ(mdctl,0);
  DEVICE_REG32_W (DEVICE_PSC_BASE + PSC_REG_MDCTL(modNum), mdctl);


  pdctl = DEVICE_REG32_R (DEVICE_PSC_BASE + PSC_REG_PDCTL(domainNum));
  pdctl = PSC_REG_PDCTL_SET_NEXT (pdctl, PSC_REG_VAL_PDCTL_NEXT_OFF);
  pdctl = PSC_REG_PDCTL_SET_PDMODE (pdctl, PSC_REG_VAL_PDCTL_PDMODE_SLEEP);

  DEVICE_REG32_W (DEVICE_PSC_BASE + PSC_REG_PDCTL(domainNum), pdctl);

  ptcmd = DEVICE_REG32_R (DEVICE_PSC_BASE + PSC_REG_PTCMD);
  ptcmd |= (uint32)(1<<domainNum);
  DEVICE_REG32_W (DEVICE_PSC_BASE + PSC_REG_PTCMD, ptcmd);


  /* Wait on the complete */
  pscWait (domainNum);
#endif

//  return (0);
}


