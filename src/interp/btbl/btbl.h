#ifndef __BTBL_H__
#define __BTBL_H__
/*******************************************************************************
 * FILE PURPOSE: Defines API and public data definitions for the Boot table (BTBL) 
 *******************************************************************************
 * FILE NAME: BTBL.h  
 *
 * DESCRIPTION: Defines API and public data definitions for boot table 
 *              processing (BTBL) module
 *
 * (C) Copyright 2004, TELOGY Networks, Inc.
 ******************************************************************************/

/* System level header files */
#include "types.h"

#if 0
#include "device.h"
#endif

/* BTBL error codes */
enum {
  BTBL_NOERR                    = 0,
  BTBL_ERR_GEN                  = 1,
  BTBL_ERR_INVALID_STATE        = 2,
  BTBL_ERR_INVALID_SECTION_ADDR = 3,
  BTBL_ERR_INVALID_START_VECTOR = 4
};

/* 
 * We need to reserve some space in the front of the buffer used to store 
 * the partial content of the boot table since the data may be shifted to 
 * achieve the 32-bit data alignment for PDMA transfer during the boot table
 * processing
 */    
#define BOOT_TBL_RESERVED_HEADER_SIZE  CHIP_BTBL_RSVD_SIZE_UINT16     /* in terms of 16-bit word */ 


/*******************************************************************************
 *  Data Definition :  BOOT_TBL_T
 *******************************************************************************
 *  DESCRIPTION :  
 *    This is an instance of a boot table  procssing control block.  
 *    It defines all the variables used during the boot table processing
 ******************************************************************************/
typedef struct boot_tbl_cb_s
{
    UINT16              state;             /* Boot table prossing state      */
    BOOL                f_wait_lsw;        /* TRUE: waiting for the least significant
                                                    16-bit word of the control 
                                                    word such as I/O register
                                                    configuration, code section
                                                    byte count and etc. */ 
    UINT32              code_start_addr;   /* record the entry point byte addr */
    UINT32              num_io_regs;       /* number of the remaining IO registers 
                                              needed to be initialized */
    UINT16              io_addr;           /* Record the address of the IO register
                                              to be processed */                                           
    UINT16              core_start_vector; /* Record the core start vector */
    /* 
     * Record the size and address of the remaining portion of the code section
     * to be programmed
     */                                          
    UINT32              section_size_bytes; /* In bytes */     
    UINT32              section_addr;       /* 32-bit byte address */
    UINT16              last_word;          /* Record the last UINT16 word in the
                                               previous code section */
                                              
                                             
    UINT16*             p_data;            /* Point to the UINT16 data to be 
                                              processed  */
    UINT32              data_size_uint16;  /* the size of the remaing 16-bit
                                              data needs to be processed */
} BOOT_TBL_CB_T;


/*******************************************************************************
 * Public function prototypes
 ******************************************************************************/
/*******************************************************************************
 * FUNCTION PURPOSE: Initialize the boot table control instance
 *******************************************************************************
 * DESCRIPTION: Allocate and initializea boot table control instance 
 *
 * void boot_init_boot_tbl_inst (
 *   void         **p_inst)  - A pointer to Boot Table Control instance
 *
 *****************************************************************************/
void boot_init_boot_tbl_inst(BOOT_TBL_CB_T *inst);

/*******************************************************************************
 * FUNCTION PURPOSE: Process the boot table
 *******************************************************************************
 * DESCRIPTION: Process the whole or partial boot table received by going 
 *              through the boot table processing state machine until all
 *              the UINT16 words are processed or the end of table condition
 *              (section size == 0) is satisfied. 
 *
 * void boot_proc_boot_tbl (
 *   UINT16       *p_data,  - 16-bit boot table content to be processed
 *   UINT32       size)     - number of 16-bit words  
 *
 *
 * Note: It is up to the caller to reserve 4 16-bit word space in front of the
 *       input buffer since the data may be moved to achieve 32-bit alignment
 *       requirement for PDMA transfer
 *****************************************************************************/
void boot_proc_boot_tbl(BOOT_TBL_CB_T *inst, UINT16* p_data, UINT32 size);

/* Generic block boot table information for non-secure boot over a master periheral.
 * The data section of the block contains boot table data */
typedef struct bootTableBlock_s
{
    UINT32 blockLenBytes;
    UINT32 data;
    
} bootTableBlock_t;


#endif  /*  __BTBL_H__ */

/* Nothing past this point */
