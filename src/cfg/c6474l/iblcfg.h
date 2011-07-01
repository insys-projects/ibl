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



/**************************************************************************
 * FILE PURPOSE: Provide build time configurations for the IBL
 **************************************************************************
 * FILE NAME: iblcfg.h
 *
 * DESCRIPTION: Build time configuration
 *
 * @file iblcfg.h
 *
 * @brief
 *	 Build time configurations for the c6474l ibl are defined
 *
 ***************************************************************************/
#ifndef IBLCFG_H
#define IBLCFG_H

/**
 * @brief  The maximum number of UDP sockets in the system
 */
#define MAX_UDP_SOCKET          3


/**
 * @brief The maximum number of timers in the system
 */
#define MAX_TIMER_BLOCKS        5


/**
 * @brief The size in bytes of the internal stream buffer
 */
#define MAX_SIZE_STREAM_BUFFER  1024


/**
 * @brief The maximum number of functions supported for BIS mode
 */
#define MAX_BIS_FUNCTION_SUPPORT    3


/**
 * @brief No I/O sections accepted in boot table format
 */
#define BOOTCONFIG_NO_BTBL_IO

/**
 * @brief The I2C bus address and data address of the ibl table.
 */
#define IBL_CFG_I2C_DEV_FREQ_MHZ            1000
#define IBL_CFG_I2C_CLK_FREQ_KHZ            100
#define IBL_CFG_I2C_OWN_ADDR                10
#define IBL_CFG_I2C_ADDR_DELAY              0x100       /* Delay between sending the address and reading data */

/**
 *  @brief The default location for the i2c map information can be overridden during make
 */
#ifndef IBL_CFG_I2C_MAP_TABLE_DATA_BUS_ADDR
 #define IBL_CFG_I2C_MAP_TABLE_DATA_BUS_ADDR 0x50
#endif

#ifndef IBL_CFG_I2C_MAP_TABLE_DATA_ADDR
 #define IBL_CFG_I2C_MAP_TABLE_DATA_ADDR     0x100
#endif
 
/**
 *  @brief
 *    GPIO pin mapping 
 */
#define NAND_CLE_GPIO_PIN 	GPIO_8     // High: Command Cycle occuring
#define NAND_ALE_GPIO_PIN 	GPIO_9     // High: Address input cycle oddcuring
#define NAND_NWE_GPIO_PIN 	GPIO_10
#define NAND_BSY_GPIO_PIN	GPIO_11     /* NAND Ready/Busy pin */
#define NAND_NRE_GPIO_PIN 	GPIO_12
#define NAND_NCE_GPIO_PIN 	GPIO_13
#define NAND_MODE_GPIO		GPIO_14

/**
 *  @brief
 *      The standard NAND delay must be big enough to handle the highest possible
 *      operating frequency of the device */
#define TARGET_NAND_STD_DELAY				25 // In cpu cycles
#define NAND_WAIT_PIN_POLL_ST_DLY	(10000)

 
#endif


