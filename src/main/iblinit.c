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

/**
 *  @file iblinit.c
 *
 *  @brief
 *		This file contains code which runs prior to loading the full IBL
 *
 *  @details
 *		The IBL loads itself in a two stage process. The ROM boot loader
 *		loads this first stage IBL first. This entire program must be
 *		endian independent in execution.
 *
 *      This first loader reads the IBL parameters, and will endian
 *      switch them if required. The PLL is configured if indicated
 *      by the parameters.
 *
 *      The I2C block which contains the I2C EEPROM address for both
 *      the big and little endian images is then read. Based on the
 *      endianness of the device the rest of the IBL is read from
 *      the I2C EEPROM, and execution is transferred to the full
 *      IBL.
 *
 *      The subsequent reads are allowed to cross 16 bit i2c EEPROM
 *      addresses. When the boundary is crossed the i2c address
 *      field is incremented.
 *
 */

#include "ibl.h"
#include "iblloc.h"
#include "iblcfg.h"
#include "device.h"
#include "iblbtbl.h"
#include "iblinit.h"
#include <string.h>


/**
 *  @brief
 *      Data structures shared between the 1st and 2nd stage IBL load
 *      are declared in a single header file, included in both stages
 */
#include "iblStage.h"

/**
 *  @brief
 *      The boot table processing status is declared in the boot table wrapper,
 *      and used here in the main status fields.
 */
extern Int32 btblWrapEcode;

/**
 *  @brief
 *      The malloc function used for both boot stages of the ibl
 */
void *iblMalloc (Uint32 size)
{
    return (malloc (size));
}

/**
 *  @brief
 *      The free function used for both stages of the ibl
 */
void iblFree (void *mem)
{
    free (mem);
}

/**
 *  @brief
 *      The memset function used for both stages of the ibl
 */
void *iblMemset (void *mem, Int32 ch, Uint32 n)
{
    return (memset (mem, ch, n));
}

/**
 *  @brief
 *      The memcpy function used for both stages of the ibl
 */
void *iblMemcpy (void *s1, const void *s2, Uint32 n)
{
    return (memcpy (s1, s2, n));

}

/**
 *  @brief
 *      Ones complement addition
 */
inline uint16 onesComplementAdd (uint16 value1, uint16 value2)
{
  uint32 result;

  result = (uint32)value1 + (uint32)value2;

  result = (result >> 16) + (result & 0xFFFF); /* add in carry   */
  result += (result >> 16);                    /* maybe one more */
  return ((uint16)result);
}


/**
 *  @brief
 *      Ones complement checksum computation 
 */
uint16 onesComplementChksum (uint16 * restrict p_data, uint16 len)
{
  uint16 chksum = 0;

  while (len > 0)
  {
    chksum = onesComplementAdd(chksum, *p_data);
    p_data++;
    len--;
  }
  return (chksum);
} 



/**
 *  @brief
 *      Do a 4 byte endian swap
 */
uint32 swap32val (uint32 v)
{
    v =  (((v >> 24) & 0xff) <<  0)  |
         (((v >> 16) & 0xff) <<  8)  |
         (((v >>  8) & 0xff) << 16)  |
         (((v >>  0) & 0xff) << 24);

    return (v);

}

/**
 *  @brief
 *      Do a 2 byte endian swap
 */
uint16 swap16val (uint16 v)
{
    v = (((v >> 8) & 0xff) << 0)  |
        (((v >> 0) & 0xff) << 8);

    return (v);

}

/**
 *  @brief
 *  Do an endian swap on the ibl structure
 */
void iblSwap (void)
{
    int i, j, k;

    ibl.iblMagic = swap32val (ibl.iblMagic);

    for (i = 0; i < ibl_N_PLL_CFGS; i++)  {
        ibl.pllConfig[i].doEnable      = swap16val (ibl.pllConfig[i].doEnable);
        ibl.pllConfig[i].prediv        = swap32val (ibl.pllConfig[i].prediv);
        ibl.pllConfig[i].mult          = swap32val (ibl.pllConfig[i].mult);
        ibl.pllConfig[i].postdiv       = swap32val (ibl.pllConfig[i].postdiv);
        ibl.pllConfig[i].pllOutFreqMhz = swap32val (ibl.pllConfig[i].pllOutFreqMhz);
    }

    ibl.ddrConfig.configDdr = swap16val (ibl.ddrConfig.configDdr);


    if (targetEmifType() == ibl_EMIF_TYPE_31)  { 
        ibl.ddrConfig.uEmif.emif3p1.sdcfg  = swap32val(ibl.ddrConfig.uEmif.emif3p1.sdcfg);
        ibl.ddrConfig.uEmif.emif3p1.sdrfc  = swap32val(ibl.ddrConfig.uEmif.emif3p1.sdrfc);
        ibl.ddrConfig.uEmif.emif3p1.sdtim1 = swap32val(ibl.ddrConfig.uEmif.emif3p1.sdtim1);
        ibl.ddrConfig.uEmif.emif3p1.sdtim2 = swap32val(ibl.ddrConfig.uEmif.emif3p1.sdtim2);
        ibl.ddrConfig.uEmif.emif3p1.dmcctl = swap32val(ibl.ddrConfig.uEmif.emif3p1.dmcctl);
       
    } else if (targetEmifType() == ibl_EMIF_TYPE_40)  {
        ibl.ddrConfig.uEmif.emif4p0.registerMask          = swap32val(ibl.ddrConfig.uEmif.emif4p0.registerMask);
        ibl.ddrConfig.uEmif.emif4p0.sdRamConfig           = swap32val(ibl.ddrConfig.uEmif.emif4p0.sdRamConfig);
        ibl.ddrConfig.uEmif.emif4p0.sdRamConfig2          = swap32val(ibl.ddrConfig.uEmif.emif4p0.sdRamConfig2);
        ibl.ddrConfig.uEmif.emif4p0.sdRamRefreshCtl       = swap32val(ibl.ddrConfig.uEmif.emif4p0.sdRamRefreshCtl);
        ibl.ddrConfig.uEmif.emif4p0.sdRamTiming1          = swap32val(ibl.ddrConfig.uEmif.emif4p0.sdRamTiming1);
        ibl.ddrConfig.uEmif.emif4p0.sdRamTiming2          = swap32val(ibl.ddrConfig.uEmif.emif4p0.sdRamTiming2);
        ibl.ddrConfig.uEmif.emif4p0.sdRamTiming3          = swap32val(ibl.ddrConfig.uEmif.emif4p0.sdRamTiming3);
        ibl.ddrConfig.uEmif.emif4p0.lpDdrNvmTiming        = swap32val(ibl.ddrConfig.uEmif.emif4p0.lpDdrNvmTiming);
        ibl.ddrConfig.uEmif.emif4p0.powerManageCtl        = swap32val(ibl.ddrConfig.uEmif.emif4p0.powerManageCtl);
        ibl.ddrConfig.uEmif.emif4p0.iODFTTestLogic        = swap32val(ibl.ddrConfig.uEmif.emif4p0.iODFTTestLogic);
        ibl.ddrConfig.uEmif.emif4p0.performCountCfg       = swap32val(ibl.ddrConfig.uEmif.emif4p0.performCountCfg);
        ibl.ddrConfig.uEmif.emif4p0.performCountMstRegSel = swap32val(ibl.ddrConfig.uEmif.emif4p0.performCountMstRegSel);
        ibl.ddrConfig.uEmif.emif4p0.readIdleCtl           = swap32val(ibl.ddrConfig.uEmif.emif4p0.readIdleCtl);
        ibl.ddrConfig.uEmif.emif4p0.sysVbusmIntEnSet      = swap32val(ibl.ddrConfig.uEmif.emif4p0.sysVbusmIntEnSet);
        ibl.ddrConfig.uEmif.emif4p0.sdRamOutImpdedCalCfg  = swap32val(ibl.ddrConfig.uEmif.emif4p0.sdRamOutImpdedCalCfg);
        ibl.ddrConfig.uEmif.emif4p0.tempAlterCfg          = swap32val(ibl.ddrConfig.uEmif.emif4p0.tempAlterCfg);
        ibl.ddrConfig.uEmif.emif4p0.ddrPhyCtl1            = swap32val(ibl.ddrConfig.uEmif.emif4p0.ddrPhyCtl1);
        ibl.ddrConfig.uEmif.emif4p0.ddrPhyCtl2            = swap32val(ibl.ddrConfig.uEmif.emif4p0.ddrPhyCtl2);
        ibl.ddrConfig.uEmif.emif4p0.priClassSvceMap       = swap32val(ibl.ddrConfig.uEmif.emif4p0.priClassSvceMap);
        ibl.ddrConfig.uEmif.emif4p0.mstId2ClsSvce1Map     = swap32val(ibl.ddrConfig.uEmif.emif4p0.mstId2ClsSvce1Map);
        ibl.ddrConfig.uEmif.emif4p0.mstId2ClsSvce2Map     = swap32val(ibl.ddrConfig.uEmif.emif4p0.mstId2ClsSvce2Map);
        ibl.ddrConfig.uEmif.emif4p0.eccCtl                = swap32val(ibl.ddrConfig.uEmif.emif4p0.eccCtl);
        ibl.ddrConfig.uEmif.emif4p0.eccRange1             = swap32val(ibl.ddrConfig.uEmif.emif4p0.eccRange1);
        ibl.ddrConfig.uEmif.emif4p0.eccRange2             = swap32val(ibl.ddrConfig.uEmif.emif4p0.eccRange2);
        ibl.ddrConfig.uEmif.emif4p0.rdWrtExcThresh        = swap32val(ibl.ddrConfig.uEmif.emif4p0.rdWrtExcThresh);
    }


    for (i = 0; i < ibl_N_ETH_PORTS; i++)  {
        ibl.sgmiiConfig[i].configure     = swap16val(ibl.sgmiiConfig[i].configure);
        ibl.sgmiiConfig[i].adviseAbility = swap32val(ibl.sgmiiConfig[i].adviseAbility);
        ibl.sgmiiConfig[i].control       = swap32val(ibl.sgmiiConfig[i].control);
        ibl.sgmiiConfig[i].txConfig      = swap32val(ibl.sgmiiConfig[i].txConfig);
        ibl.sgmiiConfig[i].rxConfig      = swap32val(ibl.sgmiiConfig[i].rxConfig);
        ibl.sgmiiConfig[i].auxConfig     = swap32val(ibl.sgmiiConfig[i].auxConfig);
    }
    

    ibl.mdioConfig.nMdioOps   = swap16val (ibl.mdioConfig.nMdioOps);
    ibl.mdioConfig.mdioClkDiv = swap16val (ibl.mdioConfig.mdioClkDiv);
    ibl.mdioConfig.interDelay = swap32val (ibl.mdioConfig.interDelay);

    for (i = 0; i < ibl_N_MDIO_CFGS; i++)
        ibl.mdioConfig.mdio[i] = swap32val (ibl.mdioConfig.mdio[i]);


    ibl.spiConfig.addrWidth  = swap16val(ibl.spiConfig.addrWidth);
    ibl.spiConfig.nPins      = swap16val(ibl.spiConfig.nPins);
    ibl.spiConfig.mode       = swap16val(ibl.spiConfig.mode);
    ibl.spiConfig.csel       = swap16val(ibl.spiConfig.csel);
    ibl.spiConfig.c2tdelay   = swap16val(ibl.spiConfig.c2tdelay);
    ibl.spiConfig.busFreqMHz = swap16val(ibl.spiConfig.busFreqMHz);

    for (i = 0; i < ibl_MAX_EMIF_PMEM; i++)  {
        ibl.emifConfig[i].csSpace    = swap16val(ibl.emifConfig[i].csSpace);
        ibl.emifConfig[i].busWidth   = swap16val(ibl.emifConfig[i].busWidth);
        ibl.emifConfig[i].waitEnable = swap16val(ibl.emifConfig[i].waitEnable);
    }


    for (i = 0; i < ibl_N_BOOT_MODES; i++)  {
        ibl.bootModes[i].bootMode = swap32val(ibl.bootModes[i].bootMode);
        ibl.bootModes[i].priority = swap32val(ibl.bootModes[i].priority);
        ibl.bootModes[i].port     = swap32val(ibl.bootModes[i].port);

        if (ibl.bootModes[i].bootMode == ibl_BOOT_MODE_TFTP)  {
            ibl.bootModes[i].u.ethBoot.doBootp            = swap16val(ibl.bootModes[i].u.ethBoot.doBootp);
            ibl.bootModes[i].u.ethBoot.useBootpServerIp   = swap16val(ibl.bootModes[i].u.ethBoot.useBootpServerIp);
            ibl.bootModes[i].u.ethBoot.useBootpFileName   = swap16val(ibl.bootModes[i].u.ethBoot.useBootpFileName);
            ibl.bootModes[i].u.ethBoot.bootFormat         = swap32val(ibl.bootModes[i].u.ethBoot.bootFormat);
            ibl.bootModes[i].u.ethBoot.blob.startAddress  = swap32val(ibl.bootModes[i].u.ethBoot.blob.startAddress);
            ibl.bootModes[i].u.ethBoot.blob.sizeBytes     = swap32val(ibl.bootModes[i].u.ethBoot.blob.sizeBytes);
            ibl.bootModes[i].u.ethBoot.blob.branchAddress = swap32val(ibl.bootModes[i].u.ethBoot.blob.branchAddress);

        }  else if (ibl.bootModes[i].bootMode == ibl_BOOT_MODE_NAND)  {
            ibl.bootModes[i].u.nandBoot.bootFormat             = swap32val(ibl.bootModes[i].u.nandBoot.bootFormat);
            for (j = 0; j < ibl_N_ENDIANS; j++)
            {
                for (k = 0; k < ibl_N_IMAGES; k++)
                {
                    ibl.bootModes[i].u.nandBoot.bootAddress[j][k] = swap32val(ibl.bootModes[i].u.nandBoot.bootAddress[j][k]);
                }
            }
            ibl.bootModes[i].u.nandBoot.interface              = swap32val(ibl.bootModes[i].u.nandBoot.interface);
            for (j = 0; j < ibl_N_ENDIANS; j++)
            {
                for (k = 0; k < ibl_N_IMAGES; k++)
                {
                    ibl.bootModes[i].u.nandBoot.blob[j][k].startAddress  = swap32val(ibl.bootModes[i].u.nandBoot.blob[j][k].startAddress);
                    ibl.bootModes[i].u.nandBoot.blob[j][k].sizeBytes     = swap32val(ibl.bootModes[i].u.nandBoot.blob[j][k].sizeBytes);
                    ibl.bootModes[i].u.nandBoot.blob[j][k].branchAddress = swap32val(ibl.bootModes[i].u.nandBoot.blob[j][k].branchAddress);
                }
            }
            ibl.bootModes[i].u.nandBoot.nandInfo.busWidthBits  = swap32val(ibl.bootModes[i].u.nandBoot.nandInfo.busWidthBits);
            ibl.bootModes[i].u.nandBoot.nandInfo.pageSizeBytes = swap32val(ibl.bootModes[i].u.nandBoot.nandInfo.pageSizeBytes);
            ibl.bootModes[i].u.nandBoot.nandInfo.pageEccBytes  = swap32val(ibl.bootModes[i].u.nandBoot.nandInfo.pageEccBytes);
            ibl.bootModes[i].u.nandBoot.nandInfo.pagesPerBlock = swap32val(ibl.bootModes[i].u.nandBoot.nandInfo.pagesPerBlock);
            ibl.bootModes[i].u.nandBoot.nandInfo.totalBlocks   = swap32val(ibl.bootModes[i].u.nandBoot.nandInfo.totalBlocks);
            ibl.bootModes[i].u.nandBoot.nandInfo.addressBytes  = swap32val(ibl.bootModes[i].u.nandBoot.nandInfo.addressBytes);
            ibl.bootModes[i].u.nandBoot.nandInfo.lsbFirst      = swap16val(ibl.bootModes[i].u.nandBoot.nandInfo.lsbFirst);
            ibl.bootModes[i].u.nandBoot.nandInfo.blockOffset   = swap32val(ibl.bootModes[i].u.nandBoot.nandInfo.blockOffset);
            ibl.bootModes[i].u.nandBoot.nandInfo.pageOffset    = swap32val(ibl.bootModes[i].u.nandBoot.nandInfo.pageOffset);
            ibl.bootModes[i].u.nandBoot.nandInfo.columnOffset  = swap32val(ibl.bootModes[i].u.nandBoot.nandInfo.columnOffset);
            ibl.bootModes[i].u.nandBoot.nandInfo.postCommand   = swap16val(ibl.bootModes[i].u.nandBoot.nandInfo.postCommand);
        }  else if (ibl.bootModes[i].bootMode == ibl_BOOT_MODE_NOR)  {
            ibl.bootModes[i].u.norBoot.bootFormat         = swap32val(ibl.bootModes[i].u.norBoot.bootFormat);
            for (j = 0; j < ibl_N_ENDIANS; j++)
            {
                for (k = 0; k < ibl_N_IMAGES; k++)
                {
                    ibl.bootModes[i].u.norBoot.bootAddress[j][k] = swap32val(ibl.bootModes[i].u.norBoot.bootAddress[j][k]);
                }
            }
            ibl.bootModes[i].u.norBoot.interface          = swap32val(ibl.bootModes[i].u.norBoot.interface);
            for (j = 0; j < ibl_N_ENDIANS; j++)
            {
                for (k = 0; k < ibl_N_IMAGES; k++)
                {
                    ibl.bootModes[i].u.norBoot.blob[j][k].startAddress  = swap32val(ibl.bootModes[i].u.norBoot.blob[j][k].startAddress);
                    ibl.bootModes[i].u.norBoot.blob[j][k].sizeBytes     = swap32val(ibl.bootModes[i].u.norBoot.blob[j][k].sizeBytes);
                    ibl.bootModes[i].u.norBoot.blob[j][k].branchAddress = swap32val(ibl.bootModes[i].u.norBoot.blob[j][k].branchAddress);
                }
            }

        }

    }

    ibl.iblEvmType = swap16val (ibl.iblEvmType);
    ibl.chkSum = swap16val (ibl.chkSum);
}

        

/**
 *  @brief
 *      The init load context consists of the address of the next block 
 *      to read, and a simple fifo-ish structure holding any existing data.
 *
 *      A full fifo is not defined here. The individual init load blocks
 *      (I2C, SPI NOR, SPI NAND) will poke the structure values directly
 *      to minimize the compiled code size. Most notably is the absence
 *      of the write function. This fifo will always be completely emptied
 *      before any writes are done, so writes are always done from the top.
 */
uint32 iFifoIn  = 0;
uint32 iFifoOut = 0;
uint8  iData[I_MAX_BLOCK_SIZE];

/**
 *  @brief
 *      Checkum calculation. 16 bit values constructed from received bytes
 *      always in big endian format, regardless of the endianness of the device 
 */
uint16 iSum[I_MAX_BLOCK_SIZE >> 1];


/**
 *  @brief
 *      Return the number of elements in the fifo
 */
Uint32 iFifoCount (void)
{
    Int32 count;

    if (iFifoIn >= iFifoOut)
        count = iFifoIn - iFifoOut;
    else
        count = iFifoIn + I_MAX_BLOCK_SIZE - iFifoOut;

    return (count);

}

    
/**
 *  @brief
 *      Read a byte from the fifo
 */
Uint8 iFifoRead(void)
{
    Uint8 v;

    v = iData[iFifoOut];

    iFifoOut += 1;

    if (iFifoOut == iFifoIn)
        iFifoOut = iFifoIn = 0;

    if (iFifoOut >= I_MAX_BLOCK_SIZE)
        iFifoOut = 0;

    return (v);

}


#define iblBITMASK(x,y)      (   (   (  ((UINT32)1 << (((UINT32)x)-((UINT32)y)+(UINT32)1) ) - (UINT32)1 )   )   <<  ((UINT32)y)   )
#define iblREAD_BITFIELD(z,x,y)   (((UINT32)z) & iblBITMASK(x,y)) >> (y)
/**
 *  @brief
 *      Return the lower 16 bits of a 32 bit value. A function is used (with cross-function optomization off)
 *      which results in an endian independent version
 */
uint16 readLower16 (uint32 v)
{
    return (iblREAD_BITFIELD(v,15,0));

}

/**
 * @brief
 *      Return the upper 16 bits of a 32 bit value. A function is used to force an endian independent version
 */
uint16 readUpper16 (uint32 v)
{
  return (iblREAD_BITFIELD(v,31,16));
}



/**
 *  @brief
 *      The main function
 *
 *  @details
 *      The ibl configuration parameters are read from the i2c, 
 *      followed by the i2c mapping information. The second stage
 *      of the IBL is then loaded, and execution transferred 
 *      to the second stage.
 */
void main (void)
{

    int32        bootDevice;
    uint32       entry;
    void         (*exit)();

    BOOT_MODULE_FXN_TABLE *bFxnTbl;

    memset (&iblStatus, 0, sizeof(iblStatus_t));
    iblStatus.iblMagic     = ibl_MAGIC_VALUE;
    iblStatus.iblVersion   = ibl_VERSION;
    iblStatus.activeDevice = ibl_ACTIVE_DEVICE_I2C;

#ifdef C665x
     /*Set GPIO as SPI,UART*/
    configureGPIO();
#endif
    /* Determine the boot device to read from */
    bootDevice = deviceReadBootDevice();

    switch (bootDevice)  {

#ifndef EXCLUDE_I2C
    case BOOT_DEVICE_I2C:       bFxnTbl = iblInitI2c ();
                                break;
#endif

#ifndef EXCLUDE_NOR_SPI
    case BOOT_DEVICE_SPI_NOR:   bFxnTbl = iblInitSpiNor ();
                                break;
#endif


    default:                    iblStatus.iblFail = ibl_FAIL_CODE_INVALID_INIT_DEVICE;
                                for (;;);

    }
    

    /* Pll configuration is device specific */
    devicePllConfig ();

    /* Enable the EDC for local memory */
    if (IBL_ENABLE_EDC)
    {
        iblEnableEDC ();
    }

    /* Check if need to enter Rom boot loader again */
    if (IBL_ENTER_ROM)
    {
        iblEnterRom ();
    }

    /* Pass control to the boot table processor */
    iblBootBtbl (bFxnTbl, &entry);

    if (btblWrapEcode != 0)  {
        iblStatus.iblFail = ibl_FAIL_CODE_BTBL_FAIL;
        for (;;);
    }

    /* jump to the exit point, which will be the entry point for the full IBL */
    exit = (void (*)())entry;
    (*exit)();


}











