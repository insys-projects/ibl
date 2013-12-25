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
#include "uart.h"
#include <string.h>

extern cregister unsigned int IER;

uint32 iblEndianIdx = 0;
uint32 iblImageIdx = 0;

static void pause(uint32 count)
{
    volatile unsigned int i;

    for (i = 0; i < count; i++) asm (" nop ");
}

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
 * The function wait until host loader write branch address in MAGIC_ADDR.
 *
 * @retval
 *  None
 */
void waitForBoot(UINT32 MAGIC_ADDR)
{
    void (*exit)();
    UINT32 i, entry_addr;

    while(1)
    {
        entry_addr = DEVICE_REG32_R(MAGIC_ADDR);
        if (entry_addr != 0)
        {
            xprintf("entry = 0x%x\n\r", entry_addr);
            exit = (void (*)())entry_addr;
            (*exit)();
        }
        for (i=0; i < 100; i++)
            asm("nop");
        xprintf("IBL: Booting from PCI Express %u times\r", iblStatus.heartBeat++);
        pause(55000000);
    }
}
/*
#define SETIP(array,i0,i1,i2,i3)      array[0]=(i0);  \
                                      array[1]=(i1);  \
                                      array[2]=(i2);  \
                                      array[3]=(i3)

void c6678_ibl_config(void)
{
    memset(&ibl, 0, sizeof(ibl_t));

    ibl.iblMagic = ibl_MAGIC_VALUE;
    ibl.iblEvmType = ibl_EVM_C6678L;

    // Main PLL: 100 MHz reference, 1GHz output
    ibl.pllConfig[ibl_MAIN_PLL].doEnable      = 1;
    ibl.pllConfig[ibl_MAIN_PLL].prediv        = 1;
    ibl.pllConfig[ibl_MAIN_PLL].mult          = 20;
    ibl.pllConfig[ibl_MAIN_PLL].postdiv       = 2;
    ibl.pllConfig[ibl_MAIN_PLL].pllOutFreqMhz = 1000;

    // DDR PLL:
    ibl.pllConfig[ibl_DDR_PLL].doEnable       = 1;
    ibl.pllConfig[ibl_DDR_PLL].prediv         = 12;
    ibl.pllConfig[ibl_DDR_PLL].mult           = 50;
    ibl.pllConfig[ibl_DDR_PLL].postdiv        = 12;
    ibl.pllConfig[ibl_DDR_PLL].pllOutFreqMhz  = 1300;

    // Net PLL: 100 MHz reference, 1050 MHz output (followed by a built in divide by 3 to give 350 MHz to PA)
    ibl.pllConfig[ibl_NET_PLL].doEnable       = 1;
    ibl.pllConfig[ibl_NET_PLL].prediv         = 2;
    ibl.pllConfig[ibl_NET_PLL].mult           = 21;
    ibl.pllConfig[ibl_NET_PLL].postdiv        = 2;
    ibl.pllConfig[ibl_NET_PLL].pllOutFreqMhz  = 1050;


    ibl.ddrConfig.configDdr = 1;
    ibl.ddrConfig.uEmif.emif4p0.registerMask = ibl_EMIF4_ENABLE_sdRamConfig | ibl_EMIF4_ENABLE_sdRamRefreshCtl | ibl_EMIF4_ENABLE_sdRamTiming1 | ibl_EMIF4_ENABLE_sdRamTiming2 | ibl_EMIF4_ENABLE_sdRamTiming3 | ibl_EMIF4_ENABLE_ddrPhyCtl1;

    ibl.ddrConfig.uEmif.emif4p0.sdRamConfig				= 0x63C452B2;
    ibl.ddrConfig.uEmif.emif4p0.sdRamConfig2			= 0;
    ibl.ddrConfig.uEmif.emif4p0.sdRamRefreshCtl			= 0x000030D4;
    ibl.ddrConfig.uEmif.emif4p0.sdRamTiming1			= 0x0AAAE51B;
    ibl.ddrConfig.uEmif.emif4p0.sdRamTiming2			= 0x2A2F7FDA;
    ibl.ddrConfig.uEmif.emif4p0.sdRamTiming3 			= 0x057F82B8;
    ibl.ddrConfig.uEmif.emif4p0.lpDdrNvmTiming			= 0;
    ibl.ddrConfig.uEmif.emif4p0.powerManageCtl			= 0;
    ibl.ddrConfig.uEmif.emif4p0.iODFTTestLogic			= 0;
    ibl.ddrConfig.uEmif.emif4p0.performCountCfg			= 0;
    ibl.ddrConfig.uEmif.emif4p0.performCountMstRegSel	= 0;
    ibl.ddrConfig.uEmif.emif4p0.readIdleCtl				= 0;
    ibl.ddrConfig.uEmif.emif4p0.sysVbusmIntEnSet		= 0;
    ibl.ddrConfig.uEmif.emif4p0.sdRamOutImpdedCalCfg	= 0;
    ibl.ddrConfig.uEmif.emif4p0.tempAlterCfg			= 0;
    ibl.ddrConfig.uEmif.emif4p0.ddrPhyCtl1 				= 0x0010010d;
    ibl.ddrConfig.uEmif.emif4p0.ddrPhyCtl2				= 0;
    ibl.ddrConfig.uEmif.emif4p0.priClassSvceMap			= 0;
    ibl.ddrConfig.uEmif.emif4p0.mstId2ClsSvce1Map		= 0;
    ibl.ddrConfig.uEmif.emif4p0.mstId2ClsSvce2Map		= 0;
    ibl.ddrConfig.uEmif.emif4p0.eccCtl					= 0;
    ibl.ddrConfig.uEmif.emif4p0.eccRange1				= 0;
    ibl.ddrConfig.uEmif.emif4p0.eccRange2				= 0;
    ibl.ddrConfig.uEmif.emif4p0.rdWrtExcThresh			= 0;


    ibl.sgmiiConfig[0].configure     = 1;
    ibl.sgmiiConfig[0].adviseAbility = 1; //(1 << 15) | (1 << 14) | (1 << 12) | (2 << 10) | 1;
    ibl.sgmiiConfig[0].control		 = 1; //0x20;
    ibl.sgmiiConfig[0].txConfig      = 0x108A1;
    ibl.sgmiiConfig[0].rxConfig      = 0x700621;
    ibl.sgmiiConfig[0].auxConfig	 = 0x81;

    ibl.sgmiiConfig[1].configure     = 1;
    ibl.sgmiiConfig[1].adviseAbility = 1; //(1 << 15) | (1 << 14) | (1 << 12) | (2 << 10) | 1;
    ibl.sgmiiConfig[1].control		 = 1; //0x20;
    ibl.sgmiiConfig[1].txConfig      = 0x108A1;
    ibl.sgmiiConfig[1].rxConfig      = 0x700621;
    ibl.sgmiiConfig[1].auxConfig	 = 0x81;

    ibl.mdioConfig.nMdioOps = 0;

    ibl.spiConfig.addrWidth  = 24;
    ibl.spiConfig.nPins      = 5;
    ibl.spiConfig.mode       = 1;
    ibl.spiConfig.csel       = 2;
    ibl.spiConfig.c2tdelay   = 1;
    ibl.spiConfig.busFreqMHz = 20;

    ibl.emifConfig[0].csSpace    = 2;
    ibl.emifConfig[0].busWidth   = 8;
    ibl.emifConfig[0].waitEnable = 0;

    ibl.emifConfig[1].csSpace    = 0;
    ibl.emifConfig[1].busWidth   = 0;
    ibl.emifConfig[1].waitEnable = 0;

    ibl.bootModes[0].bootMode = ibl_BOOT_MODE_NOR;
    ibl.bootModes[0].priority = ibl_HIGHEST_PRIORITY;
    ibl.bootModes[0].port     = 0;

    ibl.bootModes[0].u.norBoot.bootFormat	= ibl_BOOT_FORMAT_ELF;
    ibl.bootModes[0].u.norBoot.bootAddress[0][0]	= 0;		// Image 0 NOR offset byte address in LE mode
    ibl.bootModes[0].u.norBoot.bootAddress[0][1]	= 0xA00000;	// Image 1 NOR offset byte address in LE mode
    ibl.bootModes[0].u.norBoot.bootAddress[1][0]	= 0;		// Image 0 NOR offset byte address in BE mode
    ibl.bootModes[0].u.norBoot.bootAddress[1][1]	= 0xA00000;	// Image 1 NOR offset byte address in BE mode
    ibl.bootModes[0].u.norBoot.interface	= ibl_PMEM_IF_SPI;
    ibl.bootModes[0].u.norBoot.blob[0][0].startAddress  = 0x80000000;       // Image 0 load start address in LE mode
    ibl.bootModes[0].u.norBoot.blob[0][0].sizeBytes     = 0xA00000;         // Image 0 size (10 MB) in LE mode
    ibl.bootModes[0].u.norBoot.blob[0][0].branchAddress = 0x80000000;       // Image 0 branch address after loading in LE mode
    ibl.bootModes[0].u.norBoot.blob[0][1].startAddress  = 0x80000000;       // Image 1 load start address in LE mode
    ibl.bootModes[0].u.norBoot.blob[0][1].sizeBytes     = 0xA00000;         // Image 1 size (10 MB) in LE mode
    ibl.bootModes[0].u.norBoot.blob[0][1].branchAddress = 0x80000000;       // Image 1 branch address after loading in LE mode
    ibl.bootModes[0].u.norBoot.blob[1][0].startAddress  = 0x80000000;       // Image 0 load start address in BE mode
    ibl.bootModes[0].u.norBoot.blob[1][0].sizeBytes     = 0xA00000;         // Image 0 size (10 MB) in BE mode
    ibl.bootModes[0].u.norBoot.blob[1][0].branchAddress = 0x80000000;       // Image 0 branch address after loading in BE mode
    ibl.bootModes[0].u.norBoot.blob[1][1].startAddress  = 0x80000000;       // Image 1 load start address in BE mode
    ibl.bootModes[0].u.norBoot.blob[1][1].sizeBytes     = 0xA00000;         // Image 1 size (10 MB) in BE mode
    ibl.bootModes[0].u.norBoot.blob[1][1].branchAddress = 0x80000000;       // Image 1 branch address after loading in BE mode

    ibl.bootModes[1].bootMode = ibl_BOOT_MODE_NAND;
    ibl.bootModes[1].priority = ibl_HIGHEST_PRIORITY;
    ibl.bootModes[1].port     = 0;

    ibl.bootModes[1].u.nandBoot.bootFormat        = ibl_BOOT_FORMAT_BBLOB;
    ibl.bootModes[1].u.nandBoot.bootAddress[0][0]	  = 0x4000;         // Image 0 NAND offset address (block 1) in LE mode
    ibl.bootModes[1].u.nandBoot.bootAddress[0][1]	  = 0x2000000;      // Image 1 NAND offset address (block 2048) in LE mode
    ibl.bootModes[1].u.nandBoot.bootAddress[1][0]	  = 0x4000;	    // Image 0 NAND offset address (block 1) in BE mode
    ibl.bootModes[1].u.nandBoot.bootAddress[1][1]	  = 0x2000000;      // Image 1 NAND offset address (block 2048) in BE mode
    ibl.bootModes[1].u.nandBoot.interface         = ibl_PMEM_IF_CHIPSEL_2;

    ibl.bootModes[1].u.nandBoot.blob[0][0].startAddress  = 0x80000000;       // Image 0 load start address in LE mode
    ibl.bootModes[1].u.nandBoot.blob[0][0].sizeBytes     = 0xFFC000;         // Image 0 size in LE mode
    ibl.bootModes[1].u.nandBoot.blob[0][0].branchAddress = 0x80000000;       // Image 0 branch address after loading in LE mode
    ibl.bootModes[1].u.nandBoot.blob[0][1].startAddress  = 0x80000000;       // Image 1 load start address in LE mode
    ibl.bootModes[1].u.nandBoot.blob[0][1].sizeBytes     = 0xFFC000;         // Image 1 size in LE mode
    ibl.bootModes[1].u.nandBoot.blob[0][1].branchAddress = 0x80000000;       // Image 1 branch address after loading in LE mode
    ibl.bootModes[1].u.nandBoot.blob[1][0].startAddress  = 0x80000000;       // Image 0 load start address in BE mode
    ibl.bootModes[1].u.nandBoot.blob[1][0].sizeBytes     = 0xFFC000;         // Image 0 size in BE mode
    ibl.bootModes[1].u.nandBoot.blob[1][0].branchAddress = 0x80000000;       // Image 0 branch address after loading in BE mode
    ibl.bootModes[1].u.nandBoot.blob[1][1].startAddress  = 0x80000000;       // Image 1 load start address in BE mode
    ibl.bootModes[1].u.nandBoot.blob[1][1].sizeBytes     = 0xFFC000;         // Image 1 size in BE mode
    ibl.bootModes[1].u.nandBoot.blob[1][1].branchAddress = 0x80000000;       // Image 1 branch address after loading in BE mode


    ibl.bootModes[1].u.nandBoot.nandInfo.busWidthBits  = 8;
    ibl.bootModes[1].u.nandBoot.nandInfo.pageSizeBytes = 512;
    ibl.bootModes[1].u.nandBoot.nandInfo.pageEccBytes  = 16;
    ibl.bootModes[1].u.nandBoot.nandInfo.pagesPerBlock = 32;
    ibl.bootModes[1].u.nandBoot.nandInfo.totalBlocks   = 4096;

    ibl.bootModes[1].u.nandBoot.nandInfo.addressBytes  = 4;
    ibl.bootModes[1].u.nandBoot.nandInfo.lsbFirst      = TRUE;
    ibl.bootModes[1].u.nandBoot.nandInfo.blockOffset   = 14;
    ibl.bootModes[1].u.nandBoot.nandInfo.pageOffset    = 9;
    ibl.bootModes[1].u.nandBoot.nandInfo.columnOffset  = 0;

    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[0]  = 0;
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[1]  = 1;
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[2]  = 2;
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[3]  = 3;
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[4]  = 4;
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[5]  = 6;
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[6]  = 7;
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[7]  = 13;
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[8]  = 14;
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[9]  = 15;

    ibl.bootModes[1].u.nandBoot.nandInfo.badBlkMarkIdx[0]= 5;
    ibl.bootModes[1].u.nandBoot.nandInfo.badBlkMarkIdx[1]= 0xff;

    ibl.bootModes[1].u.nandBoot.nandInfo.resetCommand    = 0xff;
    ibl.bootModes[1].u.nandBoot.nandInfo.readCommandPre  = 0;
    ibl.bootModes[1].u.nandBoot.nandInfo.readCommandPost = 0;
    ibl.bootModes[1].u.nandBoot.nandInfo.postCommand     = FALSE;

    ibl.bootModes[2].bootMode = ibl_BOOT_MODE_TFTP;
    ibl.bootModes[2].priority = ibl_HIGHEST_PRIORITY+1;
    ibl.bootModes[2].port     = ibl_PORT_SWITCH_ALL;

    ibl.bootModes[2].u.ethBoot.doBootp          = FALSE;
    ibl.bootModes[2].u.ethBoot.useBootpServerIp = TRUE;
    ibl.bootModes[2].u.ethBoot.useBootpFileName = TRUE;
    ibl.bootModes[2].u.ethBoot.bootFormat       = ibl_BOOT_FORMAT_BBLOB;


    SETIP(ibl.bootModes[2].u.ethBoot.ethInfo.ipAddr,    192,168,0,197);
    SETIP(ibl.bootModes[2].u.ethBoot.ethInfo.serverIp,  192,168,0,121);
    SETIP(ibl.bootModes[2].u.ethBoot.ethInfo.gatewayIp, 192,168,0,1);
    SETIP(ibl.bootModes[2].u.ethBoot.ethInfo.netmask,   255,255,255,0);

    // Use the e-fuse value
    ibl.bootModes[2].u.ethBoot.ethInfo.hwAddress[0] = 0;
    ibl.bootModes[2].u.ethBoot.ethInfo.hwAddress[1] = 0;
    ibl.bootModes[2].u.ethBoot.ethInfo.hwAddress[2] = 0;
    ibl.bootModes[2].u.ethBoot.ethInfo.hwAddress[3] = 0;
    ibl.bootModes[2].u.ethBoot.ethInfo.hwAddress[4] = 0;
    ibl.bootModes[2].u.ethBoot.ethInfo.hwAddress[5] = 0;


    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[0]  = 'c';
    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[1]  = '6';
    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[2]  = '6';
    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[3]  = '7';
    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[4]  = '8';
    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[5]  = '-';
    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[6]  = 'l';
    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[7]  = 'e';
    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[8]  = '.';
    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[9]  = 'b';
    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[10] = 'i';
    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[11] = 'n';
    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[12] = '\0';
    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[13] = '\0';
    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[14] = '\0';

    ibl.bootModes[2].u.ethBoot.blob.startAddress  = 0x80000000;       // Load start address
    ibl.bootModes[2].u.ethBoot.blob.sizeBytes     = 0x20000000;
    ibl.bootModes[2].u.ethBoot.blob.branchAddress = 0x80000000;       // Branch address after loading

    ibl.chkSum = 0;
}
*/
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
    int32 i, boot_type;
    UINT32 v, boot_mode_idx, boot_para_idx;

    /* Initialize the status structure */
    iblMemset (&iblStatus, 0, sizeof(iblStatus_t));
    iblStatus.iblMagic   = ibl_MAGIC_VALUE;
    iblStatus.iblVersion = ibl_VERSION;

    xprintf("\n\r");
    xprintf("IBL version: %s\n\r", ibl_VERSION_STR);

    /* Power up the timer */
    //xprintf("Start devicePowerPeriph()...");
    devicePowerPeriph (TARGET_PWR_TIMER_0);
    //xprintf("complete\n\r",0);

    /* Initialize the system timer (software tracking of the hardware timer state) */
    //xprintf("Start timer_init()...");
    timer_init ();
    //xprintf("complete\n\r");

    /* Load default mac addresses for ethernet boot if requested */
    for (i = 0; i < ibl_N_BOOT_MODES; i++)  {

        if (ibl.bootModes[i].bootMode == ibl_BOOT_MODE_TFTP)  {

            if (iblMacAddrIsZero (ibl.bootModes[i].u.ethBoot.ethInfo.hwAddress))

                deviceLoadDefaultEthAddress (ibl.bootModes[i].u.ethBoot.ethInfo.hwAddress);
        }
    }

    //c6678_ibl_config();
    //xprintf(" DEBUG: c6678_ibl_config()\n\r");

    /* DDR configuration is device specific */
    //xprintf("Start deviceDdrConfig() --- \n\r");
    deviceDdrConfig ();
    //xprintf(" --- complete\n\r");

    v = DEVICE_REG32_R(DEVICE_REG_DEVSTAT);

    xprintf("DEVSTAT = 0x%x\n\r", v);
    xprintf("BOOTMODE = 0x%x\n\r", (v >> 1) & 0xfff);

    boot_type = ((v >> 4) & 0x3);

    xprintf("BOARD_BOOT_TYPE = 0x%x\n\r", boot_type );

    while(1) {

        switch(boot_type) {
        case 0: {
            LED_smart('0');
            waitForBoot(0x87fffc);
        }
        break;
        case 1: {
            iblStatus.activeBoot = ibl_BOOT_MODE_TFTP;
            iblStatus.activeDevice = ibl_ACTIVE_DEVICE_ETH;
            xprintf("IBL: Booting from ethernet %u times\n\r", iblStatus.heartBeat);
            LED_smart('1');
            iblMemcpy (&iblStatus.ethParams, &ibl.bootModes[2].u.ethBoot.ethInfo, sizeof(iblEthBootInfo_t));
            iblEthBoot(2);
        } break;
        case 2: {
            iblStatus.activeBoot = ibl_BOOT_MODE_NOR;
            iblStatus.activeDevice = ibl_ACTIVE_DEVICE_SPI;
            LED_smart('2');
            iblPmemCfg (ibl.bootModes[0].u.norBoot.interface, ibl.bootModes[0].port, FALSE);
            xprintf("IBL: Booting from SPI NOR %u times\r", iblStatus.heartBeat);
            iblNorBoot(0);
        } break;
        }

        iblStatus.heartBeat += 1;
    }

    /* Try booting forever */
    for (;;)  {

#ifndef EXCLUDE_MULTI_BOOT
        v = DEVICE_REG32_R(DEVICE_JTAG_ID_REG);
        v &= DEVICE_JTAG_ID_MASK;
        if (
            (v == DEVICE_C6678_JTAG_ID_VAL)         ||
            (v == DEVICE_C6670_JTAG_ID_VAL)	        ||
            (v == DEVICE_C6657_JTAG_ID_VAL)	        ||
            (v == DEVICE_TCI6636K2H_JTAG_ID_VAL)
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
                xprintf("IBL: Booting from ethernet %u times\r", iblStatus.heartBeat);
                iblMemcpy (&iblStatus.ethParams, &ibl.bootModes[boot_mode_idx].u.ethBoot.ethInfo, sizeof(iblEthBootInfo_t));
                iblEthBoot (boot_mode_idx);
                break;
#endif

#if ((!defined(EXCLUDE_NAND_EMIF)) || (!defined(EXCLUDE_NAND_GPIO)))
            case ibl_BOOT_MODE_NAND:
                iblPmemCfg (ibl.bootModes[boot_mode_idx].u.nandBoot.interface, ibl.bootModes[boot_mode_idx].port, TRUE);
                memset ((void *)0x80000000, 0, 0x20000000);
                xprintf("IBL: Booting from NAND %u times\r", iblStatus.heartBeat);
                iblNandBoot (boot_mode_idx);
                break;
#endif

#if (!defined(EXCLUDE_NOR_EMIF) && !defined(EXCLUDE_NOR_SPI))
            case ibl_BOOT_MODE_NOR:
                iblPmemCfg (ibl.bootModes[boot_mode_idx].u.norBoot.interface, ibl.bootModes[boot_mode_idx].port, TRUE);
                xprintf("IBL: Booting from NOR %u times\r", iblStatus.heartBeat);
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








