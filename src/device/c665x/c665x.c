/*
 *
 * Copyright (C) 2011-12 Texas Instruments Incorporated - http://www.ti.com/ 
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



/************************************************************************************
 * FILE PURPOSE: C66x Device Specific functions
 ************************************************************************************
 * FILE NAME: c66x.c
 *
 * DESCRIPTION: Implements the device specific functions for the IBL
 *
 * @file c66x.c
 *
 * @brief
 *  This file implements the device specific functions for the IBL
 *
 ************************************************************************************/
#include "ibl.h"
#include "iblloc.h"
#include "iblcfg.h"
#include "device.h"
#include "pllapi.h"
#include "emif31api.h"
#include "pscapi.h"
#include "gpio.h"
#include "qm_api.h"
#include "cpdma_api.h"
#include "pa_api.h"
#include "serdes_api.h"
#include "net.h"
#include "nandhwapi.h"
#include "nor_api.h"
#include "spi_api.h"
#include <string.h>
#include <stdint.h>
#include "target.h"
#include "uart.h"

#define PLL_DDR_INIT_LOOPMAX 10
#define IBL_RESULT_CODE_STR_LEN 20
#define IBL_RESULT_CODE_LOC 17

extern cregister unsigned int DNUM;

/**
 *  @brief Determine if an address is local
 *
 *  @details
 *    Examines an input address to determine if it is a local address. Using the largest
 *    L2 size on the C6657.
 */
bool address_is_local (Uint32 addr)
{
    /* L2 */
    if ((addr >= 0x00800000) && (addr < 0x00900000))
        return (TRUE);

    /* L1P */
    if ((addr >= 0x00e00000) && (addr < 0x00e08000))
        return (TRUE);

    /* L2D */
    if ((addr >= 0x00f00000) && (addr < 0x00f08000))
        return (TRUE);

    return (FALSE);

}


/**
 * @brief  Convert a local l1d, l1p or l2 address to a global address
 *
 * @details
 *  The global address is formed. If the address is not local then
 *  the input address is returned
 */
Uint32 deviceLocalAddrToGlobal (Uint32 addr)
{

    if (address_is_local (addr))
        addr = (1 << 28) | (DNUM << 24) | addr;

    return (addr);

}
        
        
/**
 * @brief
 *   Enable the DDR
 *
 * @details
 *   The DDR controller on the c66x is an emif 4.0. The controller is
 *   initialized directly with the supplied values
 */
void deviceDdrConfig (void)
{
    uint32 loopcount=0;
    int8  ddr_result_code_str[IBL_RESULT_CODE_STR_LEN] = "IBL Result code 0";
    /* The emif registers must be made visible. MPAX mapping 2 is used */
    DEVICE_REG_XMPAX_L(2) =  0x10000000 | 0xff;     /* replacement addr + perm*/
    DEVICE_REG_XMPAX_H(2) =  0x2100000B;         /* base addr + seg size (64KB)*/	
    
    if (ibl.ddrConfig.configDdr != 0)
        hwEmif4p0Enable (&ibl.ddrConfig.uEmif.emif4p0);

}
        


/**
 *  @brief  Enable EMIF25 or SPI interface to the NAND
 *
 */
int32 deviceConfigureForNand(void)
{

    return (0);

}

/**
 *  @brief
 *      Return the base memory address for emif25 in a given chip select space
 */
uint32 deviceEmif25MemBase (int32 cs)
{
    switch (cs)  {

        case 2:  return (TARGET_MEM_NAND_CS_2);

        case 3:  return (TARGET_MEM_NAND_CS_3);

        case 4:  return (TARGET_MEM_NAND_CS_4);

        case 5:  return (TARGET_MEM_NAND_CS_5);

    }

    return (0xffffffff);

}


/**
 *  @brief
 *      Return the PSC number for NAND/NOR through emif. 
 */
Int32 deviceEmifPscNum (void)
{
    Uint32 v;

    v = *((Uint32 *)DEVICE_JTAG_ID_REG);
    v &= DEVICE_JTAG_ID_MASK;
    if (v == DEVICE_C6657_JTAG_ID_VAL)
        return (TARGET_PWR_EMIF_C6657);

    /* Return a negative number to indicate no PSC module is associated with NAND */
    return (-1);

}



/**
 *  @brief
 *    The e-fuse mac address is loaded
 */
void deviceLoadDefaultEthAddress (uint8 *maddr)
{
    uint32 macA, macB;

    /* Read the e-fuse mac address */
    macA = *((uint32 *)0x2620110);
    macB = *((uint32 *)0x2620114);

    maddr[0] = (macB >>  8) & 0xff;
    maddr[1] = (macB >>  0) & 0xff;
    maddr[2] = (macA >> 24) & 0xff;
    maddr[3] = (macA >> 16) & 0xff;
    maddr[4] = (macA >>  8) & 0xff;
    maddr[5] = (macA >>  0) & 0xff;
}

extern nandCtbl_t nandEmifCtbl;
/**
 *  @brief Return the NAND interface (GPIO, EMIF25 or SPI) used based on the value
 *         of interface
 */
#ifndef EXCLUDE_NAND_GPIO
nandCtbl_t nandGpioCtbl =  {

    nandHwGpioDriverInit,
    nandHwGpioDriverReadBytes,
    nandHwGpioDriverReadPage,
    nandHwGpioDriverClose

};
#endif

#ifndef EXCLUDE_NAND_EMIF
extern Int32 nandHwEmifDriverInit (int32 cs, void *vdevInfo);
extern Int32 nandHwEmifDriverReadBytes (Uint32 block, Uint32 page, Uint32 byte, Uint32 nbytes, Uint8 *data);
extern Int32 nandHwEmifDriverReadPage (Uint32 block, Uint32 page, Uint8 *data);
extern Int32 nandHwEmifDriverClose (void);

nandCtbl_t nandEmifCtbl =  {

    nandHwEmifDriverInit,
    nandHwEmifDriverReadBytes,
    nandHwEmifDriverReadPage,
    nandHwEmifDriverClose

};
#endif

#ifndef EXCLUDE_NAND_SPI
nandCtbl_t nandSpiCtbl =  {


    nandHwSpiDriverInit,
    nandHwSpiDriverReadBytes,
    nandHwSpiDriverReadPage,
    nandHwSpiDriverClose

};
#endif

nandCtbl_t *deviceGetNandCtbl (int32 interface)
{
#ifndef EXCLUDE_NAND_GPIO

    if (interface == ibl_PMEM_IF_GPIO)
        return (&nandGpioCtbl);

#endif

#ifndef EXCLUDE_NAND_SPI

    if (interface == ibl_PMEM_IF_SPI)
        return (&nandSpiCtbl);

#endif

#ifndef EXCLUDE_NAND_EMIF

    if ((interface >= ibl_PMEM_IF_CHIPSEL_2) && (interface <= ibl_PMEM_IF_CHIPSEL_5))
        return (&nandEmifCtbl);

#endif

    return (NULL);

}


/**
 * @brief
 *      Get the nor call table for the specified nor interface
 */

#ifndef EXCLUDE_NOR_EMIF
norCtbl_t norEmifCtbl = {
    
    norHwEmifDriverInit,
    norHwEmifDriverReadBytes,
    norHwEmifDriverClose

};

#endif

#ifndef EXCLUDE_NOR_SPI

norCtbl_t norSpiCtbl = {
    
    norHwSpiDriverInit,
    norHwSpiDriverReadBytes,
    norHwSpiDriverClose

};

#endif

norCtbl_t *deviceGetNorCtbl (int32 interface)
{

#ifndef EXCLUDE_NOR_SPI
    
    if (interface == ibl_PMEM_IF_SPI)
        return (&norSpiCtbl);

#endif

#ifndef EXCLUDE_NOR_EMIF
   
    if ((interface >= ibl_PMEM_IF_CHIPSEL_2) && (interface <= ibl_PMEM_IF_CHIPSEL_5))
        return (&norEmifCtbl);

#endif

    return (NULL);

}

/**
 *  @brief
 *      Chip level SGMII serdes configuration
 *
 *  @details
 *      Both lanes are always setup, regardless of the port value
 */
void targetSgmiiSerdesConfig (int32 port, void *viblSgmii)
{
  serdesConfig_t scfg;
  iblSgmii_t     *sgmii = (iblSgmii_t *)viblSgmii;

  scfg.cfg      = sgmii->auxConfig;
  scfg.nLanes   = 2;
  scfg.rxCfg[0] = scfg.rxCfg[1] = sgmii->rxConfig;
  scfg.txCfg[0] = scfg.txCfg[1] = sgmii->txConfig;

  hwSerdesConfig (TARGET_SGMII_SERDES_BASE, &scfg);

  hwSerdesWaitLock (TARGET_SGMII_SERDES_STATUS_BASE);

}
    


    





    









