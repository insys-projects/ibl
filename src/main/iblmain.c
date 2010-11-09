/*****************************************************************************************
 * FILE PURPOSE: Perform the top level boot
 *****************************************************************************************
 * FILE NAME: iblmain.c
 *
 * DESCRIPTION: The top level boot examines the boot configuration and performs boot
 *				based on this configuration
 *
 * @file iblmain.c
 *
 * @brief
 *   This file is used to launch a boot based on the boot configuration structure
 *
 *****************************************************************************************/
#include "ibl.h"
#include "iblloc.h"
#include "iblcfg.h"
#include "device.h"
#include "ethboot.h"
#include "bis.h"
#include "coffwrap.h"
#include "iblbtbl.h"
#include "iblblob.h"
#include "timer.h"
#include "i2c.h"
#include "ibl_elf.h"
#include <string.h>

/**
 *  @brief
 *      Data structures shared between the 1st and 2nd stage IBL load
 *      are declared in a single header file, included in both stages
 */
#include "iblStage.h"



/* Eat printfs */
void mprintf(char *x, ...) { }

/**
 * @b Description
 * @n
 *
 *  Returns TRUE if the input priority is valid and enabled
 */
BOOL iblPriorityIsValid (uint32 priority)
{
    if ( (priority >= ibl_HIGHEST_PRIORITY)  &&
         (priority <= ibl_LOWEST_PRIORITY)   )

        return (TRUE);


    return (FALSE);

}

/**
 * @b Description
 * @n
 *
 *  Returns TRUE if the mac address is 0
 */
BOOL iblMacAddrIsZero (uint8 *maddr)
{
    int32 i;

    for (i = 0; i < 6; i++)
        if (maddr[i] != 0)
            return (FALSE);

    return (TRUE);

}


/**
 * @b Description
 * @n
 *
 * The main function kicks off the boot. If it does not find the magic value in the
 *     configuration array then default values are loaded. This default load
 *     is done only once at the start of boot. 
 *
 * @retval
 *  None
 */
void main (void)
{
    int32 i, j;

    /* Initialize the status structure */
    iblMemset (&iblStatus, 0, sizeof(iblStatus_t));
    iblStatus.iblMagic = ibl_MAGIC_VALUE;


    /* Power up the timer */
    devicePowerPeriph (TARGET_PWR_TIMER_0);

    /* Initialize the system timer (software tracking of the hardware timer state) */
    timer_init ();

    /* Load default mac addresses for ethernet boot if requested */
    for (i = 0; i < ibl_N_ETH_PORTS; i++)  {

        if ( (iblPriorityIsValid (ibl.ethConfig[i].ethPriority)       )   &&
             (iblMacAddrIsZero   (ibl.ethConfig[i].ethInfo.hwAddress) )   )

            deviceLoadDefaultEthAddress (ibl.ethConfig[i].ethInfo.hwAddress);

    }


    /* DDR configuration is device specific */
    deviceDdrConfig ();

    /* Try booting forever */
    for (;;)  {

        /* Start looping through the boot modes to find the one with the lowest priority
         * value, and try to boot it. If a boot mode is not supported the function
         * statement is simply defined to be a void statement */
        for (i = ibl_HIGHEST_PRIORITY; i < ibl_LOWEST_PRIORITY; i++)  {

            for (j = 0; j < ibl_N_ETH_PORTS; j++)  {
                if (ibl.ethConfig[j].ethPriority == i)
                iblEthBoot (j);
            }

            if (ibl.nandConfig.nandPriority == i)
                iblNandBoot ();

            iblStatus.heartBeat += 1;
        }

    }


} /* main */


    
/**
 * @b Description
 * @n
 * 
 * The ibl boot function links a device to a data format. The data format
 * parser pulls data from the boot device
 *
 * @param[in] bootFxn      The structure containing the boot device functions
 *
 * @retval
 *  None
 */
Uint32 iblBoot (BOOT_MODULE_FXN_TABLE *bootFxn, Int32 dataFormat, void *formatParams)
{ 
    Uint32 entry = 0;

    union  {

        Uint8   dataBuf[4];   /* Place holder */
        Uint32  bisValue;
        Uint16  coffVer;    

    } fid;


    /* Determine the data format if required */
    if (dataFormat == ibl_BOOT_FORMAT_AUTO)  {

        (*bootFxn->peek)((Uint8 *)&fid, sizeof(fid));

        /* BIS */
        if (fid.bisValue == BIS_MAGIC_NUMBER)
            dataFormat = ibl_BOOT_FORMAT_BIS;

        if (iblIsCoff (fid.coffVer))
            dataFormat = ibl_BOOT_FORMAT_COFF;

        if (iblIsElf (fid.dataBuf))
            dataFormat = ibl_BOOT_FORMAT_ELF;

        else  {
            iblStatus.autoDetectFailCnt += 1;
            return (0);
        }
    }        


    /* Invoke the parser */
    switch (dataFormat)  {

        case ibl_BOOT_FORMAT_BIS:
            iblBootBis (bootFxn, &entry);
            break;

        case ibl_BOOT_FORMAT_COFF:
            iblBootCoff (bootFxn, &entry);
            break;

        case ibl_BOOT_FORMAT_BTBL:
            iblBootBtbl (bootFxn, &entry);
            break;

        case ibl_BOOT_FORMAT_BBLOB:
            iblBootBlob (bootFxn, &entry, formatParams);
            break;

        case ibl_BOOT_FORMAT_ELF:
            iblBootElf (bootFxn, &entry);
            break;

        default:
            iblStatus.invalidDataFormatSpec += 1;
            break;

    }
    

    return (entry);

}








