#ifndef __TIBOOT_H__
#define __TIBOOT_H__
/******************************************************************************
 * FILE PURPOSE: Define Structures, MACROs and etc for TI Shared ROM Boot
 ******************************************************************************
 * FILE NAME:   tiboot.h
 *
 * DESCRIPTION: Define structures, macros and etc for the TI Shared ROM boot
 *              process.
 *
 * TABS: NONE
 *
 * $Id: $
 *
 * REVISION HISTORY:
 *
 * $Log: $
 *
 * (C) Copyright 2004 TELOGY Networks, Inc. 
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
  BOOT_ERROR                = 1,    /* General error */
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
  BOOT_SRIO_CONFIGINDEX     = 20    /* Failed at wrong SRIO config index */
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
   UINT16 swPll;         /* CPU PLL multiplier */ 
   
    
} BOOT_PARAMS_COMMON_T;

typedef struct boot_params_ethernet_s{

    /* common portion of the Boot parameters */
    UINT16 length;
    UINT16 checksum;
    UINT16 boot_mode;
    UINT16 portNum;
    UINT16 swPll;         /* CPU PLL multiplier */ 
     
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
     *
     * Bit 3: HD:                                      
     *        0 - Full Duplex                          
     *        1 - Half Duplex                          
     * Bit 4: SKIP TX                                           
     *        0 - Send the Ethernet Ready Frame 
     *        1 - Skip sending the Ethernet Ready Frame                                              
     * Bit 5: SKIP INIT                                           
     *        0 - Initialize the Ethernet MAC peripheral 
     *        1 - Skip initializing the Ethernet MAC peripheral 
     * Bit 6: FC
     *        0 - Disable Flow Control
     *        1 - Enable Flow Control                                               
     *
     * Other bits:  Reserved 
     */ 
     #define BOOT_PARAMS_ETH_OPTIONS_MII        0x0000
     #define BOOT_PARAMS_ETH_OPTIONS_RMII       0x0001
     #define BOOT_PARAMS_ETH_OPTIONS_GMII       0x0002
     #define BOOT_PARAMS_ETH_OPTIONS_RGMII      0x0003
     #define BOOT_PARAMS_ETH_OPTIONS_SMII       0x0004
     #define BOOT_PARAMS_ETH_OPTIONS_S3MII      0x0005
	
	/* Faraday only supports SGMII */
     #define BOOT_PARAMS_ETH_OPTIONS_SGMII      0x0006
     
     #define BOOT_PARAMS_ETH_OPTIONS_HD         0x0008
     #define BOOT_PARAMS_ETH_OPTIONS_SKIP_TX    0x0010
     #define BOOT_PARAMS_ETH_OPTIONS_SKIP_INIT  0x0020
     #define BOOT_PARAMS_ETH_OPTIONS_FC         0x0040
     
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

	 UINT16 sgmiiTx_Cfg_h;
	 UINT16 sgmiiTx_Cfg_l;
	 UINT16 sgmiiRx_Cfg_h;
	 UINT16 sgmiiRx_Cfg_l;
	 UINT16 sgmiiAux_Cfg_h;
	 UINT16 sgmiiAux_Cfg_l;
     
}   BOOT_PARAMS_ETHERNET_T;

/**************************************************************************************
 * Utopia boot options
 **************************************************************************************/
typedef struct boot_params_utopia_s{

    /* common portion of the Boot parameters */
    UINT16 length;
    UINT16 checksum;
    UINT16 boot_mode;
    UINT16 portNum;
    UINT16 swPll;         /* CPU PLL multiplier */ 
     
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
    UINT16 swPll;         /* CPU PLL multiplier */ 
    
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
    UINT16 swPll;         /* CPU PLL multiplier */ 
    
    /* Options */
    UINT16 options;
    
    #define BOOT_PARAMS_RIO_OPTIONS_TX_ENABLE  (1<<0)   /* set to enable transmit    */
    #define BOOT_PARAMS_RIO_OPTIONS_BOOT_TABLE (1<<1)   /* set to use boot tables    */
    #define BOOT_PARAMS_RIO_OPTIONS_NO_CONFIG  (1<<2)   /* set to bypass port config */
    
    UINT16 cfg_index;       /* General configuration index to use      */
    UINT16 node_id;         /* The node id for this device             */
    UINT16 serdes_ref_clk;  /* The serdes reference clock freq, in MHz */
    UINT16 link_rate;       /* Data link rate (mega bits per second    */
    UINT16 pf_low;          /* Packet forward range, low               */
    UINT16 pf_high;         /* Packet forward range, high              */
    
} BOOT_PARAMS_RIO_T;

/*
 * UNION of boot parameter structures in all modes
 * Note: We need to make sure that the structures genertaed by the C-compiler
 *       match with the boot parameter table data format i.e. a set of 16-bit
 *       data array.
 */ 
#define BOOT_PARAMS_SIZE_IN_BYTES       128
typedef union {
   BOOT_PARAMS_COMMON_T    common;  
   BOOT_PARAMS_ETHERNET_T  eth;
   BOOT_PARAMS_I2C_T       i2c;
   BOOT_PARAMS_UTOPIA_T    utopia;
   BOOT_PARAMS_RIO_T       rio;
   UINT16                  parameter[BOOT_PARAMS_SIZE_IN_BYTES/2]; 
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

#endif  /* __TIBOOT_H__ */

/* nothing past this point */
