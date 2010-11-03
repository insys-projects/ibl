#ifndef __BTBLLOC_H__
#define __BTBLLOC_H__
/*******************************************************************************
 * FILE PURPOSE: Macros and definitions private to BTBL 
 *******************************************************************************
 * FILE NAME: BTBLLOC.h  
 *
 * DESCRIPTION: Defines macros and definitions seen only by the BTBL processing
 *              module
 *
 * (C) Copyright 2004, TELOGY Networks, Inc.
 ******************************************************************************/
/* System level header files */
#include "types.h"

/* BTBL header files */
#include "btbl.h"

/*******************************************************************************
 *  Data Definition :  Define I/O Register Opcode
 ******************************************************************************/
#define BOOT_TBL_IO_CFG_DELAY       0xFFFF
#define BOOT_TBL_IO_CFG_START_CORE  0xFFFE

/*******************************************************************************
 *  Data Definition :  BOOT_TBL_STATE_T
 *******************************************************************************
 *  DESCRIPTION :  
 *    The following definitions describe the boot table processing state.  
 ******************************************************************************/
typedef UINT16 BOOT_TBL_STATE_T;

#define BOOT_TBL_STATE_INIT         0
#define BOOT_TBL_STATE_IO_CNT       1 
#define BOOT_TBL_STATE_IO_REGS      2 
#define BOOT_TBL_STATE_SIZE         3
#define BOOT_TBL_STATE_ADDR         4 
#define BOOT_TBL_STATE_DATA         5 
#define BOOT_TBL_STATE_PAD          6
#define BOOT_TBL_STATE_FLUSH        7 
#define BOOT_TBL_LAST_STATE         BOOT_TBL_STATE_FLUSH
#define BOOT_TBL_NUM_STATES         (BOOT_TBL_LAST_STATE + 1)


/******************************************************************************
 * DATA DEFINITION: Boot Table Procssing Function
 *****************************************************************************/
typedef void (*BootTblProcFcn_t) (BOOT_TBL_CB_T* p_inst);

#endif  /* __BTBLLOC_H__ */

/* Nothing past this point */
