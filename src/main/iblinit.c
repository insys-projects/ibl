/**
 *  @file iblinit.c
 *
 *  @brief
 *		This file contains code which runs prior to loading the full IBL
 *
 *  @details
 *		The IBL loads itself in a two stage process. The ROM boot loader
 *		loads this first stage IBL first. This entire program must be
 *		endian independent in execution.
 *
 *      This first loader reads the IBL parameters, and will endian
 *      switch them if required. The PLL is configured if indicated
 *      by the parameters.
 *
 *      The I2C block which contains the I2C EEPROM address for both
 *      the big and little endian images is then read. Based on the
 *      endianness of the device the rest of the IBL is read from
 *      the I2C EEPROM, and execution is transferred to the full
 *      IBL.
 *
 *      The subsequent reads are allowed to cross 16 bit i2c EEPROM
 *      addresses. When the boundary is crossed the i2c address
 *      field is incremented.
 *
 */

#include "ibl.h"
#include "iblloc.h"
#include "iblcfg.h"
#include "device.h"
#include "iblbtbl.h"
#include "i2c.h"
#include <string.h>


/**
 *  @brief
 *      Data structures shared between the 1st and 2nd stage IBL load
 *      are declared in a single header file, included in both stages
 */
#include "iblStage.h"

/**
 *  @brief
 *      byte swapping of i2c data must be done when in little endian mode
 */
bool littleEndian;

/**
 *  @brief
 *      The boot table processing status is declared in the boot table wrapper,
 *      and used here in the main status fields.
 */
extern Int32 btblWrapEcode;

/**
 *  @brief
 *      The malloc function used for both boot stages of the ibl
 */
void *iblMalloc (Uint32 size)
{
    return (malloc (size));
}

/**
 *  @brief
 *      The free function used for both stages of the ibl
 */
void iblFree (void *mem)
{
    free (mem);
}

/**
 *  @brief
 *      The memset function used for both stages of the ibl
 */
void *iblMemset (void *mem, Int32 ch, Uint32 n)
{
    return (memset (mem, ch, n));
}

/**
 *  @brief
 *      The memcpy function used for both stages of the ibl
 */
void *iblMemcpy (void *s1, const void *s2, Uint32 n)
{
    return (memcpy (s1, s2, n));

}

/**
 *  @brief
 *      Ones complement addition
 */
inline uint16 onesComplementAdd (uint16 value1, uint16 value2)
{
  uint32 result;

  result = (uint32)value1 + (uint32)value2;

  result = (result >> 16) + (result & 0xFFFF); /* add in carry   */
  result += (result >> 16);                    /* maybe one more */
  return ((uint16)result);
}


/**
 *  @brief
 *      Ones complement checksum computation 
 */
uint16 onesComplementChksum (uint16 * restrict p_data, uint16 len)
{
  uint16 chksum = 0;

  while (len > 0)
  {
    chksum = onesComplementAdd(chksum, *p_data);
    p_data++;
    len--;
  }
  return (chksum);
} 



/**
 *  @brief
 *      Do a 4 byte endian swap
 */
uint32 swap32val (uint32 v)
{
    v =  (((v >> 24) & 0xff) <<  0)  |
         (((v >> 16) & 0xff) <<  8)  |
         (((v >>  8) & 0xff) << 16)  |
         (((v >>  0) & 0xff) << 24);

    return (v);

}

/**
 *  @brief
 *      Do a 2 byte endian swap
 */
uint16 swap16val (uint16 v)
{
    v = (((v >> 8) & 0xff) << 0)  |
        (((v >> 0) & 0xff) << 8);

    return (v);

}

/**
 *  @brief
 *  Do an endian swap on the ibl structure
 */
void iblSwap (void)
{
    int i;

    ibl.iblMagic = swap32val (ibl.iblMagic);

    for (i = 0; i < ibl_N_PLL_CFGS; i++)  {
        ibl.pllConfig[i].doEnable      = swap16val (ibl.pllConfig[i].doEnable);
        ibl.pllConfig[i].prediv        = swap32val (ibl.pllConfig[i].prediv);
        ibl.pllConfig[i].mult          = swap32val (ibl.pllConfig[i].mult);
        ibl.pllConfig[i].postdiv       = swap32val (ibl.pllConfig[i].postdiv);
        ibl.pllConfig[i].pllOutFreqMhz = swap32val (ibl.pllConfig[i].pllOutFreqMhz);
    }

    ibl.ddrConfig.configDdr = swap16val (ibl.ddrConfig.configDdr);

    ibl.ddrConfig.uEmif.emif3p1.sdcfg  = swap32val(ibl.ddrConfig.uEmif.emif3p1.sdcfg);
    ibl.ddrConfig.uEmif.emif3p1.sdrfc  = swap32val(ibl.ddrConfig.uEmif.emif3p1.sdrfc);
    ibl.ddrConfig.uEmif.emif3p1.sdtim1 = swap32val(ibl.ddrConfig.uEmif.emif3p1.sdtim1);
    ibl.ddrConfig.uEmif.emif3p1.sdtim2 = swap32val(ibl.ddrConfig.uEmif.emif3p1.sdtim2);
    ibl.ddrConfig.uEmif.emif3p1.dmcctl = swap32val(ibl.ddrConfig.uEmif.emif3p1.dmcctl);

    for (i = 0; i < ibl_N_ETH_PORTS; i++)  {
        ibl.ethConfig[i].ethPriority        = swap32val (ibl.ethConfig[i].ethPriority);
        ibl.ethConfig[i].port               = swap32val (ibl.ethConfig[i].port);
        ibl.ethConfig[i].doBootp            = swap16val (ibl.ethConfig[i].doBootp);
        ibl.ethConfig[i].useBootpServerIp   = swap16val (ibl.ethConfig[i].useBootpServerIp);
        ibl.ethConfig[i].useBootpFileName   = swap16val (ibl.ethConfig[i].useBootpFileName);
        ibl.ethConfig[i].bootFormat         = swap32val (ibl.ethConfig[i].bootFormat);
        ibl.ethConfig[i].blob.startAddress  = swap32val (ibl.ethConfig[i].blob.startAddress);
        ibl.ethConfig[i].blob.sizeBytes     = swap32val (ibl.ethConfig[i].blob.sizeBytes);
        ibl.ethConfig[i].blob.branchAddress = swap32val (ibl.ethConfig[i].blob.branchAddress);

        ibl.sgmiiConfig[i].adviseAbility = swap32val (ibl.sgmiiConfig[i].adviseAbility);
        ibl.sgmiiConfig[i].control       = swap32val (ibl.sgmiiConfig[i].control);
        ibl.sgmiiConfig[i].txConfig      = swap32val (ibl.sgmiiConfig[i].txConfig);
        ibl.sgmiiConfig[i].rxConfig      = swap32val (ibl.sgmiiConfig[i].rxConfig);
        ibl.sgmiiConfig[i].auxConfig     = swap32val (ibl.sgmiiConfig[i].auxConfig);
    }

    ibl.mdioConfig.nMdioOps   = swap16val (ibl.mdioConfig.nMdioOps);
    ibl.mdioConfig.mdioClkDiv = swap16val (ibl.mdioConfig.mdioClkDiv);
    ibl.mdioConfig.interDelay = swap32val (ibl.mdioConfig.interDelay);

    for (i = 0; i < ibl_N_MDIO_CFGS; i++)
        ibl.mdioConfig.mdio[i] = swap32val (ibl.mdioConfig.mdio[i]);

    ibl.nandConfig.nandPriority       = swap32val (ibl.nandConfig.nandPriority);
    ibl.nandConfig.bootFormat         = swap32val (ibl.nandConfig.bootFormat);
    ibl.nandConfig.blob.startAddress  = swap32val (ibl.nandConfig.blob.startAddress);
    ibl.nandConfig.blob.sizeBytes     = swap32val (ibl.nandConfig.blob.sizeBytes);
    ibl.nandConfig.blob.branchAddress = swap32val (ibl.nandConfig.blob.branchAddress);

    ibl.nandConfig.nandInfo.busWidthBits  = swap32val (ibl.nandConfig.nandInfo.busWidthBits);
    ibl.nandConfig.nandInfo.pageSizeBytes = swap32val (ibl.nandConfig.nandInfo.pageSizeBytes);
    ibl.nandConfig.nandInfo.pageEccBytes  = swap32val (ibl.nandConfig.nandInfo.pageEccBytes);
    ibl.nandConfig.nandInfo.pagesPerBlock = swap32val (ibl.nandConfig.nandInfo.pagesPerBlock);
    ibl.nandConfig.nandInfo.totalBlocks   = swap32val (ibl.nandConfig.nandInfo.totalBlocks);
    ibl.nandConfig.nandInfo.addressBytes  = swap32val (ibl.nandConfig.nandInfo.addressBytes);
    ibl.nandConfig.nandInfo.lsbFirst      = swap16val (ibl.nandConfig.nandInfo.lsbFirst);
    ibl.nandConfig.nandInfo.blockOffset   = swap32val (ibl.nandConfig.nandInfo.blockOffset);
    ibl.nandConfig.nandInfo.pageOffset    = swap32val (ibl.nandConfig.nandInfo.pageOffset);
    ibl.nandConfig.nandInfo.columnOffset  = swap32val (ibl.nandConfig.nandInfo.columnOffset);
    ibl.nandConfig.nandInfo.postCommand   = swap16val (ibl.nandConfig.nandInfo.postCommand);

    ibl.chkSum = swap16val (ibl.chkSum);
}

        

/**
 *  @brief
 *      The i2c load context consists of the address of the next block 
 *      to read, and a simple fifo holding any existing data.
 */
#define I2C_MAX_BLOCK_SIZE      0x80
uint32 i2cReadAddress;

uint32 i2cFifoIn  = 0;
uint32 i2cFifoOut = 0;
uint8  i2cData[I2C_MAX_BLOCK_SIZE];
uint16 i2cSum[I2C_MAX_BLOCK_SIZE >> 1];


/**
 *  @brief
 *      Return the number of elements in the fifo
 */
Uint32 i2cFifoCount (void)
{
    Int32 count;

    if (i2cFifoIn >= i2cFifoOut)
        count = i2cFifoIn - i2cFifoOut;
    else
        count = i2cFifoIn + I2C_MAX_BLOCK_SIZE - i2cFifoOut;

    return (count);

}

    
/**
 *  @brief
 *      Read a byte from the fifo
 */
Uint8 i2cFifoRead(void)
{
    Uint8 v;

    v = i2cData[i2cFifoOut];

    i2cFifoOut += 1;

    if (i2cFifoOut == i2cFifoIn)
        i2cFifoOut = i2cFifoIn = 0;

    if (i2cFifoOut >= I2C_MAX_BLOCK_SIZE)
        i2cFifoOut = 0;

    return (v);

}

/**
 *  @brief
 *      Read a block of data from the I2C eeprom and put it in the fifo
 */
void i2cReadBlock (void)
{
    uint16 len;
    int32  i, j;
    uint32 v;

    for (;;) {
        while (hwI2cMasterRead (i2cReadAddress & 0xffff,    /* The address on the eeprom of the table */
                                4,                          /* The number of bytes to read */
                                i2cData,                    /* Where to store the bytes */
                                i2cReadAddress >> 16,       /* The bus address of the eeprom */
                                IBL_CFG_I2C_ADDR_DELAY)     /* The delay between sending the address and reading data */
    
             != I2C_RET_OK)  {

            iblStatus.i2cDataRetries += 1;
        }

        /* Form the length. The received bytes are always in big endian format */
        len    = (i2cData[0] << 8) | i2cData[1];


        if (len > I2C_MAX_BLOCK_SIZE)
            continue;


        while (hwI2cMasterRead (i2cReadAddress & 0xffff,    /* The address on the eeprom of the table */
                                len,                        /* The number of bytes to read */
                                i2cData,                    /* Where to store the bytes */
                                i2cReadAddress >> 16,       /* The bus address of the eeprom */
                                IBL_CFG_I2C_ADDR_DELAY)     /* The delay between sending the address and reading data */
    
             != I2C_RET_OK)  {

            iblStatus.i2cDataRetries += 1;
        }


        /* Must do endian conversion to verify the checksum */
        for (i = j = 0; i < len; i += 2, j += 1) 
            i2cSum[j] = (i2cData[i+0] << 8) | i2cData[i+1];

        v = onesComplementChksum (i2cSum, j);
        if ((v == 0) || (v == 0xffff))
            break;

        
        iblStatus.i2cDataRetries += 1;

    }


    i2cReadAddress += len;
    
    i2cFifoIn  = len;
    i2cFifoOut = 4;    /* The i2c header is effectively removed */

}

             


/**
 *  @brief
 *      Read data from the I2C to pass to the interpreter
 */
Int32 iblI2cRead (Uint8 *buf, Uint32 num_bytes)
{
    int i;

    for (i = 0; i < num_bytes; i++)  {

        if (i2cFifoCount() == 0)
            i2cReadBlock ();

        buf[i] = i2cFifoRead();
    }

    return (0);

}

#define iblBITMASK(x,y)      (   (   (  ((UINT32)1 << (((UINT32)x)-((UINT32)y)+(UINT32)1) ) - (UINT32)1 )   )   <<  ((UINT32)y)   )
#define iblREAD_BITFIELD(z,x,y)   (((UINT32)z) & iblBITMASK(x,y)) >> (y)
/**
 *  @brief
 *      Return the lower 16 bits of a 32 bit value. A function is used (with cross-function optomization off)
 *      which results in an endian independent version
 */
uint16 readLower16 (uint32 v)
{
    return (iblREAD_BITFIELD(v,15,0));

}

/**
 * @brief
 *      Return the upper 16 bits of a 32 bit value. A function is used to force an endian independent version
 */
uint16 readUpper16 (uint32 v)
{
  return (iblREAD_BITFIELD(v,31,16));
}


/**
 *  @brief
 *      The module function table used for boot from i2c
 */
BOOT_MODULE_FXN_TABLE i2cinit_boot_module = 
{
    NULL,           /* Open  API */
    NULL,           /* Close API */
    iblI2cRead,     /* Read  API */
    NULL,           /* Write API */
    NULL,           /* Peek  API */
    NULL,           /* Seek  API */
    NULL            /* Query API */
};



/**
 *  @brief
 *      The main function
 *
 *  @details
 *      The ibl configuration parameters are read from the i2c, 
 *      followed by the i2c mapping information. The second stage
 *      of the IBL is then loaded, and execution transferred 
 *      to the second stage.
 */
void main (void)
{

    uint16       v;
    uint16       configAddrLsw;
    uint16       configAddrMsw;
    uint32       entry;
    void         (*exit)();
    iblI2cMap_t  map;

    memset (&iblStatus, 0, sizeof(iblStatus_t));
    iblStatus.iblMagic     = ibl_MAGIC_VALUE;
    iblStatus.iblVersion   = ibl_VERSION;
    iblStatus.activePeriph = ibl_ACTIVE_PERIPH_I2C;

    /* Read the endianness setting of the device */
    littleEndian = deviceIsLittleEndian();
    
    /* Load the default configuration table from the i2c. The actual speed of the device
     * isn't really known here, since it is part of the table, so a compile time
     * value is used (the pll may have been configured during the initial load) */
    hwI2Cinit (IBL_CFG_I2C_DEV_FREQ_MHZ,        /* The CPU frequency during I2C data load */
               DEVICE_I2C_MODULE_DIVISOR,       /* The divide down of CPU that drives the i2c */
               IBL_CFG_I2C_CLK_FREQ_KHZ,        /* The I2C data rate used during table load */
               IBL_CFG_I2C_OWN_ADDR);           /* The address used by this device on the i2c bus */


    /* Read the I2C mapping information from the eeprom */
    for (;;)  {
        if (hwI2cMasterRead (IBL_CFG_I2C_MAP_TABLE_DATA_ADDR,     /* The address on the eeprom of the data mapping */
                             sizeof(iblI2cMap_t),                 /* The number of bytes to read */
                             (UINT8 *)&map,                       /* Where to store the bytes */
                             IBL_CFG_I2C_MAP_TABLE_DATA_BUS_ADDR, /* The bus address of the eeprom */
                             IBL_CFG_I2C_ADDR_DELAY)              /* The delay between sending the address and reading data */

             == I2C_RET_OK)  {

                /* On the I2C EEPROM the table is always formatted with the most significant
                 * byte first. So if the device is running little endain the endian must be
                 * swapped */
                if (littleEndian == TRUE)  {
                    map.length   = swap16val (map.length);
                    map.chkSum   = swap16val (map.chkSum);
                    map.addrLe   = swap32val (map.addrLe);
                    map.configLe = swap32val (map.configLe);
                    map.addrBe   = swap32val (map.addrBe);
                    map.configBe = swap32val (map.configBe);

                    configAddrLsw = readLower16 (map.configLe);
                    configAddrMsw = readUpper16 (map.configLe);

                }  else  {
                    configAddrLsw = readLower16 (map.configBe);
                    configAddrMsw = readUpper16 (map.configLe);

                }


                if (map.length != sizeof(iblI2cMap_t))  {
                    iblStatus.mapSizeFail += 1;
                    continue;
                }

                if (map.chkSum != 0)  {
                    
                    v = onesComplementChksum ((UINT16 *)&map, sizeof(iblI2cMap_t));
                    if ((v != 0) && (v != 0xffff))  {
                        iblStatus.mapRetries += 1;
                        continue;
                    }
                }

                break;
        }

        iblStatus.mapRetries += 1;

    }


    /* Read the i2c configuration tables until the checksum passes and the magic number
     * matches. The checksum must be verified before the endian re-ordering is done */
    for (;;)  {

        if (hwI2cMasterRead (configAddrLsw,                  /* The address on the eeprom of the table */
                             sizeof(ibl_t),                  /* The number of bytes to read */
                             (UINT8 *)&ibl,                  /* Where to store the bytes */
                             configAddrMsw,                  /* The bus address of the eeprom */
                             IBL_CFG_I2C_ADDR_DELAY)         /* The delay between sending the address and reading data */

             == I2C_RET_OK)  {

                 if (ibl.chkSum != 0)  {

                    v = onesComplementChksum ((UINT16 *)&ibl, sizeof(ibl_t) / sizeof(UINT16));
                    if ((v != 0) && (v != 0xffff))  {
                        iblStatus.i2cRetries += 1;
                        continue;
                    }

                 }  


                if (ibl.iblMagic == ibl_MAGIC_VALUE)
                    break;

                if (swap32val (ibl.iblMagic) == ibl_MAGIC_VALUE)  {
                    iblSwap ();
                    break;
                }

                iblStatus.magicRetries += 1;

            }

            iblStatus.i2cRetries += 1;
    }

    /* Pll configuration is device specific */
    devicePllConfig ();


    /* The rest of the IBL is in boot table format. Read and process the data */
    if (littleEndian == TRUE) 
        i2cReadAddress = map.addrLe;
    else
        i2cReadAddress = map.addrBe;

    if (i2cReadAddress == 0xffffffff)  {
        iblStatus.iblFail = ibl_FAIL_CODE_INVALID_I2C_ADDRESS;
        for (;;);
    }


    /* Pass control to the boot table processor */
    iblBootBtbl (&i2cinit_boot_module, &entry);

    if (btblWrapEcode != 0)  {
        iblStatus.iblFail = ibl_FAIL_CODE_BTBL_FAIL;
        for (;;);
    }

    /* jump to the exit point, which will be the entry point for the full IBL */
    exit = (void (*)())entry;
    (*exit)();


}











