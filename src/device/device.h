/**********************************************************************************
 * FILE PURPOSE: Define the device interface
 **********************************************************************************
 * FILE NAME: device.h
 *
 * DESCRIPTION: This file defines the device wrapper used by IBL modules
 *
 * @file device.h
 *
 * @brief
 *  The device wrapper API is defined
 *
 ***********************************************************************************/
#ifndef DEVICE_H 
#define DEVICE_H

#include "types.h"


/* The target specific file is pointed to via make control. There will be a different
   target.h file for each platform */
#include "target.h"
 
 
 
/**
 * @brief
 *    Fill in the ibl structure with default values
 *
 * @details
 *    If the ibl table is not pre-loaded during the initial boot, then
 *    each device will load a default table.
 */
void deviceLoadDefaultIblTable(void);


/**
 * @brief
 *   Convert a local address to a global address
 *
 *  @details
 *    a local memory address (l1d, l1p, l2) is converted to a global address
 */
Uint32 deviceLocalAddrToGlobal (Uint32 local);


/**
 * @brief Configure the PLLs
 *
 * @details
 *   The three PLLs are enabled. Only the main PLL has the ability to configure
 *   the multiplier and dividers.
 */
void devicePllConfig (void);


/**
 * @brief
 *    Power up a peripheral
 *
 * @details
 *    The specified module (and domain if required) is power up
 */
int32 devicePowerPeriph (int32 modNum);


/**
 * @brief Enable the DDR controller
 *
 * @details
 *   The DDR controller is enabled (optionally)
 */
void deviceDdrConfig (void);


/**
 * @brief
 *    Perform device level configuration for nand boot
 *
 * @detials
 *    Board/chip specific initializations
 */
int32 deviceConfigureForNand(void);

/**
 *  @brief
 *      Load the default ethernet address
 *
 *  @details
 *      The mac address from e-fuse is loaded
 */
void deviceLoadDefaultEthAddress (uint8 *maddr);

/**
 *  @brief
 *      Reset the ethernet controller
 *
 *  @details
 *      Some devices require an external ethernet reset before configuration
 */
#ifndef deviceSetEthResetState
void deviceSetEthResetState (int32 portNum, BOOL applyReset);
#endif
 
 /**
  *  @brief
  *     Return the endian status
  *
  *  @details
  *     Returns true if the device is executing in little endian mode
  */
bool deviceIsLittleEndian(void);

#endif

