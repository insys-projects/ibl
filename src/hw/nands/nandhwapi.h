#ifndef _NAND_HW_API_H
#define _NAND_HW_API_H
/****************************************************************************************
 * FILE PURPOSE: Define the nand api which applies to all nand drivers
 ****************************************************************************************
 * FILE NAME: nandhwapi.h
 *
 * DESCRIPTION: Defines the API used by all the low level nand drivers
 *
 *  @file nandhwapi.h
 *
 *  @brief
 *		Defines the API used by all the low level nand drivers
 *
 ****************************************************************************************/
 
/* Return values */
#define NAND_INVALID_ADDR_SIZE      -810
#define NAND_NULL_ARG               -811
#define NAND_INVALID_ADDR           -812
#define NAND_ECC_FAILURE            -813


/* Driver functions */
Int32 nandHwDriverInit (nandDevInfo_t *devInfo);
Int32 nandHwDriverReadBytes (Uint32 block, Uint32 page, Uint32 byte, Uint32 nbytes, Uint8 *data);
Int32 nandHwDriverReadPage(Uint32 block, Uint32 page, Uint8 *data);
Int32 nandHwDriverClose (void);





#endif /* _NAND_HW_API_H */
