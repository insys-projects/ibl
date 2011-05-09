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



/*********************************************************************************** 
 * FILE PURPOSE: The NAND boot wrapper
 ***********************************************************************************
 * FILE NAME: nandboot.c
 *
 * DESCRIPTION: This file provides the nand boot wrapper used by IBL modules
 *
 * @file nandboot.c
 *
 * @brief
 *		The nand boot wrapper
 *
 ************************************************************************************/
#include "types.h"
#include "ibl.h"
#include "iblloc.h"
#include "nand.h"
#include "device.h"

/** 
 * @brief
 *   Nandboot is disabled through iblcfg.h. Disable the definition for the compilation
 */
#ifdef iblNandBoot
 #undef iblNandBoot
#endif


void iblNandBoot (int32 eIdx)
{
    Uint32 entry;
    Int32  ret;
    void   (*exit)();



    /* Perform any device specific configurations */
    if (deviceConfigureForNand() < 0)
        return;


    /* Open the nand driver */
    if ((*nand_boot_module.open) ((void *)&ibl.bootModes[eIdx].u.nandBoot, NULL))
        return;


    entry = iblBoot (&nand_boot_module, ibl.bootModes[eIdx].u.nandBoot.bootFormat, &ibl.bootModes[eIdx].u.nandBoot.blob[iblEndianIdx][iblImageIdx]);

    (*nand_boot_module.close)();

    if (entry != 0)  {

        iblStatus.exitAddress = entry;
        exit = (void (*)())entry;
        (*exit)();
            
    }

} 


