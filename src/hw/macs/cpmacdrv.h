/**
 *  @file cpmacdrv.h
 *
 *  @brief
 *    Describes the common API wrapper for all ethernet devices
 *
 */
#ifndef CPMACDRV_H
#define CPMACDRV_H
 
#include "types.h"
#include "net.h"


Int32 cpmac_drv_start (NET_DRV_DEVICE* ptr_device);
Int32 cpmac_drv_send (NET_DRV_DEVICE* ptr_device, Uint8* buffer, int num_bytes);
Int32 cpmac_drv_receive (NET_DRV_DEVICE* ptr_device, Uint8* buffer);
Int32 cpmac_drv_stop (NET_DRV_DEVICE* ptr_device);



#endif /* CPMACDRV_H */






