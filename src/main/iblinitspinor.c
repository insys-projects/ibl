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
#include "spi_api.h"
#include "iblinit.h"
#include <string.h>

#if (!defined(EXCLUDE_NOR_SPI) || !defined(EXCLUDE_NAND_SPI))

/**
 *  @brief
 *      The next read address on the SPI flash is stored in a global for
 *      access through the boot call chain.
 */
uint32 spiReadAddress;

/**
 *  @brief
 *      Read a block of data from the SPI eeprom and put it in the fifo
 */
void spiReadBlock (void)
{
    uint16 len;
    int32  i, j;
    uint32 v;

    for (;;) {
        while (hwSpiRead  (spiReadAddress,    /* The address on the eeprom of the table */
                           4,                 /* The number of bytes to read */
                           iData)             /* Where to store the bytes */
    
             != 0)  {

            iblStatus.spiDataRetries += 1;
        }

        /* Form the length. The received bytes are always in big endian format */
        len    = (iData[0] << 8) | iData[1];


        if (len > I_MAX_BLOCK_SIZE)
            continue;


        while (hwSpiRead (spiReadAddress,    /* The address on the eeprom of the table */
                          len,               /* The number of bytes to read */
                          iData)             /* Where to store the bytes */
    
             != 0)  {

            iblStatus.spiDataRetries += 1;
        }


        /* Must do endian conversion to verify the checksum */
        for (i = j = 0; i < len; i += 2, j += 1) 
            iSum[j] = (iData[i+0] << 8) | iData[i+1];

        v = onesComplementChksum (iSum, j);
        if ((v == 0) || (v == 0xffff))
            break;

        
        iblStatus.spiDataRetries += 1;

    }


    spiReadAddress += len;
    
    iFifoIn  = len;
    iFifoOut = 4;    /* The spi header is effectively removed */

}

             

/**
 *  @brief
 *      Read data from the SPI to pass to the interpreter
 */
Int32 iblSpiRead (Uint8 *buf, Uint32 num_bytes)
{
    int i;

    for (i = 0; i < num_bytes; i++)  {

        if (iFifoCount() == 0)
            spiReadBlock ();

        buf[i] = iFifoRead();
    }

    return (0);

}


/**
 *  @brief
 *      The module function table used for boot from spi
 */
BOOT_MODULE_FXN_TABLE spiinit_boot_module = 
{
    NULL,           /* Open  API */
    NULL,           /* Close API */
    iblSpiRead,     /* Read  API */
    NULL,           /* Write API */
    NULL,           /* Peek  API */
    NULL,           /* Seek  API */
    NULL            /* Query API */
};




/**
 *  @brief
 *      Configure the SPI, then read the parameters from the SPI and
 *      pass them to the second stage boot
 */
BOOT_MODULE_FXN_TABLE *iblInitSpiNor (void)
{
    iblBootMap_t  map;
    spiConfig_t   cfg;
    uint32        configAddr;

    bool          littleEndian;
    uint16        v;

    /* Read the endianness of the device */
    littleEndian = deviceIsLittleEndian();

    /* Load the default configuration table from the SPI. The actual speed of 
     * the device isn't really known here, since it is part of the table, so a
     * compile time value is used (the pll may have been configured during the initial load) */
    deviceLoadInitSpiConfig ((void *)&cfg);

    if (hwSpiConfig (&cfg) != 0)  {

        iblStatus.iblFail = ibl_FAIL_CODE_SPI_PARAMS;
        for (;;);

    }


    /* Read the SPI mapping information from the nor flash */
    for (;;)  {

        if (hwSpiRead ((IBL_CFG_SPI_MAP_TABLE_DATA_ADDR_MSW << 16) |   /* The address on the flash of the data mapping */
                        IBL_CFG_SPI_MAP_TABLE_DATA_ADDR_LSW,
                       sizeof(iblBootMap_t),                           /* The number of bytes to read */
                       (UINT8 *)&map)                                  /* Where to store the data */

          == 0)  {

            /* On the flash the table is always formatted with the most significant 
             * byte first. So if the device is running little endian, the endian
             * must be swapped */
            if (littleEndian == TRUE)  {
                map.length   = swap16val (map.length);
                map.chkSum   = swap16val (map.chkSum);
                map.addrLe   = swap32val (map.addrLe);
                map.configLe = swap32val (map.configLe);
                map.addrBe   = swap32val (map.addrBe);
                map.configBe = swap32val (map.configBe);

                configAddr     = map.configLe;
                spiReadAddress = map.addrLe;

            }  else  {

                configAddr     = map.configBe;
                spiReadAddress = map.addrBe;

            }
            

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
        

    /* Read the SPI configuration tables until the checksum passes and the magic
     * number matches. The checksum must be verified before the endian re-ordering */
    for (;;)  {

        if (hwSpiRead (configAddr,          /* The address on the flash of the table */
                       sizeof(ibl_t),       /* The number of bytes to read */
                       (UINT8 *)&ibl)       /* Where to store the bytes */
             == 0)  {

            if (ibl.chkSum != 0)  {

                v = onesComplementChksum ((UINT16 *)&ibl, sizeof(ibl_t) / sizeof(UINT16));
                if ((v != 0) && (v != 0xffff))  {
                    iblStatus.spiRetries += 1;
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

        iblStatus.spiRetries += 1;

    }


    /* the rest of the IBL is in boot table format. Read and process the data */
    if (spiReadAddress == 0xffffffff)  {
        iblStatus.iblFail = ibl_FAIL_CODE_INVALID_SPI_ADDRESS;
        for (;;);
    }

    return (&spiinit_boot_module);

}


#endif /* (!defined(EXCLUDE_NOR_SPI) || !defined(EXCLUDE_NAND_SPI)) */



