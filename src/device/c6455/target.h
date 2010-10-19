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
 
#define TARGET_EMAC_N_PORTS            1

#define TARGET_EMAC_BASE_ADDRESSES     { 0x02c80000u }
#define TARGET_EMAC_DSC_BASE_ADDR      { 0x02c82000u }
 
 
/* No chip level reset required for ethernet, the function call is made a void statment */
#define deviceSetEthResetState(x,y)

/* There is no sgmii on the 6455, so the sgmii config is defined to a void statement */
#define hwSgmiiConfig(x,y)

/* Mdio is handled outside the emac driver */
#define dev_mdio_open()     1

#define TARGET_MAC_CONTROL    (1 << 5)    /* gmii enable */  \
                            | (1 << 0)    /* full duplex */
 
/**
 *  @brief
 *    Device Timer definitions
 */
#define TIMER0_BASE             0x02940000u
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
#define DEVICE_PLL_BASE(x)  ((x) == MAIN_PLL ? 0x29a0000 : ((x) == NET_PLL ? 0x29c0000 : 0))


/**
 * @brief
 *  Flag to indicate ethernet power up requested 
 */
#define TARGET_PWR_ETH(x)   1

/**
 * @brief
 *   Flag to indicate DDR power up requested
 */
#define TARGET_PWR_DDR      2 

/**
 * @brief
 *  Flag to indicate timer 0 power up requested
 */
#define TARGET_PWR_TIMER_0  3


/**
 *  @brief
 *    The nand is done through gpio, which is always powered up.
 *    A value < 0 tells the low level psc driver to simply return success
 */
#define TARGET_PWR_NAND     -1


/**
 *  @brief
 *    Device DDR controller definitions
 */
#define DEVICE_DDR_BASE  0x78000000

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

/**
 *  @brief
 *    GPIO pin mapping 
 */
#define NAND_CLE_GPIO_PIN 	GPIO_8     // High: Command Cycle occuring
#define NAND_ALE_GPIO_PIN 	GPIO_9     // High: Address input cycle oddcuring
#define NAND_NWE_GPIO_PIN 	GPIO_10
#define NAND_NRE_GPIO_PIN 	GPIO_12
#define NAND_NCE_GPIO_PIN 	GPIO_13
#define NAND_MODE_GPIO		GPIO_14

/**
 *  @brief
 *      The standard NAND delay must be big enough to handle the highest possible
 *      operating frequency of the device */
#define TARGET_NAND_STD_DELAY				25 // In cpu cycles

/**
 *  @brief
 *      The base address of the I2C peripheral, and the module divisor of the cpu clock
 */
#define DEVICE_I2C_BASE                 0x02b04000
#define DEVICE_I2C_MODULE_DIVISOR       6




