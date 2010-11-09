/******************************************************************************
 * FILE PURPOSE: Boot TaBLe (btbl) processing file
 ******************************************************************************
 * FILE NAME:   btblpr.c  
 *
 * DESCRIPTION: This file contains the main boot table processing state machine.
 *
 * For reference here is the Boot table format:
 *
 * (32-bit) Entry point byte address : Code Execution start address
 * (32-bit) Number of I/O Registers, delay and etc.
 * (32-bit) 1st Word for I/O Register configuration(optional)
 *  ... 
 * (32-bit) Nth Word for I/O Register configuration(optional) 
 * (32-bit) Words for added delay in CPU cycles (optional)
 * (32-bit) Words for Core Start Vector (optional)
 * (32-bit) 1st code section byte count
 * (32-bit) 1st code section starting address in bytes
 * 1st section code byte
 * ...
 * (32-bit) Nth code section byte count
 * (32-bit) Nth code section starting address in bytes
 * Nth section code byte
 * ...
 * Last word of Nth section code type
 * (32-bit) Zero byte count
 *
 * Note1: Core start vector and added delay are special format of I/O Registers
 *        0xFFFF: NOP execution
 *        0xFFFE: core start vector   
 * 
 * Note2: There will be a padded byte added if the code section contains odd 
 *        number of bytes   
 *              
 * FUNCTION                     DESCRIPTION
 * --------                     -----------
 *
 * btblpr.c:
 * boot_init_boot_tbl_inst()    Initialize the boot table control instance
 * boot_proc_boot_tbl()         Boot Table main processing routine
 * boot_proc_boot_tbl_xxx()     Boot Table procssing routine in xxx state
 *
 * (C) Copyright 2004, TELOGY Networks, Inc.
 *****************************************************************************/
/* types.h must be included before stdlib.h for splint to work */
#include "types.h"

/* Ansi header files */
#include <stdlib.h>
#include <string.h>   /* For memset() */

#include "btblwrap.h"

#if 0

/* System utility files */
#include "tiboot.h"

/* Main level */
#include "rmain.h"

/* Device utilities */
#include "device.h"

#endif

/* Module specific file */
#include "btbl.h"
#include "btblloc.h"

/* Build specific Configuration */
#include "iblcfg.h"

/*******************************************************************************
 * Private prototypes
 ******************************************************************************/
/*  boot table processing state machine functions */
void boot_proc_boot_tbl_init(BOOT_TBL_CB_T* p_inst);
void boot_proc_boot_tbl_io_cnt(BOOT_TBL_CB_T* p_inst);
void boot_proc_boot_tbl_io_regs(BOOT_TBL_CB_T* p_inst);
void boot_proc_boot_tbl_size(BOOT_TBL_CB_T* p_inst);
void boot_proc_boot_tbl_addr(BOOT_TBL_CB_T* p_inst);
void boot_proc_boot_tbl_data(BOOT_TBL_CB_T* p_inst);
void boot_proc_boot_tbl_flush(BOOT_TBL_CB_T* p_inst);
void boot_proc_boot_tbl_pad(BOOT_TBL_CB_T* p_inst);

/*******************************************************************************
 * Local variables 
 ******************************************************************************/
BOOT_TBL_CB_T   btbl_inst;  /* Boot table Control Instance */

/*******************************************************************************
 * DATA DEFINITION: Boot Table Processing State Machine
 ******************************************************************************/
BootTblProcFcn_t btbl_st_proc_fcn[BOOT_TBL_NUM_STATES];

/*******************************************************************************
 * FUNCTION PURPOSE: Process the boot table in Init State
 *******************************************************************************
 * DESCRIPTION: Process the boot table in Init State
 *              Record the entry point 
 *
 * void boot_proc_boot_tbl_init (
 *   BOOT_TBL_CB_T        *p_inst)  - A pointer to Boot Table Control instance
 *
 *****************************************************************************/
void boot_proc_boot_tbl_init(BOOT_TBL_CB_T* p_inst)
{
    if(p_inst->f_wait_lsw)
    {
        /* Process the entry point LSW */
        p_inst->code_start_addr += (*p_inst->p_data & 0xFFFF);


        /* C55x boot tables contain an I/O space config. C6x boot tables
         * do not */
#ifdef BOOTCONFIG_NO_BTBL_IO
        p_inst->state = BOOT_TBL_STATE_SIZE; 
#else
        p_inst->state = BOOT_TBL_STATE_IO_CNT;
#endif


        p_inst->f_wait_lsw = FALSE;
    }
    else
    {
        /* Process the entry point MSW */
        p_inst->code_start_addr = ((UINT32)(*p_inst->p_data & 0xFFFF)) << 16;   
        p_inst->f_wait_lsw = (bool)TRUE; 
    }
    
    /* Update data buffer and counter */
    p_inst->p_data++;
    p_inst->data_size_uint16--;
}

/*******************************************************************************
 * FUNCTION PURPOSE: Process the boot table in IO_CNT State
 *******************************************************************************
 * DESCRIPTION: Process the boot table in IO_CNT State
 *              Record the number of IO Registers 
 *
 * void boot_proc_boot_tbl_io_cnt (
 *   BOOT_TBL_CB_T        *p_inst)  - A pointer to Boot Table Control instance
 *
 *****************************************************************************/
#ifndef BOOTCONFIG_NO_BTBL_IO
void boot_proc_boot_tbl_io_cnt(BOOT_TBL_CB_T* p_inst)
{
    if(p_inst->f_wait_lsw)
    {
        /* LSW processing */
        p_inst->num_io_regs += (*p_inst->p_data & 0xFFFF);
        p_inst->f_wait_lsw = FALSE;
        if(p_inst->num_io_regs)
        {
            p_inst->state = BOOT_TBL_STATE_IO_REGS;    
        }
        else
        {
            p_inst->state = BOOT_TBL_STATE_SIZE; 
        }
        
      }
    else
    {
        /* MSW processing */
        p_inst->num_io_regs = ((UINT32)(*p_inst->p_data & 0xFFFF)) << 16;   
        p_inst->f_wait_lsw = TRUE; 
    }
    
    /* Update data buffer and counter */
    p_inst->p_data++;
    p_inst->data_size_uint16--;
}
#endif

/*******************************************************************************
 * FUNCTION PURPOSE: Process the boot table in IO_REGS State
 *******************************************************************************
 * DESCRIPTION: Process the boot table in IO_REG State
 *              Process IO Register configuration 
 *
 * void boot_proc_boot_tbl_io_regs (
 *   BOOT_TBL_CB_T        *p_inst)  - A pointer to Boot Table Control instance
 *
 *****************************************************************************/
#ifndef BOOTCONFIG_NO_BTBL_IO
void boot_proc_boot_tbl_io_regs(BOOT_TBL_CB_T* p_inst)
{
    UINT16 data;
    
    if(p_inst->f_wait_lsw)
    {
        /* LSW processing */
        data = *p_inst->p_data;
        
        /* proess the IO register configuration based on its address (opcode) */
        switch(p_inst->io_addr)
        {
            case BOOT_TBL_IO_CFG_DELAY:
                /* perform the delay loop requested */
                chipDelay(data);
                break;    
                
            case BOOT_TBL_IO_CFG_START_CORE:
                /* Verify whether the start_vector is valid */
                if(data >= (BOOT_BIT_TO_MASK(chipReadNumCores())))
                {
                    BOOT_EXCEPTION(BOOT_ERROR_CODE(BOOT_MODULE_ID_BTBL, 
                                                   BTBL_ERR_INVALID_START_VECTOR));
                }
                /* record the core start vector */
                p_inst->core_start_vector = data;
                break;    
                
            default:
                /* IO Configuration */
                chipIoWrite(p_inst->io_addr, data);
                break;                
                
        }
        p_inst->f_wait_lsw = FALSE;
        
        /* Is it the last IO Register to be configured */
        if((--p_inst->num_io_regs) == 0)
        {
            p_inst->state = BOOT_TBL_STATE_SIZE;
        }
    }
    else
    {
        /* MSW processing */
        p_inst->io_addr = *p_inst->p_data;   
        p_inst->f_wait_lsw = TRUE; 
    }
    
    /* Update data buffer and counter */
    p_inst->p_data++;
    p_inst->data_size_uint16--;
}
#endif

/*******************************************************************************
 * FUNCTION PURPOSE: Process the boot table in SIZE State
 *******************************************************************************
 * DESCRIPTION: Process the boot table in SIZE State
 *              Record the size of the new code section 
 *              perform end of table processing if size = 0
 *
 * void boot_proc_boot_tbl_size (
 *   BOOT_TBL_CB_T        *p_inst)  - A pointer to Boot Table Control instance
 *
 *****************************************************************************/
void boot_proc_boot_tbl_size(BOOT_TBL_CB_T* p_inst)
{
    if(p_inst->f_wait_lsw)
    {
        /* LSW processing */
        p_inst->section_size_bytes += (*p_inst->p_data & 0xFFFF);
        
        /* Is it the end of the boot table */
        if(p_inst->section_size_bytes == 0)
        {
            chipStartCore(p_inst->code_start_addr,
                                 (UINT32)p_boot_entry_addr,
                                  p_inst->core_start_vector);
                    
            /* End of Boot table:Clear the instance */
            p_inst->state = BOOT_TBL_STATE_FLUSH;
            p_inst->f_wait_lsw = FALSE;
            return;    
        }
        
        p_inst->state = BOOT_TBL_STATE_ADDR;
        p_inst->f_wait_lsw = FALSE;
        
        /* Update statistics */
        bootStats.btbl.num_sections++;
    }
    else
    {
        /* MSW processing */
        p_inst->section_size_bytes = ((UINT32)(*p_inst->p_data & 0xFFFF)) << 16;   
        p_inst->f_wait_lsw = (bool)TRUE; 
    }
    
    /* Update data buffer and counter */
    p_inst->p_data++;
    p_inst->data_size_uint16--;
}

/*******************************************************************************
 * FUNCTION PURPOSE: Process the boot table in ADDR State
 *******************************************************************************
 * DESCRIPTION: Process the boot table in ADDR State
 *              Record the start address of the new code section 
 *
 * void boot_proc_boot_tbl_addr (
 *   BOOT_TBL_CB_T        *p_inst)  - A pointer to Boot Table Control instance
 *
 *****************************************************************************/
void boot_proc_boot_tbl_addr(BOOT_TBL_CB_T* p_inst)
{
    if(p_inst->f_wait_lsw)
    {
        /* LSW processing */
        p_inst->section_addr += (*p_inst->p_data & 0xFFFF);
        p_inst->state = BOOT_TBL_STATE_DATA;
        p_inst->f_wait_lsw = FALSE;
    }
    else
    {
        /* MSW processing */
        p_inst->section_addr = ((UINT32)(*p_inst->p_data & 0xFFFF)) << 16;   
        p_inst->f_wait_lsw = (bool)TRUE; 
    }
    
    /* Update data buffer and counter */
    p_inst->p_data++;
    p_inst->data_size_uint16--;
}

/*******************************************************************************
 * FUNCTION PURPOSE: Process the boot table in the PAD state
 *******************************************************************************
 * DESCRIPTION: Toss data bytes to advance to the next section
 *******************************************************************************/
#define MIN(a,b)  (a)<(b) ? (a) : (b)
void boot_proc_boot_tbl_pad(BOOT_TBL_CB_T* p_inst)
{
    UINT32  num_bytes;
    UINT32  num_uint16_words;

    num_bytes = MIN (p_inst->section_size_bytes, CHIP_UINT16_TO_BYTES(p_inst->data_size_uint16));
    num_uint16_words = CHIP_BYTES_TO_UINT16(num_bytes);

    /* Update instance variables for the next code section */
    p_inst->data_size_uint16 -= num_uint16_words;
    p_inst->p_data           += num_uint16_words;
    
    p_inst->section_addr       += num_bytes;
    p_inst->section_size_bytes -= num_bytes;
    
    if(p_inst->section_size_bytes == 0)
    {
        p_inst->state = BOOT_TBL_STATE_SIZE;    

        /* Chip specific post block handling. Can be defined to an empty statement */
        chipBtblBlockDone();
    }
}



/*******************************************************************************
 * FUNCTION PURPOSE: Process the boot table in DATA State
 *******************************************************************************
 * DESCRIPTION: Process the boot table in DATA State
 *              Copy the received section data to the destination memory 
 *              locations 
 *
 * void boot_proc_boot_tbl_data (
 *   BOOT_TBL_CB_T        *p_inst)  - A pointer to Boot Table Control instance
 *
 *****************************************************************************/
void boot_proc_boot_tbl_data(BOOT_TBL_CB_T* p_inst)
{
    UINT32  num_bytes;             /* number of bytes to be copy         */
    UINT32  num_uint16_words;      /* number of 16 bit words to copy     */
    UINT32  num_uint16_words_pad;  /* number of 16 bit words, rounded up to a multiple of 4 bytes */
    UINT32  pad_uint16;            /* The amount of pad present */
    UINT16  error;
    
    num_bytes = MIN (p_inst->section_size_bytes, CHIP_UINT16_TO_BYTES(p_inst->data_size_uint16));
    num_uint16_words = CHIP_BYTES_TO_UINT16(num_bytes);

    /* Some processors add padding to the memwidth used in the rom (created by tools) */
    num_uint16_words_pad = chipAddBtblUint16Pad (num_uint16_words);
    pad_uint16           = num_uint16_words_pad - num_uint16_words;
    
    /* 
     * Record the last UINT16 word and copy the data to the destination
     * locations
     */
    
    if((error = coreCopyData(p_inst->section_addr, p_inst->p_data, num_bytes, 
                             p_inst->core_start_vector))
       != CORE_NOERR)
    {
        /* Error Processing */ 
        BOOT_EXCEPTION(BOOT_ERROR_CODE(BOOT_MODULE_ID_CHIP, 
                                       error));
    } 
    
    /* Update instance variables for the next code section */
    p_inst->data_size_uint16 -= num_uint16_words;
    p_inst->p_data           += num_uint16_words;
    
    p_inst->section_addr       += num_bytes;
    p_inst->section_size_bytes -= num_bytes;

    if(p_inst->section_size_bytes == 0)
    {

        if (pad_uint16)  
        {
            p_inst->state              = BOOT_TBL_STATE_PAD;
            p_inst->section_size_bytes = CHIP_UINT16_TO_BYTES(pad_uint16);
        } 
        else
        {
            p_inst->state = BOOT_TBL_STATE_SIZE;    

            /* Chip specific post block handling. Can be defined to an empty statement */
            chipBtblBlockDone();
        }
    }
    
    /* update statistics */
    bootStats.btbl.num_pdma_copies++;
}

/*******************************************************************************
 * FUNCTION PURPOSE: Process the boot table in Flush State
 *******************************************************************************
 * DESCRIPTION: Process the boot table in Flush State
 *              We will enter this state when the end of table or error
 *              condition occurs
 *              set data_size to 0, i.e. ignore the reamining data
 *
 * void boot_proc_boot_tbl_error (
 *   BOOT_TBL_CB_T        *p_inst)  - A pointer to Boot Table Control instance
 *
 *****************************************************************************/
void boot_proc_boot_tbl_flush(BOOT_TBL_CB_T* p_inst)
{
    p_inst->data_size_uint16 = 0;
}

/* ------------------- PUBLIC FUNCTIONS START HERE ----------------------- */
/*******************************************************************************
 * FUNCTION PURPOSE: Initialize the boot table control instance
 *******************************************************************************
 * DESCRIPTION: Initialize and allocate the boot table control instance  
 *              Initialize all the variables to zero
 *              Set the state to Init
 *
 * void boot_init_boot_tbl_inst ( void )
 *
 * Note: only one instance is used at this moment
 *       a free instance function will be needed when multiple instances are
 *       supported
 *
 *****************************************************************************/
void boot_init_boot_tbl_inst(BOOT_TBL_CB_T *p_btbl_inst)
{
    btblMemset(p_btbl_inst, 0, sizeof(BOOT_TBL_CB_T));
    p_btbl_inst->state = BOOT_TBL_STATE_INIT;
    
    /*
     * Set the core start vector to start core A only as its default value
     * in case the boot table does not contain the setting of the core
     * start vector.
     */
    p_btbl_inst->core_start_vector = 1; 
    
    
    /* Initialize the toot table processing function table */
    btbl_st_proc_fcn[BOOT_TBL_STATE_INIT]    = boot_proc_boot_tbl_init;

#ifndef BOOTCONFIG_NO_BTBL_IO
    btbl_st_proc_fcn[BOOT_TBL_STATE_IO_CNT]  = boot_proc_boot_tbl_io_cnt;
    btbl_st_proc_fcn[BOOT_TBL_STATE_IO_REGS] = boot_proc_boot_tbl_io_regs;
#endif

    btbl_st_proc_fcn[BOOT_TBL_STATE_SIZE]    = boot_proc_boot_tbl_size;
    btbl_st_proc_fcn[BOOT_TBL_STATE_ADDR]    = boot_proc_boot_tbl_addr;
    btbl_st_proc_fcn[BOOT_TBL_STATE_DATA]    = boot_proc_boot_tbl_data;
    btbl_st_proc_fcn[BOOT_TBL_STATE_PAD]     = boot_proc_boot_tbl_pad;
    btbl_st_proc_fcn[BOOT_TBL_STATE_FLUSH]   = boot_proc_boot_tbl_flush;
    
} /* end of boot_init_boot_tbl_inst() */

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
 *   UINT16       size)     - number of 16-bit words  
 *
 *
 * Note: It is up to the caller to reserve 4 16-bit word space in front of the
 *       input buffer since the data may be moved to achieve 32-bit alignment
 *       requirement for PDMA transfer
 *****************************************************************************/
void boot_proc_boot_tbl(BOOT_TBL_CB_T *p_btbl_inst, UINT16* p_data, UINT32 size)
{
    
    /* Record the input data buffer and size */
    p_btbl_inst->p_data           = p_data;
    p_btbl_inst->data_size_uint16 = size;
    
    /* Optional: verify the inst state is valid */
    if(p_btbl_inst->state > BOOT_TBL_LAST_STATE)
    {
        /* Error Processing */
        BOOT_EXCEPTION(BOOT_ERROR_CODE(BOOT_MODULE_ID_BTBL, 
                                       BTBL_ERR_INVALID_STATE));
         
    }
    
    /*
     * Invoke the Boot table processing state machine until all the input
     * data are processed or the end of table condition is satified
     */
    while (p_btbl_inst->data_size_uint16 != 0)
    {
        btbl_st_proc_fcn[p_btbl_inst->state](p_btbl_inst);
    }
    
} /* end of boot_proc_boot_tbl() */

