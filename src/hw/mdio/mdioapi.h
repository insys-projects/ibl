#ifndef MDIO_API_H
#define MDIO_API_H
/*************************************************************************************
 * FILE PURPOSE: Define the API to the mdio driver
 *************************************************************************************
 * FILE NAME: mdioapi.h
 *
 * DESCRIPTION: The MDIO api is defined
 *
 * @file mdioapi.h
 *
 * @brief
 *	The MDIO api is defined
 **************************************************************************************/
 
void hwMdio (int16 nAccess, uint32 *access, uint16 clkdiv, uint32 delayCpuCycles);




#endif /* MDIO_API_H */
