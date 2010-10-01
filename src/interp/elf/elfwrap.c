/**
 *  @file elfwrap.c
 *
 *  @brief
 *		IBL wrapper for the elf loader
 */

#include "types.h"
#include "ibl.h"
#include "iblloc.h"
#include "ewrap.h"
#include "dload_api.h"
#include "ibl_elf.h"
#include "file_ovr.h"

BOOL DLL_debug = FALSE;

/**
 *  @brief
 *	   Load an elf file
 */
void iblBootElf (BOOT_MODULE_FXN_TABLE *bootFxn, Uint32 *ientry_point)
{
  int   argc;
  char **argv;

  DLOAD_load (bootFxn, argc, argv, ientry_point);

}

