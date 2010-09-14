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
uint16 hwPllResetType (uint16 pllNum);
int16 hwPllSetPll (uint16 pllNum, uint16 prediv, uint16 mult, uint16 postdiv);
int16 hwPllDisable (uint16 pllNum);
int16 hwPllEnable (uint16 pllNum);


/**
 * @def pll_POR_RESET
 */
#define pll_POR_RESET   500     /**< last reset was a power on reset */

/**
 * @def pll_WARM_RESET
 */
#define pll_WARM_RESET  501     /**< last reset was a warm reset */



#endif /* _PLLAPI_H */


