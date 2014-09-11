/************************************************************************************************
 * FILE PURPOSE: EMIF4 peripheral definitions
 ************************************************************************************************
 * FILE NAME: emif4_fmc116v.h
 *
 * DESCRIPTION: Local definitions for the emif4 driver
 *
 ************************************************************************************************/

#ifndef _EMIF4_FM408C_1G_H
#define _EMIF4_FM408C_1G_H

#include "types.h"

unsigned int _level[18] = {
      0x98,
      0x93,
      0x91,
      0x8D,
      0x92,
      0x8D,
      0x86,
      0x83,
      0x00,

      0xC0,
      0xC5,
      0xC8,
      0xCB,
      0xC7,
      0xCB,
      0xD2,
      0xD5,
      0x00
};

unsigned int	T_RP  = 0xA; // T_RP bit field 28:25
unsigned int	T_RCD = 0xA; // T_RCD bit field 24:21
unsigned int	T_WR  = 0xB; // T_WR bit field 20:17
unsigned int	T_RAS = 0x1B; // T_RAS bit field 16:12
unsigned int	T_RC  = 0x26; // T_RC bit field 11:6
unsigned int	T_RRD = 0x5; // T_RRD bit field 5:3
unsigned int	T_WTR = 0x5; // T_WTR bit field 2:0
unsigned int	T_RTP = 0x5; // T_RTP bit field 5:3
unsigned int	T_RFC = 0x7F; // T_RFC bit field 12:4

unsigned int	CL      = 0xE; // CL bit field 13:10
unsigned int	ROWSIZE = 0x6; // ROWSIZE bit field 9:7
unsigned int	RANK    = 0x0;// EBANK bit field 3:3
unsigned int	PSIZE   = 0x3; // PAGESIZE bit field 2:0


#endif // _EMIF4_FM408C_1G_H
