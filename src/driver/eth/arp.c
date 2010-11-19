/** 
 *   @file  arp.c
 *
 *   @brief   
 *      The file implements the NET Module ARP functionality.
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
 *  \par
 *
*/
#include "types.h"
#include "iblloc.h"
#include "net.h"
#include "netif.h"
#include <string.h>
#include "net_osal.h"

/**********************************************************************
 *************************** LOCAL Structures *************************
 **********************************************************************/

/**
 * @brief 
 *  The structure describes the ARP Cache 
 *
 * @details
 *  This describes the ARP Cache which keeps track of the IPv4 address and
 *  the corresponding Layer2 MAC Address. 
 */
typedef struct NET_ARP_CACHE
{
    /**
     * @brief   This is the MAC Address i.e. Layer2 address matching the
     * corresponding IP Address.
     */
    Uint8       mac_address[6];

    /**
     * @brief   This is the IP Address stored in network order.
     */
    IPN         ip_address;

    /**
     * @brief   When the upper layers tries to send a packet and 
     * the MAC Address is not resolved; packets are stored in this 
     * temporary area. The ARP stack sends out an ARP request packet 
     * and resolves the IP  address. Once the resolution is done pending 
     * packets are transmitted. 
     */
    Uint8      pending_packet[NET_MAX_MTU];

    /**
     * @brief   This is the length of the pending packet. A value of 0
     * indicates that there is no pending packet in the ARP cache.
     */    
    Uint16     pending_packet_len;
}NET_ARP_CACHE;

/**********************************************************************
 *************************** GLOBAL Variables *************************
 **********************************************************************/

/**
 * @brief   This is the ARP cache; which keeps track of the IP Address to
 * MAC Address mapping.
 */
NET_ARP_CACHE      net_arp_cache;

/**********************************************************************
 **************************** ARP Functions ***************************
 **********************************************************************/

/**
 *  @b Description
 *  @n  
 *      This function is called by the IP Layer to resolve the layer3 
 *      address to a layer2 MAC address. The function checks the ARP cache
 *      for a match but if no entry exists then the functions sends out
 *      an ARP request and it places this packet into the pending queue.
 *
 *  @param[in]  dst_ip
 *      This is the destination IP address of the packet.
 *  @param[in]  ptr_iphdr
 *      This is the pointer to the IP header
 *  @param[in]  l3_pkt_size
 *      This is the size of the packet (including the IP Header)
 *
 *  @retval
 *      Not Applicable.
 */
void arp_resolve (IPN dst_ip, IPHDR* ptr_iphdr, Uint16 l3_pkt_size)
{
    ARPHDR* ptr_arphdr;
    ETHHDR* ptr_ethhdr;
    Uint8   BroadcastMac[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

    /* Special Case: Are we sending the packet to 255.255.255.255? */
    if (dst_ip == (IPN)0xFFFFFFFF)
    {
        /* YES. This implies that the destination MAC Address in the packet is the Broadcast address
         * We dont need to lookup the cache. */
        ptr_ethhdr = net_create_eth_header ((Uint8 *)ptr_iphdr, (void *)&BroadcastMac[0], 0x800);
        if (ptr_ethhdr == NULL)
            return;

        /* We now have a completed Ethernet packet; send it across. */
        net_send_packet (ptr_ethhdr, sizeof(ETHHDR) + l3_pkt_size);

        /* The packet has been transmitted and we can clean it up now. */
        net_free_tx_packet ((Uint8 *)ptr_iphdr);
        return;            
    }

    /* Check if the destination IP Address matches the ARP cache */
    if (net_arp_cache.ip_address == dst_ip)
    {
        /* Make sure the MAC Address in the CACHE has been resolved i.e. Non-Zero. */
        if ( (net_arp_cache.mac_address[0] != 0x00) || (net_arp_cache.mac_address[1] != 0x00) ||
             (net_arp_cache.mac_address[2] != 0x00) || (net_arp_cache.mac_address[3] != 0x00) || 
             (net_arp_cache.mac_address[4] != 0x00) || (net_arp_cache.mac_address[5] != 0x00) )
        {
            /* Perfect; the MAC Address is already resolved. 
             *  We can simply use the ARP CACHE MAC address to create the layer2 header. */
            ptr_ethhdr = net_create_eth_header ((Uint8 *)ptr_iphdr, (void *)&net_arp_cache.mac_address[0], 0x800);
            if (ptr_ethhdr == NULL)
                return;

            /* We now have a completed Ethernet packet; send it across. */
            net_send_packet (ptr_ethhdr, sizeof(ETHHDR) + l3_pkt_size);

            /* The packet has been transmitted and we can clean it up now. */
            net_free_tx_packet ((Uint8 *)ptr_iphdr);
            return;
        }
    }

    /* Initialize the ARP Cache: The cache did not have information for the resolution to work
     * Reset the cache and start again.  */
    netMemset ((void *)&net_arp_cache, 0, sizeof(NET_ARP_CACHE));

    /* Populate the ARP Cache */
    net_arp_cache.ip_address         = dst_ip;    
    net_arp_cache.pending_packet_len = l3_pkt_size;
    netMemcpy ((void *)&net_arp_cache.pending_packet[0], (void *)ptr_iphdr, l3_pkt_size);

    /* Free up the packet now; we have already stored it in the ARP cache. */
    net_free_tx_packet ((Uint8 *)ptr_iphdr);

    /* Allocate a new packet to send out the ARP request. */
    ptr_arphdr = (ARPHDR *)net_alloc_tx_packet(sizeof(ARPHDR));
    if (ptr_arphdr == NULL)
        return;

    /* Populate the ARP Header. */
    ptr_arphdr->HardType     = htons(0x1);
    ptr_arphdr->ProtocolType = htons(0x800);
    ptr_arphdr->HardSize     = 0x6;
    ptr_arphdr->ProtocolSize = 0x4;
    ptr_arphdr->Op           = htons(0x1);

    /* Populate the Source IP/MAC Address */
    netMemcpy ((void *)&ptr_arphdr->SrcAddr[0], (void *)&netmcb.net_device.mac_address[0], 6);
    netMemcpy ((void *)&ptr_arphdr->IPSrc[0], (void *)&netmcb.net_device.ip_address, 4);

    /* Populate the Target IP/MAC Address: This is set to 0 since the cache has been reset above. */
    netMemcpy ((void *)&ptr_arphdr->DstAddr[0], (void *)&net_arp_cache.mac_address[0], 6);
    netMemcpy ((void *)&ptr_arphdr->IPDst[0], (void *)&dst_ip, 4);

    /* Create the Ethernet header. */
    ptr_ethhdr = net_create_eth_header ((Uint8 *)ptr_arphdr, &BroadcastMac[0], 0x806);
    if (ptr_ethhdr == NULL)
        return;

    /* Send the packet out. */
    net_send_packet (ptr_ethhdr, sizeof(ETHHDR) + sizeof(ARPHDR));

    /* The packet has been transmitted and we can clean it up now. */
    net_free_tx_packet ((Uint8 *)ptr_arphdr);
    return;
}

/**
 *  @b Description
 *  @n  
 *       The function handles the reception and processing of ARP packets.
 *
 *  @param[in]  ptr_arphdr
 *      This is the pointer to the received ARP header.
 *  @param[in]  num_bytes
 *      This is the size of the ARP packet.
 *
 *  @retval
 *      Success -   0
 *  @retval
 *      Error   -   <0
 */
Int32 arp_receive (ARPHDR* ptr_arphdr, Int32 num_bytes)
{
    Uint32  IPAddress;
    Uint8*  ptr_pending_pkt;
    ETHHDR* ptr_ethhdr;

    /* Extract the intended target and convert it to host format. */
    IPAddress = READ32(ptr_arphdr->IPDst);

    /* Ensure that the packet has not been looped back and we received our own frame. */
    if ((ptr_arphdr->SrcAddr[0] == netmcb.net_device.mac_address[0]) &&
        (ptr_arphdr->SrcAddr[1] == netmcb.net_device.mac_address[1]) &&
        (ptr_arphdr->SrcAddr[2] == netmcb.net_device.mac_address[2]) &&
        (ptr_arphdr->SrcAddr[3] == netmcb.net_device.mac_address[3]) &&
        (ptr_arphdr->SrcAddr[4] == netmcb.net_device.mac_address[4]) &&
        (ptr_arphdr->SrcAddr[5] == netmcb.net_device.mac_address[5]))
    {
        /* We received our own frame; ignore this. */
        return -1;
    }

    /* Check if the packet is meant for us? If it not meant for us we drop the packet. */
    if (IPAddress != netmcb.net_device.ip_address)
        return -1;

    /* The ARP packet was meant for us; we need to update the ARP cache with the request. */
    net_arp_cache.ip_address = READ32(ptr_arphdr->IPSrc);
    netMemcpy ((void *)&net_arp_cache.mac_address[0], (void *)&ptr_arphdr->SrcAddr[0], 6);

    /* Check if the packet is an ARP request? */
    if (ptr_arphdr->Op == htons(0x1))
    {
        /* YES. We need to send out an ARP Reply; so create the packet. 
         *  Ensure that the Layer3 headers are aligned on the 4 byte boundary 
         *  and yet we have sufficient space for the Ethernet header. */
        ptr_arphdr = (ARPHDR *)net_alloc_tx_packet(sizeof(ARPHDR));
        if (ptr_arphdr == NULL)
            return -1;

        /* Populate the ARP Header. */
        ptr_arphdr->HardType     = htons(0x1);
        ptr_arphdr->ProtocolType = htons(0x800);
        ptr_arphdr->HardSize     = 0x6;
        ptr_arphdr->ProtocolSize = 0x4;
        ptr_arphdr->Op           = htons(0x2);

        /* Populate the Source IP/MAC Address in the ARP Header */
        netMemcpy ((void *)&ptr_arphdr->SrcAddr[0], (void *)&netmcb.net_device.mac_address[0], 6);
        netMemcpy ((void *)&ptr_arphdr->IPSrc[0], (void *)&netmcb.net_device.ip_address, 4);

        /* Populate the Target IP/MAC Address in the ARP Header */
        netMemcpy ((void *)&ptr_arphdr->DstAddr[0], (void *)&net_arp_cache.mac_address[0], 6);
        netMemcpy ((void *)&ptr_arphdr->IPDst[0], (void *)&net_arp_cache.ip_address, 4);

        /* Create the Ethernet header. */
        ptr_ethhdr = net_create_eth_header ((Uint8 *)ptr_arphdr, &net_arp_cache.mac_address[0], 0x806);
        if (ptr_ethhdr == NULL)
            return -1;

        /* Send the packet out. */
        net_send_packet (ptr_ethhdr, sizeof(ETHHDR) + sizeof(ARPHDR));

        /* The packet has been transmitted and we can clean it up now. */
        net_free_tx_packet ((Uint8 *)ptr_arphdr);
    }

    /* Check if there are any packet awaiting resolution? */
    if (net_arp_cache.pending_packet_len != 0)
    {
        /* There was a packet in the ARP cache which was awaiting resolution. 
         * We need to send out the packet now. */
        ptr_pending_pkt = net_alloc_tx_packet(net_arp_cache.pending_packet_len);
        if (ptr_pending_pkt == NULL)
            return -1;

        /* We now copy the contents of this packet from the ARP cache */
        netMemcpy ((void *)ptr_pending_pkt, (void *)&net_arp_cache.pending_packet[0], 
                    net_arp_cache.pending_packet_len);

        /* We create the Ethernet header. 
         *  Only IPv4 packets await resolution. */
        ptr_ethhdr = net_create_eth_header ((Uint8 *)ptr_pending_pkt, &net_arp_cache.mac_address[0], 0x800);
        if (ptr_ethhdr == NULL)
            return -1;

        /* Send the packet out. */
        net_send_packet (ptr_ethhdr, sizeof(ETHHDR) + net_arp_cache.pending_packet_len);

        /* The pending packet in the cache has been sent out. */
        net_arp_cache.pending_packet_len = 0;

        /* The packet has been transmitted and can be cleaned up. */
        net_free_tx_packet (ptr_pending_pkt);
    }

    /* ARP Packet has been successfully processed. */
    return 0;
}

/**
 *  @b Description
 *  @n  
 *       The function initializes the ARP Module in the NET Boot module.
 *
 *  @retval
 *      Not Applicable
 */
void arp_init (void)
{
    netMemset (&net_arp_cache, 0, sizeof(NET_ARP_CACHE));
    return;
}



