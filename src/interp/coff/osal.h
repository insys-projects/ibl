#ifndef OSAL_H_
#define OSAL_H_


/********************************************************************
 * IBL Adaptions
 *
 *  The ibl will use the file based definition, but define it to be 
 *  directed into the BOOT_MODULE_FXN_TABLE calls.
 *
 ********************************************************************/
#include "iblloc.h"

#define FILE_BASED  1
#define FILE        BOOT_MODULE_FXN_TABLE       /* use the already existing global */


/* Redirect seek and reads */
#define fseek(x,y,z)        (*x->seek)((Int32)(y),(Int32)(z))
#define fread(w,x,y,z)      (((*z->read)((Uint8 *)(w),(Uint32)((x)*(y)))) == 0 ? (y) : 0)

/* Use stdlib functions where possible */
#define mem_copy(dest,src,nbytes)           memcpy((void *)(dest),(void *)(src),nbytes)
#define mem_write(buf, nbytes, addr, page)  memcpy((void *)(addr),(void *)(buf),nbytes)
#define str_comp                            strcmp
#define strn_copy                           strncpy


/***********************
 * Function Prototypes
 **********************/
char* strn_copy(char* dst, const char* src, unsigned int n);
int str_comp(const char *s1, const char *s2);

#endif /*OSAL_H_*/
