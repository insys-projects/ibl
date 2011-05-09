/*
 *
 * Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/ 
 * 
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
*/



/*********************************************************************************
 * FILE PURPOSE: The Ethernet boot wrapper
 *********************************************************************************
 * FILE NAME: ethboot.c
 *
 * DESCRIPTION: This file provides the ethernet boot wrapper used by IBL modules
 *
 * @file ethboot.c
 *
 * @brief
 *   The ethernet boot wrapper
 *
 ***********************************************************************************/
#include "types.h"
#include "ibl.h"
#include "iblloc.h"
#include "ethboot.h"
#include "net.h"
#include "cpmacdrv.h"
#include "sgmii.h"
#include "device.h"
#include "mdioapi.h"
#include <string.h>
#include "net_osal.h"
#include "cpsw_api.h"
#include "qm_api.h"
#include "cpdma_api.h"

/**
 *  @brief Remove the possible re-definition of iblEthBoot. iblcfg.h defines this to be a void
 *         statement if there is no ethernet boot support. It must be re-enabled for the compile
 */
#ifdef iblEthBoot
 #undef iblEthBoot
#endif

/* Convert an IP address from unsigned char to IPN (uint32) */
#define FORM_IPN(x)     (  (x[0] << 24) | \
                           (x[1] << 16) | \
                           (x[2] <<  8) | \
                           (x[3] <<  0) )

#define UNFORM_IPN(x,y)  (x)[0] = (y) >> 24;  \
                         (x)[1] = (y) >> 16;  \
                         (x)[2] = (y) >>  8;  \
                         (x)[3] = (y) >>  0

#define MIN(a,b)         ((a) < (b)) ? (a) : (b)

static bool have_params;

/* Receive a call back when the boot file name is known */
void ibl_rec_params (void *params)
{
    NET_DRV_DEVICE *netdev = (NET_DRV_DEVICE *)params;

    have_params = TRUE;

    /* Copy the information to the status fields */
    UNFORM_IPN(iblStatus.ethParams.ipAddr, netdev->ip_address);
    UNFORM_IPN(iblStatus.ethParams.serverIp, netdev->server_ip);

    netMemcpy (iblStatus.ethParams.hwAddress, netdev->mac_address, sizeof(iblStatus.ethParams.hwAddress));
    strncpy (iblStatus.ethParams.fileName, netdev->file_name, sizeof(iblStatus.ethParams.fileName));

}


void iblEthBoot (Int32 eIdx)
{
    NET_DRV_DEVICE nDevice;
    Uint32  nl;
    Uint32  entry;
    Int32   n;
    Int32   dataSize;
    Int32   format;
    void    (*exit)();
    uint8   buf[16];
    char    *ext;
    unsigned int i,j;

    /* Power up the device. No action is taken if the device is already powered up */
    if (devicePowerPeriph (TARGET_PWR_ETH(ibl.bootModes[eIdx].port)) < 0)
        return;

    /* Do any mdio configuration */
    if (ibl.mdioConfig.nMdioOps > 0)
        hwMdio (ibl.mdioConfig.nMdioOps, ibl.mdioConfig.mdio, 
                ibl.mdioConfig.mdioClkDiv, ibl.mdioConfig.interDelay);

    for (j = 0; j < 0x100; j++)
    	for (i = 0; i < 0x1000000; i++);

    /* SGMII configuration. If sgmii is not present this statement is defined
     * to void in target.h */
    for (n = 0; n < ibl_N_ETH_PORTS; n++)  {
        if (ibl.sgmiiConfig[n].configure == TRUE)
            hwSgmiiConfig (n, &ibl.sgmiiConfig[n]);
    }


#ifdef DEVICE_CPSW
    /* On chip switch configuration */
    hwCpswConfig (targetGetSwitchCtl(), targetGetSwitchMaxPktSize());
#endif


#ifdef DEVICE_QM
    /* Queue manager configuration */
    hwQmSetup ((qmConfig_t *)(targetGetQmConfig()));
    targetInitQs ();
#endif
    

#ifdef DEVICE_CPDMA
    /* Cpdma configuration. */
    hwCpdmaRxConfig ((cpdmaRxCfg_t *)targetGetCpdmaRxConfig());
    hwCpdmaTxConfig ((cpdmaTxCfg_t *)targetGetCpdmaTxConfig());
#endif


#ifdef DEVICE_PA
    /* Packet accelerator configuration. If PA is not present this statement is defined
     * to void in target.h */
    targetPaConfig(ibl.bootModes[eIdx].u.ethBoot.ethInfo.hwAddress);
#endif


#ifdef DEVICE_SS
    /* Streaming switch configuration. If not present this statement is defined to void
     * in target.h.  If present this is usually defined to a series of register writes */
    hwConfigStreamingSwitch();
#endif

    nDevice.port_num = ibl.bootModes[eIdx].port;

    /* Simple transation to initialize the driver */
    netMemcpy (nDevice.mac_address, ibl.bootModes[eIdx].u.ethBoot.ethInfo.hwAddress, sizeof(nDevice.mac_address));

    nl = FORM_IPN(ibl.bootModes[eIdx].u.ethBoot.ethInfo.ipAddr);
    if (ibl.bootModes[eIdx].u.ethBoot.doBootp == TRUE)
        nDevice.ip_address = 0;
    else
        nDevice.ip_address = htonl(nl);

    nl = FORM_IPN(ibl.bootModes[eIdx].u.ethBoot.ethInfo.netmask);
    nDevice.net_mask  = htonl(nl);

    nl = FORM_IPN(ibl.bootModes[eIdx].u.ethBoot.ethInfo.serverIp);
    nDevice.server_ip           = htonl(nl);
    nDevice.use_bootp_server_ip = ibl.bootModes[eIdx].u.ethBoot.useBootpServerIp;

    /* Note - the file name structure in nDevice is only 64 bytes, but 128 in ethInfo */
    netMemcpy (nDevice.file_name, ibl.bootModes[eIdx].u.ethBoot.ethInfo.fileName, sizeof(nDevice.file_name));
    nDevice.use_bootp_file_name = ibl.bootModes[eIdx].u.ethBoot.useBootpFileName;


    nDevice.start    = cpmac_drv_start;
    nDevice.stop     = cpmac_drv_stop;
    nDevice.send     = cpmac_drv_send;
    nDevice.receive  = cpmac_drv_receive;


    /* have_params will be set to true in the tftp call back. It must be
     * set to false before opening the module, since the call back will
     * be from the open call if bootp is not used */
    have_params = FALSE;

    /* Open the network device */
    if ((*net_boot_module.open) ((void *)&nDevice, ibl_rec_params) != 0)
        return;

    /* Wait for the callback with the requested filename */
    while (have_params == FALSE)  {

       if ((*net_boot_module.peek) ((uint8 *)&nl, sizeof(nl)) < 0)  {

            (*net_boot_module.close)();
            return;
        }
    }

    format = ibl.bootModes[eIdx].u.ethBoot.bootFormat;

    /* If the data format was based on the name extension, determine
     * the boot data format */
    if (format == ibl_BOOT_FORMAT_NAME)  {

        ext = strrchr (iblStatus.ethParams.fileName, '.');

        if (ext != NULL)  {

            if (!strcmp (ext, ".bis"))
                format = ibl_BOOT_FORMAT_BIS;

            else if (!strcmp (ext, ".ais"))
                format = ibl_BOOT_FORMAT_BIS;

            else if (!strcmp (ext, ".out"))
                format = ibl_BOOT_FORMAT_COFF;

            else if (!strcmp (ext, ".coff"))
                format = ibl_BOOT_FORMAT_COFF;

            else if (!strcmp (ext, ".btbl"))
                format = ibl_BOOT_FORMAT_BTBL;
            
            else if (!strcmp (ext, ".bin"))
                format = ibl_BOOT_FORMAT_BBLOB;

            else if (!strcmp (ext, ".blob"))
                format = ibl_BOOT_FORMAT_BBLOB;

        }

        /* Name match failed it didn't change */
        if (format == ibl_BOOT_FORMAT_NAME)  {

            iblStatus.nameDetectFailCnt += 1;

            /* Close up the peripheral */
            (*net_boot_module.close)();

            return;
        }

    }


    entry = iblBoot (&net_boot_module, format, &ibl.bootModes[eIdx].u.ethBoot.blob);


    /* Before closing the module read any remaining data. In the coff boot mode the boot may
     * detect an exit before the entire file has been read. Read the rest of the file
     * to make the server happy */

    do  {

        dataSize = (*net_boot_module.query)();  /* Will return -1 when the data is done */

        if (dataSize > 0)  {

            while (dataSize > 0)  {
            
                n = MIN(dataSize, sizeof(buf));
                (*net_boot_module.read)(buf, n);
                dataSize = dataSize - n;
            }

        /* Do not peek if the data size returned in the query was > 0 */
        }  else if (dataSize == 0) {

            (*net_boot_module.peek)(buf, 1);
        }

    } while (dataSize >= 0);



    /* Close up the peripheral */
    (*net_boot_module.close)();


#ifdef DEVICE_PA
    hwPaDisable ();
#endif

#ifdef DEVICE_CPDMA
    /* Cpdma configuration. */
    hwCpdmaRxDisable ((cpdmaRxCfg_t *)targetGetCpdmaRxConfig());
    hwCpdmaTxDisable ((cpdmaTxCfg_t *)targetGetCpdmaTxConfig());
#endif

#ifdef DEVICE_QM
    targetFreeQs ();
    /* Queue manager configuration */
    hwQmTeardown ();
#endif


    if (entry != 0)  {

        iblStatus.exitAddress = entry;
        exit = (void (*)())entry;
        (*exit)();

    }


}
















