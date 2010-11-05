/** 
 *   @file  bis.h
 *
 *   @brief   
 *      The file has definitions for the BIS specification. 
 *
 *  \par
 *  NOTE:
 *      (C) Copyright 2008, Texas Instruments, Inc.
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
 *  \par
*/
#ifndef __BIS_H__
#define __BIS_H__

/* Boot Image Script (BIS) Magic number definition  
 * Every boot image should begin with TIBI (TI Boot Image) */
#define BIS_MAGIC_NUMBER        (0x54494249u)   // 'T','I','B','I'

/* Boot Image Script (BIS) Command Definitions
 * Every BIS command will read 'B','I','S',XXh, where XXh is the command number */
#define BIS_CMD_PREFIX            (0x42495300u)   // 'B','I','S',xxx
#define BIS_CMD_MASK              (0xFFFFFF00u)
#define BIS_CMD_SECTION_LOAD      (0x42495301u)
#define BIS_CMD_FUNCTION_LOAD     (0x42495302u)
#define BIS_CMD_FUNCTION_EXEC     (0x42495303u)
#define BIS_CMD_MEMORY_ACCESS     (0x42495304u)
#define BIS_CMD_SECTION_TERMINATE (0x424953FFu)


Int32 iblBootBis (BOOT_MODULE_FXN_TABLE *bootFxn, Uint32* entry_point);

#endif  /* __BIS_H__ */
