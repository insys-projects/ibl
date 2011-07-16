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


/* Function to get the endian setting of a device */
unsigned int get_device_endian();

/* Function to get the switch setting of a device */
unsigned int get_device_switch_setting();

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


/**
 *  @brief
 *      Provide an approximate delay, in cpu cycles
 *
 *  @details
 *      A delay loop
 */
void chipDelay32 (uint32 nCycles);




/**
 *  @brief
 *      Return the NAND interface function table
 *
 *  @details
 *      Some devices support multiple NAND interface at once (EMIF25 and SPI).
 *      This function returns a pointer to the lower level function
 *      table to use. The table is selected in the ibl configuration.
 */
typedef struct nandCtbl_s  {

    Int32 (*nct_driverInit)      (int32 cs, void *nandDevInfo);
    Int32 (*nct_driverReadBytes) (Uint32 block, Uint32 page, Uint32 byte, Uint32 nbytes, Uint8 *data);
    Int32 (*nct_driverReadPage)  (Uint32 block, Uint32 page, Uint8 *data);
    Int32 (*nct_driverClose)     (void);

} nandCtbl_t;

nandCtbl_t *deviceGetNandCtbl (int32 interface);


/**
 *  @brief
 *      Return the NOR interface function table
 *
 *  @details
 *      NOR is supported through both SPI and EMIF. This function returns a pointer
 *      to the lower level function table to use during boot.
 */
typedef struct norCtbl_s  {

    Int32 (*nct_driverInit)         (int32 cs);
    Int32 (*nct_driverReadBytes)    (Uint8 *data, Uint32 nbytes, Uint32 address);
    Int32 (*nct_driverClose)        (void);
    
} norCtbl_t;

norCtbl_t *deviceGetNorCtbl (int32 interface);
    


/**
 *  @brief
 *      Return the default SPI device configuration
 */
void deviceLoadInitSpiConfig (void *cfg);

#endif
