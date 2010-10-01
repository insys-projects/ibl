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
 *	 Build time configurations for the c6472 ibl are defined
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
#define IBL_I2C_DEV_FREQ_MHZ            625
#define IBL_I2C_CLK_FREQ_KHZ            100
#define IBL_I2C_OWN_ADDR                10
#define IBL_I2C_CFG_ADDR_DELAY          0x100       /* Delay between sending the address and reading data */

#ifndef IBL_I2C_CFG_EEPROM_BUS_ADDR
 #error hello
 #define IBL_I2C_CFG_EEPROM_BUS_ADDR    0x50
#endif

#define IBL_I2C_CFG_TABLE_DATA_ADDR     (0x10000 - 0x300)
 
 



#endif


