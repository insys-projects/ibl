#ifndef _FILE_OVR_H
#define _FILE_OVR_H
/**
 *  @file file_ovr.h
 *
 *  @brief
 *		Overrides the standard file operations (fseek, fread, ftell, fclose).
 *		This file must be included after the standard elf includes
 */
#include <limits.h>


/* Redirect seek and reads */
#define fseek(x,y,z)        (*x->seek)((Int32)(y),(Int32)(z))
#define fread(w,x,y,z)      (((*z->read)((Uint8 *)(w),(Uint32)((x)*(y)))) == 0 ? (y) : 0)
#define ftell(x)            UINT_MAX
#define fclose(x)           0


#define FILE BOOT_MODULE_FXN_TABLE

#endif /* _FILE_OVR_H */
