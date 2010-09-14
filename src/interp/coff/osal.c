/*****************************************************************************
 * FILE PURPOSE: IBL wrapper for the coff loader
 *****************************************************************************
 * FILE NAME: osal.c
 *
 * DESCRIPTION: Provides a boot wrapper to attach the coff loader 
 *              to the IBL. 
 *
 *****************************************************************************/
#include "types.h"
#include "ibl.h"
#include "iblloc.h"
#include "header.h"


/*****************************************************************************
 * FUNCTION PURPOSE: The main wrapper
 *****************************************************************************
 * DESCRIPTION: Attaches the coff loader to the IBL. The input parameters
 *              are associated with globals defined in cload.c. 
 *****************************************************************************/
void iblBootCoff (BOOT_MODULE_FXN_TABLE *bootFxn, Uint32 *ientry_point)
{
    
    /* Fin is declared as a FILE * in cload.c In osal.h FILE is
     * defined to be BOOT_MODULE_FXN_TABLE, and reads/seeks
     * are defined (in osal.h) to call through the BOOT_MODULE_FXN_TABLE
     * calls. */
    fin = bootFxn;

   /* cload is the main coff loader function. It returns 0 on success.
    * The entry point will be in the global entry_point declared in
    * cload.c */
   if (cload () == 0) 
      *ientry_point = entry_point;
   else
      *ientry_point = 0;

}


/******************************************************************************
 * FUNCTION PURPOSE: Determine if a file could be a coff file
 ******************************************************************************
 * DESCRIPTION: Examines the magic number and returns TRUE if it is one of 
 *              the coff numbers (different numbers for different versions
 ******************************************************************************/
bool iblIsCoff (Uint16 cv)
{
    if (ISCOFF(cv))
        return (TRUE);

    /* Try an endian swap an recheck */
    cv = ((cv >> 8) & 0x00ff) | ((cv << 8) & 0xff00);
    if (ISCOFF(cv))
        return (TRUE);

    return (FALSE);

}

/******************************************************************************
 * This is copied directly from cload_main.c from the original source
 ******************************************************************************/
unsigned int reloc = 0;                 /* RELOCATION AMOUNT               */
int set_reloc_amount()
{
  int i;

  for (i = 0; i<  n_sections; ++i) reloc_amount[i] = reloc;
  return 1;
}


