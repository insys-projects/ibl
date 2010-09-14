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
