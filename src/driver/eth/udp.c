/** 
 *   @file  udp.c
 *
 *   @brief   
 *      The file implements the NET Module UDP functionality.
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
#include "types.h"
#include "iblloc.h"
#include "net.h"
#include "netif.h"
#include "iblcfg.h"
#include <string.h>
#include "net_osal.h"


/**********************************************************************
 *************************** GLOBAL Variables *************************
 **********************************************************************/

/**
 * @brief   This is the global master control block for the UDP
 * Socket Module and contains information about all the UDP sockets
 * which are open.
 */
SOCKET  udp_socket[MAX_UDP_SOCKET];

/**********************************************************************
 **************************** UDP Functions ***************************
 **********************************************************************/

/**
 *  @b Description
 *  @n  
 *       The function computes the UDP checksum and populates the UDP
 *       Checksum field.
 *
 *  @param[in]  ptr_udphdr
 *      This is the pointer to the UDP header.
 *  @param[in]  ptr_pseudo
 *      This is the UDP Pseudo header used for checksum calculations.
 *
 *  @retval
 *      Not Applicable.
 */
static void udp_checksum (UDPHDR *ptr_udphdr, PSEUDOHDR* ptr_pseudo)
{
    Int32   tmp1;
    Uint16* pw;
    Uint32  TSum;

    /* Get header size in bytes */
    tmp1 = ntohs(ptr_pseudo->Length);

    /* Checksum field is NULL in checksum calculations */
    ptr_udphdr->UDPChecksum = 0;

    /* Checksum the header */
    pw = (Uint16 *)ptr_udphdr;
    TSum = 0;
    for( ; tmp1 > 1; tmp1 -= 2 )
        TSum += (Uint32)*pw++;
#ifdef BIGENDIAN
    if( tmp1 )
        TSum += (Uint32)(*pw & 0xFF00);
#else
    if( tmp1 )
        TSum += (Uint32)(*pw & 0x00FF);
#endif

    /* Checksum the pseudo header */
    pw = (Uint16 *)ptr_pseudo;
    for( tmp1=0; tmp1 < 6; tmp1++ )
        TSum += (Uint32)*pw++;

    TSum = (TSum&0xFFFF) + (TSum>>16);
    TSum = (TSum&0xFFFF) + (TSum>>16);

    /* Special case the 0xFFFF checksum - don't use a checksum
     * value of 0x0000 */
    if( TSum != 0xFFFF )
        TSum = ~TSum;

    /* Note checksum is Net/Host byte order independent */
    ptr_udphdr->UDPChecksum = (Uint16)TSum;
    return;
}

/**
 *  @b Description
 *  @n  
 *       The function handles the processing of UDP packets.
 *
 *  @param[in]  ptr_iphdr
 *      This is the pointer to the IP header.
 *
 *  @retval
 *      Success -   0
 *  @retval
 *      Error   -   <0
 */
Int32 udp_receive (IPHDR* ptr_iphdr)
{
    UDPHDR*     ptr_udphdr;
    Uint16      l4_pkt_size;
    PSEUDOHDR   pseudo;
    Uint16      checksum;
    Int32       index;

    /* Get the pointer to the ICMP header. */
    ptr_udphdr = (UDPHDR *) ((Uint8 *)ptr_iphdr + IPHDR_SIZE);

    /* Compute the l4 packet size. */
    l4_pkt_size = ntohs(ptr_iphdr->TotalLen) - IPHDR_SIZE;

    /* Make sure we have minimum size to proceed. */
    if (l4_pkt_size < UDPHDR_SIZE)
        return -1;

    /* Validate the UDP Checksum if one has been provided. */
    if (ptr_udphdr->UDPChecksum)
    {
        /* Create the Pseudo header. */
        pseudo.IPSrc    = ptr_iphdr->IPSrc;
        pseudo.IPDst    = ptr_iphdr->IPDst;
        pseudo.Null     = 0;
        pseudo.Protocol = 17;
        pseudo.Length   = ptr_udphdr->Length;

        /* Compute the checksum and compare with the one received. */
        checksum = ptr_udphdr->UDPChecksum;
        udp_checksum (ptr_udphdr, &pseudo);
        if (checksum != ptr_udphdr->UDPChecksum)
            return -1;
    }

    /* Cycle through the UDP sockets and pass it to the application. */
    for (index = 0; index < MAX_UDP_SOCKET; index++)
    {
        /* Check if there is a match of the destination port of the received
         * packet with a local port on the socket. */
        if (udp_socket[index].local_port == ntohs(ptr_udphdr->DstPort))
        {
            /* Found the socket! Pass the packet to the application receive handler. */
            udp_socket[index].app_fn (index, (Uint8 *)((Uint8*)ptr_udphdr + sizeof(UDPHDR)), 
                                      (ntohs(ptr_udphdr->Length) - sizeof(UDPHDR)));

            /* Packet has been successfully passed to the application. */
            return 0;
        }
    }

    /* Control comes here implies that there was no application waiting for the UDP data. */
    return -1;
}

/**
 *  @b Description
 *  @n  
 *       The function is available to the application layer to register themselves
 *       with the UDP layer. 
 *
 *  @param[in]  ptr_socket
 *      This is the pointer to the socket structure which is populated by the 
 *      application layer and registered with the UDP module.
 *
 *  @retval
 *      Success -   Socket handle
 *  @retval
 *      Error   -   <0
 */
Int32 udp_sock_open (SOCKET* ptr_socket)
{
    Int32 index = 0;

    /* Basic Validations: The structure should be populated completely. */
    if ((ptr_socket->remote_address == 0) || (ptr_socket->remote_port == 0) || 
        (ptr_socket->local_port == 0) || (ptr_socket->app_fn == NULL))
        return -1;

    /* Cycle through all the UDP socket and create an entry. */
    for (index= 0; index < MAX_UDP_SOCKET; index++)
    {
        /* Check if the socket is free or occupied? 
         *  This can simply be done by verifying that the local port is not 0 */
        if (udp_socket[index].local_port == 0)
        {
            /* Got a free slot. Copy the socket data over and return the index as the handle. */
            netMemcpy ((void *)&udp_socket[index], (void *)ptr_socket, sizeof(SOCKET));
            return index;
        }
    }
    /* Control comes here indicates that there were no free sockets available. */
    return -1;
}

/**
 *  @b Description
 *  @n  
 *       The function is called from the application layer to send out a packet
 *       to the UDP layer. 
 *
 *  @param[in]  sock
 *      This is the socket handle which was returned in the call to udp_sock_open.
 *  @param[in]  ptr_app_data
 *      This is the pointer to the application data payload.
 *  @param[in]  num_bytes
 *      This is the length of the application data payload
 *
 *  @retval
 *      Success -   Number of bytes transmitted
 *  @retval
 *      Error   -   < 0
 */
Int32 udp_sock_send (Int32 sock, Uint8* ptr_app_data, Int32 num_bytes)
{
    SOCKET*     ptr_socket;
    IPHDR*      ptr_iphdr;
    UDPHDR*     ptr_udphdr;
    Uint8*      ptr_data;
    PSEUDOHDR   pseudo;

    /* Get the pointer to the socket handle. */
    ptr_socket = &udp_socket[sock];

    /* Sanity Check: Make sure that the UDP socket is valid */
    if (ptr_socket->local_port == 0)
        return -1;

    /* Allocate memory for the packet. */
    ptr_iphdr = (IPHDR *)net_alloc_tx_packet (num_bytes + IPHDR_SIZE + UDPHDR_SIZE);
    if (ptr_iphdr == NULL)
        return -1;

    /* Get the pointer to the UDP header. */
    ptr_udphdr = (UDPHDR *)((Uint8 *)ptr_iphdr + IPHDR_SIZE);

    /* Get the pointer to the data payload in the newly allocated packet. */
    ptr_data = (Uint8 *)ptr_udphdr + UDPHDR_SIZE;

    /*******************************************************************************
     ********************************* APP Data ************************************
     *******************************************************************************/
    netMemcpy ((void *)ptr_data, (void *)ptr_app_data, num_bytes);

    /*******************************************************************************
     ********************************* UDP Header **********************************
     *******************************************************************************/
    ptr_udphdr->SrcPort = htons(ptr_socket->local_port);
    ptr_udphdr->DstPort = htons(ptr_socket->remote_port);
    ptr_udphdr->Length  = htons(UDPHDR_SIZE + num_bytes);

    /* Create the Pseudo header. */
    pseudo.IPSrc    = netmcb.net_device.ip_address;
    pseudo.IPDst    = ptr_socket->remote_address;
    pseudo.Null     = 0;
    pseudo.Protocol = 17;
    pseudo.Length   = ptr_udphdr->Length;
    udp_checksum (ptr_udphdr, &pseudo);

    /*******************************************************************************
     ********************************* IPv4 Header *********************************
     *******************************************************************************/
    ptr_iphdr->IPDst    = ptr_socket->remote_address;
    ptr_iphdr->Protocol = IPPROTO_UDP;

    /* Send the IP packet. */
    ip_send (ptr_iphdr, UDPHDR_SIZE + IPHDR_SIZE + num_bytes);

    /* Packet has been sent out. */
    return num_bytes;
}

/**
 *  @b Description
 *  @n  
 *       The function is available to the application layer to deregister themselves
 *       with the UDP layer. 
 *
 *  @param[in]  sock
 *      This is the socket handle which is to be closed.
 *
 *  @retval
 *      Not Applicable.
 */
void udp_sock_close (Int32 sock)
{
    SOCKET* ptr_socket;

    /* Basic Validation: Ensure the sock is in the valid range. */
    if ((sock < 0) || (sock > MAX_UDP_SOCKET))
        return;

    /* Get the pointer to the socket handle. */
    ptr_socket = &udp_socket[sock];

    /* Reset the memory block */
    netMemset ((void *)ptr_socket, 0, sizeof(SOCKET));
    return;
}

/**
 *  @b Description
 *  @n  
 *       The function initializes the UDP Module in the NET Boot module.
 *
 *  @retval
 *      Not Applicable
 */
void udp_init (void)
{
    /* Initialize the socket table */
    netMemset (&udp_socket, 0, sizeof(udp_socket));
    return;
}



