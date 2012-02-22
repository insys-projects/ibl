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
 *    L2 size on the C6670.
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

#ifdef PLL_REINIT_WORKAROUND
    for (loopcount = 0; loopcount < PLL_DDR_INIT_LOOPMAX; loopcount++)
    {
    	/* Calling MAIN, PA, DDR PLL init  */
    	if (ibl.pllConfig[ibl_MAIN_PLL].doEnable == TRUE)
            hwPllSetPll (MAIN_PLL, 
                         ibl.pllConfig[ibl_MAIN_PLL].prediv,
                         ibl.pllConfig[ibl_MAIN_PLL].mult,
                         ibl.pllConfig[ibl_MAIN_PLL].postdiv);
    
        if (ibl.pllConfig[ibl_NET_PLL].doEnable == TRUE)
            hwPllSetCfgPll (DEVICE_PLL_BASE(NET_PLL),
                            ibl.pllConfig[ibl_NET_PLL].prediv,
                            ibl.pllConfig[ibl_NET_PLL].mult,
                            ibl.pllConfig[ibl_NET_PLL].postdiv,
                            ibl.pllConfig[ibl_MAIN_PLL].pllOutFreqMhz,
                            ibl.pllConfig[ibl_NET_PLL].pllOutFreqMhz);
    
        if (ibl.pllConfig[ibl_DDR_PLL].doEnable == TRUE)
            hwPllSetCfg2Pll (DEVICE_PLL_BASE(DDR_PLL),
                             ibl.pllConfig[ibl_DDR_PLL].prediv,
                             ibl.pllConfig[ibl_DDR_PLL].mult,
                             ibl.pllConfig[ibl_DDR_PLL].postdiv,
                             ibl.pllConfig[ibl_MAIN_PLL].pllOutFreqMhz,
                             ibl.pllConfig[ibl_DDR_PLL].pllOutFreqMhz);
	  
        if (ibl.ddrConfig.configDdr != 0)
            hwEmif4p0Enable (&ibl.ddrConfig.uEmif.emif4p0);
			
	    if (ddr3_memory_test() == 0) 
	    {
	        break;
	    }
    }
    /* Init UART again because we are re-initializing the PLL's */ 
    uart_init();

    if (loopcount < 10) 
    {
        ddr_result_code_str[IBL_RESULT_CODE_LOC] = loopcount + '0';
    }
    else if ((loopcount >= 10) && (loopcount < 35))
    {
        ddr_result_code_str[IBL_RESULT_CODE_LOC] =   loopcount + 'A';
    }
    else 
    {
        ddr_result_code_str[IBL_RESULT_CODE_LOC] =   loopcount + 'Z';
    }

    if (loopcount == PLL_DDR_INIT_LOOPMAX) 
    {
        uart_write_string("IBL: DDR INITIALIZATION FAILED",0);
    }
    else
    {
        uart_write_string("IBL: PLL and DDR Initialization Complete",0);
    }
    uart_write_string(ddr_result_code_str,0);
#endif
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
 *      Return the PSC number for NAND/NOR through emif. Only 6678 has the emif
 */
Int32 deviceEmifPscNum (void)
{
    Uint32 v;

    v = *((Uint32 *)DEVICE_JTAG_ID_REG);
    v &= DEVICE_JTAG_ID_MASK;
    if (v == DEVICE_C6678_JTAG_ID_VAL)
        return (TARGET_PWR_EMIF_C6678);

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


/**
 *  @brief
 *    Compile time queue manager information
 */
#define DEVICE_NUM_RX_CPPIS     1
#define DEVICE_NUM_TX_CPPIS     1
#define DEVICE_NUM_CPPIS        (DEVICE_NUM_RX_CPPIS + DEVICE_NUM_TX_CPPIS)

/* The linking RAM */
#pragma DATA_SECTION(qm_linkram_buf, ".linkram")
#pragma DATA_ALIGN(qm_linkram_buf, 16)
uint8 qm_linkram_buf[DEVICE_NUM_CPPIS * 2 * (sizeof(uint32)/sizeof(uint8))];


/* The CPPI RAM */
#pragma DATA_SECTION(qm_cppi_buf, ".cppi")
#pragma DATA_ALIGN(qm_cppi_buf, 16)
uint8 qm_cppi_buf[QM_DESC_SIZE_BYTES * DEVICE_NUM_CPPIS];


/* The rx data buffers */
#pragma DATA_SECTION(qm_buffer, ".mac_buffer")
#pragma DATA_ALIGN(qm_buffer, 16)
uint8 qm_buffer[MAX_SIZE_STREAM_BUFFER * DEVICE_NUM_RX_CPPIS];

const qmConfig_t qmConfig =  {
    (UINT32) qm_linkram_buf,
    sizeof  (qm_cppi_buf),
    (UINT32) qm_cppi_buf,

    DEVICE_NUM_CPPIS,
    DEVICE_QM_FREE_Q
};

/**
 *  @brief
 *      Return the queue manager memory configuration information
 */
void *targetGetQmConfig (void)
{
    return ((void *)&qmConfig);
}

/**
 *  @brief
 *      Attach a packet buffer to each descriptor and push onto the linked buffer queue
 */
void targetInitQs (void)
{
    int32 i;
    qmHostDesc_t *hd;

    for (i = 0; i < DEVICE_NUM_RX_CPPIS; i++)  {

        hd                = hwQmQueuePop (DEVICE_QM_FREE_Q);
        hd->buffLen       = sizeof (qm_buffer) / DEVICE_NUM_CPPIS;
        hd->buffPtr       = (UINT32) &(qm_buffer[MAX_SIZE_STREAM_BUFFER * i]);
        hd->nextBDPtr     = 0;
        hd->origBufferLen = MAX_SIZE_STREAM_BUFFER;
        hd->origBuffPtr   = hd->buffPtr;

        hwQmQueuePush (hd, DEVICE_QM_LNK_BUF_Q, QM_DESC_SIZE_BYTES);

    }


    for (i = 0; i < DEVICE_NUM_TX_CPPIS; i++)  {

        hd                = hwQmQueuePop (DEVICE_QM_FREE_Q);
        hd->buffLen       = 0;
        hd->buffPtr       = 0;
        hd->nextBDPtr     = 0;
        hd->origBufferLen = 0;
        hd->origBuffPtr   = 0;

        hwQmQueuePush (hd, DEVICE_QM_TX_Q, QM_DESC_SIZE_BYTES);

    }


}



const cpdmaRxCfg_t cpdmaEthRxCfg =  {

    DEVICE_PA_CDMA_RX_CHAN_CFG_BASE,    /* Base address of PA CPDMA rx config registers */
    DEVICE_PA_CDMA_RX_NUM_CHANNELS,     /* Number of rx channels */

    DEVICE_PA_CDMA_RX_FLOW_CFG_BASE,    /* Base address of PA CPDMA rx flow registers */
    DEVICE_PA_CDMA_RX_NUM_FLOWS,        /* Number of rx flows */

    0,                                  /* Queue manager for descriptor / buffer for received packets */
    DEVICE_QM_LNK_BUF_Q,                /* Queue of descriptors /buffers for received packets */

    0,                                  /* Queue manager for received packets */
    DEVICE_QM_RCV_Q,                    /* Queue for received packets (overridden by PA)  */

    DEVICE_RX_CDMA_TIMEOUT_COUNT        /* Teardown maximum loop wait */
};


/**
 *  @brief
 *      Return the cpdma configuration information
 */
void *targetGetCpdmaRxConfig (void)
{
    return ((void *)&cpdmaEthRxCfg);

}


const cpdmaTxCfg_t cpdmaEthTxCfg = {

    DEVICE_PA_CDMA_GLOBAL_CFG_BASE,     /* Base address of global config registers      */
    DEVICE_PA_CDMA_TX_CHAN_CFG_BASE,    /* Base address of PA CPDMA tx config registers */
    DEVICE_PA_CDMA_TX_NUM_CHANNELS      /* Number of tx channels */

};


/**
 *  @brief
 *      return the tx cpdma configuration information
 */
void *targetGetCpdmaTxConfig (void)
{
    return ((void *)&cpdmaEthTxCfg);

}

/**
 *  @brief
 *     Configure the PA
 */
void targetPaConfig (uint8 *macAddr)
{
    paConfig_t     paCfg;
    qmHostDesc_t  *hd;
    SINT16         ret;

    /* Filter everything except the desired mac address and the broadcast mac */
    paCfg.mac0ms = ((uint32)macAddr[0] << 24) | ((uint32)macAddr[1] << 16) | ((uint32)macAddr[2] << 8) | (uint32)(macAddr[3]);
    paCfg.mac0ls = ((uint32)macAddr[4] << 24) | ((uint32)macAddr[5] << 16);

    paCfg.mac1ms = 0xffffffff;
    paCfg.mac1ls = 0xffff0000;

    paCfg.rxQnum = DEVICE_QM_RCV_Q;

    /* Form the configuration command in a buffer linked to a descriptor */
    hd = hwQmQueuePop (DEVICE_QM_LNK_BUF_Q);
    paCfg.cmdBuf = (uint8 *)hd->origBuffPtr;

    ret = hwPaEnable (&paCfg);
    if (ret != 0)  {
        iblStatus.iblFail = ibl_FAIL_CODE_PA;
        return;
    }


    /* Send the command to the PA through the QM */
    hd->softwareInfo0 = PA_MAGIC_ID;
    hd->buffLen = 16;
    QM_DESC_DESCINFO_SET_PKT_LEN(hd->descInfo, 16);

    /* Set the return Queue */
    QM_DESC_PINFO_SET_QM    (hd->packetInfo, 0);
    QM_DESC_PINFO_SET_QUEUE (hd->packetInfo, DEVICE_QM_LNK_BUF_Q);

    hwQmQueuePush (hd, DEVICE_QM_PA_CFG_Q, QM_DESC_SIZE_BYTES);


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


Int32 targetMacSend (void *vptr_device, Uint8* buffer, int num_bytes) 
{
    qmHostDesc_t   *hd;
    NET_DRV_DEVICE *ptr_device = (NET_DRV_DEVICE *)vptr_device;
    int             i;


    /* Must always setup the descriptor to have the minimum packet length */
    if (num_bytes < 64)
        num_bytes = 64;


    for (i = 0, hd = NULL; hd == NULL; i++, chipDelay32 (1000)) 
        hd = hwQmQueuePop (DEVICE_QM_TX_Q);
    
    if (hd == NULL)
        return (-1);

    QM_DESC_DESCINFO_SET_PKT_LEN(hd->descInfo, num_bytes);

    hd->buffLen       = num_bytes;
    hd->origBufferLen = num_bytes;

    hd->buffPtr     = deviceLocalAddrToGlobal((UINT32)buffer);
    hd->origBuffPtr = deviceLocalAddrToGlobal((UINT32)buffer);

    
    /* Return the descriptor back to the transmit queue */
    QM_DESC_PINFO_SET_QM(hd->packetInfo, 0);
    QM_DESC_PINFO_SET_QUEUE(hd->packetInfo, DEVICE_QM_TX_Q);

    hwQmQueuePush (hd, DEVICE_QM_ETH_TX_Q, QM_DESC_SIZE_BYTES);

    return (0);

}


Int32 targetMacRcv (void *vptr_device, UINT8 *buffer)
{
    Int32           pktSizeBytes; 
    qmHostDesc_t   *hd;
    NET_DRV_DEVICE *ptr_device = (NET_DRV_DEVICE *)vptr_device;

    hd = hwQmQueuePop (DEVICE_QM_RCV_Q);
    if (hd == NULL)
        return (0);

    pktSizeBytes = QM_DESC_DESCINFO_GET_PKT_LEN(hd->descInfo);
    iblMemcpy ((void *)buffer, (void *)hd->buffPtr, pktSizeBytes);

    hd->buffLen = hd->origBufferLen;
    hd->buffPtr = hd->origBuffPtr;

    hwQmQueuePush (hd, DEVICE_QM_LNK_BUF_Q, QM_DESC_SIZE_BYTES);

    return (pktSizeBytes);

}

void targetFreeQs (void)
{
    qmHostDesc_t   *hd;

    do  {

        hd = hwQmQueuePop (DEVICE_QM_FREE_Q);

    } while (hd != NULL);

    do  {

        hd = hwQmQueuePop (DEVICE_QM_LNK_BUF_Q);

    } while (hd != NULL);
    
    do  {

        hd = hwQmQueuePop (DEVICE_QM_RCV_Q);

    } while (hd != NULL);
    
    do  {

        hd = hwQmQueuePop (DEVICE_QM_TX_Q);

    } while (hd != NULL);
    
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
    


    





    









