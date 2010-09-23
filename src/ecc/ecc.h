/*******************************************************************************************
 * FILE PURPOSE: Define the ECC api used by all ECC algorithms
 *******************************************************************************************
 * FILE NAME: ecc.h
 *
 * DESCRIPTION: The API used by all software CRCs is defined
 *
 * @file ecc.h
 *
 * @brief
 *  The API used by all software CRCs is defined
 *
 *******************************************************************************************/
#include "types.h"


#define ECC_SUCCESS     0
#define ECC_FAIL       -1

Int32 eccCorrectData(Uint8 *puchData, Uint8 *puchEccRead, Uint8 *puchEccCalc);
Int32 eccComputeECC(const Uint8 *puchData, Uint8 *puchEccCode);
Int32 eccNumBytes(void);
Int32 eccBytesPerBlock (void);
