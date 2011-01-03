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



#ifndef _PLLAPI_H
#define _PLLAPI_H
/*************************************************************************************
 * FILE PURPOSE: Define the public pll api
 *************************************************************************************
 * FILE NAME: pllapi.h
 *
 * DESCRIPTION: Defines the public api for the pll driver
 *
 * @file  pllapi.h
 *
 * @brief
 *    This file defines the API used by all PLL drivers
 *
 *************************************************************************************/
#include "types.h"

/* Prototypes */
uint16 hwPllResetType (uint32 pllNum);
int16 hwPllSetPll (uint32 pllNum, uint32 prediv, uint32 mult, uint32 postdiv);
int16 hwPllDisable (uint32 pllNum);
int16 hwPllEnable (uint32 pllNum);

SINT16 hwPllSetCfgPll  (UINT32 base, UINT32 prediv, UINT32 mult, UINT32 postdiv, UINT32 chipFreqMhz, UINT32 pllFreqMhz);
SINT16 hwPllSetCfg2Pll (UINT32 base, UINT32 prediv, UINT32 mult, UINT32 postdiv, UINT32 chipFreqMhz, UINT32 pllFreqMhz);

/**
 * @def pll_POR_RESET
 */
#define pll_POR_RESET   500     /**< last reset was a power on reset */

/**
 * @def pll_WARM_RESET
 */
#define pll_WARM_RESET  501     /**< last reset was a warm reset */



#endif /* _PLLAPI_H */


