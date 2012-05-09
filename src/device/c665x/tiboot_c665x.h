#ifndef __TIBOOT_H__
#define __TIBOOT_H__
/*
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/ 
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

/******************************************************************************
 * FILE PURPOSE: Define Structures, MACROs and etc for TI Shared ROM Boot
 ******************************************************************************
 * FILE NAME:   tiboot.h
 *
 * DESCRIPTION: Define structures, macros and etc for the TI Shared ROM boot
 *              process.
 *
 ******************************************************************************/
#include "types.h"

/*******************************************************************************
 * Utility Macro definitions
 ******************************************************************************/
#define HEX_DIGIT(digit)        ((digit) + '0')
#define BOOT_BIT_TO_MASK(bit)   (1 << (bit))

/*******************************************************************************
 * Data Definition: Error Handling relatBOOT_ENTRY_POINT_ADDRed definition:
 *******************************************************************************
 * Description: Define Handling related macros, constants
 *
 ******************************************************************************/
/* Define Module IDs */
#define BOOT_MODULE_ID_MAIN         0
#define BOOT_MODULE_ID_BTBL         1
#define BOOT_MODULE_ID_BETH         2
#define BOOT_MODULE_ID_I2C          3
#define BOOT_MODULE_ID_CHIP         4
#define BOOT_MODULE_ID_HW           5

/* Boot error codes */
enum {
  BOOT_NOERR                = 0,
  BOOT_GEN_ERROR            = 1,    /* General error */
  BOOT_INVALID_BOOT_MODE    = 2,    
  BOOT_INVALID_I2C_DEV_ADDR = 3,
  BOOT_INVALID_CHECKSUM     = 4,    /* Invalid checksum of the boot parameters */
  BOOT_INVALID_PARAMS_SIZE  = 5,    /* the size of boot parameters is too big */
  BOOT_RX_ETH_QUEUE_FULL    = 6,    /* ethmain.c, hw_rxPacket */
  BOOT_CACHE_INIT_FAIL      = 7,    /* rmain.c, cache init failed */
  BOOT_CACHE_DISABLE_FAIL   = 8,    /* rmain.c, cache disable failed */
  BOOT_INVALID_CPPI_SIZE    = 9,    /* ethmain.c, invalid compile sizes */
  BOOT_INVALID_CORE_ID      = 10,   /* Invalid core ID in cold boot */
  BOOT_INVALID_MAC_ADDR     = 11,   /* Invalid MAC address (all 0's) */
  BOOT_ETH_TX_SCRATCH       = 12,   /* tx scratch size invalid */
  BOOT_ETH_TX_PACKET        = 13,   /* tx packet formation failure */
  BOOT_ETH_MAC_INIT         = 14,   /* ethmain.c - mac init failed */
  BOOT_PERIPH_POWER         = 15,   /* peripheral failed to powerup */
  BOOT_MAIN_FAIL            = 16,   /* Failed in initial boot setup (wrong core) */
  BOOT_SK_REGISTERSCWP      = 17,   /* Failed at SK_registerSCWP */
  BOOT_SK_ALLOCSC           = 18,   /* Failed at SK_allocSC */       
  BOOT_CPSGMII_CONFIGINDEX  = 19,   /* Failed at wrong CPSGMII config index */
  BOOT_SRIO_CONFIGINDEX     = 20,   /* Failed at wrong SRIO config index */
  BOOT_RETURN_FROM_CHAIN    = 21,   /* Code returned from boot main chaining, should never happen */
  BOOT_INVALID_HIB_ADDR     = 22    /* Invalid hibernation return address */
};

/* Error tracking prototypes (functions in rmain.c)*/
void bootException (UINT16 errorCode);
void bootError (UINT16 errorCode);

/* Error code = (module ID *  100) + module specific error */
#define BOOT_ERROR_CODE(id, code)     ((UINT16)((id<<8) + code))
#define BOOT_EXCEPTION(error_code)     bootException(error_code)
#define BOOT_ERROR(error_code)         bootError(error_code) 

/*******************************************************************************
 * Begin Boot Parameter definitions
 ******************************************************************************/

/*******************************************************************************
 * Boot Parameter Common 
 ******************************************************************************/
typedef struct boot_params_common_s{
   UINT16 length;       /* size of the entire boot parameters in bytes */
   UINT16 checksum;     /* non-zero: 1's complement checksum of the boot 
                         *           parameters
                         * zero: checksum is not applicable
                         */
   UINT16 boot_mode;
   UINT16 portNum;
   
#define BOOT_PARAMS_ALL_PORTS   0xffff  /* All ports on a device are configured */   
   
   UINT16 swPllCfg_msw;  /* CPU PLL configuration, MSW */
   UINT16 swPllCfg_lsw;  /* CPU PLL configuration, LSW */
   
   /*                                swPllCfg
    *
    *   /----------------------------------------------------------------\
    *   |   31        30   | 29        16 | 15          8 | 7          0 |
    *   |      PLL Ctl     |  multiplier  |  pre-divider  | post divider |
    *   \----------------------------------------------------------------/
    */
    
#define BOOT_PARAMS_PLL_CFG_CTL_MASK                0xc000
#define BOOT_PARAMS_PLL_CFG_CTL_SHIFT               14

#define BOOT_PARAMS_PLL_CFG_CTL_NO_INIT             0       /* Pll is not initialized */
#define BOOT_PARAMS_PLL_CFG_CTL_INIT_CONDITIONAL    1       /* Initialize only if PLL is currently disabled or in bypass */
#define BOOT_PARAMS_PLL_CFG_CTL_INIT                2       /* Initialize PLL regardless of current PLL state */
#define BOOT_PARAMS_PLL_CFG_CTL_BYPASS              3       /* Put the PLL into bypass  */


#define BOOT_PARAMS_PLL_CFG_MSW_MULT_MASK     0x3fff
#define BOOT_PARAMS_PLL_CFG_MSW_MULT_SHIFT    0

#define BOOT_PARAMS_PLL_CFG_LSW_PREDIV_MASK   0xff00
#define BOOT_PARAMS_PLL_CFG_LSW_PREDIV_SHIFT  8

#define BOOT_PARAMS_PLL_CFG_LSW_POSTDIV_MASK  0x00ff
#define BOOT_PARAMS_PLL_CFG_LSW_POSTDIV_SHIFT 0
   
} BOOT_PARAMS_COMMON_T;

typedef struct boot_params_ethernet_s{

    /* common portion of the Boot parameters */
    UINT16 length;
    UINT16 checksum;
    UINT16 boot_mode;
    UINT16 portNum;
    UINT16 swPllCfg_msw;  /* CPU PLL configuration, MSW */
    UINT16 swPllCfg_lsw;  /* CPU PLL configuration, LSW */
     
    /* Etherent specific portion of the Boot Parameters */
    UINT16 options;
    /*
     * Ethernet Specific Options
     *
     * Bits 2:0 interface
     *      000 - MII
     *      001 - RMII
     *      010 - GMII
     *      011 - RGMII
     *      100 - SMII
     *      101 - S3MII
     *      110 - RMII 10Mbs
     *      111 - RMII 100Mbs
     *
     * Bit 3: HD:                                      
     *        0 - Full Duplex                          
     *        1 - Half Duplex                          
     * Bit 4: SKIP TX                                           
     *        0 - Send the Ethernet Ready Frame 
     *        1 - Skip sending the Ethernet Ready Frame                                              
     * Bits 6:5 - Ethernet Initialization
     *      00 - Entire system configured
     *      01 - No initialization of peripherals that are already enabled and running
     *      10 - Reserved
     *      11 - No initialization at all
     *
     * Other bits:  Reserved 
     */ 
     #define BOOT_PARAMS_ETH_OPTIONS_MII        0x0000
     #define BOOT_PARAMS_ETH_OPTIONS_RMII       0x0001
     #define BOOT_PARAMS_ETH_OPTIONS_GMII       0x0002
     #define BOOT_PARAMS_ETH_OPTIONS_RGMII      0x0003
     #define BOOT_PARAMS_ETH_OPTIONS_SMII       0x0004
     #define BOOT_PARAMS_ETH_OPTIONS_S3MII      0x0005
     #define BOOT_PARAMS_ETH_OPTIONS_RMII_10    0x0006
     #define BOOT_PARAMS_ETH_OPTIONS_RMII_100   0x0007
	
	/* Faraday only supports SGMII */
     #define BOOT_PARAMS_ETH_OPTIONS_SGMII      0x0006
     
     #define BOOT_PARAMS_ETH_OPTIONS_HD         0x0008
     #define BOOT_PARAMS_ETH_OPTIONS_SKIP_TX    0x0010
    
     #define BOOT_PARAMS_ETH_OPTIONS_INIT_MASK  0x0060
     #define BOOT_PARAMS_ETH_OPTIONS_INIT_SHIFT 5
     #define BOOT_PARAMS_ETH_OPTIONS_INIT(x)    (((x) & BOOT_PARAMS_ETH_OPTIONS_INIT_MASK) >> BOOT_PARAMS_ETH_OPTIONS_INIT_SHIFT)
     
     #define BOOT_PARAMS_ETH_OPTIONS_INIT_FULL      0
     #define BOOT_PARAMS_ETH_OPTIONS_INIT_PARTIAL   1
     #define BOOT_PARAMS_ETH_OPTIONS_INIT_NONE      3
     
     /* 
      * he device MAC address to be used for Boot:
      * All zero mac address indicates that the device E-fuse address should
      *  be used.
      */ 
     UINT16 mac_addr_h;
     UINT16 mac_addr_m;
     UINT16 mac_addr_l;
     
     /* 
      * The multicast or broadcast MAC address which should be accepted as
      * a destination MAC address for boot table frames
      */
     UINT16 mmac_addr_h;
     UINT16 mmac_addr_m;
     UINT16 mmac_addr_l;
        
     UINT16 src_port;     /* Source UDP port number to be used during boot process */
                          /* 0: allow any SRC UDP port */
     UINT16 dest_port;    /* Destination UDP port number to be used during boot process */
     
     /* The Device ID to be included in the boot ready announcement frame */
     UINT16 device_id_12;   
     UINT16 device_id_34;   
     #define BOOT_PARAMS_DEVICE_ID_HIGH_MASK     0xFF00
     #define BOOT_PARAMS_DEVICE_ID_HIGH_SHIFT    8
     #define BOOT_PARAMS_DEVICE_ID_LOW_MASK      0x00FF
     #define BOOT_PARAMS_DEVICE_ID_LOW_SHIFT     0
     #define BOOT_PARAMS_GET_DEVICE_ID_13(device_id)    \
               (((device_id) & BOOT_PARAMS_DEVICE_ID_HIGH_MASK) > BOOT_PARAMS_DEVICE_ID_HIGH_SHIFT)
     #define BOOT_PARAMS_GET_DEVICE_ID_24(device_id)    \
               (((device_id) & BOOT_PARAMS_DEVICE_ID_LOW_MASK) > BOOT_PARAMS_DEVICE_ID_LOW_SHIFT)
 
     /* 
      * The destination MAC address used for the boot ready announce frame
      */
     UINT16 hmac_addr_h;
     UINT16 hmac_addr_m;
     UINT16 hmac_addr_l;

	 /*
      *	The CPSGMII configurations for Faraday
      */

     UINT16 sgmiiConfig;

     #define BOOT_PARAMS_SGMII_CONFIG_INDEX_MASK     0x0F     /* bit 3 to 0 is index */
     #define BOOT_PARAMS_SGMII_CONFIG_DIRECT_CONFIG  (1<<4)   /* set to use direct configurations */
     #define BOOT_PARAMS_SGMII_CONFIG_NO_CONFIG      (1<<5)   /* set to bypass CPSGMII config  */

	 UINT16 sgmiiControl;  
     #define BOOT_PARAMS_SGMII_CONTROL_MASK 0x7F

	 UINT16 sgmiiMr_Adv_Ability;
     #define BOOT_PARAMS_SGMII_ABILITY_MASK 0x0000FFFF

     /* These registers are the serdes configuration registers */
	 UINT16 sgmiiTx_Cfg_h;
	 UINT16 sgmiiTx_Cfg_l;
	 UINT16 sgmiiRx_Cfg_h;
	 UINT16 sgmiiRx_Cfg_l;
	 UINT16 sgmiiAux_Cfg_h;
	 UINT16 sgmiiAux_Cfg_l;
     
     UINT16 pktPllCfg_msw;      /* Packet subsystem PLL configuration */
     UINT16 pktPllCfg_lsw;
     
}   BOOT_PARAMS_ETHERNET_T;


/**************************************************************************************
 * Ethernet parameters used with bootp/tftp boot
 **************************************************************************************/
typedef struct boot_params_bootp_ethernet_s{

    /* common portion of the Boot parameters */
    UINT16 length;
    UINT16 checksum;
    UINT16 boot_mode;
    UINT16 portNum;
    UINT16 swPllCfg_msw;  /* CPU PLL configuration, MSW */
    UINT16 swPllCfg_lsw;  /* CPU PLL configuration, LSW */
     
    /* Etherent specific portion of the Boot Parameters */
    UINT16 options;

    /* The unicast mac address accepted by this device */
    UINT16 mac_addr_h;
    UINT16 mac_addr_m;
    UINT16 mac_addr_l;
    
    /* The multicast mac address accepted by this device */
    UINT16 mmac_addr_h;
    UINT16 mmac_addr_m;
    UINT16 mmac_addr_l;
    
    /* The number of milli-seconds to wait before timing out a bootp request and tftp packet */
    UINT16 bootpTimeoutMs;        /* This is the initial value. After each failure the time is doubled */
    UINT16 tftpAckTimeoutMs;      /* Time for an ack timeout */
    UINT16 tftpTotalTimeoutSec;   /* Number of seconds after which a transfer is timed out */
    
    /* The number of times to resend the bootp/tftp ack before giving up */
    UINT16 bootpRetryCount;
    UINT16 tftpRetryCount;
    
    
    /* The device string is sent in the bootp request vendor specific area
     * and class code fields. Up to 32 bytes packed into 16 UINT16s, 0 terminated */
    #define VENDOR_STRING_SIZE_UINT16       16
    UINT16 vendorString[VENDOR_STRING_SIZE_UINT16];
    
    /* The client ID string is sent in the bootp request client identifier code (option code 61) */
    #define CLIENT_ID_STRING_SIZE_UINT16     10
    UINT16 clientIdString[CLIENT_ID_STRING_SIZE_UINT16];
    
    
    /* SGMII configuration */
	UINT16 sgmiiControl;  
	UINT16 sgmiiMr_Adv_Ability;
    
     /* These registers are the serdes configuration registers */
	UINT16 sgmiiTx_Cfg_h;
	UINT16 sgmiiTx_Cfg_l;
	UINT16 sgmiiRx_Cfg_h;
	UINT16 sgmiiRx_Cfg_l;
	UINT16 sgmiiAux_Cfg_h;
	UINT16 sgmiiAux_Cfg_l;
    
    UINT16 pktPllCfg_msw;      /* Packet subsystem PLL configuration */
    UINT16 pktPllCfg_lsw;
    

}   BOOT_PARAMS_BOOTP_ETHERNET_T;


/**************************************************************************************
 * NAND boot options
 **************************************************************************************/
typedef struct boot_params_nand_s {

    /* common portion of the Boot parameters */
    UINT16 length;
    UINT16 checksum;
    UINT16 boot_mode;
    UINT16 portNum;
    UINT16 swPllCfg_msw;  /* CPU PLL configuration, MSW */
    UINT16 swPllCfg_lsw;  /* CPU PLL configuration, LSW */
    
    UINT16 options;
    
    #define BOOT_PARAMS_NAND_OPTIONS_I2C_PARAMS  (1 << 0)
    
    UINT16 i2cClkFreqKhz;   /* Desired speed of the I2C bus */
    UINT16 i2cTargetAddr;   /* I2C Bus address of the target */
    UINT16 i2cLocalAddr;    /* I2C Bus address of this device */
    UINT16 i2cDataAddr;     /* Where to read the configuration block */
    UINT16 i2cWtoRDelay;    /* Delay between address writes and data reads, in I2C bus cycles */
    
    UINT16 csNum;           /* Chip select region to use */
    
    UINT16 firstBlock;      /* The first block to try to read */
 
 } BOOT_PARAMS_NAND_T;


/**************************************************************************************
 * Utopia boot options
 **************************************************************************************/
typedef struct boot_params_utopia_s{

    /* common portion of the Boot parameters */
    UINT16 length;
    UINT16 checksum;
    UINT16 boot_mode;
    UINT16 portNum;
    UINT16 swPllCfg_msw;  /* CPU PLL configuration, MSW */
    UINT16 swPllCfg_lsw;  /* CPU PLL configuration, LSW */
     
    /* Utopia specific portion of the Boot Parameters */
    /* Options 
     *  ---------------------------------------------------------------
     * | 15                          3  |    2    |    1    |    0    |
     * ----------------------------------------------------------------
     *          reserved                     |         |         \-> 0 = multi phy
     *                                       |         |             1 = single phy
     *                                       |         \-> 0 = 8 bit utopia
     *                                       |             1 = 16 bit utopis
     *                                       \-> 0 = Init port
     *                                           1 = skip port init
     */
    UINT16 options;
    
    #define BOOT_PARAMS_UTOPIA_SINGLE_PHY    (1<<0)
    #define BOOT_PARAMS_UTOPIA_16BIT         (1<<1)
    #define BOOT_PARAMS_UTOPIA_SKIP_INIT     (1<<2)
    
    UINT16 cellSizeBytes;    /* Cell Size */
    UINT16 busWidthBits;     /* Bus width (8 or 16) */
    UINT16 slid;             /* Slave ID  */
    UINT16 coreFreqMhz;      /* CPU frequency after pll mult */
     
    
} BOOT_PARAMS_UTOPIA_T;

typedef struct boot_params_i2c_s{

    /* common portion of the Boot parameters */
    UINT16 length;
    UINT16 checksum;
    UINT16 boot_mode;
    UINT16 portNum;
    UINT16 swPllCfg_msw;  /* CPU PLL configuration, MSW */
    UINT16 swPllCfg_lsw;  /* CPU PLL configuration, LSW */
    
    /* I2C specific portion of the Boot Parameters */
    UINT16 options;
    /*
     * I2C Specific Options
     * Bit 01-00: BT:                                      
     *            00 - Boot Parameter Mode                            
     *            01 - Boot Table Mode                     
     *            10 - Boot Config mode
     *            11 - Slave receive boot config
     * Bit 04-02: EETYPE: EEPROM type             
     * Other bits:  Reserved 
     */ 
     #define BOOT_PARAMS_I2C_OPTIONS_BP             0x0000
     #define BOOT_PARAMS_I2C_OPTIONS_BT             0x0001
     #define BOOT_PARAMS_I2C_OPTIONS_BC             0x0002
     #define BOOT_PARAMS_I2C_OPTIONS_SLVOPT         0x0003
     
     #define BOOT_PARAMS_I2C_OPTIONS_MASK           0x0003
     #define BOOT_PARAMS_I2C_OPTIONS_SHIFT          0
     
     #define BOOT_PARAMS_I2C_OPTIONS_EETYPE_MASK    0x001C
     #define BOOT_PARAMS_I2C_OPTIONS_EETYPE_SHIFT   2 
     
     #define BOOT_PARAMS_I2C_IS_BOOTTBL_MODE(options) \
             (((options) & BOOT_PARAMS_I2C_OPTIONS_MASK) == BOOT_PARAMS_I2C_OPTIONS_BT)
             
     #define BOOT_PARAMS_I2C_IS_BOOTCONFIG_MODE(options) \
             (((options) & BOOT_PARAMS_I2C_OPTIONS_MASK) == BOOT_PARAMS_I2C_OPTIONS_BC)
             
     #define BOOT_PARAMS_I2C_IS_SLAVE_RCV_OPTIONS_MODE(options) \
             (((options) & BOOT_PARAMS_I2C_OPTIONS_MASK) == BOOT_PARAMS_I2C_OPTIONS_SLVOPT)
             
     #define BOOT_PARAMS_I2C_IS_BOOTPARAM_MODE(options) \
             (((options) & BOOT_PARAMS_I2C_OPTIONS_MASK) == BOOT_PARAMS_I2C_OPTIONS_BP)
             
     #define BOOT_PARAMS_I2C_SET_BOOTTBL_MODE(options, mode)               \
             (options) = ((options) & ~BOOT_PARAMS_I2C_OPTIONS_MASK) |     \
                         (((mode)   &  BOOT_PARAMS_I2C_OPTIONS_MASK) <<    \
                                       BOOT_PARAMS_I2C_OPTIONS_SHIFT)
             
             
     #define BOOT_PARAMS_I2C_GET_EETYPE(options)    \
             (((options) & BOOT_PARAMS_I2C_OPTIONS_EETYPE_MASK) >> BOOT_PARAMS_I2C_OPTIONS_EETYPE_SHIFT)
     #define BOOT_PARAMS_I2C_SET_EETYPE(options, ee_type)         \
             (options) = (((options) & ~BOOT_PARAMS_I2C_OPTIONS_EETYPE_MASK) |  \
                         (((ee_type) << BOOT_PARAMS_I2C_OPTIONS_EETYPE_SHIFT) & BOOT_PARAMS_I2C_OPTIONS_EETYPE_MASK))          
        
     /* The device address to be used for Boot */
     UINT16 dev_addr;           /* 16-bit device address (low) */
     UINT16 dev_addr_ext;       /* 16-bit extended device address (high) 
                                 * set to zero if not used
                                 * Note: some I2C device requires 32-bit 
                                 * address
                                 */ 
     UINT16 multi_i2c_id;      /* Multi device master write boot ID */
     UINT16 my_i2c_id;         /* This parts I2C address            */
     
     UINT16 core_freq_mhz;     /* Core frequency, MHz               */
     UINT16 i2c_clk_freq_khz;  /* Desired I2C clock frequency, kHz  */
     
     UINT16 next_dev_addr;      /* Used only for the boot config mode.         */
     UINT16 next_dev_addr_ext;  /* Copied into dev_addr* after config complete */
     
     UINT16 address_delay;      /* Rough number of cycles delay between address write
                                 * and read to the i2c eeprom */
     
     
} BOOT_PARAMS_I2C_T;   


typedef struct boot_params_rapidio_s{

    /* common portion of the Boot parameters */
    UINT16 length;
    UINT16 checksum;
    UINT16 boot_mode;
    UINT16 portNum;
    UINT16 swPllCfg_msw;  /* CPU PLL configuration, MSW */
    UINT16 swPllCfg_lsw;  /* CPU PLL configuration, LSW */
    
    /* Options */
    UINT16 options;
    
    #define BOOT_PARAMS_RIO_OPTIONS_TX_ENABLE    (1<<0)   /* set to enable transmit    */
    #define BOOT_PARAMS_RIO_OPTIONS_BOOT_TABLE   (1<<1)   /* set to use boot tables    */
    #define BOOT_PARAMS_RIO_OPTIONS_NO_CONFIG    (1<<2)   /* set to bypass port config */
    #define BOOT_PARAMS_RIO_OPTIONS_NO_QM_CONFIG (1<<3)   /* set to bypass QM config   */
    #define BOOT_PARAMS_RIO_OPTIONS_SRDS_PLL_SET (1<<4)   /* set to insert mult/rate into serdes registers */
    
    UINT16 lane_port_setup; /* Lane port configuration                        */
    #define BOOT_PARAMS_RIO_LPS_1X_1X_1X_1X     0       /* 4 ports 1 lane each */
    #define BOOT_PARAMS_RIO_LPS_2X_1X_1X        1       /* 3 ports, lanes 0,1 are a 2 lane port */
    #define BOOT_PARAMS_RIO_LPS_1X_1X_2X        2       /* 3 ports, lanes 2,3 are a 2 lane port */
    #define BOOT_PARAMS_RIO_LPS_2X_2X           3       /* 2 ports, each 2 lanes */
    #define BOOT_PARAMS_RIO_LPS_4X              4       /* 1 port of 4 lanes */
    
    UINT16  rsvd;
    
    UINT16 node_id;         /* The node id for this device                    */
    UINT16 serdes_ref_clk;  /* The serdes reference clock freq, in hundredths
                             * of MHz (1 MHz would be 100)                    */
    UINT16 link_rate;       /* Data link rate (mega bits per second           */
    UINT16 pf_low;          /* Packet forward range, low                      */
    UINT16 pf_high;         /* Packet forward range, high                     */
    
    UINT16 promiscuous_mask;    /* One bit set per master lane to indicate promiscuous mode */
    
    UINT16 serdes_aux_msw;      /* Serdes Auxillary config register */
    UINT16 serdes_aux_lsw;
    
    UINT16 serdes_rx_lane0_msw; /*  Rx Lane 0 config */
    UINT16 serdes_rx_lane0_lsw;
    
    UINT16 serdes_rx_lane1_msw; /*  Rx Lane 1 config */
    UINT16 serdes_rx_lane1_lsw;
    
    UINT16 serdes_rx_lane2_msw; /*  Rx Lane 1 config */
    UINT16 serdes_rx_lane2_lsw;
    
    UINT16 serdes_rx_lane3_msw; /*  Rx Lane 1 config */
    UINT16 serdes_rx_lane3_lsw;
    
    UINT16 serdes_tx_lane0_msw; /*  Tx Lane 0 config */
    UINT16 serdes_tx_lane0_lsw;
    
    UINT16 serdes_tx_lane1_msw; /*  Tx Lane 1 config */
    UINT16 serdes_tx_lane1_lsw;
    
    UINT16 serdes_tx_lane2_msw; /*  Tx Lane 1 config */
    UINT16 serdes_tx_lane2_lsw;
    
    UINT16 serdes_tx_lane3_msw; /*  Tx Lane 1 config */
    UINT16 serdes_tx_lane3_lsw;
    
} BOOT_PARAMS_RIO_T;


typedef struct boot_params_pcie_s
{
    /* common portion of the Boot parameters */
    UINT16 length;
    UINT16 checksum;
    UINT16 boot_mode;
    UINT16 portNum;
    UINT16 swPllCfg_msw;  /* CPU PLL configuration, MSW */
    UINT16 swPllCfg_lsw;  /* CPU PLL configuration, LSW */
    
    UINT16 options;
    
#define BOOT_PARAMS_PCIE_OPTIONS_COMPUTE_PLL_MASK    (1 << 2)
#define BOOT_PARAMS_PCIE_OPTIONS_COMPUTE_PLL         (1 << 2)
#define BOOT_PARAMS_PCIE_OPTIONS_NO_COMPUTE_PLL      (0 << 2)

#define BOOT_PARAMS_PCIE_OPTIONS_NO_INIT_MASK        (1 << 1)
#define BOOT_PARAMS_PCIE_OPTIONS_NO_INIT             (1 << 1)
#define BOOT_PARAMS_PCIE_OPTIONS_INIT                (0 << 1)


    UINT16 addressWidth;    /* The PCI address width. Valid values are 32 or 64 */
    UINT16 linkRateMhz;     /* The serdes frequency. Valid values are 2500 and 5000 */
    UINT16 refClock10kHz;   /* The reference clock in 10kHz units. Valid values are 10000, 12500, 15625, 25000, 31250 */
    UINT16 window1Size;     /* Window 1 size, in units of megabytes */
    UINT16 window2Size;     /* Window 2 size */
    UINT16 window3Size;     /* Window 3 size. Valid only if the address width is 32 */
    UINT16 window4Size;     /* Window 4 size. Valid only if the address width is 32 */
    UINT16 window5Size;     /* Window 5 size. Valid only if the address width is 32 */
    
    UINT16 vendorId;             /* Vendor ID field */
    UINT16 deviceId;             /* Device ID field */
    UINT16 classCodeRevId_Msw;   /* Class code rev ID, MSW */
    UINT16 classCodeRevId_Lsw;   /* Class code rev ID, LSW*/
    
    
    
    UINT16 serdesCfgMsw;    /* Serdes auxillary config, MSW */
    UINT16 serdesCfgLsw;    /* Serdes auxillary config, LSW */
    
    UINT16 serdesCfgLane0Msw;   /* Serdes Lane 0 config, MSW */
    UINT16 serdesCfgLane0Lsw;   /* Serdes Lane 0 config, LSW */
    
    UINT16 serdesCfgLane1Msw;   /* Serdes Lane 1 config, MSW */
    UINT16 serdesCfgLane1Lsw;   /* Serdes Lane 1 config, LSW */
    
   
} BOOT_PARAMS_PCIE_T;
    
    
typedef struct boot_params_emif_nor_s  {
    UINT16 waitEnable;
    UINT16 asyncCfgMsw;
    UINT16 asyncCfgLsw;
} boot_params_emif_nor_t;
    
    
typedef struct boot_params_emif_s
{
    /* common portion of the Boot parameters */
    UINT16 length;
    UINT16 checksum;
    UINT16 boot_mode;
    UINT16 portNum;
    UINT16 swPllCfg_msw;  /* CPU PLL configuration, MSW */
    UINT16 swPllCfg_lsw;  /* CPU PLL configuration, LSW */
    
    UINT16 options;
    
    
#define BOOT_PARAMS_EMIF_OPTIONS_USE_ASYNC_MASK      (1 << 0)
#define BOOT_PARAMS_EMIF_OPTIONS_USE_ASYNC           (1 << 1)
#define BOOT_PARAMS_EMIF_OPTIONS_USE_ASYNC_DISABLED  (1 << 0)
    
    UINT16 type;
#define BOOT_PARAMS_EMIF_TYPE_NOR  0

    UINT16 branchAddrMsw;
    UINT16 branchAddrLsw;
    
    UINT16 csNum;
    UINT16 memWidth;
    
    union  {
      boot_params_emif_nor_t nor;
    } u;
    
} BOOT_PARAMS_EMIF_T;
    
    
typedef struct boot_params_vusr_s
{
    /* common portion of the Boot parameters */
    UINT16 length;
    UINT16 checksum;
    UINT16 boot_mode;
    UINT16 portNum;
    UINT16 swPllCfg_msw;  /* CPU PLL configuration, MSW */
    UINT16 swPllCfg_lsw;  /* CPU PLL configuration, LSW */
    
    UINT16 options;
#define BOOT_PARAMS_VUSR_OPTIONS_NO_INIT_MASK        (1 << 1)
#define BOOT_PARAMS_VUSR_OPTIONS_NO_INIT             (1 << 1)
#define BOOT_PARAMS_VUSR_OPTIONS_INIT                (0 << 1)

    UINT16 nLanes;          /* The number of lanes to configure */
    
    UINT16 serdesCfgMsw;    /* Serdes auxillary config, MSW */
    UINT16 serdesCfgLsw;    /* Serdes auxillary config, LSW */
    
    UINT16 serdesCfgRxLane0Msw;   /* Serdes Rx Lane 0 config, MSW */
    UINT16 serdesCfgRxLane0Lsw;   /* Serdes Rx Lane 0 config, LSW */
    
    UINT16 serdesCfgTxLane0Msw;   /* Serdes Tx Lane 0 config, MSW */
    UINT16 serdesCfgTxLane0Lsw;   /* Serdes Tx Lane 0 config, LSW */
    
    
    
    UINT16 serdesCfgRxLane1Msw;   /* Serdes Rx Lane 1 config, MSW */
    UINT16 serdesCfgRxLane1Lsw;   /* Serdes Rx Lane 1 config, LSW */
    
    UINT16 serdesCfgTxLane1Msw;   /* Serdes Tx Lane 1 config, MSW */
    UINT16 serdesCfgTxLane1Lsw;   /* Serdes Tx Lane 1 config, LSW */
    
    
    UINT16 serdesCfgRxLane2Msw;   /* Serdes Rx Lane 2 config, MSW */
    UINT16 serdesCfgRxLane2Lsw;   /* Serdes Rx Lane 2 config, LSW */
    
    UINT16 serdesCfgTxLane2Msw;   /* Serdes Tx Lane 2 config, MSW */
    UINT16 serdesCfgTxLane2Lsw;   /* Serdes Tx Lane 2 config, LSW */
    
    

    UINT16 serdesCfgRxLane3Msw;   /* Serdes Rx Lane 3 config, MSW */
    UINT16 serdesCfgRxLane3Lsw;   /* Serdes Rx Lane 3 config, LSW */
    
    UINT16 serdesCfgTxLane3Msw;   /* Serdes Tx Lane 3 config, MSW */
    UINT16 serdesCfgTxLane3Lsw;   /* Serdes Tx Lane 3 config, LSW */
    
} BOOT_PARAMS_VUSR_T;    

typedef struct boot_params_spi_s
{
    /* common portion of the Boot parameters */
    UINT16 length;
    UINT16 checksum;
    UINT16 boot_mode;
    UINT16 portNum;
    UINT16 swPllCfg_msw;  /* CPU PLL configuration, MSW */
    UINT16 swPllCfg_lsw;  /* CPU PLL configuration, LSW */
    
    UINT16 options;
    /*
     * SPI Specific Options
     * Bit 01-00: BT:                                      
     *            00 - Boot Parameter Mode                            
     *            01 - Boot Table Mode                     
     *            10 - Boot Config mode
     *            11 - GP header blob
     * Other bits:  Reserved 
     */ 
     #define BOOT_PARAMS_SPI_OPTIONS_BP             0x0000
     #define BOOT_PARAMS_SPI_OPTIONS_BT             0x0001
     #define BOOT_PARAMS_SPI_OPTIONS_BC             0x0002
     #define BOOT_PARAMS_SPI_OPTIONS_GPHDR          0x0003
     
     #define BOOT_PARAMS_SPI_OPTIONS_MASK           0x0003
     #define BOOT_PARAMS_SPI_OPTIONS_SHIFT          0
     
     #define BOOT_PARAMS_SPI_IS_BOOTTBL_MODE(options) \
             (((options) & BOOT_PARAMS_SPI_OPTIONS_MASK) == BOOT_PARAMS_SPI_OPTIONS_BT)
             
     #define BOOT_PARAMS_SPI_IS_BOOTCONFIG_MODE(options) \
             (((options) & BOOT_PARAMS_SPI_OPTIONS_MASK) == BOOT_PARAMS_SPI_OPTIONS_BC)
             
     #define BOOT_PARAMS_SPI_IS_BOOTPARAM_MODE(options) \
             (((options) & BOOT_PARAMS_SPI_OPTIONS_MASK) == BOOT_PARAMS_SPI_OPTIONS_BP)
             
     #define BOOT_PARAMS_SPI_IS_GPHDR_MODE(options) \
             (((options) & BOOT_PARAMS_SPI_OPTIONS_MASK) == BOOT_PARAMS_SPI_OPTIONS_GPHDR)
             
             
     #define BOOT_PARAMS_SPI_SET_BOOTTBL_MODE(options, mode)               \
             (options) = ((options) & ~BOOT_PARAMS_SPI_OPTIONS_MASK) |     \
                         (((mode)   &  BOOT_PARAMS_SPI_OPTIONS_MASK) <<    \
                                       BOOT_PARAMS_SPI_OPTIONS_SHIFT)
             
             
     UINT16 addrWidth;          /* 16 or 24 are the only valid values */
     UINT16 nPins;              /* 4 or 5 pins are the only valid values */
     UINT16 csel;               /* only values 0b10 (cs0 low) or 0b01 (cs1 low) are valid */
     UINT16 mode;               /* Clock phase/polarity. These are the standard SPI modes 0-3 */
     UINT16 c2tdelay;           /* Setup time between chip select assert and the transaction */
     
     UINT16 cpuFreqMhz;         /* Speed the CPU is running after PLL configuration */
     UINT16 busFreqMhz;         /* The speed of the SPI bus, the megahertz portion */
     UINT16 busFreqKhz;         /* The KHz portion of the bus frequency. A frequency of 1.5 MHz would have the value 5 here */
     
     UINT16 read_addr_msw;      /* The base address to read from the SPI, upper 16 bits */
     UINT16 read_addr_lsw;      /* The base address to read from the SPI, lower 16 bits */
     
     UINT16 next_csel;          /* The next chip select to use if in boot config mode, when the config is complete */
     UINT16 next_read_addr_msw; /* The next read address to use if in boot config mode */
     UINT16 next_read_addr_lsw; /* The next read address to use if in boot config mode */
             
} BOOT_PARAMS_SPI_T;             

typedef struct boot_params_sleep_s
{
    /* common portion of the Boot parameters */
    UINT16 length;
    UINT16 checksum;
    UINT16 boot_mode;
    UINT16 portNum;
    UINT16 swPllCfg_msw;  /* CPU PLL configuration, MSW */
    UINT16 swPllCfg_lsw;  /* CPU PLL configuration, LSW */
    
    UINT16 options;
    
} BOOT_PARAMS_SLEEP_T;

typedef struct boot_params_uart_s
{
    /* common portion of the Boot parameters */
    UINT16 length;
    UINT16 checksum;
    UINT16 boot_mode;
    UINT16 portNum;
    UINT16 swPllCfg_msw;  /* CPU PLL configuration, MSW */
    UINT16 swPllCfg_lsw;  /* CPU PLL configuration, LSW */
    
    UINT16 options;
    
    UINT16 bootDataFormat;
    #define BOOT_PARAMS_UART_DATA_FORMAT_BLOB   0
    #define BOOT_PARAMS_UART_DATA_FORMAT_BTBL   1
    
    UINT16 protocol;
    #define BOOT_PARAMS_UART_PROTOCOL_XMODEM    0
    
    UINT16 initialNackCount;        /* Number of nacks to send at start before giving up */
    UINT16 maxErrCount;             /* Max number of consecutive receive errors          */
    
    UINT16 nackTimeoutSec;          /* Number of seconds for a nack timeout */
    UINT16 charTimeoutMsec;         /* Inter-character timeout, milli-seconds */
    
    UINT16 nDataBits;               /* Number of data bits, currently only 8 supported */
    UINT16 parity;
    #define BOOT_PARAMS_UART_PARITY_NONE  0
    #define BOOT_PARAMS_UART_PARITY_ODD   1
    #define BOOT_PARAMS_UART_PARITY_EVEN  2
    UINT16 nStopBitsx2;             /* Number of stop bits x2. Valid values are 2, 3, and 4 for 1, 1.5 and 2 stop bits */
    UINT16 overSampleFactor;        /* Valid values are 13 and 16 times oversampling */
    
    UINT16 flowControl;
    #define BOOT_PARAMS_FLOW_CONTROL_NONE       0
    #define BOOT_PARAMS_FLOW_CONTROL_RTS_CTS    1
    
    UINT16 dataRateMsw;             /* Baud rate, MSW */
    UINT16 dataRateLsw;             /* Baud rate, LSW */
    
    UINT16 timerRefMhz;             /* Timer referecnce frequency in MHz */
    
} BOOT_PARAMS_UART_T;
    


/*
 * UNION of boot parameter structures in all modes
 * Note: We need to make sure that the structures genertaed by the C-compiler
 *       match with the boot parameter table data format i.e. a set of 16-bit
 *       data array.
 */ 
#define BOOT_PARAMS_SIZE_IN_BYTES       128
typedef union {
   BOOT_PARAMS_COMMON_T          common;  
   BOOT_PARAMS_ETHERNET_T        eth;
   BOOT_PARAMS_BOOTP_ETHERNET_T  ethBootp;
   BOOT_PARAMS_NAND_T            nand;
   BOOT_PARAMS_I2C_T             i2c;
   BOOT_PARAMS_UTOPIA_T          utopia;
   BOOT_PARAMS_RIO_T             rio;
   BOOT_PARAMS_PCIE_T            pcie;
   BOOT_PARAMS_EMIF_T            emif;
   BOOT_PARAMS_VUSR_T            vusr;
   BOOT_PARAMS_SPI_T             spi;
   BOOT_PARAMS_SLEEP_T           sleep;
   BOOT_PARAMS_UART_T            uart;
   UINT16                        parameter[BOOT_PARAMS_SIZE_IN_BYTES/2]; 
} BOOT_PARAMS_T;


/*******************************************************************************
 * Definition: The time stamp and version number are placed into the stats. 
 *             This will be two characters packed per 16bits . The length
 *             value must be 32 bit divisible
 *******************************************************************************/
#define BOOT_VERSION_LEN_UINT16    32 
typedef struct BOOT_VERSION_S {

  UINT16 vstring[BOOT_VERSION_LEN_UINT16];
  
} BOOT_VERSION_T;
extern BOOT_VERSION_T bootVersion;
  

/*******************************************************************************
 * Definition: Runs time stats that are not initialized on cold boot entry
 *   !!!!!! boot.s assumes that the nonInit stats are at the top of the structure
 *   !!!!!! and that stage is the first element
 *******************************************************************************/
typedef struct BOOT_STATS_NONINIT_S {
  UINT16  stage;            /* Record the SharedROM code execution stage */
  #define BOOT_STAGE_ASM_START_UP               1
  #define BOOT_STAGE_INIT_CACHE                 2
  #define BOOT_STAGE_CHCHE_INITED               3
  #define BOOT_STAGE_ENTER_WARMBOOT             4
  #define BOOT_STAGE_INIT_CPGMAC                5
  #define BOOT_STAGE_SEND_ERA_FRAME             6
  #define BOOT_STAGE_ETH_MAIN_LOOP              7
  #define BOOT_STAGE_I2C_BOOTTBL_LOOP           8
  #define BOOT_STAGE_I2C_BOOTPARAM_LOOP         9
  #define BOOT_STAGE_DISABLE_CACHE             10
  #define BOOT_STAGE_CHCHE_DISABLED            11
  #define BOOT_STAGE_EXIT                      12
  #define BOOT_STAGE_ERROR_LOOP                13  
  #define BOOT_STAGE_I2C_BOOTCONFIG_LOOP       14
  #define BOOT_STAGE_I2C_SLV_RCV_OPTIONS_LOOP  15
  #define BOOT_STAGE_UTOPIA_MAIN_LOOP          16
  UINT16  coldBootEntries;
  
} BOOT_STATS_NONINIT_T;

/*******************************************************************************
 * Definition: Run time statistics and error counts. These stats are 
 *             initialized on cold boot entry.
 ******************************************************************************/
 
typedef struct BOOT_STATS_COMMON_S  {
  UINT32    bootStatus;
  UINT16    nColdBootEntries;
  UINT16    nBootReentries;
  UINT16    nPllWarns;
  UINT16    nResetWarns;
} BOOT_STATS_COMMON_T;
 
 
typedef struct BOOT_STATS_MAIN_S {
  UINT16  errorCode;        /* (module ID <<8 ) + module specific error */
  
  /* I2C operation related statistics */
  UINT16  numI2Cpkts;       /* number of I2C boot table packets processed */
  UINT16  numI2CchksumError;/* number of I2C checksum errors */
  UINT16  numI2ClengthError;/* number of I2C block length errors */
  UINT16  numI2CotherError; /* number of I2C section with invalid length and etc */
  UINT16  numI2Cretrys;     /* number of I2C retrys due to read access errors */
  UINT16  numI2cWrites;     /* number of I2C master writes to passive devices */
  UINT16  numI2cWriteError; /* number of I2C master write errors              */
  
  UINT16  warmBootEntry;    /* Count of entries into warm boot routine   */
} BOOT_STATS_MAIN_T;



/*****************************************************************************
 * Definition: I2C stats, Boot table and Ethernrt stats initialized 
 *             on cold boot entry
 *****************************************************************************/
typedef struct I2C_STATS_tag
{
    UINT16      num_trans;
    UINT16      num_trys;
    UINT16      num_try_ok;
    UINT16      num_try_lost_arb;
    UINT16      num_try_idle_to;
    UINT16      num_try_no_ack;
    UINT16      num_try_other_err;
    UINT32      extra_idle_waits;
    UINT32      extra_clock_waits;
    UINT32      tx_bytes;
    UINT32      rx_bytes;
    UINT32      data_re_reads;
} I2C_STATS_T;

typedef struct BTBL_STATS_tag
{
  UINT16  num_sections;     /* number of boot table sections received */
  UINT16  num_pdma_copies;  /* number of PDMA copies performed */
} BTBL_STATS_T;

typedef struct ETH_STATS_tag
{
  /* MAC packets related statistics */
  UINT16  uniMacPkts;       /* Count of packets received with valid unicast mac 
                               address   */
  UINT16  multiMacPkts;     /* Count of packets received with valid multicast or
                               broadcast mac address   */
  UINT16  invalidMacPkts;   /* Count of packets received with invalid mac 
                               address   */
  UINT16  invalidLLCPkts;   /* Count of 802.3 packets with wrong LLC/SNAP header */
  UINT16  nonIpPkts;        /* Count of non-IP packets received with valid 
                               MAC address   */
                               
  /* IP packets related statistics */                             
  UINT16  nEra;             /* Number of ERA packets transmitted */
  UINT16  nonIP4Pkts;       /* Count of non-IP4 packets        */
  UINT16  ipfragments;      /* Count of IP fragments received      */
  UINT16  ipTruncatedError; /* Count of truncated IP frame */
  UINT16  nonUDPPkts;       /* Count of IP packets with non-UDP paylaod  */
  
  /* UDP packets related statistics */
  UINT16  udpSizeError;     /* Count of UDP packet with invalid (odd) size */
  UINT16  udpPortError;     /* Count of UDP packets with invalid port number */
  UINT16  udpChksumError;   /* Count of UDP packets with checksum error */
  
  /* Boot table packets related statistics */
  UINT16  nonBtblPkts;      /* Count of UDP packets with invalid boot table paylaod */
  UINT16  outSeqPkts;       /* Count of out of sequence boot table packets received       
                               i.e. packets with unexpected seq_num      */
  UINT16  expSeqNum;        /* Expected Sequence Number */
  UINT16  lastSeqNum;       /* Last sequence number received */                             

  /* Driver errors */
  UINT16  sizeZeroPackets;  /* Count of packets arriving with 0 size */
} ETH_STATS_T;

typedef struct PCI_EEAI_STATS_tag
{
  UINT16  pciI2cReads;         /* Count of block reads of i2c eeprom */
  UINT16  pciI2cRetries;       /* Count of i2c read retries          */
  UINT16  pciChkSumErr;        /* Count of block check sum errors    */
  UINT16  pciEeaiFail;         /* Count of aborted pci attempts      */
} PCI_EEAI_STATS_T;

/* Rapid I/O stats */
typedef struct RAPIDIO_STATS_tag
{
  UINT16 rapidIoFailReady;     /* If set rapid I/O peripheral failed to report ready */
  UINT16 rapidIoBtblBlocks;    /* Count of number of boot table blocks received */
  UINT16 rapidIoBtblBadBlocks; /* Count of boot table blocks rejected */
  UINT16 rapidIoDescrEmpty;    /* Count of times ROM found rx descriptor queue empty */
} RAPIDIO_STATS_T;

typedef struct HPI_STATS_Tag
{
  UINT16 hpiBtblBlocks;        /* Count of boot table blocks received */
  UINT16 hpiBtblBadBlocks;     /* Count of boot table blocks rejected */
} HPI_STATS_T;

/* Utopia stats */
typedef struct UTOPIA_STATS_tag
{
  UINT16 cellCount;            /* Count of cells received   */
  UINT16 invalidPtr;           /* Count of invalid pointers received in processing */
  UINT16 invalidSize;          /* Count of cells that were too small               */
  UINT16 cellMagicCount;       /* Count of cells received with valid magic         */
  UINT16 cellMagicFailed;      /* Count of cells received with invalid magic       */
  UINT16 trapNoCellMem;        /* Trapped due to no cell space in memory           */
  UINT16 possibleOverrun;      /* Count of possible cell buffer overruns           */
} UTOPIA_STATS_T;

/* Ethernet bootp stats */
typedef struct ETH_BP_STATS_tag
{
    UINT16 rxCount;
    UINT16 bootpTimeout;
    UINT16 tftpAckTimeout;
    UINT16 downloadTimeout;
} ETH_BP_STATS_T;

/*******************************************************************************
 * Definition: The statistics
 *
 *   !!!!!! boot.s assumes that the nonInit stats are at the top of the structure!
 *******************************************************************************/
typedef struct BOOT_STATS_S {

 BOOT_STATS_COMMON_T   common;
 BOOT_STATS_NONINIT_T  nonInit;
 BOOT_STATS_MAIN_T     main;
 I2C_STATS_T           i2c;
 BTBL_STATS_T          btbl;
 union  {
   ETH_STATS_T           eth;
   ETH_BP_STATS_T        ethBp;
   PCI_EEAI_STATS_T      pci_eeai;
   RAPIDIO_STATS_T       rapidIo;
   UTOPIA_STATS_T        utopia;
   HPI_STATS_T           hpi;
 } u;
} BOOT_STATS_T;

extern BOOT_STATS_T bootStats;

/*******************************************************************************
 * Definition: The magic start address, known to all modules
 *******************************************************************************/
extern volatile UINT32 *p_boot_entry_addr;


/*******************************************************************************
 * Emif4 (DDR3) configuration table
 *******************************************************************************/
typedef struct bootEmif4Tbl_s  {

    UINT32  configSelect_msw;               /* Bit map defining which registers to set */
    UINT32  configSelect_slsw;              /* Bit map defining which registers to set */
    UINT32  configSelect_lsw;               /* Bit map defining which registers to set */
    
    UINT32  pllPrediv;                      /* Values of all 0s will disable the pll */
    UINT32  pllMult;
    UINT32  pllPostDiv;

    UINT32  sdRamConfig;                /* Controlled by bit 1 of configSelect_msw */
    UINT32  sdRamConfig2;               /* Bit 2 */
    UINT32  sdRamRefreshCtl;            /* Bit 3 */
    UINT32  sdRamTiming1;               /* Bit 4 */
    UINT32  sdRamTiming2;               /* Bit 5 */
    UINT32  sdRamTiming3;               /* Bit 6 */
    UINT32  lpDdrNvmTiming;             /* Bit 7 */
    UINT32  powerManageCtl;             /* Bit 8 */
    UINT32  iODFTTestLogic;             /* Bit 9 */
    UINT32  performCountCfg;            /* Bit 10 */
    UINT32  performCountMstRegSel;      /* Bit 11 */
    UINT32  readIdleCtl;                /* Bit 12 */
    UINT32  sysVbusmIntEnSet;           /* Bit 13 */
    UINT32  sdRamOutImpdedCalCfg;       /* Bit 14 */
    UINT32  tempAlterCfg;               /* Bit 15 */
    UINT32  ddrPhyCtl1;                 /* Bit 16 */
    UINT32  ddrPhyCtl2;                 /* Bit 17 */
    UINT32  priClassSvceMap;            /* Bit 18 */
    UINT32  mstId2ClsSvce1Map;          /* Bit 19 */
    UINT32  mstId2ClsSvce2Map;          /* Bit 20 */
    UINT32  eccCtl;                     /* Bit 21 */
    UINT32  eccRange1;                  /* Bit 22 */
    UINT32  eccRange2;                  /* Bit 23 */
    UINT32  rdWrtExcThresh;             /* Bit 24 */
    
    UINT32 chipConfig[64];
    
} BOOT_EMIF4_TBL_T;

#define BOOT_EMIF4_ENABLE_MSW_pllCtl                      (1 <<  0)  

#define BOOT_EMIF4_ENABLE_MSW_sdRamConfig                 (1 <<  1)
#define BOOT_EMIF4_ENABLE_MSW_sdRamConfig2                (1 <<  2)
#define BOOT_EMIF4_ENABLE_MSW_sdRamRefreshCtl             (1 <<  3)
#define BOOT_EMIF4_ENABLE_MSW_sdRamTiming1                (1 <<  4)
#define BOOT_EMIF4_ENABLE_MSW_sdRamTiming2                (1 <<  5)
#define BOOT_EMIF4_ENABLE_MSW_sdRamTiming3                (1 <<  6)
#define BOOT_EMIF4_ENABLE_MSW_lpDdrNvmTiming              (1 <<  7)
#define BOOT_EMIF4_ENABLE_MSW_powerManageCtl              (1 <<  8)
#define BOOT_EMIF4_ENABLE_MSW_iODFTTestLogic              (1 <<  9)
#define BOOT_EMIF4_ENABLE_MSW_performCountCfg             (1 << 10)
#define BOOT_EMIF4_ENABLE_MSW_performCountMstRegSel       (1 << 11)
#define BOOT_EMIF4_ENABLE_MSW_readIdleCtl                 (1 << 12)
#define BOOT_EMIF4_ENABLE_MSW_sysVbusmIntEnSet            (1 << 13)
#define BOOT_EMIF4_ENABLE_MSW_sdRamOutImpdedCalCfg        (1 << 14)
#define BOOT_EMIF4_ENABLE_MSW_tempAlterCfg                (1 << 15)
#define BOOT_EMIF4_ENABLE_MSW_ddrPhyCtl1                  (1 << 16)
#define BOOT_EMIF4_ENABLE_MSW_ddrPhyCtl2                  (1 << 17)
#define BOOT_EMIF4_ENABLE_MSW_priClassSvceMap             (1 << 18)
#define BOOT_EMIF4_ENABLE_MSW_mstId2ClsSvce1Map           (1 << 19)
#define BOOT_EMIF4_ENABLE_MSW_mstId2ClsSvce2Map           (1 << 20)
#define BOOT_EMIF4_ENABLE_MSW_eccCtl                      (1 << 21)
#define BOOT_EMIF4_ENABLE_MSW_eccRange1                   (1 << 22)
#define BOOT_EMIF4_ENABLE_MSW_eccRange2                   (1 << 23)
#define BOOT_EMIF4_ENABLE_MSW_rdWrtExcThresh              (1 << 24)


#define BOOT_EMIF_ENABLE_SLSW_config0                      (1 <<  0)
#define BOOT_EMIF_ENABLE_SLSW_config1                      (1 <<  1)
#define BOOT_EMIF_ENABLE_SLSW_config2                      (1 <<  2)
#define BOOT_EMIF_ENABLE_SLSW_config3                      (1 <<  3)
#define BOOT_EMIF_ENABLE_SLSW_config4                      (1 <<  4)
#define BOOT_EMIF_ENABLE_SLSW_config5                      (1 <<  5)
#define BOOT_EMIF_ENABLE_SLSW_config6                      (1 <<  6)
#define BOOT_EMIF_ENABLE_SLSW_config7                      (1 <<  7)
#define BOOT_EMIF_ENABLE_SLSW_config8                      (1 <<  8)
#define BOOT_EMIF_ENABLE_SLSW_config9                      (1 <<  9)
#define BOOT_EMIF_ENABLE_SLSW_config10                     (1 << 10)
#define BOOT_EMIF_ENABLE_SLSW_config11                     (1 << 11)
#define BOOT_EMIF_ENABLE_SLSW_config12                     (1 << 12)
#define BOOT_EMIF_ENABLE_SLSW_config13                     (1 << 13)
#define BOOT_EMIF_ENABLE_SLSW_config14                     (1 << 14)
#define BOOT_EMIF_ENABLE_SLSW_config15                     (1 << 15)
#define BOOT_EMIF_ENABLE_SLSW_config16                     (1 << 16)
#define BOOT_EMIF_ENABLE_SLSW_config17                     (1 << 17)
#define BOOT_EMIF_ENABLE_SLSW_config18                     (1 << 18)
#define BOOT_EMIF_ENABLE_SLSW_config19                     (1 << 19)
#define BOOT_EMIF_ENABLE_SLSW_config20                     (1 << 20)
#define BOOT_EMIF_ENABLE_SLSW_config21                     (1 << 21)
#define BOOT_EMIF_ENABLE_SLSW_config22                     (1 << 22)
#define BOOT_EMIF_ENABLE_SLSW_config23                     (1 << 23)
#define BOOT_EMIF_ENABLE_SLSW_config24                     (1 << 24)
#define BOOT_EMIF_ENABLE_SLSW_config25                     (1 << 25)
#define BOOT_EMIF_ENABLE_SLSW_config26                     (1 << 26)
#define BOOT_EMIF_ENABLE_SLSW_config27                     (1 << 27)
#define BOOT_EMIF_ENABLE_SLSW_config28                     (1 << 28)
#define BOOT_EMIF_ENABLE_SLSW_config29                     (1 << 29)
#define BOOT_EMIF_ENABLE_SLSW_config30                     (1 << 30)
#define BOOT_EMIF_ENABLE_SLSW_config31                     (1u << 31)


#define BOOT_EMIF_ENABLE_LSW_config32                    (1 <<  0)
#define BOOT_EMIF_ENABLE_LSW_config33                    (1 <<  1)
#define BOOT_EMIF_ENABLE_LSW_config34                    (1 <<  2)
#define BOOT_EMIF_ENABLE_LSW_config35                    (1 <<  3)
#define BOOT_EMIF_ENABLE_LSW_config36                    (1 <<  4)
#define BOOT_EMIF_ENABLE_LSW_config37                    (1 <<  5)
#define BOOT_EMIF_ENABLE_LSW_config38                    (1 <<  6)
#define BOOT_EMIF_ENABLE_LSW_config39                    (1 <<  7)
#define BOOT_EMIF_ENABLE_LSW_config40                    (1 <<  8)
#define BOOT_EMIF_ENABLE_LSW_config41                    (1 <<  9)
#define BOOT_EMIF_ENABLE_LSW_config42                    (1 <<  10)
#define BOOT_EMIF_ENABLE_LSW_config43                    (1 <<  11)
#define BOOT_EMIF_ENABLE_LSW_config44                    (1 <<  12)
#define BOOT_EMIF_ENABLE_LSW_config45                    (1 <<  13)
#define BOOT_EMIF_ENABLE_LSW_config46                    (1 <<  14)
#define BOOT_EMIF_ENABLE_LSW_config47                    (1 <<  15)
#define BOOT_EMIF_ENABLE_LSW_config48                    (1 <<  16)
#define BOOT_EMIF_ENABLE_LSW_config49                    (1 <<  17)
#define BOOT_EMIF_ENABLE_LSW_config50                    (1 <<  18)
#define BOOT_EMIF_ENABLE_LSW_config51                    (1 <<  19)
#define BOOT_EMIF_ENABLE_LSW_config52                    (1 <<  20)
#define BOOT_EMIF_ENABLE_LSW_config53                    (1 <<  21)
#define BOOT_EMIF_ENABLE_LSW_config54                    (1 <<  22)
#define BOOT_EMIF_ENABLE_LSW_config55                    (1 <<  23)
#define BOOT_EMIF_ENABLE_LSW_config56                    (1 <<  24)
#define BOOT_EMIF_ENABLE_LSW_config57                    (1 <<  25)
#define BOOT_EMIF_ENABLE_LSW_config58                    (1 <<  26)
#define BOOT_EMIF_ENABLE_LSW_config59                    (1 <<  27)
#define BOOT_EMIF_ENABLE_LSW_config60                    (1 <<  28)
#define BOOT_EMIF_ENABLE_LSW_config61                    (1 <<  29)
#define BOOT_EMIF_ENABLE_LSW_config62                    (1 <<  30)
#define BOOT_EMIF_ENABLE_LSW_config63                    (1u <<  31)


#define BOOT_EMIF4_ENABLE_ALL_MSW                     0x01ffffff
#define BOOT_EMIF4_ENABLE_ALL_SLSW                    0xffffffff
#define BOOT_EMIF4_ENABLE_ALL_LSW                     0xffffffff
    
    
    
    
/* Hibernation function control */    
#define TIBOOT_CTL_HIBERNATION_MODE1        1
#define TIBOOT_CTL_HIBERNATION_MODE2        2


/* Hibernation function return */
#define TIBOOT_ERR_HIBERNATION_MODE_INVALID     -1
#define TIBOOT_ERR_HIBERNATION_ADDR_MISALIGNED  -2
#define TIBOOT_ERR_HIBERNATION_ADDR_INVALID     -3

/* Boot mode values */
#define BOOT_MODE_I2C               40
#define BOOT_MODE_SPI               50

/* ROM boot loader enter address */
#define BOOT_ROM_ENTER_ADDRESS      0x20b00000


#endif  /* __TIBOOT_H__ */

/* nothing past this point */
