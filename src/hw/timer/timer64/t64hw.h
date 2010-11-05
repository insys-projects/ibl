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



/***********************************************************************
 * FILE PURPOSE: Timer 64 local definitions
 ***********************************************************************
 * FILE NAME: t64hw.h
 *
 *  @file t64h2.h
 *
 *  @brief
 *	  This file contains local definitions for the timer64 block
 *
 ***********************************************************************/

#ifndef T64HW_H
#define T64HW_H

#include "types.h"
#include "target.h"


/* Local registers */
#define treg(x) *((volatile unsigned int *)(TIMER0_BASE + (x)))

#define t64_tcr    treg(0x20)
#define t64_tgcr   treg(0x24)
#define t64_tim12  treg(0x10)
#define t64_tim34  treg(0x14)
#define t64_prd12  treg(0x18)
#define t64_prd34  treg(0x1c)


#define TIMER64_TCR_ONE_SHOT      0x40
#define TIMER64_TCR_DISABLE       0x00

#define TIMER64_TGCR_64           0x3
#define TIMER64_TGCR_DISABLE      0x0




#endif /* T64HW_H */

