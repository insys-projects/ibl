/*************************************************************************************
 * FILE PURPOSE: Write the ibl configuration table to the i2c eeprom
 *************************************************************************************
 * @file i2cparam.c
 *
 * @brief
 *	  Creates the ibl configuration table and writes to the i2c
 *
 *************************************************************************************/

#include "types.h" 
#include "ibl.h"
#include "i2c.h"
#include "pllapi.h"
#include "iblcfg.h"
#include "target.h"
#include "string.h"
#include <stdlib.h>
#include <stdio.h>

ibl_t ibl;

#define SETIP(array,i0,i1,i2,i3)      array[0]=(i0);  \
                                      array[1]=(i1);  \
                                      array[2]=(i2);  \
                                      array[3]=(i3)

/**
 * @brief  The desired ibl configuration is setup here. Edit the fields
 *         to match the desired setup.
 */

#if 0
void setupTable (void)
{

    /* Initialize the table */
    memset (&ibl, 0, sizeof (ibl_t));

    ibl.iblMagic = ibl_MAGIC_VALUE;

    ibl.pllConfig[ibl_MAIN_PLL].doEnable      = TRUE;
    ibl.pllConfig[ibl_MAIN_PLL].prediv        = 1;
    ibl.pllConfig[ibl_MAIN_PLL].mult          = 28;
    ibl.pllConfig[ibl_MAIN_PLL].postdiv       = 1;
    ibl.pllConfig[ibl_MAIN_PLL].pllOutFreqMhz = 700;

    /* The DDR PLL. The multipliers/dividers are fixed, so are really dont cares */
    ibl.pllConfig[ibl_DDR_PLL].doEnable = TRUE;

    /* The network PLL. The multipliers/dividers are fixed */
    ibl.pllConfig[ibl_NET_PLL].doEnable = TRUE;

    /* EMIF configuration. The values are for DDR at 533 MHz  */
    ibl.ddrConfig.configDdr = TRUE;

    ibl.ddrConfig.uEmif.emif3p1.sdcfg  = 0x00538832; /* timing, 32bit wide */
    ibl.ddrConfig.uEmif.emif3p1.sdrfc  = 0x0000073B; /* Refresh 533Mhz */ 
    ibl.ddrConfig.uEmif.emif3p1.sdtim1 = 0x47245BD2; /* Timing 1 */
    ibl.ddrConfig.uEmif.emif3p1.sdtim2 = 0x0125DC44; /* Timing 2 */
    ibl.ddrConfig.uEmif.emif3p1.dmcctl = 0x50001906; /* PHY read latency for CAS 5 is 5 + 2 - 1 */

    /* Ethernet configuration for port 0 */
    ibl.ethConfig[0].ethPriority      = ibl_HIGHEST_PRIORITY;
    ibl.ethConfig[0].port             = 0;

    /* Bootp is disabled. The server and file name are provided here */
    ibl.ethConfig[0].doBootp          = FALSE;
    ibl.ethConfig[0].useBootpServerIp = FALSE;
    ibl.ethConfig[0].useBootpFileName = FALSE;
    ibl.ethConfig[0].bootFormat       = ibl_BOOT_FORMAT_BBLOB;

#if 0
    SETIP(ibl.ethConfig[0].ethInfo.ipAddr,    192,168,2,100);
    SETIP(ibl.ethConfig[0].ethInfo.serverIp,  192,168,2,110);
    SETIP(ibl.ethConfig[0].ethInfo.gatewayIp, 192,168,2,2);
    SETIP(ibl.ethConfig[0].ethInfo.netmask,   255,255,255,0);
#endif
    SETIP(ibl.ethConfig[0].ethInfo.ipAddr,    10,218,109,35);
    SETIP(ibl.ethConfig[0].ethInfo.serverIp,  10,218,109,196);
    SETIP(ibl.ethConfig[0].ethInfo.gatewayIp, 10,218,109,1);
    SETIP(ibl.ethConfig[0].ethInfo.netmask,   255,255,255,0);

    /* Leave the hardware address as 0 so the e-fuse value will be used */

    strcpy (ibl.ethConfig[0].ethInfo.fileName, "test.blob");

    /* Even though the entire range of DDR2 is chosen, the load will
     * stop when the ftp reaches the end of the file */
    ibl.ethConfig[0].blob.startAddress  = 0xe0000000;       /* Base address of DDR2 */
    ibl.ethConfig[0].blob.sizeBytes     = 0x20000000;       /* All of DDR2 */
    ibl.ethConfig[0].blob.branchAddress = 0xe0000000;       /* Base of DDR2 */

    /* For port 1 use bootp */
    /* Ethernet configuration for port 0 */
    ibl.ethConfig[1].ethPriority      = ibl_HIGHEST_PRIORITY + 1;
    ibl.ethConfig[1].port             = 1;

    /* Bootp is disabled. The server and file name are provided here */
    ibl.ethConfig[1].doBootp          = TRUE;
    ibl.ethConfig[1].useBootpServerIp = TRUE;
    ibl.ethConfig[1].useBootpFileName = TRUE;
    ibl.ethConfig[1].bootFormat       = ibl_BOOT_FORMAT_BBLOB;


    /* SGMII */


    /* Leave the hardware address as 0 so the e-fuse value will be used */
    /* Leave all remaining fields as 0 since bootp will fill them in */


    /* Even though the entire range of DDR2 is chosen, the load will
     * stop when the ftp reaches the end of the file */
    ibl.ethConfig[1].blob.startAddress  = 0xe0000000;       /* Base address of DDR2 */
    ibl.ethConfig[1].blob.sizeBytes     = 0x20000000;       /* All of DDR2 */
    ibl.ethConfig[1].blob.branchAddress = 0xe0000000;       /* Base of DDR2 */
    


    /* MDIO configuration */
    ibl.mdioConfig.nMdioOps = 8;
    ibl.mdioConfig.mdioClkDiv = 0x20;
    ibl.mdioConfig.interDelay = 1400;   /* ~2ms at 700 MHz */

    ibl.mdioConfig.mdio[0] =  (1 << 30) | (27 << 21) | (24 << 16) | 0x848b;
    ibl.mdioConfig.mdio[1] =  (1 << 30) | (20 << 21) | (24 << 16) | 0x0ce0;
    ibl.mdioConfig.mdio[2] =  (1 << 30) | (24 << 21) | (24 << 16) | 0x4101;
    ibl.mdioConfig.mdio[3] =  (1 << 30) | ( 0 << 21) | (24 << 16) | 0x9140;

    ibl.mdioConfig.mdio[4] =  (1 << 30) | (27 << 21) | (25 << 16) | 0x848b;
    ibl.mdioConfig.mdio[5] =  (1 << 30) | (20 << 21) | (25 << 16) | 0x0ce0;
    ibl.mdioConfig.mdio[6] =  (1 << 30) | (24 << 21) | (25 << 16) | 0x4101;
    ibl.mdioConfig.mdio[7] =  (1 << 30) | ( 0 << 21) | (25 << 16) | 0x9140;


    /* Nand boot is disabled */
    ibl.nandConfig.nandPriority = ibl_DEVICE_NOBOOT;

}

#endif


/**
 *  @brief
 *     Display the error returned by the i2c driver 
 */
void showI2cError (I2C_RET iret, char *stage)
{
    char *ecode;

    switch (iret)  {
        case I2C_RET_LOST_ARB:        ecode = "I2C master lost an arbitration battle";
                                      break;

        case I2C_RET_NO_ACK:          ecode = "I2C master detected no ack from slave";
                                      break;

        case I2C_RET_IDLE_TIMEOUT:    ecode = "I2C timed out";
                                      break;

        case I2C_RET_BAD_REQUEST:     ecode = "I2C driver detected a bad data request";
                                      break;

        case I2C_RET_CLOCK_STUCK_LOW: ecode = "I2C driver found the bus stuck low";
                                      break;

        case I2C_RET_GEN_ERROR:       ecode = "I2C driver reported a general error";
                                      break;

    }

    printf ("Error: %s, reported at stage: %s\n", ecode, stage);

}

/**
 * @brief
 *   Group the data into a 64 byte (max) data block aligned on a 64 byte boundary 
 */
Int32 formBlock (UINT8 *base, Int32 dataSize, Int32 *offset, Uint8 *data, Uint32 baseI2cDataAddr)
{
    Uint32 currentAddress;
    Int32  nbytes;
    Int32  i;

    /* The I2C eeprom address is the parameter base address plus the current offset
     * The number of bytes is blocked to end on a 64 byte boundary */
    currentAddress = baseI2cDataAddr + *offset;
    nbytes         = 64 - (currentAddress & (64 - 1));

    /* Only write the bytes in the input array */
    if (*offset + nbytes > dataSize)
        nbytes = dataSize - *offset;

    if (nbytes == 0)
        return (0);

    /* The address is placed first */
    data[0] = (currentAddress >> 8) & 0xff;
    data[1] = (currentAddress >> 0) & 0xff;

    /* The data */
    for (i = 0; i < nbytes; i++)
        data[i+2] = base[*offset + i];

    /* Update the offset */
    *offset += nbytes;

    /* Return the total number of bytes (including address) to go on the bus */
    return (nbytes + 2);

}



void main (void)
{
    I2C_RET  iret;
    UINT8    writeBlock[66];
    char     iline[132];
    Int32    n; 
    Int32    currentOffset;

    volatile Int32 i;

    printf ("Run the GEL for for the device to be configured, press return to program the I2C\n");
    getchar ();

    /* Program the main system PLL */
    hwPllSetPll (MAIN_PLL,      
                 ibl.pllConfig[ibl_MAIN_PLL].prediv,         /* Pre-divider  */
                 ibl.pllConfig[ibl_MAIN_PLL].pllOutFreqMhz,  /* Multiplier   */
                 ibl.pllConfig[ibl_MAIN_PLL].postdiv);       /* Post-divider */


    hwI2Cinit (ibl.pllConfig[ibl_MAIN_PLL].pllOutFreqMhz,  /* The CPU frequency during I2C data load */
               DEVICE_I2C_MODULE_DIVISOR,                  /* The divide down of CPU that drives the i2c */
               IBL_I2C_CLK_FREQ_KHZ/8,                     /* The I2C data rate used during table load. Slowed for writes */
               IBL_I2C_OWN_ADDR);                          /* The address used by this device on the i2c bus */


    /* Block the data into 64 byte blocks aligned on 64 byte boundaries for the data write.
     * The block address is prepended to the data block before writing */
    currentOffset = 0;
    do  {

        n = formBlock ((UINT8 *)&ibl, sizeof(ibl_t), &currentOffset, writeBlock, IBL_I2C_CFG_TABLE_DATA_ADDR);

        if (n > 0)  {

            iret = hwI2cMasterWrite (IBL_I2C_CFG_EEPROM_BUS_ADDR,   /* The I2C bus address of the eeprom */
                                     writeBlock,                    /* The data to write */
                                     n,                             /* The number of bytes to write */
                                     I2C_RELEASE_BUS,               /* Release the bus when the write is done */
                                     FALSE );                       /* Bus is not owned at start of operation */

            if (iret != I2C_RET_OK)  {
                sprintf (iline, "Block at offset %d\n", currentOffset);
                showI2cError (iret, iline);
                return;
            }

            /* Need some delay to allow the programming to occur */
            for (i = 0; i < 600000; i++);

        }

    } while (n > 0);
            
    

    printf ("I2c table write complete\n");

}



        







