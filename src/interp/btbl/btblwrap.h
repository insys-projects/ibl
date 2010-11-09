#ifndef BTBLWRAP_H
#define BTBLWRAP_H
/********************************************************************************
 * FILE PURPOSE: Define the IBL wrapper for the TI boot table processor
 ********************************************************************************
 * FILE NAM:E btblwrap.h
 *
 * DESCRIPTION: Definitions required for the IBL wrapper of the TI boot table
 *				processor
 *
 ********************************************************************************/

/* chipStartCore does nothing, since multiple core start is not supported */
#define chipStartCore(x,y,z)

/* A minimal boot stats structure */
typedef struct bootTblStats_s  {

    UINT32 num_sections;
    UINT32 num_pdma_copies;
    
} bootTblStats_t;


typedef struct bootStats_s  {
    
    bootTblStats_t btbl;
    
} bootStats_t;

extern bootStats_t      bootStats;

/****************************************************************************
 * Definition: The c64x boot table tools will always pad boot table
 *             entries to 32 bit elements. The boot table works in 16 bit
 *             element sizes. If the size is not an even number of 16 bit
 *             elements, add one word of padding.
 ****************************************************************************/
#define chipAddBtblUint16Pad(x)     (((x)+1) & 0xfffffffe)

/* Return values */
#define CORE_NOERR      0

/* Module number */
#define BOOT_MODULE_ID_CHIP     0
#define BOOT_MODULE_ID_BTBL     1

/* Error code creation */
#define BOOT_ERROR_CODE(modnum,errnum)      (((modnum) << 16) | (errnum))

/* Fatal error handling */
#define BOOT_EXCEPTION      btblBootException

/* Macros */
#define CHIP_UINT16_TO_BYTES(x)     ((x) << 1)
#define CHIP_BYTES_TO_UINT16(x)     (((x) + 1) >> 1)


/* No interblock processing required */
#define chipBtblBlockDone()

/* No tracing dummy function */
#define chipDummy(x)

        
/* gem.c prototypes */
UINT16 coreCopyData (UINT32 dest_addr, UINT16 *p_data, UINT32 sizeBytes, UINT16 start_vector);

/* btblwrap.c prototypes */
void btblBootException (UINT32 ecode);

/* The block size allocated through malloc. Chosen to match the value defined
 * for the coff loader. malloc is used so the same heap section can be used. */
#define  TI_BTBL_BLOCK_SIZE   0x4000
 
/* Read/Write 32bit values */
#define chipStoreWord(x,y)      *(x) = (y)
#define chipReadWord(x)         *(x)

/* Boot table wrapper error codes */
#define BTBL_WRAP_ECODE_MALLOC_FAIL     100
#define BTBL_WRAP_ECODE_READ_FAIL       101
#define BTBL_WRAP_ECODE_BTBL_FAIL       102

#define btblMemset iblMemset


#endif /* BTBLWRAP_H */


