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



/************************************************************************************
 * FILE PURPOSE: The emif 3.1 driver
 ************************************************************************************
 * FILE NAME: emif31.c
 *
 * DESCRIPTION: The emif controller is setup
 *
 * @file emif31.c
 *
 * @brief
 *		The emif controller is setup
 *
 **************************************************************************************/
#include "types.h"
#include "ibl.h"
#include "emif31api.h"
#include "emif31loc.h"
#include "target.h"

#define DEVICE_REG32_W(x,y)   *(volatile unsigned int *)(x)=(y)
#define DEVICE_REG32_R(x)    (*(volatile unsigned int *)(x))

void hwEmif3p1Enable (iblEmif3p1_t *cfg)
{
    uint32 v;

    v = cfg->sdcfg | (1 << 15) | (1 << 23);  /* Timing unlock (bit 15), boot unlock (bit 23) set */
    DEVICE_REG32_W (DEVICE_DDR_BASE + DDR_REG_SDCFG, v);

    DEVICE_REG32_W (DEVICE_DDR_BASE + DDR_REG_SDRFC,  cfg->sdrfc);
    DEVICE_REG32_W (DEVICE_DDR_BASE + DDR_REG_SDTIM1, cfg->sdtim1);
    DEVICE_REG32_W (DEVICE_DDR_BASE + DDR_REG_SDTIM2, cfg->sdtim2);
    DEVICE_REG32_W (DEVICE_DDR_BASE + DDR_REG_DMCCTL, cfg->dmcctl);

    v = v & (~(1 << 23));  /* Clear boot lock */
    DEVICE_REG32_W (DEVICE_DDR_BASE + DDR_REG_SDCFG, v);


    v = v & (~(1 << 15));  /* Clear timing lock */
    DEVICE_REG32_W (DEVICE_DDR_BASE + DDR_REG_SDCFG, v);
    
}
    





