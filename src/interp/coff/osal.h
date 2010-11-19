/*
 *
 * Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/ 
 * 
 * 
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions 
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the   
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/



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
#define mem_copy(dest,src,nbytes)           iblMemcpy((void *)(dest),(void *)(src),nbytes)
#define mem_write(buf, nbytes, addr, page)  iblMemcpy((void *)(addr),(void *)(buf),nbytes)
#define str_comp                            strcmp
#define strn_copy                           strncpy

#define malloc                              iblMalloc
#define free                                iblFree


/***********************
 * Function Prototypes
 **********************/
char* strn_copy(char* dst, const char* src, unsigned int n);
int str_comp(const char *s1, const char *s2);

#endif /*OSAL_H_*/
