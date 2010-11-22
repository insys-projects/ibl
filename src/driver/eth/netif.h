/** 
 *   @file  netif.h
 *
 *   @brief   
 *      Internal header file used only by the NET Boot Module
 *      This header file is not accessible by the DEVICE layers.
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

#ifndef __NETIF_H__
#define __NETIF_H__

/**
 * @brief   This is the protocol identification field in the Ethernet
 * header which identifies the packet as an IPv4 packet.
 */
#define ETH_IP                  0x800

/**
 * @brief   This is the protocol identification field in the Ethernet
 * header which identifies the packet as an IPv4 packet.
 */
#define ETH_ARP                  0x806

/**
 * @brief   This is the protocol identification field in the IPv4 header
 * which identifies the packet as an ICMP packet.
 */
#define IPPROTO_ICMP            1

/**
 * @brief   This is the protocol identification field in the IPv4 header
 * which identifies the packet as a UDP packet.
 */
#define IPPROTO_UDP             17

/**
 * @brief   This is the ICMP Echo request type field which is present in the
 * ICMP header. This is the only type handled in the ICMP Network Boot Module.
 */
#define ICMP_ECHO_REQUEST_TYPE  8

/**
 * @brief   This is the ICMP Echo reply type field which is present in the
 * ICMP header. This is the reply packet generated in response to the REQ
 * packet.
 */
#define ICMP_ECHO_REPLY_TYPE    0

/**
 * @brief   This is the BOOTP SERVER port to which BOOTP requests are sent
 * by the BOOTP client module.
 */
#define BOOTP_SERVER_PORT       67

/**
 * @brief   This is the BOOTP Local port using which the BOOTP client will
 * send requests.
 */
#define BOOTP_CLIENT_PORT       68

/**
 * @brief   This is the BOOTP OP definition for a BOOTP request
 */
#define BOOTP_OP_REQUEST        1

/**
 * @brief   This is the BOOTP OP definition for a BOOTP reply
 */
#define BOOTP_OP_REPLY          2

/**
 * @brief   This is the BOOTP HTYPE definition for Ethernet.
 */
#define BOOTP_HTYPE_ETHERNET    1

/**
 * @brief   This is the BOOTP Seed Timeout (in milliseconds) which is being used.
 */
#define BOOTP_SEED_TIMEOUT      4000

/**
 * @brief   This is the max. number of retries for which the BOOTP client
 * will wait.
 */
#define BOOTP_MAX_RETRIES      5

/**
 * @brief   This is the default BOOTP subnet mask which is used if there
 * is none present in the BOOTP options. 
 */
#define BOOTP_DEFAULT_MASK      htonl(0xFFFFFF00)

/**
 * @brief   This is the well defined TFTP Server port.
 */
#define TFTP_SERVER_PORT       69

/**
 * @brief   This is the maximum TFTP data size that is sent out.
 */
#define TFTP_DATA_SIZE          512

/**
 * @brief   This is the TFTP opcode for READ
 */
#define TFTP_OPCODE_RRQ         1

/**
 * @brief   This is the TFTP opcode for DATA
 */
#define TFTP_OPCODE_DATA        3

/**
 * @brief   This is the TFTP opcode for ACK
 */
#define TFTP_OPCODE_ACK         4

/**
 * @brief   This is the TFTP opcode for ERROR
 */
#define TFTP_OPCODE_ERROR       5

/**
 * @brief   This is the TFTP timeout (in milliseconds) used
 * to send out periodic READ Requests if there is no response
 * detected.
 */
#define TFTP_TIMEOUT            1000

/**
 * @brief   This is the TFTP Server timeout (in milliseconds)
 * to detect the condition where the TFTP Server goes away in 
 * the middle of the download.
 */
#define TFTP_SERVER_TIMEOUT     60000

/**
 * @brief   This is the maximum number of retransmits allowed
 * in the TFTP Client after which an error is indicated.
 */
#define MAX_TFTP_RETRANSMITS    5

/**
 * @brief  This is the the ARP timeout (in milliseconds) used
 *         to resend ARP requests.
 */
#define ARP_TIMEOUT             1000

/***********************************************************************
 ****************** STANDARD NETWORK HEADER DEFINITIONS ****************
 ***********************************************************************/

#define ETHHDR_SIZE     14
typedef struct ETHHDR 
{
    Uint8   DstMac[6];
    Uint8   SrcMac[6];
    Uint16  Type;
} ETHHDR;

#define IPHDR_SIZE      20
typedef struct IPHDR
{
    Uint8    VerLen;
    Uint8    Tos;
    Uint16   TotalLen;
    Uint16   Id;
    Uint16   FlagOff;
    Uint8    Ttl;
    Uint8    Protocol;
    Uint16   Checksum;
    Uint32   IPSrc;
    Uint32   IPDst;
    Uint8    Options[1];
} IPHDR;

#define ARPHDR_SIZE     28
typedef struct ARPHDR
{
    Uint16   HardType;
    Uint16   ProtocolType;
    Uint8    HardSize;
    Uint8    ProtocolSize;
    Uint16   Op;
    Uint8    SrcAddr[6];
    Uint8    IPSrc[4];
    Uint8    DstAddr[6];
    Uint8    IPDst[4];
} ARPHDR;

typedef struct PSEUDOHDR
{
    IPN     IPSrc;
    IPN     IPDst;
    Uint16  Length;
    Uint8   Null;
    Uint8   Protocol;
} PSEUDOHDR;

#define UDPHDR_SIZE     8
typedef struct UDPHDR
{
    Uint16   SrcPort;
    Uint16   DstPort;
    Uint16   Length;
    Uint16   UDPChecksum;
}UDPHDR;

#define ICMPHDR_SIZE    4
typedef struct ICMPHDR
{
    Uint8    Type;
    Uint8    Code;
    Uint16   Checksum;
    Uint8    Data[1];
}ICMPHDR;

typedef struct BOOTPHDR
{
    Uint8   op;
    Uint8   htype;
    Uint8   hlen;
    Uint8   hops;
    Uint32  xid;
    Uint16  secs;
    Uint16  unused;
    Uint32  ciaddr;
    Uint32  yiaddr;
    Uint32  siaddr;
    Uint32  giaddr;
    Uint8   chaddr[16];
    Uint8   sname[64];
    Uint8   file[128];
    Uint8   options[64];
} BOOTPHDR;

#define TFTPHEADER_SIZE 4
typedef struct TFTPHDR
{
    Uint16   opcode;
    Uint16   block;
    Uint8    data[1];
}TFTPHDR;

/**
 * @brief 
 *  The structure describes the ROUTE Entry.
 *
 * @details
 *  This structures describes the routing entry. This is used by the network
 *  boot module to determine the next hop address. The next hop address might
 *  not be the same as the destination IP address if there exists a default
 *  gateway.
 */
typedef struct RT_ENTRY
{
    /**
     * @brief  Flags which describe the routing entry.
     */
    Uint32  flags;

    /**
     * @brief  IP Address associated with the route
     */
    IPN     ip_addr;

    /**
     * @brief  Network Mask associated with the route
     */
    IPN     net_mask;

    /**
     * @brief  Network Address associated with the route
     */
    IPN     net_addr;

    /**
     * @brief  Next Hop Address associated with the route
     */        
    IPN     next_hop;
}RT_ENTRY;

/**
 * @brief 
 *  The structure describes the NET Master Control Block.
 *
 * @details
 *  This is an *internal* structure used by the NET Core Module for storing
 *  information. This structure should not be used or be accessed by the 
 *  platform team.
 */
typedef struct NET_MCB
{
    /**
     * @brief   This is the network device which is attached to the network
     * boot module.
     */
    NET_DRV_DEVICE   net_device;

    /**
     * @brief   This is the receive packet which is passed to the driver 
     * when the Net boot module invokes the receive API. The driver copies 
     * data from its internal buffers to this buffer. The network boot module
     * only operates on this buffer.
     */
    Uint8           rx_packet[NET_MAX_MTU];

    /**
     * @brief   This is the IP ID field which is globally incremented on
     * every packet transmitted from the Network boot module.
     */
    Uint16          ipID;

    /**
     * @brief   This indicates the status of the transmission buffer and whether
     * it is available to be used or not. This is manipulated by the alloc_tx
     * and free_tx routines to ensure there is no corruption with respect to the
     * internal buffers.
     */
    Uint32          txuse;

    /**
     * @brief   This flag indicates if there was an Error detected in the 
     * NET Boot Module or not? This is then used to break out of the NET Boot
     * Module Scheduler. If there is no error detected the flag is set to 0.
     */
    Int32           error_flag; 

    /**
     * @brief   This is the transmit packet which is used by the network
     * module to send a packet to the driver.
     */
    Uint8           tx_packet[NET_MAX_MTU];
    
    
    /**
     *  @brief  This tracks the current read byte in the file 
     */
    Uint32          fileOffset; 
    
    
}NET_MCB;

/**********************************************************************
 **************************** Extern Data *****************************
 **********************************************************************/
extern NET_MCB   netmcb;

/**********************************************************************
 **************************** Exported API ****************************
 **********************************************************************/
extern void      net_init (void);
extern Uint8*    net_alloc_tx_packet(Int32 packet_len);
extern void      net_free_tx_packet (Uint8* ptr);
extern ETHHDR*   net_create_eth_header (Uint8* ptr_l3_hdr, Uint8* dst_mac, Uint16 protocol);
extern void      net_send_packet (ETHHDR* ptr_l2_hdr, Uint16 length);

/* ARP Module exported API. */
extern void      arp_init (void);
extern Int32     arp_receive (ARPHDR* ptr_arphdr, Int32 num_bytes);
extern void      arp_resolve (IPN dst_ip, IPHDR* ptr_iphdr, Uint16 l3_pkt_size);

/* IPv4 Module exported API. */
extern void      ip_init (void);
extern Int32     ip_receive (IPHDR* ptr_iphdr, Int32 num_bytes);
extern void      ip_send (IPHDR* ptr_iphdr, Uint16 size);
extern RT_ENTRY* ip_lookup_route (IPN ip_address);

/* UDP Module exported API. */
extern void     udp_init (void);
extern Int32    udp_receive (IPHDR* ptr_iphdr);

/* BOOTP Module exported API */
void bootp_init (void (*asyncComplete)(void *));

#ifdef INCLUDE_BLF_NET_ICMP
extern void      icmp_receive (IPHDR* ptr_iphdr);
#endif

#endif /* __NETIF_H__ */


