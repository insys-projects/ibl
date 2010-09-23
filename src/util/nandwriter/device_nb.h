#ifndef _DEVICE_NB_H
#define _DEVICE_NB_H
/**
 *  @file device_nb.h
 *
 *  @brief
 *		Device specific functions
 */


#include "gpio.h"
 
/**
 * @brief
 *    Initial setup for nand operation
 */
#ifdef c6472

#define NAND_MODE_GPIO		GPIO_14

inline void deviceConfigureForNand(void)
{
	hwGpioSetDirection(NAND_MODE_GPIO, GPIO_OUT);
	hwGpioSetOutput(NAND_MODE_GPIO);
} 
#endif





#endif /* _DEVICE_NB_H */



