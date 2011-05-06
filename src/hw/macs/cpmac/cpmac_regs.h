/**
 *   @file  cpmac_regs.h
 *
 *   @brief   
 *      This file contains the Register Desciptions for EMAC
 *
 *  \par
 *  NOTE:
 *      (C) Copyright 2008, Texas Instruments, Inc.
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
 *  \par
*/

#ifndef __CPMAC_REGS_H__
#define __CPMAC_REGS_H__

/**
 * @brief 
 *  The structure describes the CPMAC Register Overlay.
 *
 * @details
 *  This is the CPMAC Register overlay data structure which is used
 *  by the CPMAC Driver.
 */
typedef struct CPMAC_REGS
{
    volatile Uint32 TXIDVER;
    volatile Uint32 TXCONTROL;
    volatile Uint32 TXTEARDOWN;
    volatile Uint8  RSVD0[4];
    volatile Uint32 RXIDVER;
    volatile Uint32 RXCONTROL;
    volatile Uint32 RXTEARDOWN;
    volatile Uint8  RSVD1[100];
    volatile Uint32 TXINTSTATRAW;
    volatile Uint32 TXINTSTATMASKED;
    volatile Uint32 TXINTMASKSET;
    volatile Uint32 TXINTMASKCLEAR;
    volatile Uint32 MACINVECTOR;
    volatile Uint8  RSVD2[12];
    volatile Uint32 RXINTSTATRAW;
    volatile Uint32 RXINTSTATMASKED;
    volatile Uint32 RXINTMASKSET;
    volatile Uint32 RXINTMASKCLEAR;
    volatile Uint32 MACINTSTATRAW;
    volatile Uint32 MACINTSTATMASKED;
    volatile Uint32 MACINTMASKSET;
    volatile Uint32 MACINTMASKCLEAR;
    volatile Uint8  RSVD3[64];
    volatile Uint32 RXMBPENABLE;
    volatile Uint32 RXUNICASTSET;
    volatile Uint32 RXUNICASTCLEAR;
    volatile Uint32 RXMAXLEN;
    volatile Uint32 RXBUFFEROFFSET;
    volatile Uint32 RXFILTERLOWTHRESH;
    volatile Uint8  RSVD4[8];
    volatile Uint32 RX0FLOWTHRESH;
    volatile Uint32 RX1FLOWTHRESH;
    volatile Uint32 RX2FLOWTHRESH;
    volatile Uint32 RX3FLOWTHRESH;
    volatile Uint32 RX4FLOWTHRESH;
    volatile Uint32 RX5FLOWTHRESH;
    volatile Uint32 RX6FLOWTHRESH;
    volatile Uint32 RX7FLOWTHRESH;
    volatile Uint32 RX0FREEBUFFER;
    volatile Uint32 RX1FREEBUFFER;
    volatile Uint32 RX2FREEBUFFER;
    volatile Uint32 RX3FREEBUFFER;
    volatile Uint32 RX4FREEBUFFER;
    volatile Uint32 RX5FREEBUFFER;
    volatile Uint32 RX6FREEBUFFER;
    volatile Uint32 RX7FREEBUFFER;
    volatile Uint32 MACCONTROL;
    volatile Uint32 MACSTATUS;
    volatile Uint32 EMCONTROL;
    volatile Uint32 FIFOCONTROL;
    volatile Uint32 MACCONFIG;
    volatile Uint32 SOFTRESET;
    volatile Uint8  RSVD5[88];
    volatile Uint32 MACSRCADDRLO;
    volatile Uint32 MACSRCADDRHI;
    volatile Uint32 MACHASH1;
    volatile Uint32 MACHASH2;
    volatile Uint32 BOFFTEST;
    volatile Uint32 TPACETEST;
    volatile Uint32 RXPAUSE;
    volatile Uint32 TXPAUSE;
    volatile Uint8  RSVD6[16];
    volatile Uint32 RXGOODFRAMES;
    volatile Uint32 RXBCASTFRAMES;
    volatile Uint32 RXMCASTFRAMES;
    volatile Uint32 RXPAUSEFRAMES;
    volatile Uint32 RXCRCERRORS;
    volatile Uint32 RXALIGNCODEERRORS;
    volatile Uint32 RXOVERSIZED;
    volatile Uint32 RXJABBER;
    volatile Uint32 RXUNDERSIZED;
    volatile Uint32 RXFRAGMENTS;
    volatile Uint32 RXFILTERED;
    volatile Uint32 RXQOSFILTERED;
    volatile Uint32 RXOCTETS;
    volatile Uint32 TXGOODFRAMES;
    volatile Uint32 TXBCASTFRAMES;
    volatile Uint32 TXMCASTFRAMES;
    volatile Uint32 TXPAUSEFRAMES;
    volatile Uint32 TXDEFERRED;
    volatile Uint32 TXCOLLISION;
    volatile Uint32 TXSINGLECOLL;
    volatile Uint32 TXMULTICOLL;
    volatile Uint32 TXEXCESSIVECOLL;
    volatile Uint32 TXLATECOLL;
    volatile Uint32 TXUNDERRUN;
    volatile Uint32 TXCARRIERSENSE;
    volatile Uint32 TXOCTETS;
    volatile Uint32 FRAME64;
    volatile Uint32 FRAME65T127;
    volatile Uint32 FRAME128T255;
    volatile Uint32 FRAME256T511;
    volatile Uint32 FRAME512T1023;
    volatile Uint32 FRAME1024TUP;
    volatile Uint32 NETOCTETS;
    volatile Uint32 RXSOFOVERRUNS;
    volatile Uint32 RXMOFOVERRUNS;
    volatile Uint32 RXDMAOVERRUNS;
    volatile Uint8  RSVD7[624];
    volatile Uint32 MACADDRLO;
    volatile Uint32 MACADDRHI;
    volatile Uint32 MACINDEX;
    volatile Uint8  RSVD8[244];
    volatile Uint32 TX0HDP;
    volatile Uint32 TX1HDP;
    volatile Uint32 TX2HDP;
    volatile Uint32 TX3HDP;
    volatile Uint32 TX4HDP;
    volatile Uint32 TX5HDP;
    volatile Uint32 TX6HDP;
    volatile Uint32 TX7HDP;
    volatile Uint32 RX0HDP;
    volatile Uint32 RX1HDP;
    volatile Uint32 RX2HDP;
    volatile Uint32 RX3HDP;
    volatile Uint32 RX4HDP;
    volatile Uint32 RX5HDP;
    volatile Uint32 RX6HDP;
    volatile Uint32 RX7HDP;
    volatile Uint32 TX0CP;
    volatile Uint32 TX1CP;
    volatile Uint32 TX2CP;
    volatile Uint32 TX3CP;
    volatile Uint32 TX4CP;
    volatile Uint32 TX5CP;
    volatile Uint32 TX6CP;
    volatile Uint32 TX7CP;
    volatile Uint32 RX0CP;
    volatile Uint32 RX1CP;
    volatile Uint32 RX2CP;
    volatile Uint32 RX3CP;
    volatile Uint32 RX4CP;
    volatile Uint32 RX5CP;
    volatile Uint32 RX6CP;
    volatile Uint32 RX7CP;
}CPMAC_REGS;

/* MACADDRLO: Bit Mask Definitions. */
#define CPMAC_MACADDRLO_VALID           (0x00100000u)
#define CPMAC_MACADDRLO_MATCHFILT       (0x00080000u)

/* MACCONTROL: Bit Mask Definitions. */
#define CPMAC_MACCONTROL_RGMIIEN        (0x00040000u)
#define CPMAC_MACCONTROL_GIG            (0x00000080u)
#define CPMAC_MACCONTROL_GMIIEN         (0x00000020u)
#define CPMAC_MACCONTROL_FULLDUPLEX     (0x00000001u)

/* RXMBP Enable: Bit Mask Definitions.  */
#define CPMAC_RXMBPENABLE_RXBROADEN     (0x00002000u)

#endif /* __CPMAC_REGS_H__ */
