/******************************************************************************************
 * FILE PURPOSE: Mac sliver driver
 ******************************************************************************************
 * FILE NAME: gmacsl.c
 *
 * DESCRIPTION: The cpgmac sliver driver
 ******************************************************************************************/
#include "types.h"
#include "cpmacdrv.h"
#include "gmacsl_loc.h"
#include "gmacsl_api.h"
#include "ibl.h"
#include "iblcfg.h"
#include "iblloc.h"
#include "device.h"


/********************************************************************************************
 * FUNCTION PURPOSE: Reset the the gmac sliver
 ********************************************************************************************
 * DESCRIPTION: Soft reset is set and polled until clear, or until a timeout occurs
 ********************************************************************************************/
SINT16 hwGmacSlReset (UINT16 port)
{
    UINT32 i;
    UINT32 v;

    if (port >= DEVICE_N_GMACSL_PORTS)
        return (GMACSL_RET_INVALID_PORT);

    /* Set the soft reset bit */
    DEVICE_REG32_W (DEVICE_EMACSL_BASE(port) + CPGMACSL_REG_RESET, CPGMAC_REG_RESET_VAL_RESET);

    /* Wait for the bit to clear */
    for (i = 0; i < DEVICE_EMACSL_RESET_POLL_COUNT; i++)  {

        v = DEVICE_REG32_R (DEVICE_EMACSL_BASE(port) + CPGMACSL_REG_RESET);
        if ( (v & CPGMAC_REG_RESET_VAL_RESET_MASK) != CPGMAC_REG_RESET_VAL_RESET)
            return (GMACSL_RET_OK);

    }

    /* Timeout on the reset */
    return (GMACSL_RET_WARN_RESET_INCOMPLETE);

} /* hwGmacSlReset */


/*******************************************************************************************
 * FUNCTION PURPOSE: Configure the gmac sliver
 *******************************************************************************************
 * DESCRIPTION: The emac sliver is configured.
 *******************************************************************************************/
SINT16 hwGmacSlConfig (UINT16 port, hwGmacSlCfg_t *cfg)
{
    UINT32 v;
    UINT32 i;
    SINT16 ret = GMACSL_RET_OK;


    if (port >= DEVICE_N_GMACSL_PORTS)
        return (GMACSL_RET_INVALID_PORT);

    if (cfg->maxRxLen > CPGMAC_REG_MAXLEN_LEN)  {
        cfg->maxRxLen = CPGMAC_REG_MAXLEN_LEN;
        ret = GMACSL_RET_WARN_MAXLEN_TOO_BIG;
    }

    /* Must wait if the device is undergoing reset */
    for (i = 0; i < DEVICE_EMACSL_RESET_POLL_COUNT; i++)  {

        v = DEVICE_REG32_R (DEVICE_EMACSL_BASE(port) + CPGMACSL_REG_RESET);
        if ( (v & CPGMAC_REG_RESET_VAL_RESET_MASK) != CPGMAC_REG_RESET_VAL_RESET)
            break;

    }

    if (i == DEVICE_EMACSL_RESET_POLL_COUNT)
        return (GMACSL_RET_CONFIG_FAIL_RESET_ACTIVE);



    
    DEVICE_REG32_W(DEVICE_EMACSL_BASE(port) + CPGMACSL_REG_MAXLEN, cfg->maxRxLen);

    DEVICE_REG32_W(DEVICE_EMACSL_BASE(port) + CPGMACSL_REG_CTL, cfg->ctl);

    return (ret);

} /* hwGmacSlConfig */

    

Int32 cpmac_drv_start (NET_DRV_DEVICE* ptr_device)
{
    Int32         i;
    hwGmacSlCfg_t cfg;


    cfg.maxRxLen = MAX_SIZE_STREAM_BUFFER;
    cfg.ctl      = GMACSL_ENABLE | GMACSL_RX_ENABLE_EXT_CTL;

    if (ptr_device->port_num == ibl_PORT_SWITCH_ALL)  {

        for (i = 0; i < TARGET_EMAC_N_PORTS; i++)  {
            hwGmacSlReset  (i);
            hwGmacSlConfig (i, &cfg);
        }

    }  else  {

        hwGmacSlReset (ptr_device->port_num);
        hwGmacSlConfig (ptr_device->port_num, &cfg);
    
    }

    return (0);

}

Int32 cpmac_drv_send (NET_DRV_DEVICE* ptr_device, Uint8* buffer, int num_bytes)
{
    return (targetMacSend ((void *)ptr_device, buffer, num_bytes));
}



Int32 cpmac_drv_receive (NET_DRV_DEVICE* ptr_device, Uint8* buffer)
{
   return (targetMacRcv ((void *)ptr_device, buffer));

}


Int32 cpmac_drv_stop (NET_DRV_DEVICE* ptr_device)
{

    hwGmacSlReset (ptr_device->port_num);
    return (0);
}
