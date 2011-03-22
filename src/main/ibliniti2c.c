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

#include "ibl.h"
#include "iblloc.h"
#include "iblcfg.h"
#include "device.h"
#include "iblbtbl.h"
#include "i2c.h"
#include "iblinit.h"
#include <string.h>

#ifndef EXCLUDE_I2C
/**
 *  @brief
 *      A global value is used to track the read through the i2c during
 *      the program load.
 */
uint32 i2cReadAddress, i2cBusAddress;

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
        while (hwI2cMasterRead (i2cReadAddress,             /* The address on the eeprom of the table */
                                4,                          /* The number of bytes to read */
                                iData,                      /* Where to store the bytes */
                                i2cBusAddress,              /* The bus address of the eeprom */
                                IBL_CFG_I2C_ADDR_DELAY)     /* The delay between sending the address and reading data */
    
             != I2C_RET_OK)  {

            iblStatus.i2cDataRetries += 1;
        }

        /* Form the length. The received bytes are always in big endian format */
        len    = (iData[0] << 8) | iData[1];


        if (len > I_MAX_BLOCK_SIZE)
            continue;


        while (hwI2cMasterRead (i2cReadAddress,             /* The address on the eeprom of the table */
                                len,                        /* The number of bytes to read */
                                iData,                      /* Where to store the bytes */
                                i2cBusAddress,              /* The bus address of the eeprom */
                                IBL_CFG_I2C_ADDR_DELAY)     /* The delay between sending the address and reading data */
    
             != I2C_RET_OK)  {

            iblStatus.i2cDataRetries += 1;
        }


        /* Must do endian conversion to verify the checksum */
        for (i = j = 0; i < len; i += 2, j += 1) 
            iSum[j] = (iData[i+0] << 8) | iData[i+1];

        v = onesComplementChksum (iSum, j);
        if ((v == 0) || (v == 0xffff))
            break;

        
        iblStatus.i2cDataRetries += 1;

    }


    i2cReadAddress += len;
    i2cBusAddress = i2cReadAddress >>16;
    
    iFifoIn  = len;
    iFifoOut = 4;    /* The i2c header is effectively removed */

}

             


/**
 *  @brief
 *      Read data from the I2C to pass to the interpreter
 */
Int32 iblI2cRead (Uint8 *buf, Uint32 num_bytes)
{
    int i;

    for (i = 0; i < num_bytes; i++)  {

        if (iFifoCount() == 0)
            i2cReadBlock ();

        buf[i] = iFifoRead();
    }

    return (0);

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
 *      Configure the I2C, then read the parameters from I2C and pass
 *      to the second stage boot
 */
BOOT_MODULE_FXN_TABLE *iblInitI2c (void)
{
    uint16       v;
    uint16       configAddrLsw;
    uint16       configAddrMsw;
    iblBootMap_t map;
    bool         littleEndian;

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
                             sizeof(iblBootMap_t),                /* The number of bytes to read */
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

                    configAddrLsw  = readLower16 (map.configLe);
                    configAddrMsw  = readUpper16 (map.configLe);
                    i2cReadAddress = map.addrLe;

                }  else  {
                    configAddrLsw = readLower16 (map.configBe);
                    configAddrMsw = readUpper16 (map.configBe);
                    i2cReadAddress = map.addrBe;

                }

                i2cBusAddress = i2cReadAddress >>16;

                if (map.length != sizeof(iblBootMap_t))  {
                    iblStatus.mapSizeFail += 1;
                    continue;
                }

                if (map.chkSum != 0)  {
                    
                    v = onesComplementChksum ((UINT16 *)&map, sizeof(iblBootMap_t));
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

    /* The rest of the IBL is in boot table format. Read and process the data */
    if (i2cReadAddress == 0xffffffff)  {
        iblStatus.iblFail = ibl_FAIL_CODE_INVALID_I2C_ADDRESS;
        for (;;);
    }

    return (&i2cinit_boot_module);

}

#endif /* EXCLUDE_I2C */
