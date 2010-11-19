/** 
 *   @file  icmp.c
 *
 *   @brief   
 *      The file implements the NET Module ICMP functionality.
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
#include <blf.h>
#include "netif.h"
#include "net_osal.h"

#ifdef INCLUDE_BLF_NET_MODULE
#ifdef INCLUDE_BLF_NET_ICMP

/**
 *  @b Description
 *  @n  
 *       The function computes the ICMP checksum and populates the ICMP
 *       Checksum field.
 *
 *  @param[in]  ptr_icmp_hdr
 *      This is the pointer to the ICMP header.
 *  @param[in]  size
 *      This is the total size of the ICMP packet including header and data
 *      payload.
 *
 *  @retval
 *      Not Applicable.
 */
static void icmp_checksum (ICMPHDR *ptr_icmp_hdr, Uint16 size)
{
    Int32   tmp1;
    Uint16* pw;
    Uint32  TSum;

    /* Checksum field is NULL in checksum calculations */
    ptr_icmp_hdr->Checksum = 0;

    /* Checksum the header */
    pw = (Uint16 *)ptr_icmp_hdr;
    TSum = 0;
    for( tmp1=size; tmp1 > 1; tmp1 -= 2 )
        TSum += (Uint32)*pw++;
#ifdef BIGENDIAN
    if( tmp1 )
        TSum += (Uint32)(*pw & 0xFF00);
#else
    if( tmp1 )
        TSum += (Uint32)(*pw & 0x00FF);
#endif
    TSum = (TSum&0xFFFF) + (TSum>>16);
    TSum = (TSum&0xFFFF) + (TSum>>16);
    TSum = ~TSum;

    /* Note checksum is Net/Host byte order independent */
    ptr_icmp_hdr->Checksum = (Uint16)TSum;
    return;
}

/**
 *  @b Description
 *  @n  
 *       The function handles the processing of ICMP packets.
 *
 *  @param[in]  ptr_iphdr
 *      This is the pointer to the IP header.
 *
 *  @retval
 *      Not Applicable.
 */
void icmp_receive (IPHDR* ptr_iphdr)
{
    ICMPHDR*    ptr_icmphdr;
    Uint16      checksum;
    Uint16      l4_pkt_size;
    ICMPHDR*    ptr_reply_hdr;
    IPHDR*      ptr_reply_iphdr;

    /* Get the pointer to the ICMP header. */
    ptr_icmphdr = (ICMPHDR *) ((Uint8 *)ptr_iphdr + IPHDR_SIZE);

    /* Compute the l4 packet size. */
    l4_pkt_size = ntohs(ptr_iphdr->TotalLen) - IPHDR_SIZE;

    /* We only handle ICMP echo requests */
    if (ptr_icmphdr->Type != ICMP_ECHO_REQUEST_TYPE)
        return;

    /* Validate the checksum */
    checksum = ptr_icmphdr->Checksum;
    if (checksum == 0xFFFF)
        checksum = 0;
    icmp_checksum (ptr_icmphdr, l4_pkt_size);
    if( checksum != ptr_icmphdr->Checksum )
        return;

    /* OK; control comes here implies that a valid ICMP ECHO request
     * packet was received. Now we send back the ECHO reply; so lets 
     * allocate a new transmit packet. */
    ptr_reply_iphdr = (IPHDR *)net_alloc_tx_packet (l4_pkt_size + IPHDR_SIZE);
    if (ptr_reply_iphdr == NULL)
        return;

    /* We can blindly copy the 'original' IP packet to the 'reply' IP packet. */
    netMemcpy ((void *)ptr_reply_iphdr, (void *)ptr_iphdr, (l4_pkt_size + IPHDR_SIZE));

    /* Get the pointer to the new header. 
     *  The new header starts after the IP header. */
    ptr_reply_hdr = (ICMPHDR *)((Uint8 *)ptr_reply_iphdr + IPHDR_SIZE);

    /* This is an ECHO reply packet; so lets overwrite the field. */ 
    ptr_reply_hdr->Type = ICMP_ECHO_REPLY_TYPE;

    /* Compute the new ICMP checksum and overwrite it in the header. */
    icmp_checksum (ptr_reply_hdr, l4_pkt_size);

    /* Swap the source and destination address in the REPLY packet. */
    ptr_reply_iphdr->IPSrc = ptr_iphdr->IPDst;
    ptr_reply_iphdr->IPDst = ptr_iphdr->IPSrc;

    /* Packet looks good. Send to the IP layer for transmission. */
    ip_send (ptr_reply_iphdr, l4_pkt_size + IPHDR_SIZE);
    return;
}

#endif /* INCLUDE_BLF_NET_ICMP */
#endif /* INCLUDE_BLF_NET_MODULE */


