#ifndef IBLBLOB_H
#define IBLBLOB_H
/*****************************************************************************************
 * FILE PURPOSE: Define the API to the binary blob interpreter
 *****************************************************************************************
 * FILE NAME: iblblob.h
 *
 * DESCRIPTION: The interface to the binary blob interpreter is defined.
 *
 *****************************************************************************************/
#include "types.h"
#include "ibl.h"
#include "iblloc.h"


void iblBootBlob (BOOT_MODULE_FXN_TABLE *bootFxn, Uint32 *entry, void *formatParams);


#endif /* IBLBLOB_H */


