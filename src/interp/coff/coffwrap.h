#ifndef COFFWRAP_H
#define COFFWRAP_H
/*****************************************************************************************
 * FILE PURPOSE: Define the coff loader wrapper to the IBL
 *****************************************************************************************
 * FILE NAME: coffwrap.h
 *
 * DESCRIPTION: Defines the application interface to the coff wrapper
 *
 *****************************************************************************************/
#include "types.h"
#include "iblloc.h"


/* The wrapper functions */
void iblBootCoff (BOOT_MODULE_FXN_TABLE *bootFxn, Uint32 *ientry_point);
bool iblIsCoff   (Uint16 cv);





#endif /* COFFWRAP_H */




