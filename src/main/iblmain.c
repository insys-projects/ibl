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
 * @brief The ibl table is declared.
 *
 * @details
 *   The ibl table is declared uninitialized by this ibl program. An external
 *   initialization can be performed if the default operation of the ibl is
 *   not desired.
 */
#pragma DATA_SECTION(ibl, ".ibl_config_table")
ibl_t ibl;


/**
 * @brief The ibl status table is declared.
 *  
 * @details
 *   The ibl status table is declared. It is initialized at run time
 *   in function main.
 */
#pragma DATA_SECTION(iblStatus, ".ibl_status_table")
iblStatus_t iblStatus;


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
    memset (&iblStatus, 0, sizeof(iblStatus_t));
    iblStatus.iblMagic = ibl_MAGIC_VALUE;

    /* Initialize the system timer (software tracking of the hardware timer state */
    timer_init ();

#if 0
    /* If there is no magic value in the ibl configuration structure then a default
     * table is used. */
    if (ibl.iblMagic != ibl_MAGIC_VALUE)  {
        deviceLoadDefaultIblTable();
        iblStatus.noMagic = 1;
        ibl.iblMagic = ibl_MAGIC_VALUE;
    }
#endif


#if 0  /* debugging c6472 using ccs download. Remove for i2c config download */

    /* Load the default configuration table from the i2c. The actual speed of the device
     * isn't really known here, since it is part of the table, so a compile time
     * value is used (the pll may have been configured during the initial load) */
    hwI2Cinit (IBL_I2C_DEV_FREQ_MHZ,        /* The CPU frequency during I2C data load */
               DEVICE_I2C_MODULE_DIVISOR,   /* The divide down of CPU that drives the i2c */
               IBL_I2C_CLK_FREQ_KHZ,        /* The I2C data rate used during table load */
               IBL_I2C_OWN_ADDR);           /* The address used by this device on the i2c bus */

    if (hwI2cMasterRead (IBL_I2C_CFG_TABLE_DATA_ADDR,    /* The address on the eeprom of the table */
                         sizeof(ibl_t),                  /* The number of bytes to read */
                         (UINT8 *)&ibl,                  /* Where to store the bytes */
                         IBL_I2C_CFG_EEPROM_BUS_ADDR,    /* The bus address of the eeprom */
                         IBL_I2C_CFG_ADDR_DELAY)         /* The delay between sending the address and reading data */

         != I2C_RET_OK)  {

       /* There is no recovery if the load of the configuration table failed */
       iblStatus.tableLoadFail = 0x11111111;
       for (;;);

    }

#endif

    /* Load default mac addresses for ethernet boot if requested */
    for (i = 0; i < ibl_N_ETH_PORTS; i++)  {

        if ( (iblPriorityIsValid (ibl.ethConfig[i].ethPriority)       )   &&
             (iblMacAddrIsZero   (ibl.ethConfig[i].ethInfo.hwAddress) )   )

            deviceLoadDefaultEthAddress (ibl.ethConfig[i].ethInfo.hwAddress);

    }


    /* Pll configuration is device specific */
    devicePllConfig ();

    /* DDR configuration is device specific */
    deviceDdrConfig ();

    /* Try booting forever */
    for (;;)  {

        /* Start looping through the boot modes to find the one with the lowest priority
         * value, and try to boot it */
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








