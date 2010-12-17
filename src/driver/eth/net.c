/** 
 *   @file  net.c
 *
 *   @brief   
 *      The file implements the NET Boot Module. 
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
 *************************** Local Functions **************************
 **********************************************************************/
static Int32 net_open (void* ptr_driver, void (*asyncComplete)(void *));
static Int32 net_close(void);
static Int32 net_read (Uint8* ptr_buf, Uint32 num_bytes);
static Int32 net_peek (Uint8* ptr_buf, Uint32 num_bytes);
static Int32 net_seek (Int32 loc, Int32 from);
static Int32 net_query (void);

/**********************************************************************
 *************************** GLOBAL Variables *************************
 **********************************************************************/

/**
 * @defgroup net_op
 *
 * @ingroup net_op
 * @{
 *
 * @brief
 *   Internal definition to distinguish a read from a peek
 *
 */

/**
 *  @def NET_READ
 */
#define NET_READ    400  /**< Read from the net device */

/**
 * @def NET_PEEK
 */
#define NET_PEEK    420  /**< Peek from the net device */

/* @} */


/**
 * @brief   This is the NETWORK Master control block which keeps track
 * of all the Network boot module related information.
 */
NET_MCB     netmcb;

/**
 * @brief   This keeps track of the network statistics.
 */
NET_STATS   net_stats;

/**
 * @brief   This is the global Network Boot Module function table which
 * implements the Boot Module Interface with the kernel.
 */
BOOT_MODULE_FXN_TABLE net_boot_module = 
{
    net_open,       /* Open  API                                      */
    net_close,      /* Close API                                      */
    net_read,       /* Read  API                                      */
    NULL,           /* Write API (NULL: This is not interactive)      */
    net_peek,       /* Peek  API                                      */
    net_seek,       /* Seek  API                                      */
    net_query       /* Query API                                      */
};

/**********************************************************************
 **************************** NET Functions ***************************
 **********************************************************************/

/**
 *  @b Description
 *  @n
 *       The function is used to allocate a packet for transmission. This
 *       is called by the higher layer protocols when a packet needs to
 *       be transmitted. The function ensures there is sufficient space
 *       allocated at the beginning of the packet for the Ethernet header.
 *       Since the pointer returned by this API is used to store L3/L4 headers
 *       it always returns a 4 byte aligned buffer data pointer.
 *
 *  @param[in]  packet_len
 *      Length of the packet being transmitted.
 *
 *  @retval
 *      Success -   Pointer to the L3 header. 
 *  @retval
 *      Error   -   NULL
 */
Uint8* net_alloc_tx_packet(Int32 packet_len)
{
    /* Sanity Check: Ensure the packet being allocated does not exceed the
     * max buffer size that we have. */
    if (packet_len > NET_MAX_MTU)
        return NULL;

    /* Sanity Check: Ensure that the packet is available for use. */
    if (netmcb.txuse == 1)
        return NULL;

    /* Mark the packet as being in use. */
    netmcb.txuse = 1;

    /* Reset the contents of the packet */
    netMemset ((void *)&netmcb.tx_packet[0], 0, sizeof(netmcb.tx_packet));

    /* Reserve some space at the head of the packet for the Ethernet headers. */
    return &netmcb.tx_packet[16];
}

/**
 *  @b Description
 *  @n
 *      The function is called to free up a previously allocated transmit
 *      packet. 
 *
 *  @param[in]  ptr 
 *      Pointer to the packet being cleaned.
 *
 *  @retval
 *      Not Applicable.
 */
void net_free_tx_packet (Uint8* ptr)
{
    /* Sanity Checks: Ensure that the packet being cleaned is the same as the one 
     * which was allocated. */
    if (ptr != &netmcb.tx_packet[16])
        mprintf ("ERROR: NET Free Transmit packet detected corruption\n");

    /* Sanity Checks: Ensure that there is no double free. */
    if (netmcb.txuse == 0)
        mprintf ("ERROR: NET Free Transmit packet detected double free\n");

    /* Mark the transmit packet as free and available. */
    netmcb.txuse = 0;
    return;
}

/**
 *  @b Description
 *  @n  
 *       The function is used to append an Ethernet header on the packet.
 *       The source MAC Address in the packet is always the one which was
 *       registered by the driver. Higher layer protocol authors need to 
 *       use the 'net_alloc_tx_packet' API to get a transmit buffer.
 *
 *  @param[in]  ptr_l3_hdr
 *      This is the pointer to the layer3 header.
 *  @param[in]  dst_mac
 *      The destination MAC address
 *  @param[in]  protocol
 *      The layer3 protocol version (passed in host order)
 *
 *  @retval
 *      Success -   Pointer to the start of the Ethernet header i.e. L2.
 *  @retval
 *      Error   -   NULL
 */
ETHHDR* net_create_eth_header (Uint8* ptr_l3_hdr, Uint8* dst_mac, Uint16 protocol)
{
    Int32 rsvd_space;

    /* Compute the reserved space. */
    rsvd_space = (Int32)ptr_l3_hdr - (Int32)&netmcb.tx_packet[0];

    /* Ensure there is sufficient space to add the header. We dont want memory corruption 
     * to occur here. */
    if ((rsvd_space < 0) || (rsvd_space > NET_MAX_MTU) || (rsvd_space < ETHHDR_SIZE))
        return NULL;

    /* Convert the protocol to network order. */
    protocol = ntohs(protocol);

    /* Start adding the Ethernet header. 
     *  Move back the data pointer to account for the protocol. */
    ptr_l3_hdr = ptr_l3_hdr - 2;
    netMemcpy ((void *)ptr_l3_hdr, (void *)&protocol, sizeof(Uint16));

    /* Move back the data pointer to account for the source MAC. */
    ptr_l3_hdr = ptr_l3_hdr - 6;
    netMemcpy ((void *)ptr_l3_hdr, (void *)&netmcb.net_device.mac_address[0], 6);

    /* Move back the data pointer to account for the destination MAC. */
    ptr_l3_hdr = ptr_l3_hdr - 6;
    netMemcpy ((void *)ptr_l3_hdr, (void *)dst_mac, 6);

    /* Return the pointer to the start of the Ethernet header. */
    return (ETHHDR *)ptr_l3_hdr;
}

/**
 *  @b Description
 *  @n  
 *       The function is used to send a packet to the driver.
 *
 *  @param[in]  ptr_l2_hdr
 *      This is the pointer to the L2 header of the packet to be transmitted.
 *      All the headers need to be populated by the time comes to this API.
 *  @param[in]  length
 *      Length of the packet being transmitted. This include the L2 header
 *      information.
 *
 *  @retval
 *      Not Applicable.
 */
void net_send_packet (ETHHDR* ptr_l2_hdr, Uint16 length)
{
    /* Sanity Check: This is called after the LAYER3 and ETHER headers 
     * have been appended to the packet. Here we ensure that the layer3 header
     * is aligned on the 4 byte boundary. */
    if ( (((Uint32)ptr_l2_hdr+ETHHDR_SIZE) % 4) != 0)
        mprintf ("ERROR: Misaligned Layer3 packet transmitted 0x%p.\n", ptr_l2_hdr);

    /* Increment the stats. */
    net_stats.num_pkt_txed++;

    /* Pass the packet to the platform API for transmission. */
    netmcb.net_device.send (&netmcb.net_device, (Uint8 *)ptr_l2_hdr, (Int32)length);
    return;
}

/**
 *  @b Description
 *  @n  
 *       The function is called from any entity inside the NETWORK Boot Module
 *       to indicate that there has been a FATAL error and that the boot module
 *       processing needs to be aborted.
 *
 *  @retval
 *      Not Applicable.
 */
void net_set_error (void)
{
    /* Set the Error Flag; */
    netmcb.error_flag = 1;
    return;
}

/**
 *  @b Description
 *  @n  
 *       The function opens the BLF NET Module. The function initializes
 *       the various internal components of the NET module and also
 *       initializes the peripheral driver controller 
 *
 *  @param[in]  ptr_driver
 *      This is the pointer to the driver block which was passed to 
 *      the BLF through the BOOT Mode descriptor. For the NET boot
 *      module this points to the NET_DRV_DEVICE object.
 *
 *  @retval
 *      Success -   0
 *  @retval
 *      Error   -   <0
 */
static Int32 net_open (void* ptr_driver, void (*asyncComplete)(void *))
{
    NET_DRV_DEVICE* ptr_net_driver;

    /* Get the pointer to the net driver. */
    ptr_net_driver = (NET_DRV_DEVICE*)ptr_driver;

    /* Basic Validation: Ensure there is a valid driver block being passed. */
    if (ptr_net_driver == NULL)
        return -1;

    /* Basic Validation: Ensure that all the required API have been provided */
    if ((ptr_net_driver->start == NULL)   || (ptr_net_driver->send == NULL) ||
        (ptr_net_driver->receive == NULL) || (ptr_net_driver->stop == NULL))
    {
        /* Error: Required API was not specified. */
        return -1;
    }

    /* Initialize the NET MCB. */
    netMemset (&netmcb, 0, sizeof(NET_MCB));

    /* Initialize the Network Statistics. */
    netMemset (&net_stats, 0, sizeof(NET_STATS));

    /* Copy the driver information into the NET MCB */
    netMemcpy ((void *)&netmcb.net_device, (void *)ptr_net_driver, sizeof(NET_DRV_DEVICE));

    /* Initialize the ARP Module. */
    arp_init ();

    /* Initialize the IP Module. */
    ip_init ();

    /* Initialize the UDP Module. */
    udp_init ();

    /* Start the networking device */
    if (netmcb.net_device.start(&netmcb.net_device) < 0)
        return -1;

    /* Determine if we need to execute BOOTP or not? */
    if (netmcb.net_device.ip_address != 0)
    {
        /* IP Address was supplied by the device team. There is no need 
         * to execute the BOOTP protocol; manually create the network route also. */
        ip_add_route (FLG_RT_NETWORK, netmcb.net_device.ip_address, netmcb.net_device.net_mask, 0);

        /* Optional call back with boot server info */
        if (asyncComplete != NULL)
            (*asyncComplete)((void *)&netmcb.net_device);

        /* Lets download the file from the TFTP Server. */
        tftp_get_file (netmcb.net_device.server_ip, netmcb.net_device.file_name);
    }
    else
    {
        /* No IP Address was supplied we need to execute the BOOTP protocol. */
        bootp_init (asyncComplete);
    }
   
    /* The network module is UP and running.. */ 
    return 0;
}

/**
 *  @b Description
 *  @n  
 *       The function closes the BLF NET Module.
 *
 *  @retval
 *      Success -   0
 *  @retval
 *      Error   -   <0
 */
static Int32 net_close (void)
{
    if (netmcb.net_device.stop)
        (*netmcb.net_device.stop)(&netmcb.net_device);

    return 0;
}


/**
 *  @b Description
 *  @n
 *      If a waiting packet is found it is processed
 */
static void proc_packet (void)
{
    Uint8*      ptr_data_packet;
    Int32       packet_size;
    Uint16      protocol;
    Uint8       dst_mac_address[6];

    /* Initialize the pointer in the received packet is stored. 
     *  This is misaligned on the 2 byte boundary as this will ensure that
     *  the layer3 headers i.e. IPv4 and ARP are aligned correctly. */
    ptr_data_packet = (Uint8 *)&netmcb.rx_packet[2];

    /* Check if a packet has been received? */
    packet_size = netmcb.net_device.receive(&netmcb.net_device, ptr_data_packet);
    if (packet_size == 0)
        return;

    /* Increment the number of packets received. */
    net_stats.num_pkt_rxed++;

    /* Extract the destination MAC Address from the received packet. */
    netMemcpy ((void *)&dst_mac_address[0], (void *)ptr_data_packet, 6);

    /* Extract the protocol from the received packet. This is at offset 12 from the 
     * start of the packet. We need to skip the destination and source mac address. */
    protocol = *((Uint16 *)(ptr_data_packet + 12));
    protocol = ntohs(protocol);

    /* Process the received data.
     *  Check the destination mac address to determine if the packet is 
     *  meant for us or not? We accept only directed UNICAST & BROADCAST
     *  packets */
    if(((dst_mac_address[0] != 0xFF) || (dst_mac_address[1] != 0xFF) || 
        (dst_mac_address[2] != 0xFF) || (dst_mac_address[3] != 0xFF) || 
        (dst_mac_address[4] != 0xFF) || (dst_mac_address[5] != 0xFF)) && 
       ((dst_mac_address[0] != netmcb.net_device.mac_address[0]) ||
        (dst_mac_address[1] != netmcb.net_device.mac_address[1]) ||
        (dst_mac_address[2] != netmcb.net_device.mac_address[2]) ||
        (dst_mac_address[3] != netmcb.net_device.mac_address[3]) ||
        (dst_mac_address[4] != netmcb.net_device.mac_address[4]) ||
        (dst_mac_address[5] != netmcb.net_device.mac_address[5])))
    {
        /* Packet is not meant for us; ignore this packet. */
        net_stats.rx_l2_dropped++;
        return;
    }

    /* Move the pointer to the data packet and skip the ethernet header. */
    ptr_data_packet = ptr_data_packet + sizeof(ETHHDR);

    /* Deduct the Ethernet header size from the total number of bytes received. */
    packet_size = packet_size - sizeof(ETHHDR);

    /* Sanity Check: We need to ensure that the Layer3 headers are aligned on the
     * 4 byte boundary at this stage. */
    if (((Uint32)ptr_data_packet % 4) != 0)
        mprintf ("ERROR: Misaligned Layer3 packet received 0x%p.\n", ptr_data_packet);

    /* Demux on the protocol basis and pass it to the upper layer. */
    switch (protocol)
    {
        case ETH_IP:
        {
            /* IPv4 Packet. */
            if (ip_receive ((IPHDR *)ptr_data_packet, packet_size) < 0)
                net_stats.rx_ip_dropped++;
            break;
        }
        case ETH_ARP:
        {
            /* ARP Packet. */
            if (arp_receive ((ARPHDR *)ptr_data_packet, packet_size) < 0)
                net_stats.rx_arp_dropped++;
            break;
        }
        default:
        {
            /* Unexpected packet. Drop the packet! */
            net_stats.rx_l2_dropped++;
            break;
        }
    }

}

/**
 *  @b Description
 *  @n  
 *       The function reads/peeks data from the BLF net module.
 *
 *  @param[in]  ptr_buf
 *      This points to the data buffer which needs to be populated
 *      with the received data.
 *
 *  @param[in]  num_bytes
 *      This is the number of bytes of data which need to be read.
 *
 *  @param[in]  op
 *      Determines if a read or peek operation is performed
 *
 *  @retval
 *      Success -   0
 *  @retval
 *      Error   -   <0
 */
static Int32 net_read_peek (Uint8* ptr_buf, Uint32 num_bytes, Int32 op)
{
    Int32       num_bytes_read       = 0;
    Int32       total_num_bytes_read = 0;

    /* Basic Validations: Ensure that the parameters are valid. */
    if ((ptr_buf == NULL) || (num_bytes == 0))
        return -1;

    /* Execute the network scheduler; till there is no error. */
    while (netmcb.error_flag == 0) 
    {

        /* If the stream is empty and closed return */
        if (stream_level() < 0)  {
            if (total_num_bytes_read > 0)
                return (0);
            else
                return (-1);
        }


        /* Call the timer scheduler. */
        timer_run();

        /* Check if there is data in the STREAM? */
        if (stream_isempty() == FALSE)
        {
            /* STREAM indicates there is some data. Lets read it first. */
            if (op == NET_READ)  {
                num_bytes_read    = stream_read (((ptr_buf + total_num_bytes_read)), num_bytes);
                netmcb.fileOffset = netmcb.fileOffset + num_bytes_read;
            }  else
                num_bytes_read = stream_peek (((ptr_buf + total_num_bytes_read)), num_bytes);

            /* Keep track of the total amount of data read till now. */
            total_num_bytes_read = total_num_bytes_read + num_bytes_read;

            /* How much more data do we need to read? */ 
            num_bytes = num_bytes - num_bytes_read;

            /* Check if we have read all the data that was requested? */
            if (num_bytes == 0)
                break;
    
            /* Control comes here implies that there was more data to be read into
             * the buffer. We need it to have it available before we can exit the loop.
             * So lets fall through! */
        }
        else
        {
            /* STREAM Module is empty. */
        }

        /* Check for and process any received packets */
        proc_packet ();

    }

    /* Did we come out because of error or not? */
    if (netmcb.error_flag == 0)
        return 0;

    /* Return error */
    return -1;
}


/**
 *  @b Description
 *  @n  
 *       The function reads data from the BLF net module.
 *
 *  @param[in]  ptr_buf
 *      This points to the data buffer which needs to be populated
 *      with the received data.
 *
 *  @param[in]  num_bytes
 *      This is the number of bytes of data which need to be read.
 *
 *  @retval
 *      Success -   0
 *  @retval
 *      Error   -   <0
 */
static Int32 net_read (Uint8* ptr_buf, Uint32 num_bytes)
{
    return (net_read_peek (ptr_buf, num_bytes, NET_READ));
}


/**
 *  @b Description
 *  @n  
 *       The function peeks data from the BLF net module.
 *
 *  @param[in]  ptr_buf
 *      This points to the data buffer which needs to be populated
 *      with the received data.
 *
 *  @param[in]  num_bytes
 *      This is the number of bytes of data which need to be read.
 *
 *  @retval
 *      Success -   0
 *  @retval
 *      Error   -   <0
 */
static Int32 net_peek (Uint8* ptr_buf, Uint32 num_bytes)
{
    return (net_read_peek (ptr_buf, num_bytes, NET_PEEK));
}


/**
 *  @b  Description
 *          Read data until the transfer is done
 */
#define MIN(a,b)         ((a) < (b)) ? (a) : (b)
void net_complete_transfer (void)
{
    Int32 dataSize;
    Int32 n;
    uint8 buf[16];

    do  {

        dataSize = stream_level();

        if (dataSize > 0)  {

            while (dataSize > 0)  {

                n = MIN(dataSize, sizeof(buf));
             net_read (buf, n);
             dataSize = dataSize - n;
         }

        } else if (dataSize == 0)  {

            net_peek (buf, 1);
    
        }

    } while (dataSize >= 0);

}




/**
 *  @b  Description
 *  @n
 *      This function moves the read pointer in the stream. 
 *      Because this is a tftp boot, only forward reads
 *      are permitted.
 *
 *  @param[in] loc
 *      This points to where the stream should be
 *
 *  @param[in] from
 *      This describes parameter loc.
 *      0 = from the start of the file
 *      1 = from the current position
 *      2 = from the end of the file
 *
 *  @retval
 *      Success   - 0
 *  @retval
 *      Error     - <0
 */
static Int32 net_seek (Int32 loc, Int32 from)
{
    Uint32      num_bytes;
    Int32       num_bytes_read       = 0;
    Int32       total_num_bytes_read = 0;
    Uint32      desiredPos;


    /* This driver can only seek forward, and cannot seek from the end of the file */
    if (from == 0)
        desiredPos = loc;
    else if (from == 1)
        desiredPos = netmcb.fileOffset + loc;
    else
        return (-1);

    /* Check if already in the correct position */
    if (desiredPos == netmcb.fileOffset)
        return (0);

    /* To seek backwords the current tftp transfer is completed,
     * and then restarted */
    if (desiredPos < netmcb.fileOffset)   {

        /* Complete the transfer */
        net_complete_transfer ();

        /* Reset the current file offset */
        netmcb.fileOffset = 0;

        /* Re-request the data file */
        tftp_get_file (netmcb.net_device.server_ip, netmcb.net_device.file_name);

    }
        

    /* Read data from the file until the file position matches the desired one */
    num_bytes = desiredPos - netmcb.fileOffset;


    /* Execute the network scheduler; till there is no error. */
    while (netmcb.error_flag == 0) 
    {
        /* Call the timer scheduler. */
        timer_run();

        /* Check if there is data in the STREAM? */
        if (stream_isempty() == FALSE)
        {
            /* STREAM indicates there is some data. Lets read it first. */
            num_bytes_read    = stream_read (NULL, num_bytes);
            netmcb.fileOffset = netmcb.fileOffset + num_bytes_read;

            /* Keep track of the total amount of data read till now. */
            total_num_bytes_read = total_num_bytes_read + num_bytes_read;

            /* How much more data do we need to read? */ 
            num_bytes = num_bytes - num_bytes_read;

            /* Check if we have read all the data that was requested? */
            if (num_bytes == 0)
                break;
    
            /* Control comes here implies that there was more data to be read into
             * the buffer. We need it to have it available before we can exit the loop.
             * So lets fall through! */
        }
        else
        {
            /* STREAM Module is empty. */
        }

        /* Check for and process any received packets */
        proc_packet ();

    }

    /* Did we come out because of error or not? */
    if (netmcb.error_flag == 0)
        return 0;

    /* Return error */
    return -1;
}

/**
 *  @b  Description
 *  @n
 *      This function returns how many bytes of data
 *      are currently available for immediate read.
 *
 *  @retval
 *      The number of bytes available
 */
static Int32 net_query (void)
{
    return (stream_level());
}
    

