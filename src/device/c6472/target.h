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
 
 
/** 
 *  @brief
 *   Device EMAC definitions
 */
#define TARGET_DEVICE_CPMAC
 
#define TARGET_EMAC_N_PORTS            2

#define TARGET_EMAC_BASE_ADDRESSES     { 0x02c80000u, 0x02cc0000u }
#define TARGET_EMAC_DSC_BASE_ADDR      { 0x02c82000u, 0x02cc2000u }

/* Leave mdio disabled */
#define dev_mdio_open()     1

/* No chip level reset required for ethernet, the function call is made a void statment */
#define deviceSetEthResetState(x,y)

/* The mac control register values used */
#define TARGET_MAC_CONTROL     CPMAC_MACCONTROL_RGMIIEN |  \
                               CPMAC_MACCONTROL_GIG |      \
                               CPMAC_MACCONTROL_GMIIEN |   \
                               CPMAC_MACCONTROL_FULLDUPLEX

/* There is no sgmii on the 6472, so the sgmii config is defined to a void statement */
#define hwSgmiiConfig(x,y)



/**
 *  @brief
 *    Device Timer definitions
 */
#define TIMER0_BASE             0x025e0000u

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
#define DEVICE_PLL_BASE(x)  ((x) == MAIN_PLL ? 0x29a0000 : ((x) == NET_PLL ? 0x29c0000 : 0x29c0400))


/**
 * @brief 
 *  Device PSC definitions
 */
#define DEVICE_PSC_BASE     0x02ae0000u

/**
 * @brief
 *  The PSC number for ethernet port 0 is 7, and for port 1 it is 8 */
#define TARGET_PWR_ETH(x)   ((x) == 0  ?  7  :  8)

/**
 *  @brief
 *    The nand is done through gpio, which is always powered up.
 *    A value < 0 tells the low level psc driver to simply return success
 */
#define TARGET_PWR_NAND     -1

/**
 *  @brief
 *      The PSC number for GPIO. GPIO is in the always on domain
 */
#define TARGET_PWR_GPIO     -1


/**
 * @brief
 *  Flag to indicate timer 0 power up requested. The time is always on in the 6472
 */
#define TARGET_PWR_TIMER_0  -1

/**
 *  @brief
 *    Device DDR controller definitions
 */
#define DEVICE_DDR_BASE  0x78000000
#define targetEmifType() ibl_EMIF_TYPE_31

/**
 * @brief
 *  The highest module number
 */
#define TARGET_PWR_MAX_MOD  13
 

/**
 * @brief
 *   The base address of MDIO 
 */
#define TARGET_MDIO_BASE    0x2c81800

/**
 *  @brief
 *    GPIO address
 */
#define GPIO_GPIOPID_REG 		0x02B00000
#define GPIO_GPIOEMU_REG		0x02B00004
#define GPIO_BINTEN_REG			0x02B00008
#define GPIO_DIR_REG			0x02B00010
#define GPIO_OUT_DATA_REG		0x02B00014
#define GPIO_SET_DATA_REG		0x02B00018
#define GPIO_CLEAR_DATA_REG		0x02B0001C
#define GPIO_IN_DATA_REG		0x02B00020
#define GPIO_SET_RIS_TRIG_REG	0x02B00024
#define GPIO_CLR_RIS_TRIG_REG	0x02B00028
#define GPIO_SET_FAL_TRIG_REG	0x02B0002C
#define GPIO_CLR_FAL_TRIG_REG	0x02B00030

#define ECC_BLOCK_SIZE			256

/* NAND address pack macro */
#define	PACK_ADDR(col, page, block) \
		((col &	0x00000fff) | ((page & 0x0000003f)<<16)	| ((block & 0x000003ff)	<< 22 ))

/**
 *  @brief
 *      The base address of the I2C peripheral, and the module divisor of the cpu clock
 */
#define DEVICE_I2C_BASE                 0x02b04000
#define DEVICE_I2C_MODULE_DIVISOR       6
 
 
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
 *      The c6472 supports only booting the ibl from i2c
 */
#define deviceReadBootDevice()  BOOT_DEVICE_I2C

#define IBL_ENTER_ROM           0
#define iblEnterRom()     

#define IBL_ENABLE_EDC          0     
#define iblEnableEDC()
  
