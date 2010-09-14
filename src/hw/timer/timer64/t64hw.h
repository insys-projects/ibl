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

