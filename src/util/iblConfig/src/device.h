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


#define ibl_MAKE_VERSION(a,b,c,d)  ((a << 24) | (b << 16) | (c << 8) | (d << 0))


/**
 * @brief
 *  The version number, 1.0.0.11
 */
#define ibl_VERSION  ibl_MAKE_VERSION(1,0,0,11)

#define SETIP(array,i0,i1,i2,i3)      array[0]=(i0);  \
                                      array[1]=(i1);  \
                                      array[2]=(i2);  \
                                      array[3]=(i3)
/**
 * @defgroup iblBootModes  Defines the boot modes supported by the IBL
 *
 * @ingroup iblBootModes
 * @{
 *
 *  @def ibl_BOOT_MODE_TFTP */
#define ibl_BOOT_MODE_TFTP     10      /* Boot through a tftp interface */

 /* @def ibl_BOOT_MODE_NAND */
#define  ibl_BOOT_MODE_NAND     11      /* Boot through a nand interface */

/* @def  ibl_BOOT_MODE_NOR */
#define  ibl_BOOT_MODE_NOR      12      /* Boot through a nor (or flash) interface */

/* @def  ibl_BOOT_MODE_NONE */
#define  ibl_BOOT_MODE_NONE     13      /* Boot mode selection is inactive */

/* @} */

/**
 *  @brief
 *      Define the number of different boot modes which can be configured for
 *      a single execution of the IBL.
 */
#define ibl_N_BOOT_MODES        3

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
    char8   fileName[64];  /**< The file name to load */

} iblEthBootInfo_t;


/**
 * @def ibl_ETH_PORT_FROM_RBL
 */
#define ibl_ETH_PORT_FROM_RBL   -1  /**< The ethernet port used is the same one used
                                         during the ROM boot load process. */

/**
 *  @def ibl_PORT_SWITCH_ALL
 */
#define ibl_PORT_SWITCH_ALL     -2  /**< The ethernet subsystem is connected to a switch, and
                                         all ports on the subsystem should be configured */


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
    uint32  registerMask;               /**< Identifies which registers will be configured */
    uint32  sdRamConfig;                /**< SDRAM Config Register */
    uint32  sdRamConfig2;               /**< SDRAM Config2 Register */
    uint32  sdRamRefreshCtl;            /**< SDRAM Refresh Control Register */
    uint32  sdRamTiming1;               /**< SDRAM Timing 1 Register */
    uint32  sdRamTiming2;               /**< SDRAM Timing 2 Register */
    uint32  sdRamTiming3;               /**< SDRAM Timing 3 Register */
    uint32  lpDdrNvmTiming;             /**< LPDDR2-NVM Timing Register */
    uint32  powerManageCtl;             /**< Power Management Control Register */
    uint32  iODFTTestLogic;             /**< IODFT Test Logic Global Control Register */
    uint32  performCountCfg;            /**< Performance Counter Config Register */
    uint32  performCountMstRegSel;      /**< Performance Counter Master Region Select Register */
    uint32  readIdleCtl;                /**< Read Idle Control Register */
    uint32  sysVbusmIntEnSet;           /**< VBUSM Interrupt Enable Set Register */
    uint32  sdRamOutImpdedCalCfg;       /**< SDRAM Output Impedance Calibratin Config Register */
    uint32  tempAlterCfg;               /**< Temperature Alert Config Register */
    uint32  ddrPhyCtl1;                 /**< DDR PHY Control 1 Register */
    uint32  ddrPhyCtl2;                 /**< DDR PHY Control 2 Register */
    uint32  priClassSvceMap;            /**< DDR Priority to Class of Service Mapping Register */
    uint32  mstId2ClsSvce1Map;          /**< Master ID to Class of Service 1 Mapping Register */
    uint32  mstId2ClsSvce2Map;          /**< Master ID to Class of Service 2 Mapping Register */
    uint32  eccCtl;                     /**< ECC Control Register */
    uint32  eccRange1;                  /**< ECC Address Range 1 Register */
    uint32  eccRange2;                  /**< ECC Address Range 2 Register */
    uint32  rdWrtExcThresh;             /**< Read Write Execution Threshold Register */

} iblEmif4p0_t;


/**
 * @defgroup iblEmif4Select Defines the EMIF4 registers configured by IBL
 *
 * @ingroup iblEmif4Select
 * @{
 *    @def  ibl_EMIF4_ENABLE_sdRamConfig
 */
#define ibl_EMIF4_ENABLE_sdRamConfig                 (1 <<  0)

/** @def ibl_EMIF4_ENABLE_sdRamConfig2 */
#define  ibl_EMIF4_ENABLE_sdRamConfig2                (1 <<  1)

/** @def ibl_EMIF4_ENABLE_sdRamRefreshCtl */
#define  ibl_EMIF4_ENABLE_sdRamRefreshCtl             (1 <<  2)

/** @def ibl_EMIF4_ENABLE_sdRamTiming1 */
#define  ibl_EMIF4_ENABLE_sdRamTiming1                (1 <<  3)

/** @def ibl_EMIF4_ENABLE_sdRamTiming2 */
#define  ibl_EMIF4_ENABLE_sdRamTiming2                (1 <<  4)

/** @def ibl_EMIF4_ENABLE_sdRamTiming3 */
#define  ibl_EMIF4_ENABLE_sdRamTiming3                (1 <<  5)

/** @def ibl_EMIF4_ENABLE_lpDdrNvmTiming */
#define  ibl_EMIF4_ENABLE_lpDdrNvmTiming              (1 <<  6)

/** @def ibl_EMIF4_ENABLE_powerManageCtl */
#define  ibl_EMIF4_ENABLE_powerManageCtl              (1 <<  7)

/** @def ibl_EMIF4_ENABLE_iODFTTestLogic */
#define  ibl_EMIF4_ENABLE_iODFTTestLogic              (1 <<  8)

/** @def ibl_EMIF4_ENABLE_performCountCfg */
#define  ibl_EMIF4_ENABLE_performCountCfg             (1 <<  9)

/** @def ibl_EMIF4_ENABLE_performCountMstRegSel */
#define  ibl_EMIF4_ENABLE_performCountMstRegSel       (1 << 10)

/** @def ibl_EMIF4_ENABLE_readIdleCtl */
#define  ibl_EMIF4_ENABLE_readIdleCtl                 (1 << 11)

/** @def ibl_EMIF4_ENABLE_sysVbusmIntEnSet */
#define  ibl_EMIF4_ENABLE_sysVbusmIntEnSet            (1 << 12)

/** @def ibl_EMIF4_ENABLE_sdRamOutImpdedCalCfg */
#define  ibl_EMIF4_ENABLE_sdRamOutImpdedCalCfg        (1 << 13)

/** @def ibl_EMIF4_ENABLE_tempAlterCfg */
#define  ibl_EMIF4_ENABLE_tempAlterCfg                (1 << 14)

/** @def ibl_EMIF4_ENABLE_ddrPhyCtl1 */
#define  ibl_EMIF4_ENABLE_ddrPhyCtl1                  (1 << 15)

/** @def ibl_EMIF4_ENABLE_ddrPhyCtl2 */
#define  ibl_EMIF4_ENABLE_ddrPhyCtl2                  (1 << 16)

/** @def ibl_EMIF4_ENABLE_priClassSvceMap */
#define  ibl_EMIF4_ENABLE_priClassSvceMap             (1 << 17)

/** @def ibl_EMIF4_ENABLE_mstId2ClsSvce1Map */
#define  ibl_EMIF4_ENABLE_mstId2ClsSvce1Map           (1 << 18)

/** @def ibl_EMIF4_ENABLE_mstId2ClsSvce2Map */
#define  ibl_EMIF4_ENABLE_mstId2ClsSvce2Map           (1 << 11)

/** @def ibl_EMIF4_ENABLE_eccCtl */
#define  ibl_EMIF4_ENABLE_eccCtl                      (1 << 19)

/** @def ibl_EMIF4_ENABLE_eccRange1 */
#define  ibl_EMIF4_ENABLE_eccRange1                   (1 << 20)

/** @def ibl_EMIF4_ENABLE_eccRange2 */
#define  ibl_EMIF4_ENABLE_eccRange2                   (1 << 21)

/** @def ibl_EMIF4_ENABLE_rdWrtExcThresh */
#define  ibl_EMIF4_ENABLE_rdWrtExcThresh              (1 << 22)

/** @def ibl_BOOT_EMIF4_ENABLE_ALL */
#define  ibl_BOOT_EMIF4_ENABLE_ALL                    0x007fffff

/* @} */


/**
 * @defgroup iblEmifType Defines the EMIF4 type on a device
 *
 * @ingroup iblEmifType
 * @{
 */
/** @def ibl_EMIF_TYPE_31 */
#define  ibl_EMIF_TYPE_31           31

/** @def ibl_EMIF_TYPE_40 */
#define  ibl_EMIF_TYPE_40           40

/* @} */

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
    bool    configure;          /**< Set to false to disable configuration */
    uint32  adviseAbility;      /**< The advise ability register           */
    uint32  control;            /**< The control register                  */
    uint32  txConfig;           /**< Serdes Tx config                      */
    uint32  rxConfig;           /**< Serdes Rx config                      */
    uint32  auxConfig;          /**< Serdes Aux config                     */

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
 *  @def ibl_N_ECC_BYTES
 */
#define ibl_N_ECC_BYTES             10  /**< The number of ECC bytes to be computed for each page */

/**
 *  @def ibl_N_BAD_BLOCK_MARKER
 */
#define ibl_N_BAD_BLOCK_PAGE      2  /**< The number of pages in each block that has the bad block marker */

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

    uint8   eccBytesIdx[ibl_N_ECC_BYTES];
                                /**< Index of each ECC byte in each page data */
    uint8   badBlkMarkIdx[ibl_N_BAD_BLOCK_PAGE];
                                /**< Index of bad block marker in each page data */

    uint8   resetCommand;       /**< The command to reset the flash */
    uint8   readCommandPre;     /**< The read command sent before the address */
    uint8   readCommandPost;    /**< The read command sent after the address */
    bool    postCommand;        /**< If TRUE the post command is sent */

} nandDevInfo_t;


/**
 *  @def ibl_N_ENDIANS
 */
#define ibl_N_ENDIANS       2  /**< The number of endians supported */

#define ibl_ENDIAN_BIG      0  /**< Big endian */
#define ibl_ENDIAN_LITTLE   1  /**< Little endian */

/**
 *  @def ibl_N_IMAGES
 */
#define ibl_N_IMAGES        2  /**< The number of boot images supported on the same device */

/**
 *  @brief
 *      This structure is used to control the operation of the NAND boot
 *
 */
typedef struct iblNand_s
{

    int32    bootFormat;                                /**< The format of the boot data file. @ref iblBootFormats */
    uint32   bootAddress[ibl_N_ENDIANS][ibl_N_IMAGES];  /**< The start address of each image for booting */
    int32    interface;                                 /**< The nand interface @ref iblPmemf */
    iblBinBlob_t blob[ibl_N_ENDIANS][ibl_N_IMAGES];     /**< Used only if the format is ibl_BOOT_FORMAT_BBLOB */


    nandDevInfo_t nandInfo;     /** Low level device info */

} iblNand_t;

/**
 *  @brief
 *      Nor boot configuration.
 */
typedef struct iblNor_s
{
    int32   bootFormat;                                 /**<  The format of the boot data file. @ref iblBootFormats */
    uint32  bootAddress[ibl_N_ENDIANS][ibl_N_IMAGES];   /**<  The start address for booting */
    int32   interface;                                  /**<  The nor interface. @ref iblPmemIf */
    iblBinBlob_t blob[ibl_N_ENDIANS][ibl_N_IMAGES];     /**<  Used only if the format is ibl_BOOT_FORMAT_BBLOB */

} iblNor_t;

extern uint32 iblEndianIdx;
extern uint32 iblImageIdx;

/**
 * @defgroup iblPmemIf defines the interfaces used for NOR memory. Not all values are
 *           valid for all devices
 *
 * @ingroup iblPmemIf
 * @{
 */

/** @def ibl_PMEM_IF_GPIO - GPIO interface */
#define  ibl_PMEM_IF_GPIO         0

/** @def ibl_PMEM_IF_CHIPSEL_2 */
#define  ibl_PMEM_IF_CHIPSEL_2    2   /* EMIF interface using chip select 2, no wait enabled */

/** @def ibl_PMEM_IF_CHIPSEL_3 */
#define  ibl_PMEM_IF_CHIPSEL_3    3   /* EMIF interface using chip select 3, no wait enabled */

/** @def ibl_PMEM_IF_CHIPSEL_4 */
#define  ibl_PMEM_IF_CHIPSEL_4    4   /* EMIF interface using chip select 4 */

/** @def ibl_PMEM_IF_CHIPSEL_5 */
#define  ibl_PMEM_IF_CHIPSEL_5    5   /* EMIF interface using chip select 5 */

/** @def ibl_PMEM_IF_SPI */
#define  ibl_PMEM_IF_SPI          100 /* Interface through SPI */

/* @} */


/**
 *  @brief
 *      EMIF (nand/nor) configuration
 */
typedef struct iblEmif_s {

    int16  csSpace;           /**< Chip select space, @ref iblPmemIf */
    int16  busWidth;          /**< Bus width, bits */
    bool   waitEnable;        /**< Valid only for NOR devices */

} iblEmif_t;

/**
 *  @brief
 *      The maximum number of chip select spaces for emif boot (not ddr) configuration
 */
#define ibl_MAX_EMIF_PMEM   2


/**
 *  @brief
 *      SPI configuration used for either NOR or NAND
 */
typedef struct iblSpi_s
{
    int16  addrWidth;       /**<  16 or 24 are the only valid values */
    int16  nPins;           /**<  4 or 5 are the only valid values */
    int16  mode;            /**<  Clock / data polarities (valid values 0-3) */
    int16  csel;            /**<  Chip select value (5 pin). Only 0b10 and 0b01 are valid */
    uint16 c2tdelay;        /**<  Setup time between chip select and the transaction */
    uint16 busFreqMHz;      /**<  Bus speed */

} iblSpi_t;



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
 *  @def iblBoot_t
 *      Configures an ibl boot attempt
 *
 *  @details
 *      The ibl allows for the configuration for multiple boot attempts. This structure is
 *      used to configure the ibl boot attempt.
 */
typedef struct iblBoot_s
{

    int32   bootMode;           /**< Identifies the boot mode @ref iblBootModes */

    uint32  priority;           /**< The boot priority. @ref iblPeriphPriority */
    int32   port;               /**< The port to use, or @ref ibl_PORT_FROM_RBL */

    union  {

        iblEth_t   ethBoot;      /**< Ethernet boot configuration. @ref iblEth_t */

        iblNand_t  nandBoot;     /**< NAND boot configuration @ref iblNand_t */

        iblNor_t   norBoot;      /**< NOR boot configuration  @ref iblNor_t */

    } u;

} iblBoot_t;


/**
 * @def ibl_MAGIC_VALUE
 */
#define ibl_MAGIC_VALUE  0xCEC11EBC  /**< Indicates that the configuration table is valid */

/**
 * @def ibl_EVM_TYPE
 */
#define ibl_EVM_C6455L  0x10  	/**< C6455 Low Cost EVM */
#define ibl_EVM_C6457L  0x20  	/**< C6457 Low Cost EVM */
#define ibl_EVM_C6472L  0x30  	/**< C6472 Low Cost EVM */
#define ibl_EVM_C6474L  0x40  	/**< C6474 Low Cost EVM */
#define ibl_EVM_C6474M  0x41  	/**< C6474 Mez EVM */
#define ibl_EVM_C6670L  0x50  	/**< C6670 Low Cost EVM */
#define ibl_EVM_C6678L  0x60  	/**< C6678 Low Cost EVM */
#define ibl_EVM_C6657L  0x70	/**< C6657 Low Cost EVM */

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
    uint32     iblMagic;                      /**< @ref ibl_MAGIC_VALUE */

    iblPll_t   pllConfig[ibl_N_PLL_CFGS];     /**< PLL Configuration. @ref iblPll_t */

    iblDdr_t   ddrConfig;                     /**< DDR configuration @ref iblDdr_t  */

    iblSgmii_t sgmiiConfig[ibl_N_ETH_PORTS];  /**< SGMII boot configuration. @ref iblSgmii_t */

    iblMdio_t  mdioConfig;                    /**< MDIO configuration. @ref iblMdio_t */

    iblSpi_t   spiConfig;                     /**< SPI configuration @ref iblSpi_s */

    iblEmif_t  emifConfig[ibl_MAX_EMIF_PMEM]; /**< EMIF (nand/nor, not ddr) configuration. @ref iblEmif_t */

    iblBoot_t  bootModes[ibl_N_BOOT_MODES];   /**< Boot configuration */

    uint16     iblEvmType;                    /**< @ref ibl_EVM_TYPE */

    uint16     chkSum;                        /**< Ones complement checksum over the whole config structure */

} ibl_t;


extern ibl_t ibl;


/**
 * @defgroup iblActiveDevice
 *
 * @ingroup iblActiveDevice
 * @{
 *    @def  ibl_ACTIVE_DEVICE_ETH
 */
#define ibl_ACTIVE_DEVICE_ETH     100     /**< Data received through an ethernet interface */

/**
 *  @def ibl_ACTIVE_DEVICE_EMIF
 */
#define ibl_ACTIVE_DEVICE_EMIF    101     /**< Data read through an EMIF interface */

/**
 *  @def ibl_ACTIVE_DEVICE_I2C
 */
#define ibl_ACTIVE_DEVICE_I2C     102     /**< Data read through an I2C interface */

/**
 *  @def ibl_ACTIVE_DEVICE_SPI
 */
#define ibl_ACTIVE_DEVICE_SPI     103     /**< Data read through an SPI interface */

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

/**
 *  @def ibl_FAIL_CODE_PA
 */
#define ibl_FAIL_CODE_PA                    702     /**< Packet Accelerator setup failed */


/**
 *  @def ibl_FAIL_CODE_SPI_PARAMS
 */
#define ibl_FAIL_CODE_SPI_PARAMS            703     /**< Invalid SPI configuration found */

/**
 *  @def ibl_FAIL_CODE_INVALID_INIT_DEVICE
 */
#define ibl_FAIL_CODE_INVALID_INIT_DEVICE   704     /**< Second stage boot device specified is invalid */

/**
 *  @def ibl_FAIL_CODE_INVALID_SPI_ADDRESS
 */
#define ibl_FAIL_CODE_INVALID_SPI_ADDRESS   705     /**< Invalid data address specified on SPI */

/**
 *  @def ibl_FAIL_CODE_PERIPH_POWER_UP
 */
#define ibl_FAIL_CODE_PERIPH_POWER_UP       706     /**< Boot peripheral failed to power up */

/**
 *  @def ibl_FAIL_CODE_INVALID_NAND_PERIPH
 */
#define ibl_FAIL_CODE_INVALID_NAND_PERIPH   707     /**< Invalid nand boot peripheral specified */

/**
 *  @def ibl_FAIL_CODE_NO_EMIF_CFG
 */
#define ibl_FAIL_CODE_NO_EMIF_CFG           708     /**< No emif configuration found to match specified cs space */

/**
 *  @def ibl_FAIL_CODE_EMIF_CFG_FAIL
 */
#define ibl_FAIL_CODE_EMIF_CFG_FAIL         709     /**< Hardware setup of emif failed */

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

    uint32 iblVersion;      /**<  The version number. MSB = major, SMSB = minor, SLSB = minor minor LSB= tiny */

    uint32 iblFail;         /**<  If non-zero the IBL has encountered a fatal error */

    uint32 i2cRetries;      /**<  Count of I2C read retries */
    uint32 i2cDataRetries;  /**<  Number of retries while reading block data from the i2c */

    uint32 spiRetries;      /**<  Count of SPI read retries */
    uint32 spiDataRetries;  /**<  Number of retries while reading block data from the spi */

    uint32 magicRetries;    /**<  Count of I2C/SPI re-reads because the magic number was incorrect */
    uint32 mapSizeFail;     /**<  Number of times an invalid map table size was read from the i2c/spi */
    uint32 mapRetries;      /**<  Number of times the checksum failed on the read of the i2c/spi map */

    int32  heartBeat;       /**<  An increasing value as long as the boot code is running */

    int32  activeBoot;        /**<  Describes the active boot mode @ref iblBootModes */
    int32  activeDevice;      /**<  Describes the active boot peripheral device @ref iblActiveDevice */
    int32  activeFileFormat;  /**<  Describes the format being decoded */

    uint32  autoDetectFailCnt;      /**<  Counts the number of times an auto detect of the data format failed */
    uint32  nameDetectFailCnt;      /**<  Counts the number of times an name detect of the data format failed */

    uint32 invalidDataFormatSpec;   /**<  Counts the number of times the main boot found an invalid boot format request */

    uint32 exitAddress;             /**<  If non-zero the IBL exited and branched to this address */

    iblEthBootInfo_t ethParams;     /**<  Last ethernet boot attemp parameters */

} iblStatus_t;

extern iblStatus_t iblStatus;


/**
 *  @brief
 *      The ibl boot map structure
 *
 *  @details
 *      The ibl boot device contains a structure which identifies the location of the big and little
 *      endian ibl images on the external device.
 */
typedef struct iblBootMap_s
{
    uint16  length;         /**<  Size of the structure in bytes */
    uint16  chkSum;         /**<  Value which makes the ones complement checksum over the block equal to 0 or -0 */

    uint32  addrLe;         /**<  Base address of the boot tables for the little endian image */
    uint32  configLe;       /**<  Base address of the ibl structure for use with the little endian image */

    uint32  addrBe;         /**<  Base address of the boot tables for the big endian image */
    uint32  configBe;       /**<  Base address of the ibl structure for use with the big endian image */

} iblBootMap_t;


ibl_t c6455_ibl_config(void);
ibl_t c6474_ibl_config(void);
ibl_t c6474l_ibl_config(void);
ibl_t c6457_ibl_config(void);
ibl_t c6472_ibl_config(void);
ibl_t c6678_ibl_config(void);
ibl_t c6670_ibl_config(void);
ibl_t c6657_ibl_config(void);




#endif /* IBL_H */
