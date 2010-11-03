/************************************************************************************
 * FILE PURPOSE: Wrapper for the TI boot table processor
 ************************************************************************************
 * FILE NAME: btblwrap.c
 *
 * DESCRIPTION: The IBL wrapper for the boot table processor.
 *
 ************************************************************************************/
#include "types.h"
#include "ibl.h"
#include "iblloc.h"
#include "btblwrap.h"
#include "btbl.h"
#include "btblloc.h"
#include "iblbtbl.h"
#include <stdlib.h>
#include <string.h>


/* Globals used by the TI boot table processor */

BOOT_TBL_CB_T tiBootTable;
bootStats_t   bootStats;
Uint32        btblEcode;
Int32         btblWrapEcode;


#define MIN(a,b)  ((a) < (b)) ? (a) : (b)


/************************************************************************************
 * FUNCTION PURPOSE: The main boot wrapper for the TI boot table processor
 ************************************************************************************
 * DESCRIPTION: Interfaces the IBL to the TI boot table driver. 
 *              The TI boot table driver operates in a push data mode, the
 *              IBL operates in a pull data mode. This function pulls data
 *              from the interface and pushes is to the TI driver. To do this
 *              the internal state of the TI boot table driver is used
 *              to determine how much data to pull.
 ************************************************************************************/
void iblBootBtbl (BOOT_MODULE_FXN_TABLE *bootFxn, Uint32 *entry_point)
{
    Uint32  readSize;
    Uint32  blockSize;
    Uint32  blockSize16;
    Uint8  *data;
    Uint16 *data16;
    Uint16  t16;

    Int32 i, j;

    *entry_point  = 0;
    btblEcode     = 0;
    btblWrapEcode = 0;

    /* Initialize stats and btbl state */
    memset (&bootStats, 0, sizeof(bootStats_t));
    
    boot_init_boot_tbl_inst (&tiBootTable);    

    data = malloc (TI_BTBL_BLOCK_SIZE);
    if (data == NULL)  {
        btblWrapEcode = BTBL_WRAP_ECODE_MALLOC_FAIL;
        return;
    }

    data16 = (Uint16 *)data;

    while ((tiBootTable.state != BOOT_TBL_STATE_FLUSH) && (btblEcode == 0)) {

        switch (tiBootTable.state)  {

            case BOOT_TBL_STATE_PAD:   readSize = 2;
                                       break;

            case BOOT_TBL_STATE_INIT:  
            case BOOT_TBL_STATE_SIZE:  
            case BOOT_TBL_STATE_ADDR:  readSize = 4;
                                       break;

            case BOOT_TBL_STATE_DATA:  readSize = tiBootTable.section_size_bytes;
                                       break;
        }


        while (readSize > 0)  {

            blockSize   = MIN(readSize, TI_BTBL_BLOCK_SIZE);
            blockSize16 = (blockSize + 1) >> 1;

            /* No recovery on block read failure */
            if ((*bootFxn->read)(data, blockSize) < 0)  {
                btblWrapEcode = BTBL_WRAP_ECODE_READ_FAIL;
                free (data);
                return;
            }

            /* The data has been read as a byte stream. This data must
             * be re-formatted as a 16 bit word stream */
            for (j = i = 0; j < blockSize16; j++, i += 2)  {

                t16 = (data[i+0] << 8) | data[i+1];
                data16[j] = t16;

            }


            /* Process the block */
            boot_proc_boot_tbl (&tiBootTable, data16, blockSize16);

            readSize = readSize - blockSize;

        }

    }

    if (btblEcode == 0)
        *entry_point = tiBootTable.code_start_addr;

    free (data);

}

/***********************************************************************************
 * FUNCTION PURPOSE: Handle a fatal error from the boot table processor
 ***********************************************************************************
 * DESCRIPTION: The error code is stored. Processing is terminated
 *              through the global ecode value.
 ***********************************************************************************/
void btblBootException (UINT32 ecode)
{
    btblEcode     = ecode;
    btblWrapEcode = BTBL_WRAP_ECODE_BTBL_FAIL;
}


    












