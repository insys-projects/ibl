/*****************************************************************************
 * FILE PURPOSE: Define the Ethernet boot wrapper
 *****************************************************************************
 * FILE NAME: ethboot.h
 *
 * DESCRIPTION: This file defines the ethernet wrapper used by IBL modules
 *
 * @file ethboot.h
 *
 * @brief
 *  The ethernet boot wrapper API is defined
 *
 *****************************************************************************/

#ifndef ETHBOOT_H
#define ETHBOOT_H

#include "types.h"

/**
 * @brief
 *    Attempt an enternet boot
 *
 * @details
 *    The IBL will attempt to boot over the ethernet device
 */
void iblEthBoot (int32 eIdx);



#endif /* ETHBOOT_H */




