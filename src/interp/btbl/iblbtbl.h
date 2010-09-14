#ifndef _IBL_BTBL_H
#define _IBL_BTBL_H

/*************************************************************************************************
 * FILE PURPOSE: Define the boot table processor to the IBL
 *************************************************************************************************
 * FILE NAME: iblbtbl.h
 *
 * DESCRIPTION: Defines the API to the IBL boot table processing wrapper
 *
 *************************************************************************************************/
#include "types.h"
#include "iblloc.h"


/* Prototypes */
void iblBootBtbl (BOOT_MODULE_FXN_TABLE *bootFxn, Uint32 *entry_point);





#endif /* _IBL_BTBL_H */
