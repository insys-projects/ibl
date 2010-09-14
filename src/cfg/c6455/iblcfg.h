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
 *	 Build time configurations for the c6455 ibl are defined
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






#endif


