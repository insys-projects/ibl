#ifndef _NAND_H
#define _NAND_H
/************************************************************************************************
 * FILE PURPOSE: NAND API definitions
 ************************************************************************************************
 * FILE NAME: nand.h
 *
 * DESCRIPTION: Defines the nand api
 *
 * @file nand.h
 *
 * @brief
 *  Defines the nand api.
 *
 **************************************************************************************************/
#include "iblloc.h"

/* All error codes must be negative */
#define NAND_MALLOC_PAGE_FAIL       -400
#define NAND_MALLOC_MAP_LTOP_FAIL   -401
#define NAND_MALLOC_MAP_PTOL_FAIL   -402
#define NAND_MALLOC_BLOCK_INFO_FAIL -403
#define NAND_BAD_BAD_BLOCK_MAGIC    -404
#define NAND_BAD_APP_MAGIC          -405

extern BOOT_MODULE_FXN_TABLE nand_boot_module;


#endif /* _NAND_H */


