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


#ifdef TARGET_SGMII_TYPE_2
/**
 *  @brief
 *      Configure the sgmii on devices using the type 2 initialization sequence
 */
int32 hwSgmiiConfig (int32 port, iblSgmii_t *iblSgmii)
{
    unsigned int sgmiis[] = TARGET_SGMII_BASE_ADDRESSES;


    SGMII_ACCESS(port, TARGET_SGMII_CONTROL) = 0;  /* Disable negotiation */

#ifdef TARGET_SGMII_EXTERNAL_SERDES

    targetSgmiiSerdesConfig (port, (void *)iblSgmii);

#else

    /* Serdes configuration */
    SGMII_ACCESS(port, TARGET_SGMII_TX_CFG) = iblSgmii->txConfig;
    SGMII_ACCESS(port, TARGET_SGMII_RX_CFG) = iblSgmii->rxConfig;
    SGMII_ACCESS(port, TARGET_SGMII_AUX_CFG) = iblSgmii->auxConfig;

#endif

    SGMII_ACCESS(port, TARGET_SGMII_MR_ADV_ABILITY) = iblSgmii->adviseAbility;
    SGMII_ACCESS(port, TARGET_SGMII_CONTROL) = iblSgmii->control;

    return (0);

}


#else
/**
 *  @brief
 *      Configure the sgmii
 */
int32 hwSgmiiConfig (int32 port, iblSgmii_t *iblSgmii)
{
    unsigned int sgmiis[] = TARGET_SGMII_BASE_ADDRESSES;
    int32 v;

    if (port >= TARGET_EMAC_N_PORTS)
        return (-1);


    SGMII_ACCESS(port, TARGET_SGMII_SOFT_RESET) = 1;

    do  {

        v = SGMII_ACCESS(port, TARGET_SGMII_SOFT_RESET);

    } while (v & 1);

    /* Bit 1 holds tx/rx in reset. Used during loopback state change in 
     * the control register */
    SGMII_ACCESS(port, TARGET_SGMII_SOFT_RESET) = 2;


    SGMII_ACCESS(port, TARGET_SGMII_CONTROL) = iblSgmii->control;
    SGMII_ACCESS(port, TARGET_SGMII_SOFT_RESET) = 0;

    SGMII_ACCESS(port, TARGET_SGMII_MR_ADV_ABILITY) = iblSgmii->adviseAbility;

#ifdef TARGET_SGMII_EXTERNAL_SERDES

    targetSgmiiSerdesConfig (port, (void *)iblSgmii);

#else

    /* Serdes configuration */
    SGMII_ACCESS(port, TARGET_SGMII_TX_CFG) = iblSgmii->txConfig;
    SGMII_ACCESS(port, TARGET_SGMII_RX_CFG) = iblSgmii->rxConfig;
    SGMII_ACCESS(port, TARGET_SGMII_AUX_CFG) = iblSgmii->auxConfig;

#endif

    return (0);

}

#endif

    





