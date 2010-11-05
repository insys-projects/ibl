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



/******************************************************************************
 * FILE PURPOSE: Boot from a BIS formatted input
 ******************************************************************************
 * FILE NAME: bis.c
 *
 * DESCRIPTION: A BIS formatted data file is parsed and loaded
 *
 *  @file bis.c
 *
 *  @brief
 *      This file is used to load a BIS formatted data file
 *
 ******************************************************************************/
#include "types.h"
#include "ibl.h"
#include "iblloc.h"
#include "bis.h"
#include "iblcfg.h"

/**********************************************************************
 ************************** Local Structures **************************
 **********************************************************************/

/**
 * @brief 
 *  The structure describes the Function Entry. 
 *
 * @details
 *  The entry stores information about function which can be executed
 *  while executing the BIS Command State machine. These entries are
 *  loaded and executed through the BIS Function Load & Execute commands.
 */
typedef struct FN_ENTRY
{
    /**
     * @brief   This is the function number which is unique for all
     * functions.
     */
    Uint16       fn_num;
    
    /**
     * @brief   This is the number of arguments the function takes.
     */
    Uint16       arg_cnt;

    /**
     * @brief   This is the address where the function is loaded in
     * memory via the function load command.
     */
    Uint32       fn_address;
}FN_ENTRY;

/**
 * @brief   Global database all functions which can be loaded and executed
 * via the BIS Function Load/Execute commands.
*/
FN_ENTRY     fn_table[MAX_BIS_FUNCTION_SUPPORT];


/**
 *  @b Description
 *  @n
 *      The function interfaces with the boot module and reads data as it 
 *      is received by the boot module and executes the BIS command state
 *      machine. 
 *
 *  @param[out]  entry_point
 *      This is the entry point to which control is to be transferred on
 *      success. This is read from the "payload" of TERMINATE command.
 *
 *  @retval
 *      Success -   0
 *  @retval
 *      Error   -   <0
 */
Int32 iblBootBis (BOOT_MODULE_FXN_TABLE *bootFxn, Uint32* entry_point)
{
    Uint32      dataWord;
    Uint32      loadAddress;
    Uint32      sectionSize;
    Uint32      commandFlags;
    Uint16      argCnt;
    Uint16      fn_num;
    FN_ENTRY*   ptr_fn_entry;
    Bool        done = FALSE;

    /* Read the Magic word */
    if ((*bootFxn->read)((Uint8 *)&dataWord, sizeof(Uint32)) < 0)    
        return -1;

    /* Check if we received the magic number. */
    if (dataWord != BIS_MAGIC_NUMBER)
        return -1;

    /* If magic word was good, let's start parsing commands */
    while (done == FALSE)
    {
        /* Read BIS command */
        if ((*bootFxn->read)((Uint8 *)&dataWord, sizeof(Uint32)) < 0)
            return -1;

        /* Make sure command at least has right prefix */
        if ((dataWord & BIS_CMD_MASK) != BIS_CMD_PREFIX )
            return -1;

        /* Process the command. */
        switch (dataWord)
        {
            case BIS_CMD_SECTION_LOAD:
            {
                /* Read the Section Load Header: Load Address */
                if ((*bootFxn->read)((Uint8 *)&loadAddress, sizeof(Uint32)) < 0)
                    return -1;

                /* Read the Section Load Header: Section Size */
                if ((*bootFxn->read)((Uint8 *)&sectionSize, sizeof(Uint32)) < 0)
                    return -1;

                /* Read the Section Load Header: Flags */
                if ((*bootFxn->read)((Uint8 *)&commandFlags, sizeof(Uint32)) < 0)
                    return -1;

#if 0
                /* DEBUG Message: */
                mprintf ("*****************************\n");
                mprintf ("Section Load Command\n");
                mprintf ("    Load Address : 0x%x\n", loadAddress);
                mprintf ("    Section Size : %d\n",   sectionSize);
                mprintf ("    Command Flags: 0x%x\n", commandFlags);
                mprintf ("*****************************\n");
#endif
                /* Read the data from the Section Load: Payload section. */
                if ((*bootFxn->read)((Uint8 *)loadAddress, sectionSize) < 0)
                    return -1;

                /* Section Load command has been processed. */
                break;
            }
            case BIS_CMD_FUNCTION_LOAD:
            {
                /* Read the Function Load Header: Load Address */
                if ((*bootFxn->read)((Uint8 *)&loadAddress, sizeof(Uint32)) < 0)
                    return -1;

                /* Read the Function Load Header: Section Size */
                if ((*bootFxn->read)((Uint8 *)&sectionSize, sizeof(Uint32)) < 0)
                    return -1;

                /* Read the Function Load Header: Flags */
                if ((*bootFxn->read)((Uint8 *)&commandFlags, sizeof(Uint32)) < 0)
                    return -1;

                /* Read the Function Load Header: Argument Count */
                if ((*bootFxn->read)((Uint8 *)&argCnt, sizeof(Uint16)) < 0)
                    return -1;

                /* Read the Function Load Header: Function Number */
                if ((*bootFxn->read)((Uint8 *)&fn_num, sizeof(Uint16)) < 0)
                    return -1;

#if 0
                /* DEBUG Message: */
                mprintf ("*****************************\n");
                mprintf ("Function Load Command\n");
                mprintf ("    Load Address : 0x%x\n", loadAddress);
                mprintf ("    Section Size : %d\n",   sectionSize);
                mprintf ("    Command Flags: 0x%x\n", commandFlags);
                mprintf ("    Argument Cnt : %d\n",   argCnt);
                mprintf ("    Function Num : %d\n",   fn_num);
                mprintf ("*****************************\n");
#endif
                /* Read the data from the Function Load: Payload section. */
                if ((*bootFxn->read)((Uint8 *)loadAddress, sectionSize) < 0)
                    return -1;

                /* Validate the arguments: Ensure that the function number is in range */
                if (fn_num > MAX_BIS_FUNCTION_SUPPORT)
                    return -1;

                /* Get access to the kernel function entry. */
                ptr_fn_entry = (FN_ENTRY *)&fn_table[fn_num];

                /* Validate the arguments: Ensure that the argument count is in range */
                if (argCnt > MAX_BIS_FUNCTION_SUPPORT)
                    return -1;

                /* Validate the arguments: Make sure there is no function entry already loaded. 
                 * This is a user error while generating the BIS images. If the function address
                 * is non zero this implies that there is already a function loaded. */
                if (ptr_fn_entry->fn_address != 0)
                    return -1;

                /* Populate the function entry. */
                ptr_fn_entry->fn_address = loadAddress;
                ptr_fn_entry->arg_cnt    = argCnt;
                ptr_fn_entry->fn_num     = fn_num;
                break;
            }
            case BIS_CMD_FUNCTION_EXEC:
            {
                void   (*fnEntryPoint)(void);

                /* Read the Function Execute Header: Load Address */
                if ((*bootFxn->read)((Uint8 *)&argCnt, sizeof(Uint16)) < 0)
                    return -1;

                /* Read the Function Execute Header: Function Number */
                if ((*bootFxn->read)((Uint8 *)&fn_num, sizeof(Uint16)) < 0)
                    return -1;

#if 0
                /* DEBUG Message: */
                mprintf ("*****************************\n");
                mprintf ("Function Execute Command\n");
                mprintf ("    Argument Cnt : %d\n",   argCnt);
                mprintf ("    Function Num : %d\n",   fn_num);
                mprintf ("*****************************\n");
#endif
                /* Validate the arguments: Ensure that the function number is in range */
                if (fn_num > MAX_BIS_FUNCTION_SUPPORT)
                    return -1;

                /* Get access to the kernel function entry. */
                ptr_fn_entry = (FN_ENTRY *)&fn_table[fn_num];

                /* Validate the arguments: Make sure that the function entry is already loaded. 
                 * If not then we have a FATAL Error. */
                if (ptr_fn_entry->fn_address == 0)
                    return -1;

                /* Call the kernel function entry. */
                fnEntryPoint = (void (*)(void))ptr_fn_entry->fn_address;
                fnEntryPoint();
                break;
            }
            case BIS_CMD_MEMORY_ACCESS:
            {
                Uint16  op_type;
                Uint32  mask;
                Uint32  sleep_cnt;

                /* Read the Memory Access Header: Address */
                if ((*bootFxn->read)((Uint8 *)&loadAddress, sizeof(Uint32)) < 0)
                    return -1;

                /* Read the Memory Access Header: Word Count */
                if ((*bootFxn->read)((Uint8 *)&argCnt, sizeof(Uint16)) < 0)
                    return -1;

                /* Read the Memory Access Header: Op Type */
                if ((*bootFxn->read)((Uint8 *)&op_type, sizeof(Uint16)) < 0)
                    return -1;

                /* Read the Memory Access Header: Mask */
                if ((*bootFxn->read)((Uint8 *)&mask, sizeof(Uint32)) < 0)
                    return -1;

                /* Read the Memory Access Header: Sleep Count */
                if ((*bootFxn->read)((Uint8 *)&sleep_cnt, sizeof(Uint32)) < 0)
                    return -1;

                /* Read the Memory Access Header: Data Payload */
                if ((*bootFxn->read)((Uint8 *)&dataWord, sizeof(Uint32)) < 0)
                    return -1;

#if 0
                /* DEBUG Message: */
                mprintf ("*****************************\n");
                mprintf ("Memory Access Command\n");
                mprintf ("    Address      : 0x%x\n", loadAddress);
                mprintf ("    Word Count   : %d\n", argCnt);
                mprintf ("    Op Type      : %d\n", op_type);
                mprintf ("    Mask         : %d\n", mask);
                mprintf ("    Sleep Count  : %d\n", sleep_cnt);
                mprintf ("    Data Payload : %d\n", dataWord);
                mprintf ("*****************************\n");
#endif
                break;
            }
            case BIS_CMD_SECTION_TERMINATE:
            {
                /* Read the Terminate Command Payload  */
                if ((*bootFxn->read)((Uint8 *)&dataWord, sizeof(Uint32)) < 0)
                    return -1;

                /* DEBUG Message: */
                mprintf ("*****************************\n");
                mprintf ("Terminate Command\n");
                mprintf ("    Entry Point : 0x%x\n", dataWord);
                mprintf ("*****************************\n");

                /* We are done with the BIS Image parsing. */
                done         = TRUE;
                *entry_point = dataWord;
                break;
            }
            default:
            {
                /* BIS Command is not supported. */
                mprintf ("Error: BIS Command 0x%x not supported\n", dataWord);
                return -1;
            }
        }
    }

    /* Control comes here indicates that the parsing was successful. */
    return 0;
}





