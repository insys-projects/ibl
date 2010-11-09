/********************************************************************************************************
 * FILE PURPOSE: IBL configuration and control definitions
 ********************************************************************************************************
 * FILE NAME: ibl.h
 *
 * DESCRIPTION: Defines the data structure used to handle initial configuration and control
 * 				of the ibl. This data structure resides at a fixed location in the device memory
 *				map. It is initially populated either during the rom boot. The table can be
 *				over-written during the ibl process to redirect the boot. For example the ibl
 *				can initially load from an i2c which repopulates this table with parameters
 *				for an ethernet boot.
 *
 *  @file  ibl.h
 *
 *  @brief
 *      This file defines the configuration and control of the IBL
 *  
 *
 ********************************************************************************************************/
#ifndef IBL_H
#define IBL_H

#include "types.h"

/* Information used to make generate a bootp request */
/**
 * @brief
 *    Defines parameters used for making a bootp request
 *
 * @details
 *    The bootp request parameters are created from these fields
 */
typedef struct iblBootp_s
{
    uint8   hwAddress[6]; /**< The hardware (mac) address of this device. If set to 0
                               the ibl will values from e-fuse */
    
    uint8   ipDest[4];    /**< The IP address of this device. This is typically set
                               to IP broadcast */
    
} iblBootp_t;


/**
 * @brief
 *   This structure contains information used for tftp boot. 
 *
 * @details These fields are typically filled in by the bootp packet, but 
 *          can be provided if bootp will not be used.
 */
typedef struct iblEthBootInfo_s
{
    uint8   ipAddr[4];      /**< The IP address of this device */
    uint8   serverIp[4];    /**< The IP address of the tftp server */
    uint8   gatewayIp[4];   /**< The IP address of the gateway */
    uint8   netmask[4];     /**< The IP netmask */
    uint8   hwAddress[6];   /**< The hardware (mac) address of this device */
    char8   fileName[128];  /**< The file name to load */

} iblEthBootInfo_t;
    
    
/**
 * @def ibl_ETH_PORT_FROM_RBL
 */
#define ibl_ETH_PORT_FROM_RBL   -1  /**< The ethernet port used is the same one used 
                                         during the ROM boot load process. */


/**
 * @defgroup iblBootFormats
 *
 * @ingroup iblBootFormats
 * @{
 */ 
#define ibl_BOOT_FORMAT_AUTO    0   /**< Auto determine the boot format from the data */
#define ibl_BOOT_FORMAT_NAME    1   /**< Determines the boot format based on file name (bootp/tftp only) */
#define ibl_BOOT_FORMAT_BIS     2   /**< Boot TI AIS format */
#define ibl_BOOT_FORMAT_COFF    3   /**< Boot a COFF file */
#define ibl_BOOT_FORMAT_ELF     4   /**< Boot an ELF file */
#define ibl_BOOT_FORMAT_BBLOB   5   /**< Boot a binary blob */
#define ibl_BOOT_FORMAT_BTBL    6   /**< Boot a TI boot table file */

/* @} */
 
/**
 * @defgroup iblPeriphPriority  Defines the boot sequence
 *
 * @ingroup iblPeriphPriority
 * @{
 *    @def  ibl_LOWEST_PRIORITY
 */
#define ibl_LOWEST_PRIORITY     10  /**< The lowest priority assignable to a peripheral for boot */

/**
 *    @def  ibl_HIGHEST_PRIORITY
 */
#define ibl_HIGHEST_PRIORITY     1  /**< The highest priority assignable to a peripheral for boot */

/**
 *    @def  ibl_DEVICE_NOBOOT
 */
#define ibl_DEVICE_NOBOOT       20  /**< Indicates that the device is not to be used for boot */

/* @} */  


/**
 *  @brief
 *      Emif controller 3.1 configuration
 *
 *  @details
 *      The paramters are directly placed into the emif controller
 */
typedef struct iblEmif3p1_s
{
    uint32 sdcfg;           /**< SD configuration register */
    uint32 sdrfc;           /**< Refresh timing register   */
    uint32 sdtim1;          /**< DDR timing register 1 */
    uint32 sdtim2;          /**< DDR timing register 2 */
    uint32 dmcctl;          /**< CAS match timing */
    
} iblEmif3p1_t;


/**
 *  @brief
 *      Emif controller 4.0 configuration
 *
 *  @details
 *      The parameters are placed directly into the emif controller
 */
typedef struct iblEmif4p0_s
{
    uint32 dummy;           /**< placeholder */
   
} iblEmif4p0_t;

/**
 * @brief
 *    This structure is used to configure the DDR interface
 *
 * @details
 *    The DDR configuration parameters are setup
 *
 */
typedef struct idblDdr_s
{
    bool configDdr;                  /**<  Set to non-zero to enable EMIF configuration */
    
    union  {
    
        iblEmif3p1_t  emif3p1;       /**<  Configuration of devices with emif controller version 3.1 */
        iblEmif4p0_t  emif4p0;       /**<  Configuration of devices with emif controller version 4.0 */
    } uEmif;

} iblDdr_t;

/**
 *  @brief
 *      This structure is used to identify binary blob load parameters.
 *
 *  @details
 *      Since binary blob is formatless the start address, size and branch to address
 *      can be specified. In the case of network boot, boot will terminate when no
 *      more data is received (or timed out), even if the size is not reached.
 */
typedef struct iblBinBlob_s
{
    uint32   startAddress;          /**< Where the loaded data is placed */
    uint32   sizeBytes;             /**< How much data to load */
    uint32   branchAddress;         /**< Where to branch to when the load is complete */

} iblBinBlob_t;

/**
 * @brief
 *   This structure is used to control the operation of the ibl ethernet boot.
 *
 * @details
 *   The ethernet port and bootp request are controlled through this structure.
 */
typedef struct iblEth_s
{
    uint32   ethPriority;       /**< The ethernet boot priority. @ref iblPeriphPriority */
    int32    port;              /**< The ethernet port to use, or @ref ibl_ETH_PORT_FROM_RBL */
    bool     doBootp;           /**< If true a bootp request is generated. If false the @ref iblEthBootInfo_t
                                     table must be populated before the ibl begins execution */
    bool     useBootpServerIp;  /**< If TRUE then the server IP received from the bootp server
                                     is used, if FALSE the one in the ethInfo field is used */
    bool     useBootpFileName;  /**< If TRUE then the file name received from the bootp server
                                     is used, if FALSE the one in the ethInfo field is used */
    int32    bootFormat;        /**< The format of the boot data file. @ref iblBootFormats */                            
    
    iblBinBlob_t blob;          /**< Used only if the format is ibl_BOOT_FORMAT_BBLOB */
    
    iblEthBootInfo_t  ethInfo;  /**< Low level ethernet information */
    
} iblEth_t;


/**
 *  @brief
 *      This structure is used to control the operation of the ibl sgmii ports
 *
 *  @details
 *      The physical register configuration is provided
 */
typedef struct iblSgmii_s
{
    uint32  adviseAbility;      /**< The advise ability register */
    uint32  control;            /**< The control register        */
    uint32  txConfig;           /**< Serdes Tx config            */
    uint32  rxConfig;           /**< Serdes Rx config            */
    uint32  auxConfig;          /**< Serdes Aux config           */
  
} iblSgmii_t;


/**
 *  @def ibl_N_ETH_PORTS
 */
#define ibl_N_ETH_PORTS     2  /**< The number of ethernet port configurations available */

/**
 *  @def ibl_N_MDIO_CFGS
 */
#define ibl_N_MDIO_CFGS     16  /**< The maximum number of mdio configurations */
 

/**
 * @brief
 *      This structure is used to configure phys through the mdio interface
 *
 * @details
 *      Defines optional configuration through MDIO.
 *
 *      The mdio transaction values are mapped as follows:
 *
 *      /-------------------------------------------------------------\
 *      |  31  |  30   | 29  26 | 25      21 | 20      16 | 15       0|
 *      | rsvd | write |  rsvd  |  register  |  phy addr  |     data  |
 *      \-------------------------------------------------------------/
 */
typedef struct iblMdio_s
{
    int16  nMdioOps;         /**< The number of mdio writes to perform  */
    uint16 mdioClkDiv;       /**< The divide down of the mac clock which drives the mdio */
    
    uint32 interDelay;       /**< The number of cpu cycles to wait between mdio writes */
    
    uint32 mdio[ibl_N_MDIO_CFGS];   /* The MDIO transactions */

} iblMdio_t;


/** 
 *  @brief
 *      This structure defines the physical parameters of the NAND device
 */
typedef struct nandDevInfo_s
{
    uint32  busWidthBits;       /**< 8 or 16 bit bus width */
    uint32  pageSizeBytes;      /**< The size of each page */
    uint32  pageEccBytes;       /**< Number of ecc bytes in each page */
    uint32  pagesPerBlock;      /**< The number of pages in each block */
    uint32  totalBlocks;        /**< The total number of blocks in a device */
    
    uint32  addressBytes;       /**< Number of bytes in the address */
    bool    lsbFirst;           /**< Set to true if the LSB is output first, otherwise msb is first */
    uint32  blockOffset;        /**< Address bits which specify the block number */
    uint32  pageOffset;         /**< Address bits which specify the page number */
    uint32  columnOffset;       /**< Address bits which specify the column number */
    
    uint8   resetCommand;       /**< The command to reset the flash */
    uint8   readCommandPre;     /**< The read command sent before the address */
    uint8   readCommandPost;    /**< The read command sent after the address */
    bool    postCommand;        /**< If TRUE the post command is sent */
    
} nandDevInfo_t;
    
    
/**
 *  @brief
 *      This structure is used to control the operation of the NAND boot
 *
 */
typedef struct iblNand_s
{

    uint32   nandPriority;      /**< The nand boot priority. @ref iblPeriphPriority */
    int32    bootFormat;        /**< The format of the boot data file. @ref iblBootFormats */                            
    iblBinBlob_t blob;          /**< Used only if the format is ibl_BOOT_FORMAT_BBLOB */
    
    
    nandDevInfo_t nandInfo;     /** Low level device info */

} iblNand_t;
    
    
/**
 *  @brief
 *      This structure is used to control the programming of the device PLL
 *
 *  @details
 *      The system PLLs are optionally configured
 */
typedef struct iblPll_s  {

    bool    doEnable;       /**< If true the PLL is configured */
    
    Uint32  prediv;         /**< The pll pre-divisor */
    Uint32  mult;           /**< The pll multiplier */
    Uint32  postdiv;        /**< The pll post divider */
    
    Uint32  pllOutFreqMhz;  /**<  The resulting output frequency, required for timer setup */ 
    
} iblPll_t;


/** 
 *  @defgroup iblPllNum
 * 
 *  @ingroup iblPllNum
 *  @{
 * 
 *  @def ibl_MAIN_PLL
 */
#define ibl_MAIN_PLL    0  /**< The main cpu pll */

/**
 * @def ibl_DDR_PLL
 */
#define ibl_DDR_PLL     1  /**< The ddr pll */

/**
 * @def ibl_NET_PLL
 */
#define ibl_NET_PLL     2  /**< The network pll */

/**
 * @def the number of PLL configuration entries in the table
 */
#define ibl_N_PLL_CFGS  (ibl_NET_PLL + 1)

/* @} */
    



/**
 * @def ibl_MAGIC_VALUE
 */
#define ibl_MAGIC_VALUE  0xCEC11EBB  /**< Indicates that the configuration table is valid */
    
/**
 *  @brief
 *    The main configuration/control structure for the ibl
 *
 *  @details
 *    The operation of the ibl is configured/controlled based on the values in this structure. 
 *    This structure resides at a fixed location in the memory map. It can be changed during
 *    the boot operation itself by loading new values into it, but these changes must occur
 *    as part of the boot process itself (not through an asynchronous write through a master
 *    peripheral).
 *
 *    Each boot mode is assigned a priority, with lower values indicating a higher 
 *    priority. The lowest valid priority is @ref ibl_LOWEST_BOOT_PRIORITY, and the value
 *    @ref ibl_DEVICE_NOBOOT indicates no boot will be attempted on that peripheral. 
 */
typedef struct ibl_s
{
    uint32   iblMagic;                       /**< @ref ibl_MAGIC_VALUE */
    
    iblPll_t  pllConfig[ibl_N_PLL_CFGS];     /**< PLL Configuration. @ref iblPll_t */
    
    iblDdr_t  ddrConfig;                     /**< DDR configuration @ref iblDdr_t  */
    
    iblEth_t  ethConfig[ibl_N_ETH_PORTS];    /**< Ethernet boot configuration. @ref iblEth_t */
    
    iblSgmii_t sgmiiConfig[ibl_N_ETH_PORTS]; /**< SGMII boot configuration. @ref iblSgmii_t */
    
    iblMdio_t mdioConfig;                    /**< MDIO configuration. @ref iblMdio_t */
    
    iblNand_t nandConfig;                    /**< NAND configuration @ref iblNand_t */
    
    uint16    chkSum;                        /**< Ones complement checksum over the whole config structure */
    
    
/*    iblI2c_t  i2cConfig;  */
/*    iblSpi_t  spiConfig;  */
    
     
} ibl_t;


extern ibl_t ibl;



/**
 * @defgroup iblActivePeriph
 *
 * @ingroup iblActivePeriph
 * @{
 *    @def  ibl_ACTIVE_PERIPH_ETH
 */
#define ibl_ACTIVE_PERIPH_ETH     100     /**< An ethernet boot in progress */

/**
 *  @def ibl_ACTIVE_PERIPH_NAND
 */
#define ibl_ACTIVE_PERIPH_NAND    101     /**< A nand boot in progress */

/**
 *  @def ibl_ACTIVE_PERIPH_I2C
 */
#define ibl_ACTIVE_PERIPH_I2C     102     /**< An i2c boot in progress */

/**
 *  @def ibl_ACTIVE_PERIPH_SPI
 */
#define ibl_ACTIVE_PERIPH_SPI     103     /**< An SPI boot in progress */

/* @} */


/**
 *  @defgroup iblFailCode
 *
 * @ingroup iblFailCode
 * @{
 *      @def ibl_FAIL_CODE_INVALID_I2C_ADDRESS
 */
#define ibl_FAIL_CODE_INVALID_I2C_ADDRESS  700      /**< Invalid i2c eeprom address encountered */
 
/**
 *  @def ibl_FAIL_CODE_BTBL_FAIL
 */
#define ibl_FAIL_CODE_BTBL_FAIL             701     /**< Boot table processing function error */

 
 /* @} */


/**
 * @brief
 *   Provide status on the boot operation
 *
 * @details
 *  Run time status of the IBL is provided to aid in debugging
 *
 */
typedef struct iblStatus_s
{
    uint32 iblMagic;        /**<  The @ref ibl_MAGIC_VALUE is placed here to indicate the boot has begun */
    
    uint32 iblFail;         /**<  If non-zero the IBL has encountered a fatal error */
    
    uint32 i2cRetries;      /**<  Count of I2C read retries */
    uint32 magicRetries;    /**<  Count of I2C re-reads because the magic number was incorrect */ 
    uint32 mapSizeFail;     /**<  Number of times an invalid map table size was read from the i2c */
    uint32 mapRetries;      /**<  Number of times the checksum failed on the read of the i2c map */
    uint32 i2cDataRetries;  /**<  Number of retries while reading block data from the i2c */
    
    int32  tableLoadFail;   /**<  If non-zero then the load of the parameter table from i2c failed */
    
    int32  heartBeat;       /**<  An increasing value as long as the boot code is running */
    int32  noMagic;         /**<  A non-zero value here indicates that @ref ibl_MAGIC_VALUE was not found
                                  in the @ref ibl_t magic field, and default values were loaded. */
    int32  activePeriph;    /**<  Describes the active boot peripheral @ref iblActivePeriph */
    int32  activeFormat;    /**<  Describes the format being decoded */
    
    int32  autoDetectFailCnt;       /**<  Counts the number of times an auto detect of the data format failed */
    int32  nameDetectFailCnt;       /**<  Counts the number of times an name detect of the data format failed */
    
    int32 invalidDataFormatSpec;    /**<  Counts the number of times the main boot found an invalid boot format request */
    
    uint32 exitAddress;             /**<  If non-zero the IBL exited and branched to this address */
    
    iblEthBootInfo_t ethParams;     /**<  Last ethernet boot attemp parameters */
    
} iblStatus_t;                               
                               
extern iblStatus_t iblStatus;                               


/** 
 *  @brief
 *      The i2c map structure
 *
 *  @details 
 *      The i2c eeprom contains a structure which identifies the location of the big and little
 *      endian ibl images on the eeprom.
 */
typedef struct iblI2cMap_s 
{
    uint16  length;         /**<  Size of the structure in bytes */
    uint16  chkSum;         /**<  Value which makes the ones complement checksum over the block equal to 0 or -0 */
    
    uint32  addrLe;         /**<  Base address of the boot tables for the little endian image */
    uint32  configLe;       /**<  Base address of the ibl structure for use with the little endian image */
    
    uint32  addrBe;         /**<  Base address of the boot tables for the big endian image */
    uint32  configBe;       /**<  Base address of the ibl structure for use with the big endian image */

} iblI2cMap_t;








#endif /* IBL_H */
