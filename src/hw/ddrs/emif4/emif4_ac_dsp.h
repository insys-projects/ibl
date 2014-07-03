/************************************************************************************************
 * FILE PURPOSE: EMIF4 peripheral definitions
 ************************************************************************************************
 * FILE NAME: emif4_ac_dsp.h
 *
 * DESCRIPTION: Local definitions for the emif4 driver
 *
 ************************************************************************************************/

#ifndef _EMIF4_AC_DSP_H
#define _EMIF4_AC_DSP_H

#include "types.h"

unsigned int _level[18] = {
      0x95,
      0x8D,
      0x9A,
      0x8F,
      0x97,
      0x91,
      0x92,
      0x8C,
      0x00,

      0xE5,
      0xED,
      0xE0,
      0xEB,
      0xE3,
      0xEA,
      0xE8,
      0xEF,
      0x00
};

unsigned int	T_RP  = 0xA; // T_RP bit field 28:25
unsigned int	T_RCD = 0xA; // T_RCD bit field 24:21
unsigned int	T_WR  = 0xB; // T_WR bit field 20:17
unsigned int	T_RAS = 0x1B; // T_RAS bit field 16:12
unsigned int	T_RC  = 0x26; // T_RC bit field 11:6
unsigned int	T_RRD = 0x3; // T_RRD bit field 5:3
unsigned int	T_WTR = 0x5; // T_WTR bit field 2:0
unsigned int	T_RTP = 0x5; // T_RTP bit field 5:3
unsigned int	T_RFC = 0xCF; // T_RFC bit field 12:4

unsigned int	CL      = 0xE; // CL bit field 13:10
unsigned int	ROWSIZE = 0x6; // ROWSIZE bit field 9:7
unsigned int	RANK    = 0x0;// EBANK bit field 3:3
unsigned int	PSIZE   = 0x2; // PAGESIZE bit field 2:0


#endif // _EMIF4_AC_DSP_H
