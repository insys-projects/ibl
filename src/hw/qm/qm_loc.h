#ifndef _QM_LOC_H
#define _QM_LOC_H
/***************************************************************************************
 * FILE PURPOSE: Queue Manager local definitions
 ***************************************************************************************
 * FILE NAME: qm_loc.h
 *
 * DESCRIPTION: Local queue manager definitions
 *
 ***************************************************************************************/

/* Memory map */
/* Relative to the queue manager region */
#define QM_REG_REVISION         0x00
#define QM_REG_DIVERSION        0x08
#define QM_REG_LINKRAM_BASE(x)  (0x0c + 8*(x))
#define QM_REG_LINKRAM_SIZE(x)  (0x10 + 8*(x))

/* The queue peek registers (includes thresholds) */
#define QM_REG_STAT_CFG_REGD(x)  (0xc + 16*(x))

/* Relative to the descriptor setup region */
#define QM_REG_MEMR_BASE_ADDR(x)  (0x00 + 16*(x))
#define QM_REG_MEMR_START_IDX(x)  (0x04 + 16*(x))
#define QM_REG_MEMR_DESC_SETUP(x) (0x08 + 16*(x))

/* Queues, register A */
#define QM_REG_QUEUE_REGA(x)  (0x00 + 16*(x))
#define QM_QA_ENTRY_COUNT_MSB  18
#define QM_QA_ENTRY_COUNT_LSB   0
    
/* Queues, register D */
#define QM_REG_QUEUE_REGD(x)  (0x0c + 16*(x))

/* Description region setup */
#define QM_REG_VAL_DESC_SETUP_SET_DESC_SIZE(x,v)  (x) = BOOT_SET_BITFIELD((x),((v) >> 4)-1, 28, 16)


/* Maximum linking RAM size mask */
#define QM_REG_LINKRAM_SIZE_MAX_MASK  0x7ffff



#endif /* _QM_LOC_H */
