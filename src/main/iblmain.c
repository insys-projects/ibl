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

#ifndef EXCLUDE_ETH
            for (j = 0; j < ibl_N_ETH_PORTS; j++)  {
                if (ibl.ethConfig[j].ethPriority == i)  {
                    iblStatus.activePeriph = ibl_ACTIVE_PERIPH_ETH;
                    memcpy (&iblStatus.ethParams, &ibl.ethConfig[j].ethInfo, sizeof (iblEthBootInfo_t));
                    iblEthBoot (j);
                }
            }
#endif

#ifndef EXCLUDE_NAND
            if (ibl.nandConfig.nandPriority == i)  {
                iblStatus.activePeriph = ibl_ACTIVE_PERIPH_NAND;
                iblNandBoot ();
            }
#endif

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
    Uint32  entry = 0;
    Uint32  value32;
    Uint8   dataBuf[4];   
    Uint16  value16;

    /* Determine the data format if required */
    if (dataFormat == ibl_BOOT_FORMAT_AUTO)  {

        (*bootFxn->peek)(dataBuf, sizeof(dataBuf));
        value32 = (dataBuf[0] << 24) | (dataBuf[1] << 16) | (dataBuf[2] << 8) | (dataBuf[3] << 0);
        value16 = (dataBuf[0] <<  8) | (dataBuf[1] <<  0);

        /* BIS */
#ifndef EXCLUDE_BIS
        if (value32 == BIS_MAGIC_NUMBER)
            dataFormat = ibl_BOOT_FORMAT_BIS;
#endif

#ifndef EXCLUDE_COFF
        if (iblIsCoff (value16))
            dataFormat = ibl_BOOT_FORMAT_COFF;
#endif

#ifndef EXCLUDE_ELF
        if (iblIsElf (dataBuf))
            dataFormat = ibl_BOOT_FORMAT_ELF;
#endif

        if (dataFormat == ibl_BOOT_FORMAT_AUTO)  {
            iblStatus.autoDetectFailCnt += 1;
            return (0);
        }
    }        


    iblStatus.activeFormat = dataFormat;


    /* Invoke the parser */
    switch (dataFormat)  {

#ifndef EXCLUDE_BIS
        case ibl_BOOT_FORMAT_BIS:
            iblBootBis (bootFxn, &entry);
            break;
#endif

#ifndef EXCLUDE_COFF
        case ibl_BOOT_FORMAT_COFF:
            iblBootCoff (bootFxn, &entry);
            break;
#endif

        case ibl_BOOT_FORMAT_BTBL:
            iblBootBtbl (bootFxn, &entry);
            break;

#ifndef EXCLUDE_BLOB
        case ibl_BOOT_FORMAT_BBLOB:
            iblBootBlob (bootFxn, &entry, formatParams);
            break;
#endif

#ifndef EXCLUDE_ELF
        case ibl_BOOT_FORMAT_ELF:
            iblBootElf (bootFxn, &entry);
            break;
#endif

        default:
            iblStatus.invalidDataFormatSpec += 1;
            break;

    }
    

    return (entry);

}








