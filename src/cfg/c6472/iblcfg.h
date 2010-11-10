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
 * @brief Estimates of operating parameters. Actual values will be known once they are
 *        read from the i2c.
 */
#define IBL_I2C_DEV_FREQ_MHZ            625
#define IBL_I2C_CLK_FREQ_KHZ            100
#define IBL_I2C_OWN_ADDR                10
#define IBL_I2C_CFG_ADDR_DELAY          0x100       /* Delay between sending the address and reading data */


/**
 *  @brief The default location for the i2c map information can be overridden during make
 */
#ifndef IBL_I2C_MAP_TABLE_DATA_BUS_ADDR
 #define IBL_I2C_MAP_TABLE_DATA_BUS_ADDR 0x50
#endif


#ifndef IBL_I2C_MAP_TABLE_DATA_ADDR
 #define IBL_I2C_MAP_TABLE_DATA_ADDR     0x420
#endif
 



#endif


