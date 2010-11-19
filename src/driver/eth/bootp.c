/**
 *   @file  bootp.c
 *
 *   @brief   
 *      The file implements the NET Module BOOTP functionality.
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
#include "timer.h"
#include "stream.h"
#include <string.h>
#include "net_osal.h"


/**********************************************************************
 *************************** LOCAL Structures *************************
 **********************************************************************/

/**
 * @brief 
 *  The structure describes the BOOTP Master Control Block.
 *
 * @details
 *  The BOOTP Master control block stores information used by the
 *  BOOTP module.
 */
typedef struct BOOTP_MCB
{
    /**
     * @brief   This is the BOOTP header which is populated and sent across
     * to the server.
     */
    BOOTPHDR    boothdr;

    /**
     * @brief   This is the BOOTP handle to the UDP socket.
     */
    Int32       sock;

    /**
     * @brief   This is the number of BOOTP requests sent out.
     */
    Int32       num_request;

    /**
     * @brief   This is the BOOTP timer handle.
     */
    Int32       bootp_timer;

    /**
     * @brief   The optional application call back when the 
     *          bootp file name and IP address has been received. 
     */
    void (*asyncComplete)(void *);

}BOOTP_MCB;

/**********************************************************************
 *************************** GLOBAL Variables *************************
 **********************************************************************/

/**
 * @brief   This is the global master control block for the BOOTP module
 * and stores all the necessary information.
 */
BOOTP_MCB   bootpmcb;

/**********************************************************************
 **************************** BOOTP Functions *************************
 **********************************************************************/

/** 
 *  @b Description
 *  @n  
 *      This is a call back function registered with the TIMER module
 *      to be called if there is a timeout and no BOOTP reply is 
 *      received.
 *
 *  @retval
 *      Not Applicable.
 */
static void bootp_tmr_expiry (void)
{
    BOOTPHDR*   ptr_bootphdr;

    /* Get the pointer to the BOOTP Header. */
    ptr_bootphdr = &bootpmcb.boothdr;

    /* Populate the BOOTP header with all the information we have. */
    ptr_bootphdr->op        = BOOTP_OP_REQUEST;
    ptr_bootphdr->htype     = BOOTP_HTYPE_ETHERNET;
    ptr_bootphdr->hlen      = 6;
    ptr_bootphdr->xid       = htonl(0x1);
    netMemcpy ((void *)&ptr_bootphdr->chaddr, (void *)&netmcb.net_device.mac_address[0], 6);

    /* The packet has been populated; send it to the server. */
    udp_sock_send (bootpmcb.sock, (Uint8 *)ptr_bootphdr, sizeof(BOOTPHDR));

    /* Increment the number of requests sent out. */
    bootpmcb.num_request++;

    /* We need to delete the current timer and create another with the backoff strategy. */
    timer_delete (bootpmcb.bootp_timer);

    /* Check if we have exceeded the max. permissible? */
    if (bootpmcb.num_request > BOOTP_MAX_RETRIES)
    {
        /* Error: Maximum retransmissions have been exceeded; indicate error. */
        mprintf ("BOOTP Failure: Max Retransmissions exceeded\n");
        net_set_error ();
        udp_sock_close(bootpmcb.sock);
        return;
    }

    /* Create the new backoff timer. */
    bootpmcb.bootp_timer = timer_add (BOOTP_SEED_TIMEOUT*bootpmcb.num_request, bootp_tmr_expiry);
    if (bootpmcb.bootp_timer < 0)
    {
        /* Error: Unable to create the new backoff timer; indicate error. */
        mprintf ("BOOTP Failure: Backoff timer failed\n");
        net_set_error ();
        stream_close();
        udp_sock_close(bootpmcb.sock);
        return;
    }
    return;
}

/** 
 *  @b Description
 *  @n  
 *      This is a call back function registered with the UDP module to 
 *      be invoked when a BOOTP packet is received.
 *
 *  @param[in]  sock
 *      This is the socket handle on which packet was received.
 *  @param[in]  ptr_data
 *      This is the pointer to the BOOTP data payload.
 *  @param[in]  num_bytes
 *      This is the number of bytes of BOOTP data received.
 *
 *  @retval
 *      Success -   0
 *  @retval
 *      Error   -   <0
 */
static Int32 bootp_receive (Int32 sock, Uint8* ptr_data, Int32 num_bytes)
{
    BOOTPHDR*   ptr_bootphdr;
    Int32       index = 0;
    IPN         subnetmask = BOOTP_DEFAULT_MASK;
    IPN         defaultRouter = 0;
    IPN         serverIP      = 0;

    /* Received a BOOTP packet from the UDP stack. */
    ptr_bootphdr = (BOOTPHDR *)ptr_data;

    /* Check if this is a BOOTP reply packet? */
    if (ptr_bootphdr->op != BOOTP_OP_REPLY)
        return -1;

    /* Ensure the transaction id matches the one we sent out. */
    if (ptr_bootphdr->xid != bootpmcb.boothdr.xid)
        return -1;

    /* Ensure the MAC Address matches our MAC Address */
    if ((ptr_bootphdr->chaddr[0] != netmcb.net_device.mac_address[0]) || 
        (ptr_bootphdr->chaddr[1] != netmcb.net_device.mac_address[1]) ||
        (ptr_bootphdr->chaddr[2] != netmcb.net_device.mac_address[2]) ||
        (ptr_bootphdr->chaddr[3] != netmcb.net_device.mac_address[3]) ||
        (ptr_bootphdr->chaddr[4] != netmcb.net_device.mac_address[4]) ||
        (ptr_bootphdr->chaddr[5] != netmcb.net_device.mac_address[5]))
    {
        /* The MAC Address do not match. Ignore the reply packet */
        return -1;
    }

    /* Cycle through the options; we are only interested in retreiving the SUBNET Mask option
     * Since we need to configure the routing table. */
    while (index < 64)
    {
        /* Get the option tag and process it appropriately. */
        switch (ptr_bootphdr->options[index])
        {
            case 0x0:
            {
                /* Padding option. Skip this. */
                index++;
                break;
            }
            case 0x1:
            {
                /* SUBNET option. Got it! We dont need to parse anymore. */
                subnetmask = ((ptr_bootphdr->options[index+2] << 24) | 
                              (ptr_bootphdr->options[index+3] << 16) |
                              (ptr_bootphdr->options[index+4] << 8)  | 
                              (ptr_bootphdr->options[index+5]));

                /* Jump to the next option. */
                index = index + ptr_bootphdr->options[index + 1] + 2;
                break;
            }
            case 0x3:
            {
                /* ROUTER option. Got it! We dont need to parse anymore. */
                defaultRouter = ((ptr_bootphdr->options[index+2] << 24) | 
                                 (ptr_bootphdr->options[index+3] << 16) |
                                 (ptr_bootphdr->options[index+4] << 8)  | 
                                 (ptr_bootphdr->options[index+5]));

                /* Jump to the next option. */
                index = index + ptr_bootphdr->options[index + 1] + 2;
                break;
            }
            case 150:
            {
                /* TFTP Server IP Address: */
                serverIP = ((ptr_bootphdr->options[index+2] << 24) | 
                            (ptr_bootphdr->options[index+3] << 16) |
                            (ptr_bootphdr->options[index+4] << 8)  | 
                            (ptr_bootphdr->options[index+5]));

                /* Convert to host order; so that it is in SYNC with "siaddr" field below. */
                serverIP = ntohl(serverIP);

                /* Jump to the next option. */
                index = index + ptr_bootphdr->options[index + 1] + 2;
                break;
            }
            case 0xFF:
            {
                /* End option. Terminate the loop. */
                index = 64;
                break;
            }
            default:
            {
                /* Any other option is not handled; but we need to skip it */
                index = index + ptr_bootphdr->options[index + 1] + 2;
                break;
            }
        }
    }

    /* The BOOTP Reply looks good. Kill the BOOTP timer. */
    timer_delete (bootpmcb.bootp_timer);

    /* Check if we have received the TFTP Server IP address or not? If not we assume
     * that the TFTP Server and BOOTP Server address are one and the same. */
    if (serverIP == 0x0)
        serverIP = ptr_bootphdr->siaddr;

    /* We have all the information with us from the BOOTP Reply Packet. 
     *  a) IP Address
     *  b) Subnet Mask
     *  c) TFTP File Name.
     *  d) TFTP Server IP
     * Lets configure the IPv4 Routing Table with the appropriate information and
     * also configure the global netdevice structure. */
    netmcb.net_device.ip_address = ptr_bootphdr->yiaddr;

    if (netmcb.net_device.use_bootp_server_ip == TRUE)
        netmcb.net_device.server_ip  = serverIP;

    netmcb.net_device.net_mask   = subnetmask;

    ip_add_route (FLG_RT_NETWORK, netmcb.net_device.ip_address, netmcb.net_device.net_mask, 0);

    if (netmcb.net_device.use_bootp_file_name == TRUE)
        netMemcpy (netmcb.net_device.file_name, ptr_bootphdr->file, sizeof(netmcb.net_device.file_name));

    /* Check if we had received a default router? */
    if (defaultRouter != 0)
        ip_add_route (FLG_RT_DEFAULT, 0x0, 0x0, htonl(defaultRouter));

    /* DEBUG Message: */
    mprintf ("*****************************\n");
    mprintf ("BOOTP Complete\n");
    mprintf ("    IP Address    : 0x%x\n", ntohl(netmcb.net_device.ip_address));
    mprintf ("    Net Mask      : 0x%x\n", subnetmask);
    mprintf ("    Default Router: 0x%x\n", defaultRouter);
    mprintf ("    Server IP     : 0x%x\n", ntohl(serverIP));
    mprintf ("    File Name     : %s\n",   ptr_bootphdr->file);
    mprintf ("*****************************\n");

    /* Close the BOOTP sockets. */
    stream_close();
    udp_sock_close (sock);

    /* Optional call back with bootp params */
    if (bootpmcb.asyncComplete != NULL)  
        (*bootpmcb.asyncComplete)((void *)&netmcb.net_device);

    /* Initiate the TFTP Transfer. */
    tftp_get_file (netmcb.net_device.server_ip, (char *)netmcb.net_device.file_name);

    /* BOOTP Reply has been processed. */ 
    return 0;
}

/** 
 *  @b Description
 *  @n  
 *       The function is used to initialize the BOOTP client.
 *
 *  @retval
 *      Not Applicable.
 */
void bootp_init (void (*asyncComplete)(void *))
{
    BOOTPHDR*   ptr_bootphdr;
    SOCKET      socket;

    /* Initialize the BOOT MCB */ 
    netMemset ((void *)&bootpmcb, 0, sizeof(BOOTP_MCB));

    bootpmcb.asyncComplete = asyncComplete;


    /* Populate the socket structure and register this with the UDP module. */
    socket.local_port       = BOOTP_CLIENT_PORT;
    socket.remote_port      = BOOTP_SERVER_PORT;
    socket.remote_address   = 0xFFFFFFFF;
    socket.app_fn           = bootp_receive;

    /* Open the BOOTP socket. */
    bootpmcb.sock = udp_sock_open (&socket);
    if (bootpmcb.sock < 0)
    {
        /* Error: Socket could not be opened. */
        mprintf ("ERROR: BOOTP SOCK Open Failed\n");
        net_set_error ();
        return;
    }

    /* Open the stream to receive packets */
    stream_open(TFTP_DATA_SIZE);

    /* Get the pointer to the BOOTP Header. */
    ptr_bootphdr = &bootpmcb.boothdr;

    /* Populate the BOOTP header with all the information we have. */
    ptr_bootphdr->op        = BOOTP_OP_REQUEST;
    ptr_bootphdr->htype     = BOOTP_HTYPE_ETHERNET;
    ptr_bootphdr->hlen      = 6;
    ptr_bootphdr->xid       = htonl(0x1);
    netMemcpy ((void *)&ptr_bootphdr->chaddr, (void *)&netmcb.net_device.mac_address[0], 6);

    /* The packet has been populated; send it to the server. */
    udp_sock_send (bootpmcb.sock, (Uint8 *)ptr_bootphdr, sizeof(BOOTPHDR));

    /* Increment the number of requests sent out. */
    bootpmcb.num_request++;

    /* Create the BOOTP Timer; if timer creation fails then BOOTP Retransmissions 
     * will not work and so we treat this as a fatal error. */
    bootpmcb.bootp_timer = timer_add (BOOTP_SEED_TIMEOUT, bootp_tmr_expiry);
    if (bootpmcb.bootp_timer < 0)
    {
        /* Error: Timer could not be created; we need to close the socket and signal error. */
        mprintf ("ERROR: BOOTP ADD Timer Failed\n");
        stream_close();
        udp_sock_close (bootpmcb.sock);
        net_set_error ();
        return;
    }
    return;
}



