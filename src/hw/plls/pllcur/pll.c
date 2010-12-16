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
#include "pllapi.h"
#include "target.h"

#define PLL1_BASE           0x029A0000
#define PLL1_PLLCTL         (PLL1_BASE + 0x100)   // PLL1 Control
#define PLL1_PLLM           (PLL1_BASE + 0x110)   // PLL1 Multiplier
#define PLL1_PREDIV         (PLL1_BASE + 0x114)   // PREDIV divider
#define PLL1_POSTDIV        (PLL1_BASE + 0x128)   // POSTDIV divider
#define PLL1_CMD            (PLL1_BASE + 0x138)   // CMD control
#define PLL1_STAT           (PLL1_BASE + 0x13C)   // STAT control
#define PLL1_DIV3           (PLL1_BASE + 0x120)   // DIV3 divider
#define PLL1_DIV6           (PLL1_BASE + 0x168)   // DIV6 divider
#define PLL1_DIV7           (PLL1_BASE + 0x16C)   // DIV7 divider
#define PLL1_DIV8           (PLL1_BASE + 0x170)   // DIV8 divider

#define DEVICE_REG32_W(x,y)   *(volatile unsigned int *)(x)=(y)
#define DEVICE_REG32_R(x)    (*(volatile unsigned int *)(x))

/**************************************************************************************
 * FUNCTION PURPOSE: Return the last boot type
 **************************************************************************************
 * DESCRIPTION: Returns the last boot mode type
 **************************************************************************************/

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
int16 hwPllSetPll (uint32 pllNum1, uint32 prediv1, uint32 mult1, uint32 postdiv1)
{
    volatile uint32* pll_ctl       = (volatile uint32* )PLL1_PLLCTL;
    volatile uint32* pll_mult      = (volatile uint32* )PLL1_PLLM;
    uint32 i;
    uint32 rbmult;
    uint32 pll_multiplier = 20;
    
    uint32 div3=3, div6=10, div7=10, div8=10, prediv=1, postdiv=1;
    
    volatile uint32* pll_div3      = (volatile uint32* )PLL1_DIV3;
    volatile uint32* pll_div6      = (volatile uint32* )PLL1_DIV6;
    volatile uint32* pll_div7      = (volatile uint32* )PLL1_DIV7;
    volatile uint32* pll_div8      = (volatile uint32* )PLL1_DIV8;
    volatile uint32* pll_cmd       = (volatile uint32* )PLL1_CMD;
    volatile uint32* pll_stat      = (volatile uint32* )PLL1_STAT;
    volatile uint32* pll_prediv    = (volatile uint32* )PLL1_PREDIV;
    volatile uint32* pll_postdiv    = (volatile uint32* )PLL1_POSTDIV;

    uint32 dsp_freq;

    rbmult = pll_multiplier;
   
    if (pll_multiplier>=8 && pll_multiplier<=32)
    {
        //  Step 1: Set PLL to BYPASS mode
        *pll_ctl &= 0xFFFFFFDF;             // Set PLL to Bypass mode
        *pll_ctl &= 0xFFFFFFFE;

        // Wait Bypass mode switch
        // Bypass sw time is 4 clkin cycles = (1/50000000) * 4 ~0.12 usec
        // The following delay is much more than necessary...
        hw_pll_delay(1500);

        //  Step 2: Configure and stabilize PLL
        *pll_ctl |= 0x8;                    // Reset PLL

        // !!! M.T. Verify this section
        // Verify if pll is in power down
        if ((*pll_ctl & 0x00000002) !=0 )
        {
            //*pll_ctl |= 0x00000010;         // Disable PLL
            *pll_ctl &= 0xFFFFFFFD;         // Power up PLL
            //*pll_ctl &= 0xFFFFFFEF;         // Enable PLL

            // Wait PLL Stabilization time
            // that is 150 usec
            // The following delay is much more than necessary and provide stable PLL...
            hw_pll_delay(5000);
        }
        else
        {
            //*pll_ctl &= 0xFFFFFFEF;         // Enable PLL
        }

        // Step 3: Set PLL multiplier register (minus 1 desired mult value)
        *pll_mult = (pll_multiplier-1);       // Set PLL multipler
//        *pll_prediv = (prediv-1) | 0x8000;    // Set PLL prediv divider
        *pll_postdiv = (postdiv-1) | 0x8000;    // Set PLL postdiv divider

        // Wait for GOSTAT to be cleared so no go operation is in progress
        hw_pll_delay(1000);
	hw_pll_delay(1000);
	hw_pll_delay(1000);

        while( (*pll_stat & 0x01) !=0 );

        // Check if we got timeout error while waiting

         hw_pll_delay(1000);
	hw_pll_delay(1000);
	hw_pll_delay(1000);

        // Step 4. Set PLL dividers if needed
        *pll_div3 = (0x8000) | (div3 - 1);
        *pll_div6 = (0x8000) | (div6 - 1);
        *pll_div7 = (0x8000) | (div7 - 1);
        *pll_div8 = (0x8000) | (div8 - 1);
        
        // Gives the GO cmd
        *pll_cmd |= 0x00000001;

        // Wait for phase alignment
        hw_pll_delay(1000);
	hw_pll_delay(1000);
	hw_pll_delay(1000);

        while( (*pll_stat & 0x01) !=0 );

	hw_pll_delay(1000);
	hw_pll_delay(1000);
	hw_pll_delay(1000);


        // Step 5: Wait for PLL to lock

        // Wait for PLL to Reset
        // !!! M.T.
        // Reset time =128C (For 50 MHz, C=20.000) ~2560 nsec = 2.56 usec
        hw_pll_delay(1000);
	hw_pll_delay(1000);
        
        *pll_ctl &= 0xFFFFFFF7;             // Release PLL from Reset

        // Wait for PLL to LOCK
        // !!! M.T.
        // Lock time =2000C (For 50 MHz, C=20.000) ~40000 nsec = 40.0 usec
        hw_pll_delay(1000);
	hw_pll_delay(1000);
	hw_pll_delay(1000);
	hw_pll_delay(1000);

        *pll_ctl |= 0x00000001;             // Set PLL to PLL mode

#if 0
        // Read back pll dividers and multipliers for validation
        div3 = (*pll_div3 & 0x1f) +1;
        div6 = (*pll_div6 & 0x1f) +1;
        div7 = (*pll_div7 & 0x1f) +1;
        div8 = (*pll_div8 & 0x1f) +1;
        prediv = (*pll_prediv & 0x1f) +1;
        postdiv = (*pll_postdiv & 0x1f) +1;
        rbmult = (*pll_mult & 0x1f) +1;

        dsp_freq = (50 * rbmult)/prediv/postdiv;
#endif
	return (0);

    }
    else
    {
      return (0);

    }

} /* hwPllSetPll */

 






   




