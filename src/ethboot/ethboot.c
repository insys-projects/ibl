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

    memcpy (iblStatus.ethParams.hwAddress, netdev->mac_address, sizeof(iblStatus.ethParams.hwAddress));
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



    /* Power up the device. No action is taken if the device is already powered up */
    if (devicePowerPeriph (TARGET_PWR_ETH(ibl.ethConfig[eIdx].port)) < 0)
        return;

    /* Do any mdio configuration */
    if (ibl.mdioConfig.nMdioOps > 0)
        hwMdio (ibl.mdioConfig.nMdioOps, ibl.mdioConfig.mdio, 
                ibl.mdioConfig.mdioClkDiv, ibl.mdioConfig.interDelay);


    /* SGMII configuration. If sgmii is not present this statement is defined
     * to void in target.h */
    hwSgmiiConfig (ibl.ethConfig[eIdx].port, &ibl.sgmiiConfig[eIdx]);


    nDevice.port_num = ibl.ethConfig[eIdx].port;

    /* Simple transation to initialize the driver */
    memcpy (nDevice.mac_address, ibl.ethConfig[eIdx].ethInfo.hwAddress, sizeof(nDevice.mac_address));

    nl = FORM_IPN(ibl.ethConfig[eIdx].ethInfo.ipAddr);
    if (ibl.ethConfig[eIdx].doBootp == TRUE)
        nDevice.ip_address = 0;
    else
        nDevice.ip_address = htonl(nl);

    nl = FORM_IPN(ibl.ethConfig[eIdx].ethInfo.netmask);
    nDevice.net_mask  = htonl(nl);

    nl = FORM_IPN(ibl.ethConfig[eIdx].ethInfo.serverIp);
    nDevice.server_ip           = htonl(nl);
    nDevice.use_bootp_server_ip = ibl.ethConfig[eIdx].useBootpServerIp;

    /* Note - the file name structure in nDevice is only 64 bytes, but 128 in ethInfo */
    memcpy (nDevice.file_name, ibl.ethConfig[eIdx].ethInfo.fileName, sizeof(nDevice.file_name));
    nDevice.use_bootp_file_name = ibl.ethConfig[eIdx].useBootpFileName;


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

    format = ibl.ethConfig[eIdx].bootFormat;

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

    entry = iblBoot (&net_boot_module, format, &ibl.ethConfig[eIdx].blob);


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

    if (entry != 0)  {

        iblStatus.exitAddress = entry;
        exit = (void (*)())entry;
        (*exit)();

    }


}
















