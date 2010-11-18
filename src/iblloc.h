/****************************************************************************
 * FILE PURPOSE: Local inter-module definitions for the IBL
 ****************************************************************************
 * FILE NAME: iblloc.h
 *
 * DESCRIPTION: Defines global structures used throughout the IBL
 *
 * @file iblloc.h
 *
 * @brief
 *    Provides global inter-module definitions
 *
 *****************************************************************************/
#ifndef IBLLOC_H
#define IBLLOC_H

#include "types.h"

typedef struct BOOT_MODULE_FXN_TABLE
{
    /**
     * @brief   The API is called by the kernel to open the specific
     * boot module. This is used by boot module to initialize themselves
     * and the peripheral drivers.
     */
    Int32 (*open)(void* ptr_driver, void (*asyncComplete)(void *));

    /**
     * @brief   The API is called by the kernel to close the boot module
     * Boot Modules use this API and to close themselves and the peripheral 
     * drivers.
     */
    Int32 (*close)(void);

    /**
     * @brief   The API is called by the kernel to read a specific number
     * of bytes from the boot module and to populate the data buffer.
     */
    Int32 (*read)(Uint8* ptr_buf, Uint32 num_bytes);

    /**
     * @brief   The API is called by the kernel to write a specific number
     * of bytes to the boot module from the data buffer. This is *optional*
     */
    Int32 (*write)(Uint8* ptr_buf, Uint32 num_bytes);

    /**
     * @brief   This API is called by the kernal to peek at some bytes
     * without removing them from the buffer
     */
    Int32 (*peek)(Uint8 *ptr_buf, Uint32 num_bytes);
    
    /**
     * @brief   This API is called by the kernel to move the read/write
     *          pointers in the buffer. The values of parameter from
     *          match those of fseek - 0 = from start of file,
     *          1 = from current position, 2 = from end of file.
     *          Returns 0 if the seek is successful, -1 if it failed.
     */
    Int32 (*seek)(Int32 loc, Int32 from);
    
   /**
    * @brief    This API is called by the kernel to determine how much data
    *           is currently available for immediate read. Returns -1 if
    *           the stream has been closed.
    */
   Int32 (*query)(void);
    
    
}BOOT_MODULE_FXN_TABLE;


/* Prototypes */
Uint32 iblBoot (BOOT_MODULE_FXN_TABLE *bootFxn, int32 dataFormat, void *formatParams);

/* Stdlibs referenced through functions to use a single copy in the two stage boot */
void *iblMalloc (Uint32 size);
void  iblFree   (void *mem);
void *iblMemset (void *mem, Int32 ch, Uint32 n);
void *iblMemcpy (void *s1, const void *s2, Uint32 n);

/* squash printfs */
void mprintf(char *x, ...);

#endif /* _IBLLOC_H */
