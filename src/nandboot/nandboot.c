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


void iblNandBoot (void)
{
    Uint32 entry;
    void   (*exit)();

    /* Power up the device */
    if (devicePowerPeriph (TARGET_PWR_NAND) < 0)
        return;


    /* Perform any device specific configurations */
    if (deviceConfigureForNand() < 0)
        return;


    /* Open the nand driver */
    if ((*nand_boot_module.open) ((void *)&ibl.nandConfig, NULL))
        return;


    entry = iblBoot (&nand_boot_module, ibl.nandConfig.bootFormat, &ibl.nandConfig.blob);

    (*nand_boot_module.close)();

    if (entry != 0)  {

        iblStatus.exitAddress = entry;
        exit = (void (*)())entry;
        (*exit)();
            
    }

  } 


