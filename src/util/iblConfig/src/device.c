/************************************************************************************
 * FILE PURPOSE: C6455 Device Specific functions used in the 1st load stage
 ************************************************************************************
 * FILE NAME: c6455init.c
 *
 * DESCRIPTION: Implements the device specific functions for the IBL
 *
 * @file c6455.c
 *
 * @brief
 *  This file implements the device specific functions for the IBL
 *
 ************************************************************************************/
#include "device.h"

ibl_t c6455_ibl_config(void)
{
    ibl_t ibl;	
    memset(&ibl, 0, sizeof(ibl_t));
	
    ibl.iblMagic = ibl_MAGIC_VALUE;
    ibl.iblEvmType = ibl_EVM_C6455L;

    ibl.pllConfig[ibl_MAIN_PLL].doEnable      = TRUE;
    ibl.pllConfig[ibl_MAIN_PLL].prediv        = 1;
    ibl.pllConfig[ibl_MAIN_PLL].mult          = 20;
    ibl.pllConfig[ibl_MAIN_PLL].postdiv       = 1;
    ibl.pllConfig[ibl_MAIN_PLL].pllOutFreqMhz = 1000;

    /* The DDR PLL. The multipliers/dividers are fixed, so are really dont cares */
    ibl.pllConfig[ibl_DDR_PLL].doEnable = TRUE;

    /* The network PLL. The multipliers/dividers are fixed */
    ibl.pllConfig[ibl_NET_PLL].doEnable = TRUE;

    /* EMIF configuration. The values are for DDR at 500 MHz  */
    ibl.ddrConfig.configDdr = TRUE;

    ibl.ddrConfig.uEmif.emif3p1.sdcfg  = 0x00538822; /* timing, 32bit wide */
    ibl.ddrConfig.uEmif.emif3p1.sdrfc  = 0x000007a2; /* Refresh 500Mhz */ 
    ibl.ddrConfig.uEmif.emif3p1.sdtim1 = 0x3edb4b91; /* Timing 1 */
    ibl.ddrConfig.uEmif.emif3p1.sdtim2 = 0x00a2c722; /* Timing 2 */
    ibl.ddrConfig.uEmif.emif3p1.dmcctl = 0x00000005; /* PHY read latency for CAS 4 is 4 + 2 - 1 */

    /* SGMII not present */
    ibl.sgmiiConfig[0].configure = FALSE;
    ibl.sgmiiConfig[1].configure = FALSE;

    /* MDIO configuration */
    ibl.mdioConfig.nMdioOps = 0;
    ibl.mdioConfig.mdioClkDiv = 0x20;
    ibl.mdioConfig.interDelay = 2000;   /* ~2ms at 1000 MHz */

    ibl.mdioConfig.mdio[0] =  (1 << 30) | (14 << 21) | (0 << 16) | 0xd5d0;

    /* spiConfig and emifConfig not needed */

    /* Ethernet configuration for Boot mode 0 */
    ibl.bootModes[0].bootMode = ibl_BOOT_MODE_TFTP;
    ibl.bootModes[0].priority = ibl_HIGHEST_PRIORITY;
    ibl.bootModes[0].port     = 0;

    /* Bootp is disabled. The server and file name are provided here */
    ibl.bootModes[0].u.ethBoot.doBootp          = TRUE;
    ibl.bootModes[0].u.ethBoot.useBootpServerIp = TRUE;
    ibl.bootModes[0].u.ethBoot.useBootpFileName = TRUE;
    ibl.bootModes[0].u.ethBoot.bootFormat       = ibl_BOOT_FORMAT_BBLOB;

    SETIP(ibl.bootModes[0].u.ethBoot.ethInfo.ipAddr,    192,168,1,3);
    SETIP(ibl.bootModes[0].u.ethBoot.ethInfo.serverIp,  192,168,1,2);
    SETIP(ibl.bootModes[0].u.ethBoot.ethInfo.gatewayIp, 192,168,1,1);
    SETIP(ibl.bootModes[0].u.ethBoot.ethInfo.netmask,   255,255,255,0);

    /* There is no e-fuse mac address. A value must be assigned */
    ibl.bootModes[0].u.ethBoot.ethInfo.hwAddress[0] = 10;
    ibl.bootModes[0].u.ethBoot.ethInfo.hwAddress[1] = 224;
    ibl.bootModes[0].u.ethBoot.ethInfo.hwAddress[2] = 166;
    ibl.bootModes[0].u.ethBoot.ethInfo.hwAddress[3] = 102;
    ibl.bootModes[0].u.ethBoot.ethInfo.hwAddress[4] = 87;
    ibl.bootModes[0].u.ethBoot.ethInfo.hwAddress[5] = 25;


    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[0]  = 'c';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[1]  = '6';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[2]  = '4';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[3]  = '5';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[4]  = '5';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[5]  = '-';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[6]  = 'l';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[7]  = 'e';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[8]  = '.';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[9]  = 'b';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[10] = 'i';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[11] = 'n';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[12] = '\0';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[13] = '\0';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[14] = '\0';

    /* Even though the entire range of DDR2 is chosen, the load will
     * stop when the ftp reaches the end of the file */
    ibl.bootModes[0].u.ethBoot.blob.startAddress  = 0xe0000000;       /* Base address of DDR2 */
    ibl.bootModes[0].u.ethBoot.blob.sizeBytes     = 0x20000000;       /* All of DDR2 */
    ibl.bootModes[0].u.ethBoot.blob.branchAddress = 0xe0000000;       /* Base of DDR2 */

    /* Alternative bootMode not configured for now */
    ibl.bootModes[1].bootMode = ibl_BOOT_MODE_NONE;

    ibl.chkSum = 0;

    return(ibl);
}

ibl_t c6474_ibl_config(void)
{
    ibl_t ibl;
    memset(&ibl, 0, sizeof(ibl_t));
	
    ibl.iblMagic = ibl_MAGIC_VALUE;
    ibl.iblEvmType = ibl_EVM_C6474M;

    ibl.pllConfig[ibl_MAIN_PLL].doEnable      = TRUE;
    ibl.pllConfig[ibl_MAIN_PLL].prediv        = 1;
    ibl.pllConfig[ibl_MAIN_PLL].mult          = 20;
    ibl.pllConfig[ibl_MAIN_PLL].postdiv       = 1;
    ibl.pllConfig[ibl_MAIN_PLL].pllOutFreqMhz = 1000;

    /* The DDR PLL. The multipliers/dividers are fixed, so are really dont cares */
    ibl.pllConfig[ibl_DDR_PLL].doEnable = TRUE;

    /* The network PLL. The multipliers/dividers are fixed */
    ibl.pllConfig[ibl_NET_PLL].doEnable = TRUE;

    /* EMIF configuration. The values are for DDR at 533 MHz  */
    ibl.ddrConfig.configDdr = TRUE;

    ibl.ddrConfig.uEmif.emif3p1.sdcfg  = 0x00d38a32; /* cas5, 8 banks, 10 bit column */
    ibl.ddrConfig.uEmif.emif3p1.sdrfc  = 0x00000a29; /* Refresh 333Mhz */ 
    ibl.ddrConfig.uEmif.emif3p1.sdtim1 = 0x4d246c9a; /* Timing 1 */
    ibl.ddrConfig.uEmif.emif3p1.sdtim2 = 0x00993c42; /* Timing 2 */
    ibl.ddrConfig.uEmif.emif3p1.dmcctl = 0x50001906; /* PHY read latency for CAS 5 is 5 + 2 - 1 */

    /* SGMII 0 is present */
    ibl.sgmiiConfig[0].configure = TRUE;
    ibl.sgmiiConfig[0].adviseAbility = 0x9801;
    ibl.sgmiiConfig[0].control       = 0x20;
    ibl.sgmiiConfig[0].txConfig      = 0x00000ea3;
    ibl.sgmiiConfig[0].rxConfig      = 0x00081023;
    ibl.sgmiiConfig[0].auxConfig     = 0x0000000b;

    /* There is no port 1 on the 6474 */
    ibl.sgmiiConfig[1].configure = FALSE;

    /* MDIO configuration */
    ibl.mdioConfig.nMdioOps = 8;
    ibl.mdioConfig.mdioClkDiv = 0x26;
    ibl.mdioConfig.interDelay = 2000;   /* ~2ms at 1000 MHz */

    ibl.mdioConfig.mdio[0] =  (1 << 30) | ( 4 << 21) | (27 << 16) | 0x0081;
    ibl.mdioConfig.mdio[1] =  (1 << 30) | (26 << 21) | (14 << 16) | 0x0047;
    ibl.mdioConfig.mdio[2] =  (1 << 30) | (26 << 21) | (13 << 16) | 0x0047;
    ibl.mdioConfig.mdio[3] =  (1 << 30) | ( 0 << 21) | (14 << 16) | 0x8140;

    ibl.mdioConfig.mdio[4] =  (1 << 30) | ( 0 << 21) | (13 << 16) | 0x8140;
    ibl.mdioConfig.mdio[5] =  (1 << 30) | ( 1 << 21) | (22 << 16) | 0x043e;
    ibl.mdioConfig.mdio[6] =  (1 << 30) | ( 1 << 21) | (22 << 16) | 0x043e;
    ibl.mdioConfig.mdio[7] =  (1 << 30) | ( 0 << 21) | ( 1 << 16) | 0x9140;

    /* spiConfig and emifConfig not needed */

    /* Ethernet configuration for Boot mode 0 */
    ibl.bootModes[0].bootMode = ibl_BOOT_MODE_TFTP;
    ibl.bootModes[0].priority = ibl_HIGHEST_PRIORITY;
    ibl.bootModes[0].port     = 0;

    /* Bootp is disabled. The server and file name are provided here */
    ibl.bootModes[0].u.ethBoot.doBootp          = TRUE;
    ibl.bootModes[0].u.ethBoot.useBootpServerIp = TRUE;
    ibl.bootModes[0].u.ethBoot.useBootpFileName = TRUE;
    ibl.bootModes[0].u.ethBoot.bootFormat       = ibl_BOOT_FORMAT_BBLOB;

    SETIP(ibl.bootModes[0].u.ethBoot.ethInfo.ipAddr,    192,168,1,3);
    SETIP(ibl.bootModes[0].u.ethBoot.ethInfo.serverIp,  192,168,1,2);
    SETIP(ibl.bootModes[0].u.ethBoot.ethInfo.gatewayIp, 192,168,1,1);
    SETIP(ibl.bootModes[0].u.ethBoot.ethInfo.netmask,   255,255,255,0);

    /* Set the hardware address as 0 so the e-fuse value will be used */
    ibl.bootModes[0].u.ethBoot.ethInfo.hwAddress[0] = 0;
    ibl.bootModes[0].u.ethBoot.ethInfo.hwAddress[1] = 0;
    ibl.bootModes[0].u.ethBoot.ethInfo.hwAddress[2] = 0;
    ibl.bootModes[0].u.ethBoot.ethInfo.hwAddress[3] = 0;
    ibl.bootModes[0].u.ethBoot.ethInfo.hwAddress[4] = 0;
    ibl.bootModes[0].u.ethBoot.ethInfo.hwAddress[5] = 0;

    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[0]  = 'c';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[1]  = '6';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[2]  = '4';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[3]  = '7';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[4]  = '4';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[5]  = '-';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[6]  = 'l';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[7]  = 'e';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[8]  = '.';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[9]  = 'b';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[10] = 'i';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[11] = 'n';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[12] = '\0';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[13] = '\0';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[14] = '\0';

    /* Even though the entire range of DDR2 is chosen, the load will
     * stop when the ftp reaches the end of the file */
    ibl.bootModes[0].u.ethBoot.blob.startAddress  = 0x80000000;       /* Base address of DDR2 */
    ibl.bootModes[0].u.ethBoot.blob.sizeBytes     = 0x20000000;       /* All of DDR2 */
    ibl.bootModes[0].u.ethBoot.blob.branchAddress = 0x80000000;       /* Base of DDR2 */

    /* Alternative bootMode not configured for now */
    ibl.bootModes[1].bootMode = ibl_BOOT_MODE_NONE;

    ibl.chkSum = 0;

    return(ibl);
}

ibl_t c6474l_ibl_config(void)
{
    ibl_t ibl;
    memset(&ibl, 0, sizeof(ibl_t));
	
    ibl.iblMagic = ibl_MAGIC_VALUE;
    ibl.iblEvmType = ibl_EVM_C6474L;

    ibl.pllConfig[ibl_MAIN_PLL].doEnable      = TRUE;
    ibl.pllConfig[ibl_MAIN_PLL].prediv        = 1;
    ibl.pllConfig[ibl_MAIN_PLL].mult          = 20;
    ibl.pllConfig[ibl_MAIN_PLL].postdiv       = 1;
    ibl.pllConfig[ibl_MAIN_PLL].pllOutFreqMhz = 1000;

    /* The DDR PLL. The multipliers/dividers are fixed, so are really dont cares */
    ibl.pllConfig[ibl_DDR_PLL].doEnable = TRUE;

    /* The network PLL. The multipliers/dividers are fixed */
    ibl.pllConfig[ibl_NET_PLL].doEnable = TRUE;

    /* EMIF configuration. The values are for DDR at 533 MHz  */
    ibl.ddrConfig.configDdr = TRUE;

    ibl.ddrConfig.uEmif.emif3p1.sdcfg  = 0x00d38a32; /* cas5, 8 banks, 10 bit column */
    ibl.ddrConfig.uEmif.emif3p1.sdrfc  = 0x00000a29; /* Refresh 333Mhz */ 
    ibl.ddrConfig.uEmif.emif3p1.sdtim1 = 0x4d246c9a; /* Timing 1 */
    ibl.ddrConfig.uEmif.emif3p1.sdtim2 = 0x00993c42; /* Timing 2 */
    ibl.ddrConfig.uEmif.emif3p1.dmcctl = 0x50001906; /* PHY read latency for CAS 5 is 5 + 2 - 1 */

    /* SGMII 0 is present */
    ibl.sgmiiConfig[0].configure = TRUE;
    ibl.sgmiiConfig[0].adviseAbility = 0x9801;
    ibl.sgmiiConfig[0].control       = 0x20;
    ibl.sgmiiConfig[0].txConfig      = 0x00000e23;
    ibl.sgmiiConfig[0].rxConfig      = 0x00081023;
    ibl.sgmiiConfig[0].auxConfig     = 0x0000000b;

    /* There is no port 1 on the 6474 */
    ibl.sgmiiConfig[1].configure = FALSE;

    /* MDIO configuration */
    ibl.mdioConfig.nMdioOps = 5;
    ibl.mdioConfig.mdioClkDiv = 0x20;
    ibl.mdioConfig.interDelay = 2000;   /* ~2ms at 1000 MHz */

    ibl.mdioConfig.mdio[0] =  (1 << 30) | ( 4 << 21) | (27 << 16) | 0x0081;
    ibl.mdioConfig.mdio[1] =  (1 << 30) | (26 << 21) | (14 << 16) | 0x0047;
    ibl.mdioConfig.mdio[2] =  (1 << 30) | ( 0 << 21) | (14 << 16) | 0x8140;

    ibl.mdioConfig.mdio[3] =  (1 << 30) | ( 1 << 21) | (22 << 16) | 0x043e;
    ibl.mdioConfig.mdio[4] =  (1 << 30) | ( 0 << 21) | ( 1 << 16) | 0x9140;

    /* spiConfig and emifConfig not needed */

    /* Ethernet configuration for Boot mode 0 */
    ibl.bootModes[0].bootMode = ibl_BOOT_MODE_TFTP;
    ibl.bootModes[0].priority = ibl_HIGHEST_PRIORITY;
    ibl.bootModes[0].port     = 0;

    /* Bootp is disabled. The server and file name are provided here */
    ibl.bootModes[0].u.ethBoot.doBootp          = TRUE;
    ibl.bootModes[0].u.ethBoot.useBootpServerIp = TRUE;
    ibl.bootModes[0].u.ethBoot.useBootpFileName = TRUE;
    ibl.bootModes[0].u.ethBoot.bootFormat       = ibl_BOOT_FORMAT_BBLOB;

    SETIP(ibl.bootModes[0].u.ethBoot.ethInfo.ipAddr,    192,168,1,3);
    SETIP(ibl.bootModes[0].u.ethBoot.ethInfo.serverIp,  192,168,1,2);
    SETIP(ibl.bootModes[0].u.ethBoot.ethInfo.gatewayIp, 192,168,1,1);
    SETIP(ibl.bootModes[0].u.ethBoot.ethInfo.netmask,   255,255,255,0);

    /* Set the hardware address as 0 so the e-fuse value will be used */
    ibl.bootModes[0].u.ethBoot.ethInfo.hwAddress[0] = 0;
    ibl.bootModes[0].u.ethBoot.ethInfo.hwAddress[1] = 0;
    ibl.bootModes[0].u.ethBoot.ethInfo.hwAddress[2] = 0;
    ibl.bootModes[0].u.ethBoot.ethInfo.hwAddress[3] = 0;
    ibl.bootModes[0].u.ethBoot.ethInfo.hwAddress[4] = 0;
    ibl.bootModes[0].u.ethBoot.ethInfo.hwAddress[5] = 0;


    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[0]  = 'c';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[1]  = '6';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[2]  = '4';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[3]  = '7';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[4]  = '4';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[5]  = 'l';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[6]  = '-';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[7]  = 'l';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[8]  = 'e';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[9]  = '.';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[10] = 'b';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[11] = 'i';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[12] = 'n';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[13] = '\0';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[14] = '\0';


    /* Even though the entire range of DDR2 is chosen, the load will
     * stop when the ftp reaches the end of the file */
    ibl.bootModes[0].u.ethBoot.blob.startAddress  = 0x80000000;       /* Base address of DDR2 */
    ibl.bootModes[0].u.ethBoot.blob.sizeBytes     = 0x20000000;       /* All of DDR2 */
    ibl.bootModes[0].u.ethBoot.blob.branchAddress = 0x80000000;       /* Base of DDR2 */

    ibl.bootModes[1].bootMode = ibl_BOOT_MODE_NAND;
    ibl.bootModes[1].priority = ibl_HIGHEST_PRIORITY;
    ibl.bootModes[1].port     = 0;

    ibl.bootModes[1].u.nandBoot.bootFormat		= ibl_BOOT_FORMAT_BBLOB;
    ibl.bootModes[1].u.nandBoot.bootAddress[0][0]	  = 0x200000;         /* Image 0 NAND offset address (block 1) in LE mode */
    ibl.bootModes[1].u.nandBoot.bootAddress[0][1]	  = 0x4000000;      /* Image 1 NAND offset address (block 2048) in LE mode */
    ibl.bootModes[1].u.nandBoot.bootAddress[1][0]	  = 0x200000;	    /* Image 0 NAND offset address (block 1) in BE mode */
    ibl.bootModes[1].u.nandBoot.bootAddress[1][1]	  = 0x4000000;      /* Image 1 NAND offset address (block 2048) in BE mode */
    ibl.bootModes[1].u.nandBoot.interface         = ibl_PMEM_IF_GPIO;
    
    ibl.bootModes[1].u.nandBoot.blob[0][0].startAddress  = 0x80000000;       /* Image 0 load start address in LE mode */
    ibl.bootModes[1].u.nandBoot.blob[0][0].sizeBytes     = 0xA00000;         /* Image 0 size (10 MB) in LE mode */
    ibl.bootModes[1].u.nandBoot.blob[0][0].branchAddress = 0x80000000;       /* Image 0 branch address after loading in LE mode */
    ibl.bootModes[1].u.nandBoot.blob[0][1].startAddress  = 0x80000000;       /* Image 1 load start address in LE mode */
    ibl.bootModes[1].u.nandBoot.blob[0][1].sizeBytes     = 0xA00000;         /* Image 1 size (10 MB) in LE mode */
    ibl.bootModes[1].u.nandBoot.blob[0][1].branchAddress = 0x80000000;       /* Image 1 branch address after loading in LE mode */
    ibl.bootModes[1].u.nandBoot.blob[1][0].startAddress  = 0x80000000;       /* Image 0 load start address in BE mode */
    ibl.bootModes[1].u.nandBoot.blob[1][0].sizeBytes     = 0xA00000;         /* Image 0 size (10 MB) in BE mode */
    ibl.bootModes[1].u.nandBoot.blob[1][0].branchAddress = 0x80000000;       /* Image 0 branch address after loading in BE mode */
    ibl.bootModes[1].u.nandBoot.blob[1][1].startAddress  = 0x80000000;       /* Image 1 load start address in BE mode */
    ibl.bootModes[1].u.nandBoot.blob[1][1].sizeBytes     = 0xA00000;         /* Image 1 size (10 MB) in BE mode */
    ibl.bootModes[1].u.nandBoot.blob[1][1].branchAddress = 0x80000000;       /* Image 1 branch address after loading in BE mode */


    ibl.bootModes[1].u.nandBoot.nandInfo.busWidthBits  = 8;
    ibl.bootModes[1].u.nandBoot.nandInfo.pageSizeBytes = 2048;
    ibl.bootModes[1].u.nandBoot.nandInfo.pageEccBytes  = 64;
    ibl.bootModes[1].u.nandBoot.nandInfo.pagesPerBlock = 64;
    ibl.bootModes[1].u.nandBoot.nandInfo.totalBlocks   = 512;

    ibl.bootModes[1].u.nandBoot.nandInfo.addressBytes  = 4;
    ibl.bootModes[1].u.nandBoot.nandInfo.lsbFirst      = TRUE;
    ibl.bootModes[1].u.nandBoot.nandInfo.blockOffset   = 22;
    ibl.bootModes[1].u.nandBoot.nandInfo.pageOffset    = 16;
    ibl.bootModes[1].u.nandBoot.nandInfo.columnOffset  = 0;

    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[0]  = 0; 
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[1]  = 1; 
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[2]  = 2; 
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[3]  = 3; 
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[4]  = 4; 
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[5]  = 6; 
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[6]  = 7; 
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[7]  = 13; 
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[8]  = 14; 
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[9]  = 15; 

    ibl.bootModes[1].u.nandBoot.nandInfo.badBlkMarkIdx[0]= 0;
    ibl.bootModes[1].u.nandBoot.nandInfo.badBlkMarkIdx[1]= 0xff;

    ibl.bootModes[1].u.nandBoot.nandInfo.resetCommand    = 0xff;
    ibl.bootModes[1].u.nandBoot.nandInfo.readCommandPre  = 0;
    ibl.bootModes[1].u.nandBoot.nandInfo.readCommandPost = 0;
    ibl.bootModes[1].u.nandBoot.nandInfo.postCommand     = FALSE;


    /* bootMode[2] not configured */
    ibl.bootModes[2].bootMode = ibl_BOOT_MODE_NONE;

    ibl.chkSum = 0;

    return(ibl);
}

ibl_t c6457_ibl_config(void)
{
    ibl_t ibl;
    memset(&ibl, 0, sizeof(ibl_t));
	
    ibl.iblMagic = ibl_MAGIC_VALUE;
    ibl.iblEvmType = ibl_EVM_C6457L;

    ibl.pllConfig[ibl_MAIN_PLL].doEnable      = TRUE;
    ibl.pllConfig[ibl_MAIN_PLL].prediv        = 1;
    ibl.pllConfig[ibl_MAIN_PLL].mult          = 20;
    ibl.pllConfig[ibl_MAIN_PLL].postdiv       = 1;
    ibl.pllConfig[ibl_MAIN_PLL].pllOutFreqMhz = 1000;

    /* The DDR PLL. The multipliers/dividers are fixed, so are really dont cares */
    ibl.pllConfig[ibl_DDR_PLL].doEnable = TRUE;

    /* The network PLL. The multipliers/dividers are fixed */
    ibl.pllConfig[ibl_NET_PLL].doEnable = TRUE;

    /* EMIF configuration */
    ibl.ddrConfig.configDdr = TRUE;

    ibl.ddrConfig.uEmif.emif3p1.sdcfg  = 0x00d38a32; /* cas5, 8 banks, 10 bit column */
    ibl.ddrConfig.uEmif.emif3p1.sdrfc  = 0x00000a0e; /* Refresh 333Mhz */ 
    ibl.ddrConfig.uEmif.emif3p1.sdtim1 = 0x832474da; /* Timing 1 */
    ibl.ddrConfig.uEmif.emif3p1.sdtim2 = 0x0144c742; /* Timing 2 */
    ibl.ddrConfig.uEmif.emif3p1.dmcctl = 0x001800C6;

    /* SGMII 0 is present */
    ibl.sgmiiConfig[0].configure = TRUE;
    ibl.sgmiiConfig[0].adviseAbility = 0x9801;
    ibl.sgmiiConfig[0].control       = 0x20;
    ibl.sgmiiConfig[0].txConfig      = 0x00000e21;
    ibl.sgmiiConfig[0].rxConfig      = 0x00081021;
    ibl.sgmiiConfig[0].auxConfig     = 0x0000000b;

    /* There is no port 1 on the 6457 */
    ibl.sgmiiConfig[1].configure = FALSE;

    /* MDIO configuration */
    ibl.mdioConfig.nMdioOps = 5;
    ibl.mdioConfig.mdioClkDiv = 0xa5;
    ibl.mdioConfig.interDelay = 3000;   /* ~2ms at 1000 MHz */

    ibl.mdioConfig.mdio[0] =  (1 << 30) | ( 4 << 21) | (27 << 16) | 0x0081;
    ibl.mdioConfig.mdio[1] =  (1 << 30) | (26 << 21) | (14 << 16) | 0x0047;
    ibl.mdioConfig.mdio[2] =  (1 << 30) | ( 0 << 21) | (14 << 16) | 0x8140;
    ibl.mdioConfig.mdio[3] =  (1 << 30) | ( 1 << 21) | (22 << 16) | 0x043e;
    ibl.mdioConfig.mdio[4] =  (1 << 30) | ( 0 << 21) | ( 1 << 16) | 0x9140;

    /* spiConfig and emifConfig not needed */

    /* Ethernet configuration for Boot mode 0 */
    ibl.bootModes[0].bootMode = ibl_BOOT_MODE_TFTP;
    ibl.bootModes[0].priority = ibl_HIGHEST_PRIORITY;
    ibl.bootModes[0].port = 0;

    /* Bootp is disabled. The server and file name are provided here */
    ibl.bootModes[0].u.ethBoot.doBootp          = TRUE;
    ibl.bootModes[0].u.ethBoot.useBootpServerIp = TRUE;
    ibl.bootModes[0].u.ethBoot.useBootpFileName = TRUE;
    ibl.bootModes[0].u.ethBoot.bootFormat       = ibl_BOOT_FORMAT_BBLOB;

    SETIP(ibl.bootModes[0].u.ethBoot.ethInfo.ipAddr,    192,168,1,3);
    SETIP(ibl.bootModes[0].u.ethBoot.ethInfo.serverIp,  192,168,1,2);
    SETIP(ibl.bootModes[0].u.ethBoot.ethInfo.gatewayIp, 192,168,1,1);
    SETIP(ibl.bootModes[0].u.ethBoot.ethInfo.netmask,   255,255,255,0);

    /* Set the hardware address as 0 so the e-fuse value will be used */
    ibl.bootModes[0].u.ethBoot.ethInfo.hwAddress[0] = 0;
    ibl.bootModes[0].u.ethBoot.ethInfo.hwAddress[1] = 0;
    ibl.bootModes[0].u.ethBoot.ethInfo.hwAddress[2] = 0;
    ibl.bootModes[0].u.ethBoot.ethInfo.hwAddress[3] = 0;
    ibl.bootModes[0].u.ethBoot.ethInfo.hwAddress[4] = 0;
    ibl.bootModes[0].u.ethBoot.ethInfo.hwAddress[5] = 0;

    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[0]  = 'c';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[1]  = '6';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[2]  = '4';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[3]  = '5';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[4]  = '7';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[5]  = '-';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[6]  = 'l';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[7]  = 'e';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[8]  = '.';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[9]  = 'b';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[10] = 'i';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[11] = 'n';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[12] = '\0';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[13] = '\0';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[14] = '\0';


    /* Even though the entire range of DDR2 is chosen, the load will
     * stop when the ftp reaches the end of the file */
    ibl.bootModes[0].u.ethBoot.blob.startAddress  = 0xe0000000;       /* Base address of DDR2 */
    ibl.bootModes[0].u.ethBoot.blob.sizeBytes     = 0x20000000;       /* All of DDR2 */
    ibl.bootModes[0].u.ethBoot.blob.branchAddress = 0xe0000000;       /* Base of DDR2 */

    ibl.bootModes[1].bootMode = ibl_BOOT_MODE_NAND;
    ibl.bootModes[1].priority = ibl_HIGHEST_PRIORITY;
    ibl.bootModes[1].port     = 0;

    ibl.bootModes[1].u.nandBoot.bootFormat		= ibl_BOOT_FORMAT_BBLOB;
    ibl.bootModes[1].u.nandBoot.bootAddress[0][0]	  = 0x200000;         /* Image 0 NAND offset address (block 1) in LE mode */
    ibl.bootModes[1].u.nandBoot.bootAddress[0][1]	  = 0x4000000;      /* Image 1 NAND offset address (block 2048) in LE mode */
    ibl.bootModes[1].u.nandBoot.bootAddress[1][0]	  = 0x200000;	    /* Image 0 NAND offset address (block 1) in BE mode */
    ibl.bootModes[1].u.nandBoot.bootAddress[1][1]	  = 0x4000000;      /* Image 1 NAND offset address (block 2048) in BE mode */
    ibl.bootModes[1].u.nandBoot.interface         = ibl_PMEM_IF_GPIO;
    
    ibl.bootModes[1].u.nandBoot.blob[0][0].startAddress  = 0xe0000000;       /* Image 0 load start address in LE mode */
    ibl.bootModes[1].u.nandBoot.blob[0][0].sizeBytes     = 0xA00000;         /* Image 0 size (10 MB) in LE mode */
    ibl.bootModes[1].u.nandBoot.blob[0][0].branchAddress = 0xe0000000;       /* Image 0 branch address after loading in LE mode */
    ibl.bootModes[1].u.nandBoot.blob[0][1].startAddress  = 0xe0000000;       /* Image 1 load start address in LE mode */
    ibl.bootModes[1].u.nandBoot.blob[0][1].sizeBytes     = 0xA00000;         /* Image 1 size (10 MB) in LE mode */
    ibl.bootModes[1].u.nandBoot.blob[0][1].branchAddress = 0xe0000000;       /* Image 1 branch address after loading in LE mode */
    ibl.bootModes[1].u.nandBoot.blob[1][0].startAddress  = 0xe0000000;       /* Image 0 load start address in BE mode */
    ibl.bootModes[1].u.nandBoot.blob[1][0].sizeBytes     = 0xA00000;         /* Image 0 size (10 MB) in BE mode */
    ibl.bootModes[1].u.nandBoot.blob[1][0].branchAddress = 0xe0000000;       /* Image 0 branch address after loading in BE mode */
    ibl.bootModes[1].u.nandBoot.blob[1][1].startAddress  = 0xe0000000;       /* Image 1 load start address in BE mode */
    ibl.bootModes[1].u.nandBoot.blob[1][1].sizeBytes     = 0xA00000;         /* Image 1 size (10 MB) in BE mode */
    ibl.bootModes[1].u.nandBoot.blob[1][1].branchAddress = 0xe0000000;       /* Image 1 branch address after loading in BE mode */


    ibl.bootModes[1].u.nandBoot.nandInfo.busWidthBits  = 8;
    ibl.bootModes[1].u.nandBoot.nandInfo.pageSizeBytes = 2048;
    ibl.bootModes[1].u.nandBoot.nandInfo.pageEccBytes  = 64;
    ibl.bootModes[1].u.nandBoot.nandInfo.pagesPerBlock = 64;
    ibl.bootModes[1].u.nandBoot.nandInfo.totalBlocks   = 512;

    ibl.bootModes[1].u.nandBoot.nandInfo.addressBytes  = 4;
    ibl.bootModes[1].u.nandBoot.nandInfo.lsbFirst      = TRUE;
    ibl.bootModes[1].u.nandBoot.nandInfo.blockOffset   = 22;
    ibl.bootModes[1].u.nandBoot.nandInfo.pageOffset    = 16;
    ibl.bootModes[1].u.nandBoot.nandInfo.columnOffset  = 0;

    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[0]  = 0; 
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[1]  = 1; 
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[2]  = 2; 
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[3]  = 3; 
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[4]  = 4; 
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[5]  = 6; 
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[6]  = 7; 
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[7]  = 13; 
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[8]  = 14; 
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[9]  = 15; 

    ibl.bootModes[1].u.nandBoot.nandInfo.badBlkMarkIdx[0]= 0;
    ibl.bootModes[1].u.nandBoot.nandInfo.badBlkMarkIdx[1]= 0xff;

    ibl.bootModes[1].u.nandBoot.nandInfo.resetCommand    = 0xff;
    ibl.bootModes[1].u.nandBoot.nandInfo.readCommandPre  = 0;
    ibl.bootModes[1].u.nandBoot.nandInfo.readCommandPost = 0;
    ibl.bootModes[1].u.nandBoot.nandInfo.postCommand     = FALSE;


    /* bootMode[2] not configured */
    ibl.bootModes[2].bootMode = ibl_BOOT_MODE_NONE;

    ibl.chkSum = 0;

    return(ibl);
}

ibl_t c6472_ibl_config(void)
{
    ibl_t ibl;
    memset(&ibl, 0, sizeof(ibl_t));
	
    ibl.iblMagic = ibl_MAGIC_VALUE;
    ibl.iblEvmType = ibl_EVM_C6472L;

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

    /* SGMII not present */
    ibl.sgmiiConfig[0].configure = FALSE;
    ibl.sgmiiConfig[1].configure = FALSE;

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

    /* spiConfig and emifConfig not needed */

    /* Ethernet configuration for Boot mode 0 */
    ibl.bootModes[0].bootMode = ibl_BOOT_MODE_TFTP;
    ibl.bootModes[0].priority = ibl_HIGHEST_PRIORITY;
    ibl.bootModes[0].port     = 0;

    /* Bootp is disabled. The server and file name are provided here */
    ibl.bootModes[0].u.ethBoot.doBootp          = TRUE;
    ibl.bootModes[0].u.ethBoot.useBootpServerIp = TRUE;
    ibl.bootModes[0].u.ethBoot.useBootpFileName = TRUE;
    ibl.bootModes[0].u.ethBoot.bootFormat       = ibl_BOOT_FORMAT_BBLOB;

    /* Even though the entire range of DDR2 is chosen, the load will
     * stop when the ftp reaches the end of the file */
    ibl.bootModes[0].u.ethBoot.blob.startAddress  = 0xe0000000;       /* Base address of DDR2 */
    ibl.bootModes[0].u.ethBoot.blob.sizeBytes     = 0x20000000;       /* All of DDR2 */
    ibl.bootModes[0].u.ethBoot.blob.branchAddress = 0xe0000000;       /* Base of DDR2 */

    SETIP(ibl.bootModes[0].u.ethBoot.ethInfo.ipAddr,    192,168,1,3);
    SETIP(ibl.bootModes[0].u.ethBoot.ethInfo.serverIp,  192,168,1,2);
    SETIP(ibl.bootModes[0].u.ethBoot.ethInfo.gatewayIp, 192,168,1,1);
    SETIP(ibl.bootModes[0].u.ethBoot.ethInfo.netmask,   255,255,255,0);

    /* Leave the hardware address as 0 so the e-fuse value will be used */
    ibl.bootModes[0].u.ethBoot.ethInfo.hwAddress[0] = 0;
    ibl.bootModes[0].u.ethBoot.ethInfo.hwAddress[1] = 0;
    ibl.bootModes[0].u.ethBoot.ethInfo.hwAddress[2] = 0;
    ibl.bootModes[0].u.ethBoot.ethInfo.hwAddress[3] = 0;
    ibl.bootModes[0].u.ethBoot.ethInfo.hwAddress[4] = 0;
    ibl.bootModes[0].u.ethBoot.ethInfo.hwAddress[5] = 0;

    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[0]  = 'c';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[1]  = '6';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[2]  = '4';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[3]  = '7';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[4]  = '2';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[5]  = '-';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[6]  = 'l';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[7]  = 'e';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[8]  = '.';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[9]  = 'b';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[10] = 'i';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[11] = 'n';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[12] = '\0';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[13] = '\0';
    ibl.bootModes[0].u.ethBoot.ethInfo.fileName[14] = '\0';

    ibl.bootModes[1].bootMode = ibl_BOOT_MODE_NAND;
    ibl.bootModes[1].priority = ibl_HIGHEST_PRIORITY;
    ibl.bootModes[1].port     = 0;

    ibl.bootModes[1].u.nandBoot.bootFormat		= ibl_BOOT_FORMAT_BBLOB;
    ibl.bootModes[1].u.nandBoot.bootAddress[0][0]	  = 0x200000;         /* Image 0 NAND offset address (block 1) in LE mode */
    ibl.bootModes[1].u.nandBoot.bootAddress[0][1]	  = 0x4000000;      /* Image 1 NAND offset address (block 2048) in LE mode */
    ibl.bootModes[1].u.nandBoot.bootAddress[1][0]	  = 0x200000;	    /* Image 0 NAND offset address (block 1) in BE mode */
    ibl.bootModes[1].u.nandBoot.bootAddress[1][1]	  = 0x4000000;      /* Image 1 NAND offset address (block 2048) in BE mode */
    ibl.bootModes[1].u.nandBoot.interface         = ibl_PMEM_IF_GPIO;

    ibl.bootModes[1].u.nandBoot.blob[0][0].startAddress  = 0xe0000000;       /* Image 0 load start address in LE mode */
    ibl.bootModes[1].u.nandBoot.blob[0][0].sizeBytes     = 0xA00000;         /* Image 0 size (10 MB) in LE mode */
    ibl.bootModes[1].u.nandBoot.blob[0][0].branchAddress = 0xe0000000;       /* Image 0 branch address after loading in LE mode */
    ibl.bootModes[1].u.nandBoot.blob[0][1].startAddress  = 0xe0000000;       /* Image 1 load start address in LE mode */
    ibl.bootModes[1].u.nandBoot.blob[0][1].sizeBytes     = 0xA00000;         /* Image 1 size (10 MB) in LE mode */
    ibl.bootModes[1].u.nandBoot.blob[0][1].branchAddress = 0xe0000000;       /* Image 1 branch address after loading in LE mode */
    ibl.bootModes[1].u.nandBoot.blob[1][0].startAddress  = 0xe0000000;       /* Image 0 load start address in BE mode */
    ibl.bootModes[1].u.nandBoot.blob[1][0].sizeBytes     = 0xA00000;         /* Image 0 size (10 MB) in BE mode */
    ibl.bootModes[1].u.nandBoot.blob[1][0].branchAddress = 0xe0000000;       /* Image 0 branch address after loading in BE mode */
    ibl.bootModes[1].u.nandBoot.blob[1][1].startAddress  = 0xe0000000;       /* Image 1 load start address in BE mode */
    ibl.bootModes[1].u.nandBoot.blob[1][1].sizeBytes     = 0xA00000;         /* Image 1 size (10 MB) in BE mode */
    ibl.bootModes[1].u.nandBoot.blob[1][1].branchAddress = 0xe0000000;       /* Image 1 branch address after loading in BE mode */


    ibl.bootModes[1].u.nandBoot.nandInfo.busWidthBits  = 8;
    ibl.bootModes[1].u.nandBoot.nandInfo.pageSizeBytes = 2048;
    ibl.bootModes[1].u.nandBoot.nandInfo.pageEccBytes  = 64;
    ibl.bootModes[1].u.nandBoot.nandInfo.pagesPerBlock = 64;
    ibl.bootModes[1].u.nandBoot.nandInfo.totalBlocks   = 512;

    ibl.bootModes[1].u.nandBoot.nandInfo.addressBytes  = 4;
    ibl.bootModes[1].u.nandBoot.nandInfo.lsbFirst      = TRUE;
    ibl.bootModes[1].u.nandBoot.nandInfo.blockOffset   = 22;
    ibl.bootModes[1].u.nandBoot.nandInfo.pageOffset    = 16;
    ibl.bootModes[1].u.nandBoot.nandInfo.columnOffset  = 0;

    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[0]  = 0; 
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[1]  = 1; 
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[2]  = 2; 
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[3]  = 3; 
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[4]  = 4; 
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[5]  = 6; 
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[6]  = 7; 
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[7]  = 13; 
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[8]  = 14; 
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[9]  = 15; 

    ibl.bootModes[1].u.nandBoot.nandInfo.badBlkMarkIdx[0]= 0;
    ibl.bootModes[1].u.nandBoot.nandInfo.badBlkMarkIdx[1]= 0xff;

    ibl.bootModes[1].u.nandBoot.nandInfo.resetCommand    = 0xff;
    ibl.bootModes[1].u.nandBoot.nandInfo.readCommandPre  = 0;
    ibl.bootModes[1].u.nandBoot.nandInfo.readCommandPost = 0;
    ibl.bootModes[1].u.nandBoot.nandInfo.postCommand     = FALSE;

    /* bootMode[2] not configured */
    ibl.bootModes[2].bootMode = ibl_BOOT_MODE_NONE;
    
    ibl.chkSum = 0;

    return(ibl);
}

ibl_t c6678_ibl_config(void)
{
	ibl_t ibl;
    memset(&ibl, 0, sizeof(ibl_t));
    
	ibl.iblMagic = ibl_MAGIC_VALUE;
	ibl.iblEvmType = ibl_EVM_C6678L;

	/* Main PLL: 100 MHz reference, 1GHz output */
	ibl.pllConfig[ibl_MAIN_PLL].doEnable      = 1;
	ibl.pllConfig[ibl_MAIN_PLL].prediv        = 1;
	ibl.pllConfig[ibl_MAIN_PLL].mult          = 20;
	ibl.pllConfig[ibl_MAIN_PLL].postdiv       = 2;
	ibl.pllConfig[ibl_MAIN_PLL].pllOutFreqMhz = 1000;

	/* DDR PLL: */
	ibl.pllConfig[ibl_DDR_PLL].doEnable       = 1; 
	ibl.pllConfig[ibl_DDR_PLL].prediv         = 1;
	ibl.pllConfig[ibl_DDR_PLL].mult           = 20;
	ibl.pllConfig[ibl_DDR_PLL].postdiv        = 2;
	ibl.pllConfig[ibl_DDR_PLL].pllOutFreqMhz  = 1333;

    /* Net PLL: 100 MHz reference, 1050 MHz output (followed by a built in divide by 3 to give 350 MHz to PA) */
	ibl.pllConfig[ibl_NET_PLL].doEnable       = 1;
	ibl.pllConfig[ibl_NET_PLL].prediv         = 1;
	ibl.pllConfig[ibl_NET_PLL].mult			  = 21;
	ibl.pllConfig[ibl_NET_PLL].postdiv        = 2;
	ibl.pllConfig[ibl_NET_PLL].pllOutFreqMhz  = 1050;


	ibl.ddrConfig.configDdr = 1;
	ibl.ddrConfig.uEmif.emif4p0.registerMask = ibl_EMIF4_ENABLE_sdRamConfig | ibl_EMIF4_ENABLE_sdRamRefreshCtl | ibl_EMIF4_ENABLE_sdRamTiming1 | ibl_EMIF4_ENABLE_sdRamTiming2 | ibl_EMIF4_ENABLE_sdRamTiming3 | ibl_EMIF4_ENABLE_ddrPhyCtl1;

	ibl.ddrConfig.uEmif.emif4p0.sdRamConfig				= 0x63C452B2;
	ibl.ddrConfig.uEmif.emif4p0.sdRamConfig2			= 0;
	ibl.ddrConfig.uEmif.emif4p0.sdRamRefreshCtl			= 0x000030D4;
	ibl.ddrConfig.uEmif.emif4p0.sdRamTiming1			= 0x0AAAE51B;
	ibl.ddrConfig.uEmif.emif4p0.sdRamTiming2			= 0x2A2F7FDA;
	ibl.ddrConfig.uEmif.emif4p0.sdRamTiming3 			= 0x057F82B8;
	ibl.ddrConfig.uEmif.emif4p0.lpDdrNvmTiming			= 0;
	ibl.ddrConfig.uEmif.emif4p0.powerManageCtl			= 0;
	ibl.ddrConfig.uEmif.emif4p0.iODFTTestLogic			= 0;
	ibl.ddrConfig.uEmif.emif4p0.performCountCfg			= 0;
	ibl.ddrConfig.uEmif.emif4p0.performCountMstRegSel	= 0;
	ibl.ddrConfig.uEmif.emif4p0.readIdleCtl				= 0;
	ibl.ddrConfig.uEmif.emif4p0.sysVbusmIntEnSet		= 0;
	ibl.ddrConfig.uEmif.emif4p0.sdRamOutImpdedCalCfg	= 0;
	ibl.ddrConfig.uEmif.emif4p0.tempAlterCfg			= 0;
	ibl.ddrConfig.uEmif.emif4p0.ddrPhyCtl1 				= 0x0010010d;
	ibl.ddrConfig.uEmif.emif4p0.ddrPhyCtl2				= 0;
	ibl.ddrConfig.uEmif.emif4p0.priClassSvceMap			= 0;
	ibl.ddrConfig.uEmif.emif4p0.mstId2ClsSvce1Map		= 0;
	ibl.ddrConfig.uEmif.emif4p0.mstId2ClsSvce2Map		= 0;
	ibl.ddrConfig.uEmif.emif4p0.eccCtl					= 0;
	ibl.ddrConfig.uEmif.emif4p0.eccRange1				= 0;
	ibl.ddrConfig.uEmif.emif4p0.eccRange2				= 0;
	ibl.ddrConfig.uEmif.emif4p0.rdWrtExcThresh			= 0;


	ibl.sgmiiConfig[0].configure     = 1;
	ibl.sgmiiConfig[0].adviseAbility = 1;
	ibl.sgmiiConfig[0].control		 = 1;
	ibl.sgmiiConfig[0].txConfig      = 0x108a1;
	ibl.sgmiiConfig[0].rxConfig      = 0x700621;
	ibl.sgmiiConfig[0].auxConfig	 = 0x41;

	ibl.sgmiiConfig[1].configure     = 1;
	ibl.sgmiiConfig[1].adviseAbility = 1;
	ibl.sgmiiConfig[1].control		 = 1;
	ibl.sgmiiConfig[1].txConfig      = 0x108a1;
	ibl.sgmiiConfig[1].rxConfig      = 0x700621;
	ibl.sgmiiConfig[1].auxConfig	 = 0x41;

	ibl.mdioConfig.nMdioOps = 0;

	ibl.spiConfig.addrWidth  = 24;
	ibl.spiConfig.nPins      = 5;
	ibl.spiConfig.mode       = 1;
	ibl.spiConfig.csel       = 2;
	ibl.spiConfig.c2tdelay   = 1;
	ibl.spiConfig.busFreqMHz = 20;

	ibl.emifConfig[0].csSpace    = 2;
	ibl.emifConfig[0].busWidth   = 8;
	ibl.emifConfig[0].waitEnable = 0;

	ibl.emifConfig[1].csSpace    = 0;
	ibl.emifConfig[1].busWidth   = 0;
	ibl.emifConfig[1].waitEnable = 0;

	ibl.bootModes[0].bootMode = ibl_BOOT_MODE_NOR;
	ibl.bootModes[0].priority = ibl_HIGHEST_PRIORITY;
	ibl.bootModes[0].port     = 0;

	ibl.bootModes[0].u.norBoot.bootFormat	= ibl_BOOT_FORMAT_ELF;
	ibl.bootModes[0].u.norBoot.bootAddress[0][0]	= 0;		/* Image 0 NOR offset byte address in LE mode */ 
	ibl.bootModes[0].u.norBoot.bootAddress[0][1]	= 0xA00000;	/* Image 1 NOR offset byte address in LE mode  */
	ibl.bootModes[0].u.norBoot.bootAddress[1][0]	= 0;		/* Image 0 NOR offset byte address in BE mode */ 
	ibl.bootModes[0].u.norBoot.bootAddress[1][1]	= 0xA00000;	/* Image 1 NOR offset byte address in BE mode  */
	ibl.bootModes[0].u.norBoot.interface	= ibl_PMEM_IF_SPI;
    ibl.bootModes[0].u.norBoot.blob[0][0].startAddress  = 0x80000000;       /* Image 0 load start address in LE mode */
    ibl.bootModes[0].u.norBoot.blob[0][0].sizeBytes     = 0xA00000;         /* Image 0 size (10 MB) in LE mode */
    ibl.bootModes[0].u.norBoot.blob[0][0].branchAddress = 0x80000000;       /* Image 0 branch address after loading in LE mode */
    ibl.bootModes[0].u.norBoot.blob[0][1].startAddress  = 0x80000000;       /* Image 1 load start address in LE mode */
    ibl.bootModes[0].u.norBoot.blob[0][1].sizeBytes     = 0xA00000;         /* Image 1 size (10 MB) in LE mode */
    ibl.bootModes[0].u.norBoot.blob[0][1].branchAddress = 0x80000000;       /* Image 1 branch address after loading in LE mode */
    ibl.bootModes[0].u.norBoot.blob[1][0].startAddress  = 0x80000000;       /* Image 0 load start address in BE mode */
    ibl.bootModes[0].u.norBoot.blob[1][0].sizeBytes     = 0xA00000;         /* Image 0 size (10 MB) in BE mode */
    ibl.bootModes[0].u.norBoot.blob[1][0].branchAddress = 0x80000000;       /* Image 0 branch address after loading in BE mode */
    ibl.bootModes[0].u.norBoot.blob[1][1].startAddress  = 0x80000000;       /* Image 1 load start address in BE mode */
    ibl.bootModes[0].u.norBoot.blob[1][1].sizeBytes     = 0xA00000;         /* Image 1 size (10 MB) in BE mode */
    ibl.bootModes[0].u.norBoot.blob[1][1].branchAddress = 0x80000000;       /* Image 1 branch address after loading in BE mode */

    ibl.bootModes[1].bootMode = ibl_BOOT_MODE_NAND;
    ibl.bootModes[1].priority = ibl_HIGHEST_PRIORITY;
    ibl.bootModes[1].port     = 0;

    ibl.bootModes[1].u.nandBoot.bootFormat        = ibl_BOOT_FORMAT_BBLOB;
    ibl.bootModes[1].u.nandBoot.bootAddress[0][0]	  = 0x4000;         /* Image 0 NAND offset address (block 1) in LE mode */
    ibl.bootModes[1].u.nandBoot.bootAddress[0][1]	  = 0x2000000;      /* Image 1 NAND offset address (block 2048) in LE mode */
    ibl.bootModes[1].u.nandBoot.bootAddress[1][0]	  = 0x4000;	    /* Image 0 NAND offset address (block 1) in BE mode */
    ibl.bootModes[1].u.nandBoot.bootAddress[1][1]	  = 0x2000000;      /* Image 1 NAND offset address (block 2048) in BE mode */
    ibl.bootModes[1].u.nandBoot.interface         = ibl_PMEM_IF_CHIPSEL_2;

    ibl.bootModes[1].u.nandBoot.blob[0][0].startAddress  = 0x80000000;       /* Image 0 load start address in LE mode */
    ibl.bootModes[1].u.nandBoot.blob[0][0].sizeBytes     = 0xFFC000;         /* Image 0 size in LE mode */
    ibl.bootModes[1].u.nandBoot.blob[0][0].branchAddress = 0x80000000;       /* Image 0 branch address after loading in LE mode */
    ibl.bootModes[1].u.nandBoot.blob[0][1].startAddress  = 0x80000000;       /* Image 1 load start address in LE mode */
    ibl.bootModes[1].u.nandBoot.blob[0][1].sizeBytes     = 0xFFC000;         /* Image 1 size in LE mode */
    ibl.bootModes[1].u.nandBoot.blob[0][1].branchAddress = 0x80000000;       /* Image 1 branch address after loading in LE mode */
    ibl.bootModes[1].u.nandBoot.blob[1][0].startAddress  = 0x80000000;       /* Image 0 load start address in BE mode */
    ibl.bootModes[1].u.nandBoot.blob[1][0].sizeBytes     = 0xFFC000;         /* Image 0 size in BE mode */
    ibl.bootModes[1].u.nandBoot.blob[1][0].branchAddress = 0x80000000;       /* Image 0 branch address after loading in BE mode */
    ibl.bootModes[1].u.nandBoot.blob[1][1].startAddress  = 0x80000000;       /* Image 1 load start address in BE mode */
    ibl.bootModes[1].u.nandBoot.blob[1][1].sizeBytes     = 0xFFC000;         /* Image 1 size in BE mode */
    ibl.bootModes[1].u.nandBoot.blob[1][1].branchAddress = 0x80000000;       /* Image 1 branch address after loading in BE mode */


    ibl.bootModes[1].u.nandBoot.nandInfo.busWidthBits  = 8;
    ibl.bootModes[1].u.nandBoot.nandInfo.pageSizeBytes = 512;
    ibl.bootModes[1].u.nandBoot.nandInfo.pageEccBytes  = 16;
    ibl.bootModes[1].u.nandBoot.nandInfo.pagesPerBlock = 32;
    ibl.bootModes[1].u.nandBoot.nandInfo.totalBlocks   = 4096;

    ibl.bootModes[1].u.nandBoot.nandInfo.addressBytes  = 4;
    ibl.bootModes[1].u.nandBoot.nandInfo.lsbFirst      = TRUE;
    ibl.bootModes[1].u.nandBoot.nandInfo.blockOffset   = 14;
    ibl.bootModes[1].u.nandBoot.nandInfo.pageOffset    = 9;
    ibl.bootModes[1].u.nandBoot.nandInfo.columnOffset  = 0;

    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[0]  = 0; 
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[1]  = 1; 
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[2]  = 2; 
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[3]  = 3; 
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[4]  = 4; 
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[5]  = 6; 
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[6]  = 7; 
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[7]  = 13; 
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[8]  = 14; 
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[9]  = 15; 
    
    ibl.bootModes[1].u.nandBoot.nandInfo.badBlkMarkIdx[0]= 5;
    ibl.bootModes[1].u.nandBoot.nandInfo.badBlkMarkIdx[1]= 0xff;

    ibl.bootModes[1].u.nandBoot.nandInfo.resetCommand    = 0xff;
    ibl.bootModes[1].u.nandBoot.nandInfo.readCommandPre  = 0;
    ibl.bootModes[1].u.nandBoot.nandInfo.readCommandPost = 0;
    ibl.bootModes[1].u.nandBoot.nandInfo.postCommand     = FALSE;
    
	ibl.bootModes[2].bootMode = ibl_BOOT_MODE_TFTP;
	ibl.bootModes[2].priority = ibl_HIGHEST_PRIORITY+1;
	ibl.bootModes[2].port     = ibl_PORT_SWITCH_ALL;

	ibl.bootModes[2].u.ethBoot.doBootp          = TRUE;
	ibl.bootModes[2].u.ethBoot.useBootpServerIp = TRUE;
	ibl.bootModes[2].u.ethBoot.useBootpFileName = TRUE;
	ibl.bootModes[2].u.ethBoot.bootFormat       = ibl_BOOT_FORMAT_BBLOB;


    SETIP(ibl.bootModes[2].u.ethBoot.ethInfo.ipAddr,    192,168,1,3);
    SETIP(ibl.bootModes[2].u.ethBoot.ethInfo.serverIp,  192,168,1,2);
    SETIP(ibl.bootModes[2].u.ethBoot.ethInfo.gatewayIp, 192,168,1,1);
    SETIP(ibl.bootModes[2].u.ethBoot.ethInfo.netmask,   255,255,255,0);

    /* Use the e-fuse value */
    ibl.bootModes[2].u.ethBoot.ethInfo.hwAddress[0] = 0;
    ibl.bootModes[2].u.ethBoot.ethInfo.hwAddress[1] = 0;
    ibl.bootModes[2].u.ethBoot.ethInfo.hwAddress[2] = 0;
    ibl.bootModes[2].u.ethBoot.ethInfo.hwAddress[3] = 0;
    ibl.bootModes[2].u.ethBoot.ethInfo.hwAddress[4] = 0;
    ibl.bootModes[2].u.ethBoot.ethInfo.hwAddress[5] = 0;


    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[0]  = 'c';
    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[1]  = '6';
    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[2]  = '6';
    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[3]  = '7';
    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[4]  = '8';
    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[5]  = '-';
    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[6]  = 'l';
    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[7]  = 'e';
    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[8]  = '.';
    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[9]  = 'b';
    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[10] = 'i';
    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[11] = 'n';
    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[12] = '\0';
    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[13] = '\0';
    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[14] = '\0';

    ibl.bootModes[2].u.ethBoot.blob.startAddress  = 0x80000000;       /* Load start address */
    ibl.bootModes[2].u.ethBoot.blob.sizeBytes     = 0x20000000;
    ibl.bootModes[2].u.ethBoot.blob.branchAddress = 0x80000000;       /* Branch address after loading */

    ibl.chkSum = 0;

    return(ibl);
}

ibl_t c6670_ibl_config(void)
{
	ibl_t ibl;
    memset(&ibl, 0, sizeof(ibl_t));
	
	ibl.iblMagic = ibl_MAGIC_VALUE;
	ibl.iblEvmType = ibl_EVM_C6670L;

	/* Main PLL: 122.88 MHz reference, 983 MHz output */
	ibl.pllConfig[ibl_MAIN_PLL].doEnable      = 1;
	ibl.pllConfig[ibl_MAIN_PLL].prediv        = 1;
	ibl.pllConfig[ibl_MAIN_PLL].mult          = 16;
	ibl.pllConfig[ibl_MAIN_PLL].postdiv       = 2;
	ibl.pllConfig[ibl_MAIN_PLL].pllOutFreqMhz = 983;

	/* DDR PLL */
	ibl.pllConfig[ibl_DDR_PLL].doEnable       = 1; 
	ibl.pllConfig[ibl_DDR_PLL].prediv         = 1;
	ibl.pllConfig[ibl_DDR_PLL].mult           = 20;
	ibl.pllConfig[ibl_DDR_PLL].postdiv        = 2;
	ibl.pllConfig[ibl_DDR_PLL].pllOutFreqMhz  = 1333;

    /* Net PLL: 122.88 MHz reference, 1044 MHz output (followed by a built in divide by 3 to give 348 MHz to PA) */
	ibl.pllConfig[ibl_NET_PLL].doEnable       = 1;
	ibl.pllConfig[ibl_NET_PLL].prediv         = 1;
	ibl.pllConfig[ibl_NET_PLL].mult           = 17;
	ibl.pllConfig[ibl_NET_PLL].postdiv        = 2;
	ibl.pllConfig[ibl_NET_PLL].pllOutFreqMhz  = 1044;


	ibl.ddrConfig.configDdr = 1;
	ibl.ddrConfig.uEmif.emif4p0.registerMask = ibl_EMIF4_ENABLE_sdRamConfig | ibl_EMIF4_ENABLE_sdRamRefreshCtl | ibl_EMIF4_ENABLE_sdRamTiming1 | ibl_EMIF4_ENABLE_sdRamTiming2 | ibl_EMIF4_ENABLE_sdRamTiming3 | ibl_EMIF4_ENABLE_ddrPhyCtl1;

	ibl.ddrConfig.uEmif.emif4p0.sdRamConfig				= 0x63C452B2;
	ibl.ddrConfig.uEmif.emif4p0.sdRamConfig2			= 0;
	ibl.ddrConfig.uEmif.emif4p0.sdRamRefreshCtl			= 0x000030D4;
	ibl.ddrConfig.uEmif.emif4p0.sdRamTiming1			= 0x0AAAE51B;
	ibl.ddrConfig.uEmif.emif4p0.sdRamTiming2			= 0x2A2F7FDA;
	ibl.ddrConfig.uEmif.emif4p0.sdRamTiming3 			= 0x057F82B8;
	ibl.ddrConfig.uEmif.emif4p0.lpDdrNvmTiming			= 0;
	ibl.ddrConfig.uEmif.emif4p0.powerManageCtl			= 0;
	ibl.ddrConfig.uEmif.emif4p0.iODFTTestLogic			= 0;
	ibl.ddrConfig.uEmif.emif4p0.performCountCfg			= 0;
	ibl.ddrConfig.uEmif.emif4p0.performCountMstRegSel	= 0;
	ibl.ddrConfig.uEmif.emif4p0.readIdleCtl				= 0;
	ibl.ddrConfig.uEmif.emif4p0.sysVbusmIntEnSet		= 0;
	ibl.ddrConfig.uEmif.emif4p0.sdRamOutImpdedCalCfg	= 0;
	ibl.ddrConfig.uEmif.emif4p0.tempAlterCfg			= 0;
	ibl.ddrConfig.uEmif.emif4p0.ddrPhyCtl1 				= 0x0010010d;
	ibl.ddrConfig.uEmif.emif4p0.ddrPhyCtl2				= 0;
	ibl.ddrConfig.uEmif.emif4p0.priClassSvceMap			= 0;
	ibl.ddrConfig.uEmif.emif4p0.mstId2ClsSvce1Map		= 0;
	ibl.ddrConfig.uEmif.emif4p0.mstId2ClsSvce2Map		= 0;
	ibl.ddrConfig.uEmif.emif4p0.eccCtl					= 0;
	ibl.ddrConfig.uEmif.emif4p0.eccRange1				= 0;
	ibl.ddrConfig.uEmif.emif4p0.eccRange2				= 0;
	ibl.ddrConfig.uEmif.emif4p0.rdWrtExcThresh			= 0;


	ibl.sgmiiConfig[0].configure     = 1;
	ibl.sgmiiConfig[0].adviseAbility = 1;
	ibl.sgmiiConfig[0].control		 = 1;
	ibl.sgmiiConfig[0].txConfig      = 0x108a1;
	ibl.sgmiiConfig[0].rxConfig      = 0x700621;
	ibl.sgmiiConfig[0].auxConfig	 = 0x41;

	ibl.sgmiiConfig[1].configure     = 1;
	ibl.sgmiiConfig[1].adviseAbility = 1;
	ibl.sgmiiConfig[1].control		 = 1;
	ibl.sgmiiConfig[1].txConfig      = 0x108a1;
	ibl.sgmiiConfig[1].rxConfig      = 0x700621;
	ibl.sgmiiConfig[1].auxConfig	 = 0x51;

	ibl.mdioConfig.nMdioOps = 0;

	ibl.spiConfig.addrWidth  = 24;
	ibl.spiConfig.nPins      = 5;
	ibl.spiConfig.mode       = 1;
	ibl.spiConfig.csel       = 2;
	ibl.spiConfig.c2tdelay   = 1;
	ibl.spiConfig.busFreqMHz = 20;

	ibl.emifConfig[0].csSpace    = 2;
	ibl.emifConfig[0].busWidth   = 8;
	ibl.emifConfig[0].waitEnable = 0;

	ibl.emifConfig[1].csSpace    = 0;
	ibl.emifConfig[1].busWidth   = 0;
	ibl.emifConfig[1].waitEnable = 0;

	ibl.bootModes[0].bootMode = ibl_BOOT_MODE_NOR;
	ibl.bootModes[0].priority = ibl_HIGHEST_PRIORITY;
	ibl.bootModes[0].port     = 0;

	ibl.bootModes[0].u.norBoot.bootFormat	= ibl_BOOT_FORMAT_ELF;
	ibl.bootModes[0].u.norBoot.bootAddress[0][0]	= 0;		/* Image 0 NOR offset byte address in LE mode */ 
	ibl.bootModes[0].u.norBoot.bootAddress[0][1]	= 0xA00000;	/* Image 1 NOR offset byte address in LE mode  */
	ibl.bootModes[0].u.norBoot.bootAddress[1][0]	= 0;		/* Image 0 NOR offset byte address in BE mode */ 
	ibl.bootModes[0].u.norBoot.bootAddress[1][1]	= 0xA00000;	/* Image 1 NOR offset byte address in BE mode  */
	ibl.bootModes[0].u.norBoot.interface	= ibl_PMEM_IF_SPI;
    ibl.bootModes[0].u.norBoot.blob[0][0].startAddress  = 0x80000000;       /* Image 0 load start address in LE mode */
    ibl.bootModes[0].u.norBoot.blob[0][0].sizeBytes     = 0xA00000;         /* Image 0 size (10 MB) in LE mode */
    ibl.bootModes[0].u.norBoot.blob[0][0].branchAddress = 0x80000000;       /* Image 0 branch address after loading in LE mode */
    ibl.bootModes[0].u.norBoot.blob[0][1].startAddress  = 0x80000000;       /* Image 1 load start address in LE mode */
    ibl.bootModes[0].u.norBoot.blob[0][1].sizeBytes     = 0xA00000;         /* Image 1 size (10 MB) in LE mode */
    ibl.bootModes[0].u.norBoot.blob[0][1].branchAddress = 0x80000000;       /* Image 1 branch address after loading in LE mode */
    ibl.bootModes[0].u.norBoot.blob[1][0].startAddress  = 0x80000000;       /* Image 0 load start address in BE mode */
    ibl.bootModes[0].u.norBoot.blob[1][0].sizeBytes     = 0xA00000;         /* Image 0 size (10 MB) in BE mode */
    ibl.bootModes[0].u.norBoot.blob[1][0].branchAddress = 0x80000000;       /* Image 0 branch address after loading in BE mode */
    ibl.bootModes[0].u.norBoot.blob[1][1].startAddress  = 0x80000000;       /* Image 1 load start address in BE mode */
    ibl.bootModes[0].u.norBoot.blob[1][1].sizeBytes     = 0xA00000;         /* Image 1 size (10 MB) in BE mode */
    ibl.bootModes[0].u.norBoot.blob[1][1].branchAddress = 0x80000000;       /* Image 1 branch address after loading in BE mode */

    ibl.bootModes[1].bootMode = ibl_BOOT_MODE_NAND;
    ibl.bootModes[1].priority = ibl_HIGHEST_PRIORITY;
    ibl.bootModes[1].port     = 0;

    ibl.bootModes[1].u.nandBoot.bootFormat        = ibl_BOOT_FORMAT_BBLOB;
    ibl.bootModes[1].u.nandBoot.bootAddress[0][0]	  = 0x4000;         /* Image 0 NAND offset address (block 1) in LE mode */
    ibl.bootModes[1].u.nandBoot.bootAddress[0][1]	  = 0x2000000;      /* Image 1 NAND offset address (block 2048) in LE mode */
    ibl.bootModes[1].u.nandBoot.bootAddress[1][0]	  = 0x4000;	    /* Image 0 NAND offset address (block 1) in BE mode */
    ibl.bootModes[1].u.nandBoot.bootAddress[1][1]	  = 0x2000000;      /* Image 1 NAND offset address (block 2048) in BE mode */
    ibl.bootModes[1].u.nandBoot.interface         = ibl_PMEM_IF_GPIO;

    ibl.bootModes[1].u.nandBoot.blob[0][0].startAddress  = 0x80000000;       /* Image 0 load start address in LE mode */
    ibl.bootModes[1].u.nandBoot.blob[0][0].sizeBytes     = 0xFFC000;         /* Image 0 size in LE mode */
    ibl.bootModes[1].u.nandBoot.blob[0][0].branchAddress = 0x80000000;       /* Image 0 branch address after loading in LE mode */
    ibl.bootModes[1].u.nandBoot.blob[0][1].startAddress  = 0x80000000;       /* Image 1 load start address in LE mode */
    ibl.bootModes[1].u.nandBoot.blob[0][1].sizeBytes     = 0xFFC000;         /* Image 1 size in LE mode */
    ibl.bootModes[1].u.nandBoot.blob[0][1].branchAddress = 0x80000000;       /* Image 1 branch address after loading in LE mode */
    ibl.bootModes[1].u.nandBoot.blob[1][0].startAddress  = 0x80000000;       /* Image 0 load start address in BE mode */
    ibl.bootModes[1].u.nandBoot.blob[1][0].sizeBytes     = 0xFFC000;         /* Image 0 size mode */
    ibl.bootModes[1].u.nandBoot.blob[1][0].branchAddress = 0x80000000;       /* Image 0 branch address after loading in BE mode */
    ibl.bootModes[1].u.nandBoot.blob[1][1].startAddress  = 0x80000000;       /* Image 1 load start address in BE mode */
    ibl.bootModes[1].u.nandBoot.blob[1][1].sizeBytes     = 0xFFC000;         /* Image 1 size in BE mode */
    ibl.bootModes[1].u.nandBoot.blob[1][1].branchAddress = 0x80000000;       /* Image 1 branch address after loading in BE mode */


    ibl.bootModes[1].u.nandBoot.nandInfo.busWidthBits  = 8;
    ibl.bootModes[1].u.nandBoot.nandInfo.pageSizeBytes = 512;
    ibl.bootModes[1].u.nandBoot.nandInfo.pageEccBytes  = 16;
    ibl.bootModes[1].u.nandBoot.nandInfo.pagesPerBlock = 32;
    ibl.bootModes[1].u.nandBoot.nandInfo.totalBlocks   = 4096;

    ibl.bootModes[1].u.nandBoot.nandInfo.addressBytes  = 4;
    ibl.bootModes[1].u.nandBoot.nandInfo.lsbFirst      = TRUE;
    ibl.bootModes[1].u.nandBoot.nandInfo.blockOffset   = 14;
    ibl.bootModes[1].u.nandBoot.nandInfo.pageOffset    = 9;
    ibl.bootModes[1].u.nandBoot.nandInfo.columnOffset  = 0;

    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[0]  = 0; 
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[1]  = 1; 
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[2]  = 2; 
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[3]  = 3; 
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[4]  = 4; 
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[5]  = 6; 
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[6]  = 7; 
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[7]  = 13; 
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[8]  = 14; 
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[9]  = 15; 

    ibl.bootModes[1].u.nandBoot.nandInfo.badBlkMarkIdx[0]= 5;
    ibl.bootModes[1].u.nandBoot.nandInfo.badBlkMarkIdx[1]= 0xff;

    ibl.bootModes[1].u.nandBoot.nandInfo.resetCommand    = 0xff;
    ibl.bootModes[1].u.nandBoot.nandInfo.readCommandPre  = 0;
    ibl.bootModes[1].u.nandBoot.nandInfo.readCommandPost = 0;
    ibl.bootModes[1].u.nandBoot.nandInfo.postCommand     = FALSE;
    
	ibl.bootModes[2].bootMode = ibl_BOOT_MODE_TFTP;
	ibl.bootModes[2].priority = ibl_HIGHEST_PRIORITY+1;
	ibl.bootModes[2].port     = ibl_PORT_SWITCH_ALL;

	ibl.bootModes[2].u.ethBoot.doBootp          = TRUE;
	ibl.bootModes[2].u.ethBoot.useBootpServerIp = TRUE;
	ibl.bootModes[2].u.ethBoot.useBootpFileName = TRUE;
	ibl.bootModes[2].u.ethBoot.bootFormat       = ibl_BOOT_FORMAT_BBLOB;


    SETIP(ibl.bootModes[2].u.ethBoot.ethInfo.ipAddr,    192,168,1,3);
    SETIP(ibl.bootModes[2].u.ethBoot.ethInfo.serverIp,  192,168,1,2);
    SETIP(ibl.bootModes[2].u.ethBoot.ethInfo.gatewayIp, 192,168,1,1);
    SETIP(ibl.bootModes[2].u.ethBoot.ethInfo.netmask,   255,255,255,0);

    /* Use the e-fuse value */
    ibl.bootModes[2].u.ethBoot.ethInfo.hwAddress[0] = 0;
    ibl.bootModes[2].u.ethBoot.ethInfo.hwAddress[1] = 0;
    ibl.bootModes[2].u.ethBoot.ethInfo.hwAddress[2] = 0;
    ibl.bootModes[2].u.ethBoot.ethInfo.hwAddress[3] = 0;
    ibl.bootModes[2].u.ethBoot.ethInfo.hwAddress[4] = 0;
    ibl.bootModes[2].u.ethBoot.ethInfo.hwAddress[5] = 0;


    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[0]  = 'c';
    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[1]  = '6';
    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[2]  = '6';
    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[3]  = '7';
    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[4]  = '0';
    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[5]  = '-';
    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[6]  = 'l';
    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[7]  = 'e';
    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[8]  = '.';
    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[9]  = 'b';
    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[10] = 'i';
    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[11] = 'n';
    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[12] = '\0';
    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[13] = '\0';
    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[14] = '\0';

    ibl.bootModes[2].u.ethBoot.blob.startAddress  = 0x80000000;       /* Load start address */
    ibl.bootModes[2].u.ethBoot.blob.sizeBytes     = 0x20000000;
    ibl.bootModes[2].u.ethBoot.blob.branchAddress = 0x80000000;       /* Branch address after loading */

    ibl.chkSum = 0;

    return(ibl);
}
ibl_t c6657_ibl_config(void)
{
	ibl_t ibl;
    memset(&ibl, 0, sizeof(ibl_t));
    
	ibl.iblMagic = ibl_MAGIC_VALUE;
	ibl.iblEvmType = ibl_EVM_C6657L;

	/* Main PLL: 100 MHz reference, 1GHz output */
	ibl.pllConfig[ibl_MAIN_PLL].doEnable      = 1;
	ibl.pllConfig[ibl_MAIN_PLL].prediv        = 1;
	ibl.pllConfig[ibl_MAIN_PLL].mult          = 20;
	ibl.pllConfig[ibl_MAIN_PLL].postdiv       = 2;
	ibl.pllConfig[ibl_MAIN_PLL].pllOutFreqMhz = 1000;

	/* DDR PLL: */
	ibl.pllConfig[ibl_DDR_PLL].doEnable       = 1; 
	ibl.pllConfig[ibl_DDR_PLL].prediv         = 3;
	ibl.pllConfig[ibl_DDR_PLL].mult           = 40;
	ibl.pllConfig[ibl_DDR_PLL].postdiv        = 2;
	ibl.pllConfig[ibl_DDR_PLL].pllOutFreqMhz  = 1333;

    /* Net PLL: 100 MHz reference, 1050 MHz output (followed by a built in divide by 3 to give 350 MHz to PA) */

	ibl.pllConfig[ibl_NET_PLL].doEnable       = 0;


	ibl.ddrConfig.configDdr = TRUE;
	ibl.ddrConfig.uEmif.emif4p0.registerMask = ibl_EMIF4_ENABLE_sdRamConfig | ibl_EMIF4_ENABLE_sdRamRefreshCtl | ibl_EMIF4_ENABLE_sdRamTiming1 | ibl_EMIF4_ENABLE_sdRamTiming2 | ibl_EMIF4_ENABLE_sdRamTiming3 | ibl_EMIF4_ENABLE_ddrPhyCtl1;

	ibl.ddrConfig.uEmif.emif4p0.sdRamConfig				= 0x62477AB2;
	ibl.ddrConfig.uEmif.emif4p0.sdRamConfig2			= 0;
	ibl.ddrConfig.uEmif.emif4p0.sdRamRefreshCtl			= 0x000030D4;
	ibl.ddrConfig.uEmif.emif4p0.sdRamTiming1			= 0x1333780C;
	ibl.ddrConfig.uEmif.emif4p0.sdRamTiming2			= 0x30717FE3;
	ibl.ddrConfig.uEmif.emif4p0.sdRamTiming3 			= 0x559F86AF;
	ibl.ddrConfig.uEmif.emif4p0.lpDdrNvmTiming			= 0;
	ibl.ddrConfig.uEmif.emif4p0.powerManageCtl			= 0;
	ibl.ddrConfig.uEmif.emif4p0.iODFTTestLogic			= 0;
	ibl.ddrConfig.uEmif.emif4p0.performCountCfg			= 0;
	ibl.ddrConfig.uEmif.emif4p0.performCountMstRegSel		= 0;
	ibl.ddrConfig.uEmif.emif4p0.readIdleCtl				= 0;
	ibl.ddrConfig.uEmif.emif4p0.sysVbusmIntEnSet			= 0;
	ibl.ddrConfig.uEmif.emif4p0.sdRamOutImpdedCalCfg		= 0;
	ibl.ddrConfig.uEmif.emif4p0.tempAlterCfg			= 0;
	ibl.ddrConfig.uEmif.emif4p0.ddrPhyCtl1 				= 0x0010010F;
	ibl.ddrConfig.uEmif.emif4p0.ddrPhyCtl2				= 0;
	ibl.ddrConfig.uEmif.emif4p0.priClassSvceMap			= 0;
	ibl.ddrConfig.uEmif.emif4p0.mstId2ClsSvce1Map			= 0;
	ibl.ddrConfig.uEmif.emif4p0.mstId2ClsSvce2Map			= 0;
	ibl.ddrConfig.uEmif.emif4p0.eccCtl				= 0;
	ibl.ddrConfig.uEmif.emif4p0.eccRange1				= 0;
	ibl.ddrConfig.uEmif.emif4p0.eccRange2				= 0;
	ibl.ddrConfig.uEmif.emif4p0.rdWrtExcThresh			= 0;

  	/* SGMII 0 is present */
	ibl.sgmiiConfig[0].configure     = 1;
        ibl.sgmiiConfig[0].adviseAbility = 1;
        ibl.sgmiiConfig[0].control       = 1;
        ibl.sgmiiConfig[0].txConfig      = 0x108a1;
        ibl.sgmiiConfig[0].rxConfig      = 0x700621;
        ibl.sgmiiConfig[0].auxConfig     = 0x51;

        /* There is no port 1 on the 6657 */
        ibl.sgmiiConfig[1].configure = FALSE;

        /* MDIO configuration */
        ibl.mdioConfig.nMdioOps = 0;
        ibl.mdioConfig.mdioClkDiv = 0xa5;
        ibl.mdioConfig.interDelay = 3000;   /* ~2ms at 1000 MHz */

        ibl.spiConfig.addrWidth  = 24;
	ibl.spiConfig.nPins      = 5;
	ibl.spiConfig.mode       = 1;	
	ibl.spiConfig.csel       = 2;
	ibl.spiConfig.c2tdelay   = 1;
	ibl.spiConfig.busFreqMHz = 20;

	ibl.emifConfig[0].csSpace    = 2;
	ibl.emifConfig[0].busWidth   = 8;
	ibl.emifConfig[0].waitEnable = 0;

	ibl.emifConfig[1].csSpace    = 0;
	ibl.emifConfig[1].busWidth   = 0;
	ibl.emifConfig[1].waitEnable = 0;

	ibl.bootModes[0].bootMode = ibl_BOOT_MODE_NOR;
	ibl.bootModes[0].priority = ibl_HIGHEST_PRIORITY;
	ibl.bootModes[0].port     = 0;

	ibl.bootModes[0].u.norBoot.bootFormat	= ibl_BOOT_FORMAT_ELF;
	ibl.bootModes[0].u.norBoot.bootAddress[0][0]	= 0;		/* Image 0 NOR offset byte address in LE mode */ 
	ibl.bootModes[0].u.norBoot.bootAddress[0][1]	= 0xA00000;	/* Image 1 NOR offset byte address in LE mode  */
	ibl.bootModes[0].u.norBoot.bootAddress[1][0]	= 0;		/* Image 0 NOR offset byte address in BE mode */ 
	ibl.bootModes[0].u.norBoot.bootAddress[1][1]	= 0xA00000;	/* Image 1 NOR offset byte address in BE mode  */
	ibl.bootModes[0].u.norBoot.interface	= ibl_PMEM_IF_SPI;
    ibl.bootModes[0].u.norBoot.blob[0][0].startAddress  = 0x80000000;       /* Image 0 load start address in LE mode */
    ibl.bootModes[0].u.norBoot.blob[0][0].sizeBytes     = 0xA00000;         /* Image 0 size (10 MB) in LE mode */
    ibl.bootModes[0].u.norBoot.blob[0][0].branchAddress = 0x80000000;       /* Image 0 branch address after loading in LE mode */
    ibl.bootModes[0].u.norBoot.blob[0][1].startAddress  = 0x80000000;       /* Image 1 load start address in LE mode */
    ibl.bootModes[0].u.norBoot.blob[0][1].sizeBytes     = 0xA00000;         /* Image 1 size (10 MB) in LE mode */
    ibl.bootModes[0].u.norBoot.blob[0][1].branchAddress = 0x80000000;       /* Image 1 branch address after loading in LE mode */
    ibl.bootModes[0].u.norBoot.blob[1][0].startAddress  = 0x80000000;       /* Image 0 load start address in BE mode */
    ibl.bootModes[0].u.norBoot.blob[1][0].sizeBytes     = 0xA00000;         /* Image 0 size (10 MB) in BE mode */
    ibl.bootModes[0].u.norBoot.blob[1][0].branchAddress = 0x80000000;       /* Image 0 branch address after loading in BE mode */
    ibl.bootModes[0].u.norBoot.blob[1][1].startAddress  = 0x80000000;       /* Image 1 load start address in BE mode */
    ibl.bootModes[0].u.norBoot.blob[1][1].sizeBytes     = 0xA00000;         /* Image 1 size (10 MB) in BE mode */
    ibl.bootModes[0].u.norBoot.blob[1][1].branchAddress = 0x80000000;       /* Image 1 branch address after loading in BE mode */

    ibl.bootModes[1].bootMode = ibl_BOOT_MODE_NAND;
    ibl.bootModes[1].priority = ibl_HIGHEST_PRIORITY;
    ibl.bootModes[1].port     = 0;

    ibl.bootModes[1].u.nandBoot.bootFormat        = ibl_BOOT_FORMAT_BBLOB;
    ibl.bootModes[1].u.nandBoot.bootAddress[0][0]	  = 0x20000;         /* Image 0 NAND offset address (block 1) in LE mode */
    ibl.bootModes[1].u.nandBoot.bootAddress[0][1]	  = 0x4000000;      /* Image 1 NAND offset address (block 512) in LE mode */
    ibl.bootModes[1].u.nandBoot.bootAddress[1][0]	  = 0x20000;	    /* Image 0 NAND offset address (block 1) in BE mode */
    ibl.bootModes[1].u.nandBoot.bootAddress[1][1]	  = 0x4000000;      /* Image 1 NAND offset address (block 512) in BE mode */
    ibl.bootModes[1].u.nandBoot.interface         = ibl_PMEM_IF_CHIPSEL_2;

    ibl.bootModes[1].u.nandBoot.blob[0][0].startAddress  = 0x80000000;       /* Image 0 load start address in LE mode */
    ibl.bootModes[1].u.nandBoot.blob[0][0].sizeBytes     = 0xFFC000;         /* Image 0 size in LE mode */
    ibl.bootModes[1].u.nandBoot.blob[0][0].branchAddress = 0x80000000;       /* Image 0 branch address after loading in LE mode */
    ibl.bootModes[1].u.nandBoot.blob[0][1].startAddress  = 0x80000000;       /* Image 1 load start address in LE mode */
    ibl.bootModes[1].u.nandBoot.blob[0][1].sizeBytes     = 0xFFC000;         /* Image 1 size in LE mode */
    ibl.bootModes[1].u.nandBoot.blob[0][1].branchAddress = 0x80000000;       /* Image 1 branch address after loading in LE mode */
    ibl.bootModes[1].u.nandBoot.blob[1][0].startAddress  = 0x80000000;       /* Image 0 load start address in BE mode */
    ibl.bootModes[1].u.nandBoot.blob[1][0].sizeBytes     = 0xFFC000;         /* Image 0 size in BE mode */
    ibl.bootModes[1].u.nandBoot.blob[1][0].branchAddress = 0x80000000;       /* Image 0 branch address after loading in BE mode */
    ibl.bootModes[1].u.nandBoot.blob[1][1].startAddress  = 0x80000000;       /* Image 1 load start address in BE mode */
    ibl.bootModes[1].u.nandBoot.blob[1][1].sizeBytes     = 0xFFC000;         /* Image 1 size in BE mode */
    ibl.bootModes[1].u.nandBoot.blob[1][1].branchAddress = 0x80000000;       /* Image 1 branch address after loading in BE mode */


    ibl.bootModes[1].u.nandBoot.nandInfo.busWidthBits  = 8;
    ibl.bootModes[1].u.nandBoot.nandInfo.pageSizeBytes = 2048;
    ibl.bootModes[1].u.nandBoot.nandInfo.pageEccBytes  = 64;
    ibl.bootModes[1].u.nandBoot.nandInfo.pagesPerBlock = 64;
    ibl.bootModes[1].u.nandBoot.nandInfo.totalBlocks   = 1024;

    ibl.bootModes[1].u.nandBoot.nandInfo.addressBytes  = 4;
    ibl.bootModes[1].u.nandBoot.nandInfo.lsbFirst      = TRUE;
    ibl.bootModes[1].u.nandBoot.nandInfo.blockOffset   = 22;
    ibl.bootModes[1].u.nandBoot.nandInfo.pageOffset    = 16;
    ibl.bootModes[1].u.nandBoot.nandInfo.columnOffset  = 0;

    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[0]  = 0;
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[1]  = 1;
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[2]  = 2;
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[3]  = 3;
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[4]  = 4;
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[5]  = 6;
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[6]  = 7;
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[7]  = 13;
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[8]  = 14;
    ibl.bootModes[1].u.nandBoot.nandInfo.eccBytesIdx[9]  = 15;

    ibl.bootModes[1].u.nandBoot.nandInfo.badBlkMarkIdx[0]= 5;
    ibl.bootModes[1].u.nandBoot.nandInfo.badBlkMarkIdx[1]= 0xff;

    ibl.bootModes[1].u.nandBoot.nandInfo.resetCommand    = 0xff;
    ibl.bootModes[1].u.nandBoot.nandInfo.readCommandPre  = 0x00;
    ibl.bootModes[1].u.nandBoot.nandInfo.readCommandPost = 0x30;
    ibl.bootModes[1].u.nandBoot.nandInfo.postCommand     = TRUE;


	ibl.bootModes[2].bootMode = ibl_BOOT_MODE_TFTP;
	ibl.bootModes[2].priority = ibl_HIGHEST_PRIORITY+1;
	ibl.bootModes[2].port     = 0;

	ibl.bootModes[2].u.ethBoot.doBootp          = TRUE;
	ibl.bootModes[2].u.ethBoot.useBootpServerIp = TRUE;
	ibl.bootModes[2].u.ethBoot.useBootpFileName = TRUE;
	ibl.bootModes[2].u.ethBoot.bootFormat       = ibl_BOOT_FORMAT_BBLOB;


    SETIP(ibl.bootModes[2].u.ethBoot.ethInfo.ipAddr,    192,168,1,3);
    SETIP(ibl.bootModes[2].u.ethBoot.ethInfo.serverIp,  192,168,1,2);
    SETIP(ibl.bootModes[2].u.ethBoot.ethInfo.gatewayIp, 192,168,1,1);
    SETIP(ibl.bootModes[2].u.ethBoot.ethInfo.netmask,   255,255,255,0);

    /* Use the e-fuse value */
    ibl.bootModes[2].u.ethBoot.ethInfo.hwAddress[0] = 0;
    ibl.bootModes[2].u.ethBoot.ethInfo.hwAddress[1] = 0;
    ibl.bootModes[2].u.ethBoot.ethInfo.hwAddress[2] = 0;
    ibl.bootModes[2].u.ethBoot.ethInfo.hwAddress[3] = 0;
    ibl.bootModes[2].u.ethBoot.ethInfo.hwAddress[4] = 0;
    ibl.bootModes[2].u.ethBoot.ethInfo.hwAddress[5] = 0;


    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[0]  = 'c';
    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[1]  = '6';
    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[2]  = '6';
    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[3]  = '5';
    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[4]  = '7';
    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[5]  = '-';
    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[6]  = 'l';
    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[7]  = 'e';
    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[8]  = '.';
    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[9]  = 'b';
    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[10] = 'i';
    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[11] = 'n';
    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[12] = '\0';
    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[13] = '\0';
    ibl.bootModes[2].u.ethBoot.ethInfo.fileName[14] = '\0';

    ibl.bootModes[2].u.ethBoot.blob.startAddress  = 0x80000000;       /* Load start address */
    ibl.bootModes[2].u.ethBoot.blob.sizeBytes     = 0x20000000;
    ibl.bootModes[2].u.ethBoot.blob.branchAddress = 0x80000000;       /* Branch address after loading */

    ibl.chkSum = 0;

    return(ibl);
}


