#ifndef _IBL_ELF_H
#define _IBL_ELF_H
/**
 *  @file ibl_elf.h
 *
 *	@brief
 *	  Defines the API to the ibl elf loader
 */
#include "types.h"
#include "ibl.h"
#include "iblloc.h"
 

void iblBootElf (BOOT_MODULE_FXN_TABLE *bootFxn, Uint32 *ientry_point);


#endif /* _IBL_ELF_H */






