/**************************************************************************
 * FILE PURPOSE: Target specific definitions
 **************************************************************************
 * FILE NAME: target.h
 *
 * DESCRIPTION: This file defines target specific values used by low level
 *				drivers.
 *
 * @file target.h
 *
 * @brief
 *  Low level target specific values are defined
 *
 ***************************************************************************/
 
 
#define EMAC_BASE_ADDRESS       0x02C80000u
#define _EMAC_DSC_BASE_ADDR     0x02c82000u
#define TIMER0_BASE             0x02940000u


#define TIMER_INPUT_DIVIDER     6           /* Timer driven from cpu clock / 6 */


/* Leave mdio disabled */
#define dev_mdio_open()     1






