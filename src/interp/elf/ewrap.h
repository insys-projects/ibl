#ifndef _EWRAP_H
#define _EWRAP_H
/**
 *  @file  ewrap.h
 *
 *  @brief
 *		Defines the wrapper used to attach the elf loader to the IBL
 */
 
#include "iblloc.h"

#define LOADER_FILE_DESC  BOOT_MODULE_FXN_TABLE


#define TYPE_STACK_DEFINITION(x,y)
#define C60_TARGET                      1
#define DLDBG_add_segment_record(x)
#define DLTMM_malloc(x,y)               (((x)->host_address = (y)->target_address) == 0 ? 1 : 1)
#define DLTMM_free(x)
#define dload_dynamic_segment(x,y)      TRUE
#define DLSYM_copy_globals(x)

extern BOOL DLL_debug;

#endif /* _EWRAP_H */
