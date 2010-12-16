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



/**
 *  @file sgmii.c
 *
 *  @brief
 *		The sgmii driver for the ibl
 *
 */

#include "types.h"
#include "ibl.h"
#include "sgmii.h"
#include "target.h"

#define SGMII_ACCESS(port,reg)   *((volatile unsigned int *)(sgmiis[port] + reg))

/**
 *  @brief
 *      Configure the sgmii
 */
int32 hwSgmiiConfig (int32 port, iblSgmii_t *iblSgmii)
{
    unsigned int sgmiis[] = TARGET_SGMII_BASE_ADDRESSES;
    int32 v, i;
    unsigned int status = 0;

    if (port >= TARGET_EMAC_N_PORTS)
        return (-1);


    SGMII_ACCESS(port, TARGET_SGMII_SOFT_RESET) = 1;

    do  {

        v = SGMII_ACCESS(port, TARGET_SGMII_SOFT_RESET);

    } while (v & 1);


    SGMII_ACCESS(port, TARGET_SGMII_TX_CFG) = iblSgmii->txConfig;
    SGMII_ACCESS(port, TARGET_SGMII_RX_CFG) = iblSgmii->rxConfig;

    *(volatile unsigned int *)(0x02C81088) = 1;

    SGMII_ACCESS(port, TARGET_SGMII_AUX_CFG) = iblSgmii->auxConfig;

	for (i = 0; i < 100; i++)  {
		sgmii_delay (15000);
		sgmii_delay (15000);
		sgmii_delay (15000);
		status = SGMII_ACCESS(port, TARGET_SGMII_STATUS);
		if ( (status & 0x10) != 0 )
			break;
	}

     SGMII_ACCESS(port, TARGET_SGMII_MR_ADV_ABILITY) = iblSgmii->adviseAbility;      
     SGMII_ACCESS(port, TARGET_SGMII_CONTROL) = iblSgmii->control;

	while(((SGMII_ACCESS(port, TARGET_SGMII_STATUS)) & 0x00000001) != 0x1);

    return (0);

}

int32 hwSgmiiChecklink (int32 port, iblSgmii_t *iblSgmii)
{

    unsigned int sgmiis[] = TARGET_SGMII_BASE_ADDRESSES;
    int32 v;
    int32 stat;
    int32 timeout = 20000;

    if (port >= TARGET_EMAC_N_PORTS)
        return (-1);

    do // wait for sgmii link
    {                            
        stat = SGMII_ACCESS(port, TARGET_SGMII_STATUS); 		// Read status reg
        stat = stat & 0x10;       	// Extract link status bit and autoneg complete if not forcedlink 
        timeout--;          	// If timeout is reached
     
    } while(stat != 0x10);



    do // wait for sgmii link
    {                            
        stat = SGMII_ACCESS(port, TARGET_SGMII_STATUS); 		// Read status reg
        stat = stat & 0x1;       	// Extract link status bit and autoneg complete if not forcedlink 
        timeout--;          	// If timeout is reached
     
    } while(stat != 0x1);


	return (0);

}
    
void sgmii_delay (uint32 del)
{
    volatile unsigned int i;

    for (i = 0; i < del/8; i++);

}




