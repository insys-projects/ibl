#ifndef _PLLAPI_H
#define _PLLAPI_H
/*************************************************************************************
 * FILE PURPOSE: Define the public pll api
 *************************************************************************************
 * FILE NAME: pllapi.h
 *
 * DESCRIPTION: Defines the public api for the pll driver
 *
 * @file  pllapi.h
 *
 * @brief
 *    This file defines the API used by all PLL drivers
 *
 *************************************************************************************/
#include "types.h"

/* Prototypes */
uint16 hwPllResetType (uint32 pllNum);
int16 hwPllSetPll (uint32 pllNum, uint32 prediv, uint32 mult, uint32 postdiv);
int16 hwPllDisable (uint32 pllNum);
int16 hwPllEnable (uint32 pllNum);


/**
 * @def pll_POR_RESET
 */
#define pll_POR_RESET   500     /**< last reset was a power on reset */

/**
 * @def pll_WARM_RESET
 */
#define pll_WARM_RESET  501     /**< last reset was a warm reset */



#endif /* _PLLAPI_H */


