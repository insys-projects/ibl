/**
 *   @file  tftp.c
 *
 *   @brief   
 *      The file implements the NET Module TFTP functionality.
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
 *  The structure describes the TFTP State
 *
 * @details
 *  The TFTP client can be in one of the following states which are
 *  described in this structure.
 */
typedef enum TFTP_STATE
{
    /**
     * @brief   This is the initial state of the TFTP client during startup
     * In this state the TFTP client has sent out the READ Request and has
     * not yet received a data packet in acknowledgment.
     */        
    READ_REQUEST    = 0x1,

    /**
     * @brief   This is the data receive state of the TFTP client in which
     * the TFTP client is receiving data packets from the TFTP server.
     */    
    DATA_RECEIVE
}TFTP_STATE;

/**
 * @brief 
 *  The structure describes the TFTP Master Control Block.
 *
 * @details
 *  The TFTP Master control block stores information used by the
 *  TFTP module.
 */
typedef struct TFTP_MCB
{
    /**
     * @brief   This describes the state of the TFTP client.
     */
    TFTP_STATE   state;

    /**
     * @brief   This is the IP Address of the server from where the file
     * is downloaded.
     */
    IPN         server_ip;

    /**
     * @brief   This is the TFTP socket which is used to communicate
     * with the UDP module.
     */
    Int32        sock;

    /**
     * @brief   This is the TFTP Timer handle which is used to handle
     * retransmissions of the READ REQUEST.
     */
    Int32       timer;

    /**
     * @brief   This is the name of the file which is being downloaded.
     * File Names are typically exchanged through the BOOTP protocol 
     * where the max file name length is 64. 
     */    
    Uint8       filename[64];

    /**
     * @brief   This is the block number we expect.
     */
    Uint16      block_num;

    /**
     * @brief   This is a generic buffer used by the TFTP module,
     */
    Uint8       buffer[TFTP_DATA_SIZE + TFTPHEADER_SIZE];

    /**
     * @brief   Number of retransmission done.
     */
    Uint32      num_retransmits;
}TFTP_MCB;

/**********************************************************************
 *************************** GLOBAL Variables *************************
 **********************************************************************/

/**
 * @brief   This is the global master control block for the TFTP
 * module and keeps track of all the information regarding TFTP. 
 */
TFTP_MCB   tftpmcb;

/**********************************************************************
 **************************** TFTP Functions **************************
 **********************************************************************/

/**
 *  @b Description
 *  @n  
 *      The function cleans up the TFTP Client. This can be called 
 *      on an ERROR or SUCCESSFUL exit.
 *
 *  @retval
 *      Not Applicable.
 */
static void tftp_cleanup (void)
{
    /* Close any open timers. */
    timer_delete (tftpmcb.timer);
    tftpmcb.timer = -1;

    /* Close the UDP Sockets. */
    udp_sock_close (tftpmcb.sock);

    /* Close the STREAM module */
    stream_close ();
    return;        
}

/**
 *  @b Description
 *  @n  
 *      The function creates the TFTP read request and populates it 
 *      in the internal TFTP buffer.
 *
 *  @retval
 *      Size of the TFTP Read Request.
 */
static Int32 tftp_create_read_req (Uint8* filename)
{
    Uint16* ptr_op;
    Int32   index = 0;

    /* Create the Read Request: Populate the Request op type */
    ptr_op = (Uint16 *)&tftpmcb.buffer[0];
    *ptr_op = htons(TFTP_OPCODE_RRQ);

    /* Copy the file name */
    index = 0;
    while (filename[index] != 0)
    {
        tftpmcb.buffer[index + 2] = filename[index];
        index++;
    }

    /* Null Terminate the file name. */
    tftpmcb.buffer[index + 2] = 0;
    index = index + 3;

    /* Copy the transfer mode. */
    tftpmcb.buffer[index++] = (Uint8)'o';
    tftpmcb.buffer[index++] = (Uint8)'c';
    tftpmcb.buffer[index++] = (Uint8)'t';
    tftpmcb.buffer[index++] = (Uint8)'e';
    tftpmcb.buffer[index++] = (Uint8)'t';
    tftpmcb.buffer[index++] = (Uint8)0;

    /* Return the size of the read request */
    return index;
}

/**
 *  @b Description
 *  @n  
 *      The function is used to send an ACK back to TFTP Server.
 *
 *  @retval
 *      Not Applicable
 */
static void tftp_send_ack(void) 
{
    TFTPHDR* ptr_tftphdr;

    /* Initialize the data buffer. */ 
    netMemset ((void *)&tftpmcb.buffer[0], 0, TFTP_DATA_SIZE + TFTPHEADER_SIZE);

    /* Create an ACK packet which is to be sent out. Get the pointer to the
     * TFTP Header. */
    ptr_tftphdr = (TFTPHDR *)&tftpmcb.buffer[0];
    ptr_tftphdr->opcode = htons (TFTP_OPCODE_ACK);
    ptr_tftphdr->block  = htons (tftpmcb.block_num);

    /* The packet has been populated; send it to the server; this transfer is now done
     * over the data socket. */
    udp_sock_send (tftpmcb.sock, (Uint8 *)ptr_tftphdr, TFTPHEADER_SIZE);

    /* Increment the block number. */
    tftpmcb.block_num++;
    return;
}

/**
 *  @b Description
 *  @n  
 *      The function is the timer expiration for TFTP.
 *
 *  @retval
 *      Not Applicable
 */
static void tftp_timer_expiry (void)
{
    Int32 len;

    /* Determine the state of the TFTP. */
    if (tftpmcb.state == READ_REQUEST)
    {
        /* We were sending out READ Request and have not received a response 
         * Increment the number of retransmissions which have been done. */
        tftpmcb.num_retransmits++;

        /* Check if we have exceeded the max allowed? */
        if (tftpmcb.num_retransmits > MAX_TFTP_RETRANSMITS)
        {
            /* FATAL Error: We need to close the TFTP Client and signal Error to the NET Boot Module. */
            mprintf ("Error: TFTP READ REQ Retransmissions have exceeded\n");
            tftp_cleanup ();
            net_set_error();
            return;
        }

        /* Create the TFTP Read Request */
        len = tftp_create_read_req (&tftpmcb.filename[0]);

        /* Send out the READ Request again. */
        udp_sock_send (tftpmcb.sock, (Uint8 *)&tftpmcb.buffer[0], len);
    }
    else
    {
        /* We were receiving data from the TFTP Server and there was a timeout. This can 
         * happen only if we have not received any data from the TFTP server. */
        mprintf ("Error: TFTP server is down; no packet received.\n");
        tftp_cleanup();
        net_set_error();
        return;
    }
    return;
}

/**
 *  @b Description
 *  @n  
 *      This is a call back function registered with the UDP module to 
 *      be invoked when a TFTP packet is received.
 *
 *  @param[in]  sock
 *      This is the socket handle on which packet was received.
 *  @param[in]  ptr_data
 *      This is the pointer to the TFTP data payload.
 *  @param[in]  num_bytes
 *      This is the number of bytes of TFTP data received.
 *
 *  @retval
 *      Success -   0
 *  @retval
 *      Error   -   <0
 */
static Int32 tftp_receive (Int32 sock, Uint8* ptr_data, Int32 num_bytes)
{
    TFTPHDR*    ptr_tftphdr;
    UDPHDR*     ptr_udphdr;
    Uint16      src_port;
    SOCKET      socket;

    /* Get the pointer to the TFTP Header. */
    ptr_tftphdr = (TFTPHDR *)ptr_data;

    /* Process the received packet depending on the type of packet received */
    switch (ntohs(ptr_tftphdr->opcode))
    {
        case TFTP_OPCODE_DATA:
        {
            /* Is this the first data packet we have received? */
            if (tftpmcb.state == READ_REQUEST)
            {
                /* YES. In this case the socket on which the request was sent has completed
                 * its job. Lets shut it down and open another one for the data transfers. */
                udp_sock_close (sock);

                /* We need to get the source port from where the data was received. 
                 *  This information is present in the UDP layer. This is required to
                 *  open the data socket. */
                ptr_udphdr = (UDPHDR *)(ptr_data - sizeof(UDPHDR));
                src_port   = ntohs(ptr_udphdr->SrcPort);

                /* Populate the socket structure and register this with the UDP module. */
                socket.local_port       = 1234;
                socket.remote_port      = src_port;
                socket.remote_address   = tftpmcb.server_ip;
                socket.app_fn           = tftp_receive;

                /* Move to the DATA State. */
                tftpmcb.state           = DATA_RECEIVE;
                tftpmcb.num_retransmits = 0;

                /* Close the timer.  */
                timer_delete (tftpmcb.timer);
                tftpmcb.timer = -1;

                /* Open the TFTP data socket. */
                tftpmcb.sock = udp_sock_open (&socket);
                if (tftpmcb.sock < 0)
                {
                    /* Error: Data Socket open failed. */
                    mprintf ("Error: TFTP Data Socket Open Failed\n");
                    tftp_cleanup();
                    net_set_error();
                    return -1;
                }
            }

            /* We are in the DATA State: Start the TFTP Server Keep Alive Timer. This timer
             * keeps track of the TFTP Server and ensures it does not die behind us. This will
             * help detect that error. */
            timer_delete (tftpmcb.timer);
            tftpmcb.timer = timer_add (TFTP_SERVER_TIMEOUT, tftp_timer_expiry);
            if (tftpmcb.timer < 0)
            {
                /* Timer creation failed. */
                mprintf ("Error: TFTP Timer creation failed\n");
                tftp_cleanup();
                net_set_error();
                return -1;
            }

            /* Received a data block. Ensure that the block number matches what we expect! */
            if (ntohs(ptr_tftphdr->block) != tftpmcb.block_num)
            {
                /* There is a block number mismatch. This could occur if the ACK we sent was lost. 
                 * Increment the number of retransmissions. */
                tftpmcb.num_retransmits++;
                if (tftpmcb.num_retransmits > MAX_TFTP_RETRANSMITS)
                {
                    /* OK; we resent the ACK multiple times & the server still kept sending the
                     * same packet back. We just give up now. */
                    mprintf ("Error: TFTP ACK Retransmits Exceeded\n");
                    tftp_cleanup();
                    net_set_error();
                    return -1;
                }

                /* We need to send out the ACK for the previous 'block' */
                tftpmcb.block_num = tftpmcb.block_num - 1;

                /* Send the ACK out again. */
                tftp_send_ack ();

                /* We dont need to process this packet since we had already picked it up. */
                return 0;
            }

            /* The packet looks good. Reset the number of retransmissions. */
            tftpmcb.num_retransmits = 0;

            /* Pass the received data packet to the STREAM Module. 
             *  We need to skip the TFTP Header. */
            if (stream_write ((ptr_data + TFTPHEADER_SIZE), (num_bytes - TFTPHEADER_SIZE)) == 0)
            {
                /* Packet has been copied successfully into the STREAM Buffer. */
                tftp_send_ack();
            }
            else
            {
                /* Packet could not be copied; let the server retransmit the packet
                 * because we did not send the ACK. */
            }

            /* Determine if the TFTP file transfer is complete or not? 
             *  If the received number of bytes is less than the TFTP Data Size this 
             *  indicates that the transfer is successfully completed. */
            if (num_bytes < (TFTP_DATA_SIZE + TFTPHEADER_SIZE))
            {
                /* Successfully downloaded the file */
                tftp_cleanup();
            }
            break;
        }
        default:
        {
            /* Control comes here for ERROR, ACK and WRQ which are all indicate 
             * failure. ERROR packet is the only expected failure which is mentioned
             * in the RFC. All other packets are violation of the RFC. Both these
             * cases are handled similarly. */
            mprintf ("Error: TFTP Error Packet Received\n");
            tftp_cleanup();
            net_set_error();

            /* Return Error. */
            return -1;
        }
    }
    return 0;
}

/**
 *  @b Description
 *  @n  
 *       The function gets a file from the TFTP Server. The function
 *       simply initiates the transfer. Successful completion of this
 *       API does not gurantee that the file was downloaded.
 *
 *  @param[in]  server_ip
 *      TFTP Server IP address from where the file is downloaded.
 *  @param[in]  filename
 *      Name of the file to be downloaded.
 *
 *  @retval
 *      Success -   0
 *  @retval
 *      Error   -   <0
 */
Int32 tftp_get_file (IPN server_ip, Int8* filename)
{
    SOCKET  socket;
    Int32   index = 0;

    /* Basic Validations: Ensure the parameters passed are correct. */
    if ((server_ip == 0) || (filename == NULL))
    {
        /* Error: Invalid parameters passed. Stop the network boot module. */
        net_set_error();
        return -1;
    }

    /* Open the stream module. */
    if (stream_open (TFTP_DATA_SIZE) < 0)
    {
        /* Error: Unable to open the stream device. */
        net_set_error();
        return -1;
    }

    /* Initialize the TFTP MCB at this stage... */
    netMemset ((void *)&tftpmcb, 0, sizeof(TFTP_MCB));

    /* Populate the socket structure and register this with the UDP module. */
    socket.local_port       = 1234;
    socket.remote_port      = TFTP_SERVER_PORT;
    socket.remote_address   = server_ip;
    socket.app_fn           = tftp_receive;

    /* Open the TFTP Control socket. */
    tftpmcb.sock = udp_sock_open (&socket);
    if (tftpmcb.sock < 0)
    {
        /* ERROR: UDP Socket could not be opened. */        
        stream_close();
        net_set_error();
        return -1;
    }

    /* Remember the parameters passed to the TFTP. */
    tftpmcb.server_ip = server_ip;

    /* Copy the file name over...  */
    while (filename[index] != 0)
    {
        tftpmcb.filename[index] = filename[index];
        index++;
    }

    /* Initialize the block number expected. */
    tftpmcb.block_num = 1;

    /* Create the TFTP Read Request. */
    index = tftp_create_read_req ((Uint8 *)filename);

    /* Initialize the TFTP Client state */
    tftpmcb.state = READ_REQUEST;

    /* Initialize the TFTP Timer. */
    tftpmcb.timer = timer_add (TFTP_TIMEOUT, tftp_timer_expiry);
    if (tftpmcb.timer < 0)
    {
        /* Error: TFTP Timer Creation Failed. TFTP is not operational. */
        mprintf ("Error: TFTP Timer Creation Failed\n");
        tftp_cleanup ();
        net_set_error();
        return -1;
    }

    /* The packet has been populated; send it to the server. */
    udp_sock_send (tftpmcb.sock, (Uint8 *)&tftpmcb.buffer[0], index);
 
    /* Send out the TFTP Read request. */
    return 0;
}


