/** 
 *   @file  ip.c
 *
 *   @brief   
 *      The file implements the NET Module IPv4 functionality.
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
#include <string.h>
#include "net_osal.h"


/**
 * @brief   This is the size of the NET Boot Module Routing table.
 * This definition should never be modified. If changed then the API
 * for adding/deleting routes will also need to be modified.
 */
#define MAX_RT_TABLE_SIZE   2

/**
 * @brief   This is the global routing table which is used by 
 * the stack to determine the route on which the packet needs to 
 * be transmitted.
 */
RT_ENTRY    rt_table[MAX_RT_TABLE_SIZE];

/**********************************************************************
 **************************** IP Functions ****************************
 **********************************************************************/

/**
 *  @b Description
 *  @n  
 *       The function adds a route to the routing table. Default routes are 
 *       always added to the last entry in the routing table. There is no
 *       duplicate route checking done here and if an entry exists it is
 *       overriden with the new values.
 *
 *  @param[in]  flag
 *      Flag properties associated with the route
 *  @param[in]  ip_address
 *      IP Address associated with the route. 
 *  @param[in]  net_mask
 *      Network Mask associated with the route
 *  @param[in]  next_hop
 *      Next Hop Address associated with the route. This is applicable
 *      for default gateways. For network routes this is ignored.
 *
 *  @retval
 *      Success -   0
 *  @retval
 *      Error   -   <0
 */
Int32 ip_add_route (Uint32 flag, IPN ip_address, IPN net_mask, IPN next_hop)
{
    RT_ENTRY*   ptr_rt;

    /* Basic Sanity Check: The flag should be correct! */
    if ((flag != FLG_RT_NETWORK) && (flag != FLG_RT_DEFAULT))
        return -1;

    /* Check if the flag is a network route being added? */
    if (flag == FLG_RT_NETWORK)
    {
        /* Network route is being added. */
        ptr_rt = &rt_table[0];

        /* Populate the routing entry. */
        ptr_rt->flags    = FLG_RT_NETWORK;
        ptr_rt->ip_addr  = ip_address;
        ptr_rt->net_mask = net_mask;
        ptr_rt->net_addr = ip_address & net_mask;
        ptr_rt->next_hop = ip_address;
    }
    else
    {
        /* Default route is being added. */
        ptr_rt = &rt_table[1];

        /* Populate the routing entry. */
        ptr_rt->flags    = FLG_RT_DEFAULT;
        ptr_rt->ip_addr  = 0x0;
        ptr_rt->net_mask = 0x0;
        ptr_rt->net_addr = 0x0;
        ptr_rt->next_hop = next_hop;
    }

    /* Routes were added successfully. */
    return 0;
}

/**
 *  @b Description
 *  @n  
 *       The function checks the routing table to determine a
 *       suitable route which needs to be taken to send out the
 *       packet.
 *
 *  @param[in]  ip_address
 *      IP Address for which the route is being looked up.
 *
 *  @retval
 *      Success -  Pointer to the routing entry being used.
 *  @retval
 *      Error   -  NULL (There is no route which can be used)
 */
RT_ENTRY* ip_lookup_route (IPN ip_address)
{
    Int32       index = 0;
    RT_ENTRY*   ptr_rt;
    IPN         network_address;

    /* Cycle through the routing table. */
    for (index = 0; index < MAX_RT_TABLE_SIZE; index++)
    {
        /* Get the pointer to the routing table entry. */
        ptr_rt = &rt_table[index];

        /* Check if there is a valid entry or not? */
        if (ptr_rt->flags & (FLG_RT_NETWORK | FLG_RT_DEFAULT))
        {
            /* Valid routing entry was found;  calculate the network address 
             * with respect to the route netmask and match it. */
            network_address = ip_address & ptr_rt->net_mask;

            /* Check if this matches the same network to which we are sending the packet. */
            if (network_address == ptr_rt->net_addr)
            {
                /* YES. Perfect; we can return this routing entry. */
                return ptr_rt;
            }
        }
    }

    /* Control comes here indicates that there was no routing entry which could be used. */
    return NULL;
}

/**
 *  @b Description
 *  @n  
 *       The function deletes the routing table entry.
 *
 *  @param[in]  flag
 *      This is the routing entry which is being deleted. 
 *
 *  @retval
 *      Not Applicable.
 */
void ip_del_route (Uint32 flag)
{
    RT_ENTRY*   ptr_rt;

    /* Check if the flag is a network route being added? */
    if (flag == FLG_RT_NETWORK)
    {
        /* Network route is being deleted. */
        ptr_rt = &rt_table[0];
    }
    else
    {
        if (flag == FLG_RT_DEFAULT)
        {
            /* Default route is being deleted. */
            ptr_rt = &rt_table[0];
        }
        else
        {
            /* Invalid Flag combination passed. */
            return;
        }           
    }
    
    /* Simply reset all the fields */
    netMemset ((void *)ptr_rt, 0, sizeof(RT_ENTRY));
    return;
}

/**
 *  @b Description
 *  @n  
 *       The function computes the IP checksum. The computed checksum
 *       is populated in the IP header.
 *
 *  @param[in]  ptr_iphdr
 *      This is the pointer to the IPv4 header for which the checksum
 *      is computed.
 *
 *  @retval
 *      Not Applicable.
 */
static void ip_checksum(IPHDR *ptr_iphdr)
{
    Int32   tmp1;
    Uint16  *pw;
    Uint32  TSum = 0;

    /* Get header size in 4 byte chunks */
    tmp1 = ptr_iphdr->VerLen & 0xF;

    /* Checksum field is NULL in checksum calculations */
    ptr_iphdr->Checksum = 0;

    /* Checksum the header */
    pw = (Uint16 *)ptr_iphdr;
    do {
        TSum += (Uint32)*pw++;
        TSum += (Uint32)*pw++;
    } while( --tmp1 );
    TSum = (TSum&0xFFFF) + (TSum>>16);
    TSum = (TSum&0xFFFF) + (TSum>>16);
    TSum = ~TSum;

    /* Note checksum is Net/Host byte order independent */
    ptr_iphdr->Checksum = (Uint16)TSum;
    return;
}

/**
 *  @b Description
 *  @n  
 *       The function handles the processing of IPv4 packets.
 *
 *  @param[in]  ptr_iphdr
 *      This is the pointer to the IPv4 header.
 *  @param[in]  num_bytes
 *      This is the total number of bytes which includes the IPv4 header 
 *      and data payload.
 *
 *  @retval
 *      Success -   0
 *  @retval
 *      Error   -   <0
 */
Int32 ip_receive (IPHDR* ptr_iphdr, Int32 num_bytes)
{
    Uint32  checksum;

    /* Basic IPv4 Packet Validations: Ensure that this is an IPv4 packet. */
    if ((ptr_iphdr->VerLen & 0xF0) != 0x40)
        return -1;

    /* Validate the length of the received packet; it cannot be greater 
     * than the total length of the received packet. */
    if (ntohs(ptr_iphdr->TotalLen) > num_bytes)
        return -1;

    /* Checksum validation. */
    checksum = ptr_iphdr->Checksum;
    if (checksum == 0xFFFF )
        checksum = 0;
    ip_checksum (ptr_iphdr);
    if (checksum != ptr_iphdr->Checksum )
        return -1;

    /* We dont handle any IP option processing. 
     *  Thus if the IP header length is greater than 20 bytes the packet is dropped. */
    if (((ptr_iphdr->VerLen & 0xF) << 2) != IPHDR_SIZE)
        return -1;

    /* We accept only the following packets:-
     *  a) Destination Address is the address of the NET Boot module. 
     *  b) Destination Address is the a special 255.255.255.255 address for BOOTP Reply*/
    if (netmcb.net_device.use_bootp_server_ip == 0)
	if ((ptr_iphdr->IPDst != netmcb.net_device.ip_address) && (ptr_iphdr->IPDst != 0xFFFFFFFF))
        	return -1;

    /* Pass the packet to the layer4 receive handlers. */
    switch (ptr_iphdr->Protocol)
    {
#ifdef INCLUDE_BLF_NET_ICMP
        case IPPROTO_ICMP:
        {
            icmp_receive (ptr_iphdr);
            break;
        }
#endif /* INCLUDE_BLF_NET_ICMP */
        case IPPROTO_UDP:
        {
            /* UDP Packet. */
            if (udp_receive (ptr_iphdr) < 0)
                net_stats.rx_udp_dropped++;
            break;
        }
        default:
        {
            /* No other protocol is handled. */
            return -1;
        }
    }

    /* Packet has been successfully processed at the IP layer. */
    return 0;
}

/**
 *  @b Description
 *  @n  
 *       The function is called from the layer4 protocol to send 
 *       out an IPv4 packet.
 *
 *  @param[in]  ptr_iphdr
 *      This is the pointer to the IPv4 header + payload which includes
 *      the layer4 header and actual data payload.
 *      The layer4 protocol is supposed to populate the following IPv4 fields:-
 *          - Protocol
 *          - Destination IP
 *      The rest of the fields in the IPv4 header are populated by this function.
 *
 *  @param[in]  size
 *      This is the total number of bytes which includes the layer4 header
 *      and data payload and the IPv4 header.
 *
 *  @retval
 *      Not Applicable.
 */
void ip_send (IPHDR* ptr_iphdr, Uint16 size)
{
    RT_ENTRY* ptr_rt;
    IPN       next_hop_address;

    /* Special Case: If the IP Address of the packet is 255.255.255.255 
     * we dont need to check the routing table. */
    if (ptr_iphdr->IPDst != 0xFFFFFFFF)
    {
        /* Lookup the routing table for a routing entry. */
        ptr_rt = ip_lookup_route (ptr_iphdr->IPDst);
        if (ptr_rt == NULL)
        {
            /* There is no route which exists to send the packet. Drop it! */
            net_free_tx_packet ((Uint8 *)ptr_iphdr);
            return;
        }

        /* Control comes here indicating that a route exists and the packet
         * can be sent out. There are 2 types of routes in the system:-
         *  a) Network Routes which are directly attached and in this case
         *     the next hop ip address is the same as the destination IP address
         *  b) Default Routes which is the address of the default gateway to which
         *     we need to send the packet since the destination network is not
         *     directly connected.
         * So here we determine the next hop address based on the route type. */
        if (ptr_rt->flags == FLG_RT_NETWORK)
            next_hop_address = ptr_iphdr->IPDst;
        else
            next_hop_address = ptr_rt->next_hop;
    }
    else
    {
        /* This is a BOOTP request being sent out to 255.255.255.255. Set the
         * next hop address to be the same. */
        next_hop_address = ptr_iphdr->IPDst;
    }

    /* Ensure all the fields in the IPv4 header are populated correctly */
    ptr_iphdr->VerLen   = 0x45;
    ptr_iphdr->Tos      = 0;
    ptr_iphdr->Id       = netmcb.ipID++;
    ptr_iphdr->FlagOff  = 0;
    ptr_iphdr->Ttl      = 64;
    ptr_iphdr->TotalLen = htons(size);
    ptr_iphdr->IPSrc    = netmcb.net_device.ip_address;

    /* Here we recompute the IP Checksum. */
    ip_checksum (ptr_iphdr);

    /* Layer2 resolution is done on the Next hop address. */
    arp_resolve (next_hop_address, ptr_iphdr, size);
    return;
}

/**
 *  @b Description
 *  @n  
 *       The function initializes the IP Module in the NET Boot module.
 *
 *  @retval
 *      Not Applicable
 */
void ip_init (void)
{
    /* Reset the routing table */
    netMemset (&rt_table, 0, sizeof(rt_table));
    return;
}



