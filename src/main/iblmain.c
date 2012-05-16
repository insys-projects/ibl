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
#include "nandboot.h"
#include "norboot.h"
#include "bis.h"
#include "coffwrap.h"
#include "iblbtbl.h"
#include "iblblob.h"
#include "timer.h"
#include "i2c.h"
#include "spi_api.h"
#include "ibl_elf.h"
#include <string.h>
#include "uart.h"

extern cregister unsigned int IER;

uint32 iblEndianIdx = 0;
uint32 iblImageIdx = 0;

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
 *  @b Description
 *  @n
 *  
 *  For NAND and NOR boots, configure the specified peripheral or memory interface
 */
void iblPmemCfg (int32 interface, int32 port, bool enableNand)
{
    int32 ret;

    switch (interface)  {

        #if (!defined(EXCLUDE_NAND_GPIO))

        case ibl_PMEM_IF_GPIO:
                ret = devicePowerPeriph (TARGET_PWR_GPIO);
                break;
        #endif

        #if (!defined(EXCLUDE_NOR_SPI) && !defined(EXCLUDE_NAND_SPI))

            case ibl_PMEM_IF_SPI:  {

                    Uint32      v;
                    spiConfig_t cfg;

                    ret = devicePowerPeriph (TARGET_PWR_SPI);
                    if (ret != 0)
                        break;

                    cfg.port      = port;
                    cfg.mode      = ibl.spiConfig.mode;
                    cfg.addrWidth = ibl.spiConfig.addrWidth;
                    cfg.npin      = ibl.spiConfig.nPins;
                    cfg.csel      = ibl.spiConfig.csel;
                    cfg.c2tdelay  = ibl.spiConfig.c2tdelay;

                    /* On c66x devices the PLL module has a built in divide by 6, and the SPI
                     * has a maximum clock divider value of 0xff */
                    v = ibl.pllConfig[ibl_MAIN_PLL].pllOutFreqMhz / (DEVICE_SPI_MOD_DIVIDER * ibl.spiConfig.busFreqMHz);
                    if (v > 0xff)
                        v = 0xff;

                    cfg.clkdiv =  (UINT16) v;

                    ret = hwSpiConfig (&cfg);
                    if (ret != 0)  {
                        iblStatus.iblFail = ibl_FAIL_CODE_SPI_PARAMS;
                        return;
                    }

                }
                break;
        #endif

        #if (!defined(EXCLUDE_NOR_EMIF) || !defined(EXCLUDE_NAND_EMIF))

            case ibl_PMEM_IF_CHIPSEL_2:
            case ibl_PMEM_IF_CHIPSEL_3:
            case ibl_PMEM_IF_CHIPSEL_4:
            case ibl_PMEM_IF_CHIPSEL_5:  {

                    int i;

                    /* Locate the configuration corresponding to this chip select space */
                    for (i = 0; i < ibl_MAX_EMIF_PMEM; i++) 
                        if (ibl.emifConfig[i].csSpace == interface)
                            break;
                        
                    if (i == ibl_MAX_EMIF_PMEM)  {
                        iblStatus.iblFail = ibl_FAIL_CODE_NO_EMIF_CFG;
                        return;
                    }
#ifdef C665x
					ret = devicePowerPeriph (TARGET_PWR_EMIF_C6657);
#else
                    ret = devicePowerPeriph (TARGET_PWR_EMIF);
#endif
                    if (ret != 0)
                        break;

                    if (hwEmif25Init (interface, ibl.emifConfig[i].busWidth, ibl.emifConfig[i].waitEnable, enableNand) != 0)
                        iblStatus.iblFail = ibl_FAIL_CODE_EMIF_CFG_FAIL;

                }
                break;

            #endif

            default:



                iblStatus.iblFail = ibl_FAIL_CODE_INVALID_NAND_PERIPH;
                return;
    }

    if (ret != 0)  {
        iblStatus.iblFail = ibl_FAIL_CODE_PERIPH_POWER_UP;
        return;
    }

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
    UINT32 v, boot_mode_idx, boot_para_idx;
    unsigned int dip_setting;

    /* Initialize the status structure */
    iblMemset (&iblStatus, 0, sizeof(iblStatus_t));
    iblStatus.iblMagic   = ibl_MAGIC_VALUE;
    iblStatus.iblVersion = ibl_VERSION;

    /* Init UART */
    uart_init();
    uart_write_string("", 0);
    uart_write_string("IBL version: "ibl_VERSION_STR, 0);

    /* Power up the timer */
    devicePowerPeriph (TARGET_PWR_TIMER_0);

    /* Initialize the system timer (software tracking of the hardware timer state) */
    timer_init ();

    /* Load default mac addresses for ethernet boot if requested */
    for (i = 0; i < ibl_N_BOOT_MODES; i++)  {

        if (ibl.bootModes[i].bootMode == ibl_BOOT_MODE_TFTP)  {

            if (iblMacAddrIsZero (ibl.bootModes[i].u.ethBoot.ethInfo.hwAddress))

                deviceLoadDefaultEthAddress (ibl.bootModes[i].u.ethBoot.ethInfo.hwAddress);
        }
    }


    /* DDR configuration is device specific */
    deviceDdrConfig ();

    /* Try booting forever */
    for (;;)  {

#ifndef EXCLUDE_MULTI_BOOT
        v = DEVICE_REG32_R(DEVICE_JTAG_ID_REG);
        v &= DEVICE_JTAG_ID_MASK;
        if (
            (v == DEVICE_C6678_JTAG_ID_VAL)         || 
            (v == DEVICE_C6670_JTAG_ID_VAL)	    ||
	    (v == DEVICE_C6657_JTAG_ID_VAL)	
           )
        {
            IER = 0;

            /* For C66x devices, check the DEVSTAT register to find which image on which device to boot. */
            v = DEVICE_REG32_R(DEVICE_REG_DEVSTAT);
            
            /* Get the Endianness */
            if (ibl_N_ENDIANS == 1)
            {
                iblEndianIdx = 0;
            }
            else
            {
                if (v & ibl_ENDIAN_LITTLE)
                {
                    iblEndianIdx = 0;
                }
                else
                {
                    iblEndianIdx = 1;
                }
            }

            /* Get the boot mode index */
            boot_para_idx = BOOT_READ_BITFIELD(v,8,4);

            /* Only 1 image supported for TFTP boot */
            if (boot_para_idx > (ibl_N_IMAGES*(ibl_N_BOOT_MODES-1)))
            {
                /* boot parameter index not supported */
                continue;
            }
            boot_mode_idx = boot_para_idx/ibl_N_IMAGES;
            /* Get the boot image index */
            iblImageIdx = boot_para_idx & (ibl_N_IMAGES - 1);

            iblStatus.activeBoot = ibl.bootModes[boot_mode_idx].bootMode;

            switch (ibl.bootModes[boot_mode_idx].bootMode)  
            {
#ifndef EXCLUDE_ETH
            case ibl_BOOT_MODE_TFTP:
                iblStatus.activeDevice = ibl_ACTIVE_DEVICE_ETH;
                /*Print something to UART, max len=80, if len is pased as 0 UART prints entire string upto '\n' or max len */
                uart_write_string("IBL: Booting from ethernet",0);
                iblMemcpy (&iblStatus.ethParams, &ibl.bootModes[boot_mode_idx].u.ethBoot.ethInfo, sizeof(iblEthBootInfo_t));
                iblEthBoot (boot_mode_idx);
                break;
#endif
                
#if ((!defined(EXCLUDE_NAND_EMIF)) || (!defined(EXCLUDE_NAND_GPIO)))
            case ibl_BOOT_MODE_NAND:
                iblPmemCfg (ibl.bootModes[boot_mode_idx].u.nandBoot.interface, ibl.bootModes[boot_mode_idx].port, TRUE);
                memset ((void *)0x80000000, 0, 0x20000000);
                /*Print something to UART, max len=80, if len is pased as 0 UART prints entire string upto '\n' or max len */
                uart_write_string("IBL: Booting from NAND",0);
                iblNandBoot (boot_mode_idx);
                break;
#endif
                
#if (!defined(EXCLUDE_NOR_EMIF) && !defined(EXCLUDE_NOR_SPI))
            case ibl_BOOT_MODE_NOR:
                iblPmemCfg (ibl.bootModes[boot_mode_idx].u.norBoot.interface, ibl.bootModes[boot_mode_idx].port, TRUE);
                /*Print something to UART, max len=80, if len is pased as 0 UART prints entire string upto '\n' or max len */
                uart_write_string("IBL: Booting from NOR",0);
                iblNorBoot (boot_mode_idx);
                break;
#endif
            }
            iblStatus.heartBeat += 1;
        }
#else

	dip_setting = get_device_switch_setting();
	
	if (dip_setting == 0)
		boot_mode_idx = 0;
	else if (dip_setting == 1)
		boot_mode_idx = 1;
	
	iblStatus.activeBoot = ibl.bootModes[boot_mode_idx].bootMode;
	
	switch (ibl.bootModes[boot_mode_idx].bootMode) {                            
#ifndef EXCLUDE_ETH
                case ibl_BOOT_MODE_TFTP:
                        iblStatus.activeDevice = ibl_ACTIVE_DEVICE_ETH;
                        iblMemcpy (&iblStatus.ethParams, &ibl.bootModes[boot_mode_idx].u.ethBoot.ethInfo, sizeof(iblEthBootInfo_t));
                        /*Print something to UART, max len=80, if len is pased as 0 UART prints entire string upto '\n' or max len */
                        uart_write_string("IBL: Booting from Ethernet",0);
                        iblEthBoot (boot_mode_idx);
                        break;
#endif
                            
#if ((!defined(EXCLUDE_NAND_EMIF)) || (!defined(EXCLUDE_NAND_GPIO)))
		case ibl_BOOT_MODE_NAND:
                        iblPmemCfg (ibl.bootModes[boot_mode_idx].u.nandBoot.interface, ibl.bootModes[boot_mode_idx].port, TRUE);
                        /*Print something to UART, max len=80, if len is pased as 0 UART prints entire string upto '\n' or max len */
                        uart_write_string("IBL: Booting from NAND",0);
                        iblNandBoot (boot_mode_idx);
                        break;
#endif
	}
	iblStatus.heartBeat += 1;
#endif

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


    iblStatus.activeFileFormat = dataFormat;


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








