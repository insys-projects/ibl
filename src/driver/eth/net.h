/**
 *   @file  net.h
 *
 *   @brief   
 *      The file has data structures and API definitions for the
 *      NET Boot Module 
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
#ifndef __NET_H__
#define __NET_H__

#include "iblloc.h"

/**
 * @brief   This is the MAX MTU of the packet that can be received in
 * the network module. This is configured to Ethernet standards at 1518
 */
#define NET_MAX_MTU             1518

/**
 * @brief   This field indicates that the route is a network route
 * and any packet destined to the specific network is directly accessible.
 * Network Routes have the Next Hop address and the IP address to be the 
 * same.
 *
 * @sa 
 *  ip_add_route
 * @sa
 *  ip_del_route
 */
#define FLG_RT_NETWORK      0x1

/**
 * @brief   This field indicates that the route is a default route. 
 * This is a last entry in the routing table and is an indication that
 * the IP address to which the packet is destined is not directly accessible 
 * but needs to be sent through a gateway (i.e. Next Hop)
 *
 * @sa 
 *  ip_add_route
 * @sa
 *  ip_del_route
 */
#define FLG_RT_DEFAULT      0x2

/**
 * @brief 
 *  The structure describes the Network Device Configuration.
 *
 * @details
 *  This structures stores configuration data which is used populated
 *  by the Device layer and passed to the network boot module.
 */
typedef struct NET_DRV_DEVICE
{
    /**
     * @brief  The physical port number to use
     */
    Uint32  port_num;
    
    /**
     * @brief   This is the MAC Address which is populated by the device
     * team. All packets sent by the NET module will use this MAC Address.
     */
    Uint8   mac_address[6];

    /**
     * @brief   This is the IP Address associated with the networking device
     * Device team can specify an IP address which will be used for all 
     * further communications. If BOOTP is enabled and this is 0; then the 
     * BOOTP protocol kicks in to determine an IP Address to be used.
     */
    IPN     ip_address;

    /**
     * @brief   This is the network mask. This is only required to be specified
     * if the device team has specified an IP address. If BOOTP is being used
     * this should be set to 0.
     */
    IPN     net_mask;

    /**
     * @brief   This is the TFTP Server IP Address which is to be used to
     * download the file. This can be retreived from the BOOTP options but if
     * BOOTP is not being used; the Server IP address can be specified here.
     * This is ignored by the NET Boot Module if BOOTP is being used.
     */
    IPN     server_ip;
    
    /**
     *  @brief  Set this to TRUE to override the server IP received from
     *          the bootp server and use the server_ip value above.
     */
    Bool    use_bootp_server_ip;

    /**
     * @brief   This is the File Name which is to be downloaded from the TFTP 
     * server. This can be retreived by decoding the BOOTP options but if BOOTP
     * is not being used then the file name can be specified here. This is ignored
     * by the NET Boot Module if BOOTP is being used.
     */
    Int8   file_name[64];
    
    /**
     * @brief  Set this to TRUE to override the file_name received from the 
     *         bootp server and use the file_name value above.
     */
    Bool    use_bootp_file_name;

    /**
     * @brief   This API is used to start the Ethernet controller. This is a call
     * back function which is invoked by the NET boot module when it has been opened.
     */
    Int32 (*start) (struct NET_DRV_DEVICE* ptr_device);

    /**
     * @brief   This API is used to stop the Ethernet controller. This is a call
     * back function which is invoked by the NET boot module when it has been closed.
     */
    Int32 (*stop) (struct NET_DRV_DEVICE* ptr_device);

    /**
     * @brief   The API is invoked by the NET module to send data to the driver
     * for transmission. 
     */
    Int32 (*send) (struct NET_DRV_DEVICE* ptr_device, Uint8* buffer, Int32 buffer_size);

    /**
     * @brief   The API is invoked by the NET module to receive data from the 
     * driver. The driver populates the buffer passed with the received data
     * and returns the number of bytes received. If there is no data which
     * has been received then the function returns 0
     */
    Int32 (*receive) (struct NET_DRV_DEVICE* ptr_device, Uint8* buffer);
    
}NET_DRV_DEVICE;

/**
 * @brief 
 *  The structure describes the SOCKET structure
 *
 * @details
 *  This is a very primitive 'SOCKET' layer structure which is populated
 *  by the application layer and registered with the UDP module. This is
 *  the basic interface through which packets are sent/received by the 
 *  application layer and NET boot module. The structure is exported as it
 *  allows even device developers to write their own UDP application.
 */
typedef struct SOCKET
{
    /**
     * @brief   This is the local port on which the application is waiting 
     * for packets to arrive. 
     */
    Uint16  local_port;

    /**
     * @brief   This is the remote port to which the application will send 
     * data.
     */
    Uint16  remote_port;

    /**
     * @brief   The remote IP address to which the application will send data.
     */
    IPN     remote_address;

    /**
     * @brief   This is the application supplied call back function which is 
     * invoked by the UDP module when a packet is received on the specific
     * port.
     */
    Int32   (*app_fn) (Int32 sock, Uint8* ptr_data, Int32 num_bytes);
}SOCKET;

/**
 * @brief 
 *  The structure describes the NET Stats
 *
 * @details
 *  The structure keeps track of the network boot module statistics. This
 *  is useful for debugging the network boot operations and drivers 
 */
typedef struct NET_STATS
{
    /**
     * @brief   Total number of packets received.
     */
    Uint32      num_pkt_rxed;

    /**
     * @brief   Total number of received packets dropped at the the layer2.
     * The stat is incremented because of the following reasons:-
     *  - Incorrect L2 Protocol received (Only IPv4 and ARP are handled)
     *  - Dst MAC Address is not directed unicast or broadcast.
     */
    Uint32      rx_l2_dropped;

    /**
     * @brief   Total number of received packets dropped at the the ARP layer.
     * The stat is incremented because of the following reasons:-
     *  - Source MAC Address in the ARP packet is not meant for the stack.
     *  - Source IP Address in the ARP packet is not meant for the stack.
     */
    Uint32      rx_arp_dropped;    

    /**
     * @brief   Total number of received packets dropped at the the IPv4 layer.
     * The stat is incremented because of the following reasons:-
     *  - Invalid IP Header (Version 4 is only supported)
     *  - Incorrect Destination IP Address
     *  - Invalid IP checksum.
     */
    Uint32      rx_ip_dropped;

    /**
     * @brief   Total number of received packets dropped at the the UDP layer.
     * The stat is incremented because of the following reasons:-
     *  - Invalid UDP checksum.
     *  - Packet length is below min UDP size.
     *  - No application is registered to handle the packet.
     */
    Uint32      rx_udp_dropped;

    /**
     * @brief   Total number of packets transmitted.
     */
    Uint32      num_pkt_txed;
}NET_STATS;

#ifdef _BIG_ENDIAN
 #define  ntohs(a) (a)
 #define  htons(a) (a)
 #define  htonl(a) (a)
 #define  ntohl(a) (a)
 #define  READ32(x)    (((Uint32)(*(Uint16 *)(x))<<16)|(Uint32)(*(Uint16 *)(((Uint8 *)(x))+2)))
#else
 #define  htons(a)    ( (((a)>>8)&0xff) + (((a)<<8)&0xff00) )
 #define  ntohs(a)   htons(a)
 #define  htonl(a)    ( (((a)>>24)&0xff) + (((a)>>8)&0xff00) + \
                        (((a)<<8)&0xff0000) + (((a)<<24)&0xff000000) )
 #define  ntohl(a)   htonl(a)
 #define  READ32(x)    ((Uint32)(*(Uint16 *)(x))|((Uint32)(*(Uint16 *)(((Uint8 *)(x))+2))<<16))
#endif

/**********************************************************************
 **************************** Exported Data ***************************
 **********************************************************************/

/**
 * @brief   This is the NET Boot Module function table which has been 
 * exported. Device developers who wish to use the NET Boot Module are 
 * advised to populate the address of this into the BOOT Mode descriptor.
 */
extern BOOT_MODULE_FXN_TABLE net_boot_module;

/**
 * @brief   This is the NET Boot Module statistics.
 */
extern NET_STATS             net_stats;

/**********************************************************************
 **************************** Exported Functions **********************
 **********************************************************************/

/* IPv4 Route Management Functions: Use these API to manually add/delete routes
 * to the NET Boot Module. If BOOTP is being used the routes are automatically
 * added; but if BOOTP is not used; the device layer needs to ensure that 
 * routes are correctly configured */
extern Int32 ip_add_route (Uint32 flag, IPN ip_address, IPN net_mask, IPN next_hop);
extern void  ip_del_route (Uint32 flag);

/* Socket Functions: These are a very primitive stripped down versions of the
 * BSD socket API which are available to device authors to write their own 
 * UDP application if one is so desired. BOOTP and TFTP are two applications
 * provided as a part of the NET Boot Module which use these. */
extern Int32 udp_sock_open (SOCKET* ptr_socket);
extern Int32 udp_sock_send (Int32 sock, Uint8* ptr_app_data, Int32 num_bytes);
extern void  udp_sock_close(Int32 sock);

/* TFTP Functions: This is a TFTP exported API available to device authors to
 * be able to retrieve a file from the TFTP Server. */
extern Int32 tftp_get_file (IPN server_ip, Int8* filename);

/* NET Core Functions: This function is useful if the device authors are writing
 * their own application; this is an ERROR Signal to the NET Boot Module that something
 * catastrophic has happened and that the application is not enable to proceed further. */
extern void net_set_error(void);


#endif /* __NET_H__ */
