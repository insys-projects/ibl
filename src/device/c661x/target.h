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



/**************************************************************************
 * FILE PURPOSE: Target specific definitions
 **************************************************************************
 * FILE NAME: target.h
 *
 * DESCRIPTION: This file defines target specific values used by low level
 *				drivers.
 *
 * @file target.h
 *
 * @brief
 *  Low level target specific values are defined
 *
 ***************************************************************************/
#ifndef _TARGET_H
#define _TARGET_H
#include "types.h"
 
 
/** 
 *  @brief
 *   Device EMAC definitions
 */
 

/**
 *  @brief
 *    Device Timer definitions
 */
#define TIMER0_BASE             0x02200000u

#define TIMER_INPUT_DIVIDER     6           /* Timer driven from cpu clock / 6 */


/**
 *  @def MAIN_PLL
 */
#define MAIN_PLL        0   /**< The index to the main PLL */

/**
 * @def NET_PLL
 */
#define NET_PLL         1   /**< The index to the network PLL */

/**
 *  @def DDR_PLL
 */
#define DDR_PLL         2   /**< The index to the DDR PLL */


/**
 *  @brief
 *    Device PLL definitions
 */
#define DEVICE_PLL_BASE(x)      ((x) == MAIN_PLL ? 0x2310000 : ((x) == NET_PLL ? 0x2620338 : 0x2620330))
#define DEVICE_MAIN_PLL_CTL_0       0x2620328
#define DEVICE_MAIN_PLL_CTL_1       0x262032c


/**
 *  @brief
 *   The c661x devices use a register external to the PLL controller for prediv configuration
 */
#define chipPllExternalPrediv(x)    TRUE

/**
 * @brief 
 *  Device PSC definitions
 */
#define DEVICE_PSC_BASE     0x02350000u


/**
 * @brief
 *  The PSC number for the PA sub-system */
#define TARGET_PWR_PA       7

/**
 * @brief
 *  The PSC number for the SGMII */
#define TARGET_PWR_ETH(x)   8

/** 
 * @brief
 *   The PSC numbers for EMIF16 and SPI vary between devices. The devices are run time
 *   identified by reading the JTAG ID register 
 */
#define DEVICE_C6616_JTAG_ID_VAL    0x9d02f
#define DEVICE_C6618_JTAG_ID_VAL    0x9e02f
#define DEVICE_JTAG_ID_REG          0x2620018


/**
 *  @brief
 *    The PSC number for NAND depends on the device
 */
#define TARGET_PWR_NAND         deviceNandPscNum()
#define TARGET_PWR_NAND_C6618   3

/*
 *  @brief
 *     The PSC number for SPI depends on the device
 */
#define TARGET_PWR_SPI      deviceSpiPscNum()

/**
 * @brief
 *  Flag to indicate timer 0 power up requested. The time is always on in the 6472
 */
#define TARGET_PWR_TIMER_0  -1

/**
 *  @brief
 *    Device DDR controller definitions
 */
#define DEVICE_EMIF4_BASE  0x21000000
#define targetEmifType()   ibl_EMIF_TYPE_40

/**
 *  @brief
 *     Device EMIF 2.5 controller definitions
 */
#define DEVICE_EMIF25_BASE  0x20c00000

/** 
 *  @brief
 *      NAND memory regions
 */
#define TARGET_MEM_NAND_CS_2    0x70000000 
#define TARGET_MEM_NAND_CS_3    0x74000000 
#define TARGET_MEM_NAND_CS_4    0x78000000 
#define TARGET_MEM_NAND_CS_5    0x7c000000 
uint32 deviceNandMemBase (int32 cs);
#define TARGET_SHFL(x)  _shfl(x)  /* The shuffle intrinsic */


/**
 * @brief
 *  The highest module number. The value for nyquist is used
 */
#define TARGET_PWR_MAX_MOD  30
 

/**
 * @brief
 *   The base address of MDIO 
 */
#define TARGET_MDIO_BASE    0x2090300

/**
 * @brief
 *   The number of external ethernet ports
 */
#define TARGET_EMAC_N_PORTS     2



/**
 *  @brief
 *      The base address of the I2C peripheral, and the module divisor of the cpu clock
 */
#define DEVICE_I2C_BASE                 0x02530000
#define DEVICE_I2C_MODULE_DIVISOR       6
 
/**
 * @brief
 *     Prototypes for the PLL functions handled outside the main PLL registers
 */
SINT16 chipPllSetExternalPrediv(UINT16 pllNum, UINT32 predivRegVal);
SINT16 chipPllExternalBwAdj (UINT16 pllNum, UINT16 mult);
UINT32 chipPllExternalMult (UINT16 pllNum, UINT16 mult);


/**
 *  @brief
 *      Hardware network subsystem support, ethernet switch
 */
#define DEVICE_CPSW
#define DEVICE_CPSW_NUM_PORTS       3                    /* 3 switch ports */
#define DEVICE_CPSW_BASE            (0x02090800)
#define targetGetSwitchCtl()        CPSW_CTL_P0_ENABLE   /* Enable port 0 */
#define targetGetSwitchMaxPktSize() 9000

#define DEVICE_QM
#define DEVICE_QM_MANAGER_BASE          0x02a68000
#define DEVICE_QM_DESC_SETUP_BASE       0x02a6a000
#define DEVICE_QM_MANAGER_QUEUES_BASE   0x02a20000
#define DEVICE_QM_MANAGER_Q_PROXY_BASE  0x02a40000
#define DEVICE_QM_QUEUE_STATUS_BASE		0x02a00000
#define DEVICE_QM_NUM_LINKRAMS          2
#define DEVICE_QM_NUM_MEMREGIONS        20
void    *targetGetQmConfig(void);
void     targetInitQs (void);

#define chipLmbd(x,y) _lmbd(x,y)




#define DEVICE_CPDMA

#define DEVICE_PA_CDMA_GLOBAL_CFG_BASE   0x02004000
#define DEVICE_PA_CDMA_TX_CHAN_CFG_BASE  0x02004400
#define DEVICE_PA_CDMA_RX_CHAN_CFG_BASE  0x02004800
#define DEVICE_PA_CDMA_RX_FLOW_CFG_BASE  0x02005000

#define DEVICE_PA_CDMA_RX_NUM_CHANNELS   24
#define DEVICE_PA_CDMA_RX_NUM_FLOWS      32
#define DEVICE_PA_CDMA_TX_NUM_CHANNELS   9


#define DEVICE_QM_FREE_Q                910
#define DEVICE_QM_LNK_BUF_Q             911
#define DEVICE_QM_RCV_Q                 912
#define DEVICE_QM_TX_Q                  913
#define DEVICE_QM_PA_CFG_Q              640
#define DEVICE_QM_ETH_TX_Q              648

#define DEVICE_RX_CDMA_TIMEOUT_COUNT    1000



#define DEVICE_PA
#define DEVICE_PA_BASE                  0x02000000
#define DEVICE_PA_NUM_PDSPS             6
#define DEVICE_PA_RUN_CHECK_COUNT       100         /* Number of loops to verify PA firmware is running */
#define DEVICE_PA_PLL_BASE              0x02620338
#define chipLower8(x)                   ((x) & 0x00ff)


#define TARGET_SGMII_EXTERNAL_SERDES
#define TARGET_SGMII_TYPE_2             /* Use second sgmii setup sequence */
#define TARGET_SGMII_BASE_ADDRESSES    { 0x02090100, 0x02090200 }
#define TARGET_SGMII_SERDES_BASE        0x2620340
#define TARGET_SGMII_SERDES_STATUS_BASE 0x2620158
#define TARGET_SGMII_SOFT_RESET         0x04
#define TARGET_SGMII_CONTROL            0x10
#define TARGET_SGMII_MR_ADV_ABILITY     0x18
void targetSgmiiSerdesConfig (int32 port, void *cfg);
#define chipKickOpenSerdes(x)           *((uint32 *)0x2620038) = 0x83e70b13; *((uint32 *)0x262003c) = 0x95a4f1e0
#define chipKickClosedSerdes(x)         ;       /* never lock the registers */
#define TARGET_SERDES_LOCK_DELAY        (1600*1000)

#define DEVICE_EMACSL_BASE(x)           (0x02090900 + (x)*0x040)
#define DEVICE_N_GMACSL_PORTS           2
#define DEVICE_EMACSL_RESET_POLL_COUNT  100
Int32 targetMacSend (void *ptr_device, Uint8* buffer, int num_bytes);
Int32 targetMacRcv (void *ptr_device, UINT8 *buffer);

#define DEVICE_SS
#define DEVICE_PSTREAM_CFG_REG_ADDR                 0x2000604
#define DEVICE_PSTREAM_CFG_REG_VAL_ROUTE_PDSP0      0
#define hwConfigStreamingSwitch()                   DEVICE_REG32_W(DEVICE_PSTREAM_CFG_REG_ADDR, DEVICE_PSTREAM_CFG_REG_VAL_ROUTE_PDSP0);


/**
 *  @brief
 *      Register access macros
 */
#define DEVICE_REG32_W(x,y)   *(volatile unsigned int *)(x)=(y)
#define DEVICE_REG32_R(x)    (*(volatile unsigned int *)(x))

#define BOOTBITMASK(x,y)      (   (   (  ((UINT32)1 << (((UINT32)x)-((UINT32)y)+(UINT32)1) ) - (UINT32)1 )   )   <<  ((UINT32)y)   )
#define BOOT_READ_BITFIELD(z,x,y)   (((UINT32)z) & BOOTBITMASK(x,y)) >> (y)
#define BOOT_SET_BITFIELD(z,f,x,y)  (((UINT32)z) & ~BOOTBITMASK(x,y)) | ( (((UINT32)f) << (y)) & BOOTBITMASK(x,y) )

/** 
 *  @brief
 *      Mpax configuration registers 
 */
#define DEVICE_REG_XMPAX_L(x) *((volatile unsigned int *)(0x08000000 + (8*(x))))
#define DEVICE_REG_XMPAX_H(x) *((volatile unsigned int *)(0x08000004 + (8*(x))))

#endif /* _TARGET_H */
