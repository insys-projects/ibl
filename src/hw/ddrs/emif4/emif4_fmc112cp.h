/************************************************************************************************
 * FILE PURPOSE: EMIF4 peripheral definitions
 ************************************************************************************************
 * FILE NAME: emif4_fmc112cp.h
 *
 * DESCRIPTION: Local definitions for the emif4 driver
 *
 ************************************************************************************************/

#ifndef _EMIF4_FMC112CP_H
#define _EMIF4_FMC112CP_H

#include "types.h"

#if defined (CPU0)
unsigned int _level[18] = {
      0xE8,
      0xD7,
      0xB6,
      0xA3,
      0xE2,
      0xC4,
      0xC1,
      0x88,
      0x00,

      0x1C6,
      0x1AC,
      0x196,
      0x180,
      0x1CB,
      0x1BF,
      0x18B,
      0x19B,
      0x00
};

#elif defined (CPU1)

unsigned int _level[18] = {
      0xDC,
      0xCD,
      0xAE,
      0x9E,
      0xEB,
      0xD1,
      0xBA,
      0xA3,
      0x00,

      0x1F4,
      0x1D9,
      0x1C1,
      0x1A7,
      0x1E4,
      0x1D5,
      0x1B5,
      0x1A2,
      0x00
};
#else
#error  "You need specify INSYS_CPU environment variable to select board configuration!"
#endif

unsigned int	T_RP  = 0xA; // T_RP bit field 28:25
unsigned int	T_RCD = 0xA; // T_RCD bit field 24:21
unsigned int	T_WR  = 0xB; // T_WR bit field 20:17
unsigned int	T_RAS = 0x1B; // T_RAS bit field 16:12
unsigned int	T_RC  = 0x26; // T_RC bit field 11:6
unsigned int	T_RRD = 0x3; // T_RRD bit field 5:3
unsigned int	T_WTR = 0x5; // T_WTR bit field 2:0
unsigned int	T_RTP = 0x5; // T_RTP bit field 5:3
unsigned int	T_RFC = 0x7F; // T_RFC bit field 12:4

unsigned int	CL      = 0xE; // CL bit field 13:10
unsigned int	ROWSIZE = 0x6; // ROWSIZE bit field 9:7
unsigned int	RANK    = 0x0;// EBANK bit field 3:3
unsigned int	PSIZE   = 0x2; // PAGESIZE bit field 2:0


#endif // _EMIF4_FMC112CP_H
