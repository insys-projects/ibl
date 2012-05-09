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

#include "types.h"

 
/** 
 *  @brief
 *   Device EMAC definitions
 */
#define TARGET_DEVICE_CPMAC

#define TARGET_EMAC_N_PORTS            1

#define TARGET_EMAC_BASE_ADDRESSES     { 0x02c08000u }
#define TARGET_EMAC_DSC_BASE_ADDR      { 0x02c0a000u }

#define TARGET_SGMII_BASE_ADDRESSES    { 0x02c08900u }

/* SGMII offsets (at least the serdes configs, vary between devices, so
 * they are defined here. */
#define TARGET_SGMII_IDVER             0x000
#define TARGET_SGMII_STATUS            0x014
#define TARGET_SGMII_MR_LP_ADV_ABILITY 0x020
#define TARGET_SGMII_TX_CFG            0x030
#define TARGET_SGMII_RX_CFG            0x034
#define TARGET_SGMII_AUX_CFG           0x038

/* Leave mdio disabled */
#define dev_mdio_open()     1

/* No chip level reset required for ethernet, the function call is made a void statment */
#define deviceSetEthResetState(x,y)

/* The mac control register values */
#define TARGET_MAC_CONTROL    ( 1 << 18)            /* EXT_EN              */     \
                            | ( 0 << 9 )            /* Round robin         */     \
                            | ( 1 << 7 )            /* GIG                 */     \
                            | ( 0 << 6 )            /* TX pacing disabled  */     \
                            | ( 1 << 5 )            /* GMII RX & TX        */     \
                            | ( 0 << 4 )            /* TX flow disabled    */     \
                            | ( 0 << 3 )            /* RX flow disabled    */     \
                            | ( 0 << 1 )            /* Loopback enabled    */     \
                            | ( 1 << 0 )            /* full duplex         */
 

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

/**
 *  @def DDR_PLL
 */
#define DDR_PLL         1   /**< The index to the DDR PLL */


/**
 *  @brief
 *    Device PLL definitions
 */
#define DEVICE_PLL_BASE(x)      ((x) == MAIN_PLL ? 0x2310000 : 0x2620330)
#define DEVICE_MAIN_PLL_CTL_0       0x2620328
#define DEVICE_MAIN_PLL_CTL_1       0x262032c


/**
 *  @brief
 *   The c66x devices use a register external to the PLL controller for prediv configuration
 */
#define chipPllExternalPrediv(x)    TRUE

/**
 * @brief 
 *  Device PSC definitions
 */
#define DEVICE_PSC_BASE     0x02350000u

/**
 *  @brief
 *    The SPI module base and module divider
 */
#define DEVICE_SPI_BASE(x)          0x20bf0000u
#define DEVICE_SPI_MOD_DIVIDER      6
#define DEVICE_SPI_MAX_DIVIDER      0xff     


/**
 * @brief
 *  The PSC number for the EMAC */
#define TARGET_PWR_ETH(x)   3

/** 
 * @brief
 *   The PSC numbers for EMIF16 and SPI vary between devices. The devices are run time
 *   identified by reading the JTAG ID register,
 *   NOTE: Have the variant bits as zero while defining the JTAG ID 
 */
#define DEVICE_C6657_JTAG_ID_VAL   0xb97a02f	/* C6657 */	
#define DEVICE_C6678_JTAG_ID_VAL   0x9e02f      /* C6678 */
#define DEVICE_C6670_JTAG_ID_VAL   0xb94102f    /* C6670 */

#define DEVICE_JTAG_ID_REG          0x2620018
#define DEVICE_JTAG_ID_MASK         0x0fffffff /* exclude variant bits(31:28) */


/**
 *  @brief
 *    The PSC number for NAND depends on the device
 */
#define TARGET_PWR_EMIF         deviceEmifPscNum()
#define TARGET_PWR_EMIF_C6657   -1
#define NAND_TYPE_LARGE
/*
 *  @brief
 *     The PSC number for SPI depends on the device
 */
#define TARGET_PWR_SPI      -1

/**
 *  @brief
 *      The PSC number for GPIO. GPIO is in the always on domain
 */
#define TARGET_PWR_GPIO     -1

/**
 *  @brief
 *      The LPSC number for PCIe. PCIe is 10 for C6657
 */
#define TARGET_PWR_PCIE    10 

/**
 * @brief
 *  Flag to indicate timer 0 power up requested. The time is always on in the 6472
 */
#define TARGET_PWR_TIMER_0  23

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
#define TARGET_MDIO_BASE    0x2C080800

/**
 * @brief
 *   The number of external ethernet ports
 */
#define TARGET_EMAC_N_PORTS     1

/**
 *  @brief
 *    GPIO address
 */
#define GPIO_GPIOPID_REG	0x02320000
#define GPIO_GPIOEMU_REG	0x02320004
#define GPIO_BINTEN_REG		0x02320008
#define GPIO_DIR_REG		0x02320010
#define GPIO_OUT_DATA_REG	0x02320014
#define GPIO_SET_DATA_REG	0x02320018
#define GPIO_CLEAR_DATA_REG	0x0232001C
#define GPIO_IN_DATA_REG	0x02320020
#define GPIO_SET_RIS_TRIG_REG	0x02320024
#define GPIO_CLR_RIS_TRIG_REG	0x02320028
#define GPIO_SET_FAL_TRIG_REG	0x0232002C
#define GPIO_CLR_FAL_TRIG_REG	0x02320030
#define PIN_CONTROL_0		0x02620580
#define PIN_CONTROL_1		0x02620584
/**
 *  @brief
 *      The base address of the I2C peripheral, and the module divisor of the cpu clock
 */
#define DEVICE_I2C_BASE                 0x02530000
#define DEVICE_I2C_MODULE_DIVISOR       6
 
/**
 * @brief
 *      The address of the DEVSTAT register
 */
#define DEVICE_REG_DEVSTAT              0x02620020

/**
 * @brief
 *     Prototypes for the PLL functions handled outside the main PLL registers
 */
SINT16 chipPllSetExternalPrediv(UINT16 pllNum, UINT32 predivRegVal);
SINT16 chipPllExternalBwAdj (UINT16 pllNum, UINT16 mult);
UINT32 chipPllExternalMult (UINT16 pllNum, UINT16 mult);
void configureSPIpins(void);

#define TARGET_SGMII_EXTERNAL_SERDES
#define TARGET_SGMII_TYPE_2             /* Use second sgmii setup sequence */
#define TARGET_SGMII_SERDES_BASE        0x2620340
#define TARGET_SGMII_SERDES_STATUS_BASE 0x2620158
#define TARGET_SGMII_SOFT_RESET         0x04
#define TARGET_SGMII_CONTROL            0x10
#define TARGET_SGMII_MR_ADV_ABILITY     0x18
void targetSgmiiSerdesConfig (int32 port, void *cfg);
#define chipKickOpenSerdes(x)           *((uint32 *)0x2620038) = 0x83e70b13; *((uint32 *)0x262003c) = 0x95a4f1e0
#define chipKickClosedSerdes(x)         ;       /* never lock the registers */


#define TARGET_SERDES_LOCK_DELAY        (1600*1000)

#define ECC_BLOCK_SIZE			256

/* NAND address pack macro */
#define PACK_ADDR(col, page, block) \
        ((col & 0x000000ff) | ((page & 0x0000001f) << 9) | ((block & 0x00000fff) << 14))

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


/**
 *  @brief
 *      ROM boot loader boot modes and table locations
 */
#define BOOT_MODE_I2C               40
#define BOOT_MODE_SPI               50

#define ROM_BOOT_PARAMS_ADDR_C6678   0x873680
#define ROM_BOOT_PARAMS_ADDR_C6657   0x8fff00
#define ROM_BOOT_PARAMS_ADDR_C6670   0x8f3680

/**
 *  @brief
 *      No device specific configuration required for NOR boot, so
 *      the function call is defined to return success.
 */
#define deviceConfigureForNor()    0


/****************************************************************
 *
 * NOTE: Following build flags enable DEVICE specific workarounds
 * and have code which is specific to C6657 LC EVMs
 *
 ****************************************************************/

/**
 *  @brief
 *      Support for PLL workaround to re-enter ROM boot loader.
 */
#define IBL_ENTER_ROM              1

/**
 *  @brief
 *      Support for enabling EDC for internal memory.
 */
#define IBL_ENABLE_EDC             1 

/**
 *  @brief
 *      Support for enabling PCIe workarond for C6678/C6670.
 */
#define IBL_ENABLE_PCIE_WORKAROUND 1 

/**
 *  @brief
 *     DDR start and end address needed for DDR memory test 
 */
#define DDR3_TEST_START_ADDRESS 0x80000000
#define DDR3_TEST_END_ADDRESS   (DDR3_TEST_START_ADDRESS + (128 *1024))

UINT32 ddr3_memory_test();
void configureGPIO(void);


