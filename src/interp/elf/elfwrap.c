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

/**
 *  @brief
 *      Required for the dload module
 */
BOOL DLL_debug = FALSE;

/**
 *  @brief
 *	   Load an elf file
 */
void iblBootElf (BOOT_MODULE_FXN_TABLE *bootFxn, Uint32 *ientry_point)
{
  int   argc;
  char **argv;
  int32_t  dloadHandle;

  dloadHandle = DLOAD_load (bootFxn, argc, argv, ientry_point);

}

/**
 *  @brief
 *      Look for the elf signature in the 1st 4 bytes of a data file
 */
BOOL iblIsElf (Uint8 *elfhdr)
{
  if (  (elfhdr[0] == 0x7f)    &&
        (elfhdr[1] == 'E')     &&
        (elfhdr[2] == 'L')     &&
        (elfhdr[3] == 'F')     )
      return (TRUE);

  return (FALSE);

}




