/************************************************************************************
 * FILE PURPOSE: C6455 Device Specific functions
 ************************************************************************************
 * FILE NAME: c6455.c
 *
 * DESCRIPTION: Implements the device specific functions for the IBL
 *
 * @file c6455.c
 *
 * @brief
 *  This file implements the device specific functions for the IBL
 *
 ************************************************************************************/
#include "ibl.h"
#include "device.h"


/**
 * @brief The default boot configuration table is filled in
 *
 * @details
 *   A default ibl configuraiton table is provided when one is not found
 *   preloaded.
 */
void deviceLoadDefaultIblTable (void)
{

    ibl.ethConfig.ethPriority = ibl_HIGHEST_PRIORITY;
    ibl.ethConfig.port        = 0;
    ibl.ethConfig.doBootp     = TRUE;
    ibl.ethConfig.bootFormat  = ibl_BOOT_FORMAT_AUTO;

    memset (ibl.ethConfig.ethInfo, 0, sizeof(ibl.ethConfig.ethInfo));
}







