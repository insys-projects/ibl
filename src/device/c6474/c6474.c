/************************************************************************************
 * FILE PURPOSE: C6474 Device Specific functions
 ************************************************************************************
 * FILE NAME: c6474.c
 *
 * DESCRIPTION: Implements the device specific functions for the IBL
 *
 * @file c6474.c
 *
 * @brief
 *  This file implements the device specific functions for the IBL
 *
 ************************************************************************************/
#include "ibl.h"
#include "device.h"
#include "pllapi.h"
#include "emif31api.h"
#include "pscapi.h"
#include "gpio.h"
#include <string.h>

extern cregister unsigned int DNUM;


/**
 * @brief The default boot configuration table is filled in
 *
 * @details
 *   A default ibl configuraiton table is provided when one is not found
 *   preloaded.
 */
void deviceLoadDefaultIblTable (void)
{
    uint32 macA, macB;

#if 0  /* This is really the default. Switching to a direct tftp boot until I have a bootp server
        * on a private lan for test */
    ibl.ethConfig[0].ethPriority = ibl_LOWEST_PRIORITY;
    ibl.ethConfig[0].port        = 0;
    ibl.ethConfig[0].doBootp     = TRUE;
    ibl.ethConfig[0].bootFormat  = ibl_BOOT_FORMAT_AUTO;

    memset (&ibl.ethConfig[0].ethInfo, 0, sizeof(ibl.ethConfig[0].ethInfo));
#endif

    /* This is the temporary code */
    ibl.ethConfig[0].ethPriority      = ibl_LOWEST_PRIORITY;
    ibl.ethConfig[0].port             = 0;
    ibl.ethConfig[0].doBootp          = FALSE;
    ibl.ethConfig[0].useBootpServerIp = FALSE;
    ibl.ethConfig[0].useBootpFileName = FALSE;
    ibl.ethConfig[0].bootFormat       = ibl_BOOT_FORMAT_NAME;

    memset (&ibl.ethConfig[0].ethInfo, 0, sizeof(ibl.ethConfig[0].ethInfo));

    ibl.ethConfig[0].ethInfo.ipAddr[0] = 10;
    ibl.ethConfig[0].ethInfo.ipAddr[1] = 218;
    ibl.ethConfig[0].ethInfo.ipAddr[2] = 109;
    ibl.ethConfig[0].ethInfo.ipAddr[3] = 21;

    ibl.ethConfig[0].ethInfo.serverIp[0] = 10;
    ibl.ethConfig[0].ethInfo.serverIp[1] = 218;
    ibl.ethConfig[0].ethInfo.serverIp[2] = 109;
    ibl.ethConfig[0].ethInfo.serverIp[3] = 196;

    ibl.ethConfig[0].ethInfo.gatewayIp[0] = 10;
    ibl.ethConfig[0].ethInfo.gatewayIp[1] = 218;
    ibl.ethConfig[0].ethInfo.gatewayIp[2] = 109;
    ibl.ethConfig[0].ethInfo.gatewayIp[3] = 1;

    /* Leave hw address as 0 */

    strcpy (ibl.ethConfig[0].ethInfo.fileName, "test2_little.out");

    ibl.ethConfig[0].blob.startAddress  = 0x00200000;       /* Base address of SL2 */
    ibl.ethConfig[0].blob.sizeBytes     = 0x000c0000;       /* All of SL2 */
    ibl.ethConfig[0].blob.branchAddress = 0x00200000;       /* Base of SL2 */



    macA = *((uint32 *)0x2a80700);
    macB = *((uint32 *)0x2a80704);

    ibl.ethConfig[0].ethInfo.hwAddress[0] = (macA >> 24) & 0xff;
    ibl.ethConfig[0].ethInfo.hwAddress[1] = (macA >> 16) & 0xff;
    ibl.ethConfig[0].ethInfo.hwAddress[2] = (macA >>  8) & 0xff;
    ibl.ethConfig[0].ethInfo.hwAddress[3] = (macA >>  0) & 0xff;
    ibl.ethConfig[0].ethInfo.hwAddress[4] = (macB >> 24) & 0xff;
    ibl.ethConfig[0].ethInfo.hwAddress[5] = (macB >> 16) & 0xff;


    ibl.ethConfig[1].ethPriority = ibl_DEVICE_NOBOOT;


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


    /* Main Pll configuration */
    ibl.pllConfig[ibl_MAIN_PLL].doEnable = TRUE;
    ibl.pllConfig[ibl_MAIN_PLL].prediv   = 1;
    ibl.pllConfig[ibl_MAIN_PLL].mult     = 28;
    ibl.pllConfig[ibl_MAIN_PLL].postdiv  = 1;

    ibl.pllConfig[ibl_MAIN_PLL].pllOutFreqMhz = 500;

    /* The DDR PLL. The multipliers/dividers are fixed, so are really dont cares */
    ibl.pllConfig[ibl_DDR_PLL].doEnable = TRUE;

    /* The network PLL. The multipliers/dividers are fixed */
    ibl.pllConfig[ibl_NET_PLL].doEnable = TRUE;

    /* EMIF configuration */
    ibl.ddrConfig.uEmif.emif3p1.sdcfg  = 0x00538832; /* timing, 32bit wide */
    ibl.ddrConfig.uEmif.emif3p1.sdrfc  = 0x0000073B; /* Refresh 533Mhz */ 
    ibl.ddrConfig.uEmif.emif3p1.sdtim1 = 0x47245BD2; /* Timing 1 */
    ibl.ddrConfig.uEmif.emif3p1.sdtim2 = 0x0125DC44; /* Timing 2 */
    ibl.ddrConfig.uEmif.emif3p1.dmcctl = 0x50001906; /* PHY read latency for CAS 5 is 5 + 2 - 1 */


    /* NAND configuration for the MT29F1G08 flash */
    ibl.nandConfig.nandPriority = ibl_HIGHEST_PRIORITY;
    ibl.nandConfig.bootFormat   = ibl_BOOT_FORMAT_COFF;

    ibl.nandConfig.nandInfo.busWidthBits  = 8;
    ibl.nandConfig.nandInfo.pageSizeBytes = 2048;
    ibl.nandConfig.nandInfo.pageEccBytes  = 64;
    ibl.nandConfig.nandInfo.pagesPerBlock = 64;
    ibl.nandConfig.nandInfo.totalBlocks   = 1024;

    ibl.nandConfig.nandInfo.addressBytes  = 4;
    ibl.nandConfig.nandInfo.lsbFirst      = TRUE;
    ibl.nandConfig.nandInfo.blockOffset   = 22;
    ibl.nandConfig.nandInfo.pageOffset    = 16;
    ibl.nandConfig.nandInfo.columnOffset  = 0;

    ibl.nandConfig.nandInfo.resetCommand    = 0xff;
    ibl.nandConfig.nandInfo.readCommandPre  = 0;
    ibl.nandConfig.nandInfo.readCommandPost = 0x30;
    ibl.nandConfig.nandInfo.postCommand     = TRUE;
    

}

/**
 *  @brief Determine if an address is local
 *
 *  @details
 *    Examines an input address to determine if it is a local address
 */
bool address_is_local (Uint32 addr)
{
    /* L2 */
    if ((addr >= 0x00800000) && (addr < 0x00898000))
        return (TRUE);

    /* L1P */
    if ((addr >= 0x00e00000) && (addr < 0x00e08000))
        return (TRUE);

    /* L2D */
    if ((addr >= 0x00f00000) && (addr < 0x00f08000))
        return (TRUE);

    return (FALSE);

}


/**
 * @brief  Convert a local l1d, l1p or l2 address to a global address
 *
 * @details
 *  The global address is formed. If the address is not local then
 *  the input address is returned
 */
Uint32 deviceLocalAddrToGlobal (Uint32 addr)
{

    if (address_is_local (addr))
        addr = (1 << 28) | (DNUM << 24) | addr;

    return (addr);

}
        
        
/**
 * @brief Configure the PLLs
 *
 * @details
 *   Only the main PLL can be configured here. The DDR pll is enabled by default,
 *   and the network PLL is enabled through serdes configuration.
 *   the multiplier and dividers.
 */
void devicePllConfig (void)
{
    if (ibl.pllConfig[ibl_MAIN_PLL].doEnable == TRUE)
        hwPllSetPll (MAIN_PLL, 
                     ibl.pllConfig[ibl_MAIN_PLL].prediv,
                     ibl.pllConfig[ibl_MAIN_PLL].mult,
                     ibl.pllConfig[ibl_MAIN_PLL].postdiv);

}

/**
 * @brief
 *   Enable the DDR
 *
 * @details
 *   The DDR controller on the c6474 is an emif 3.1. The controller is
 *   initialized directly with the supplied values
 */
void deviceDdrConfig (void)
{
    if (ibl.ddrConfig.configDdr != 0)
        hwEmif3p1Enable (&ibl.ddrConfig.uEmif.emif3p1);

}
        

/**
 *  @brief Power up a peripheral
 *
 *  @details
 *    Boot peripherals are powered up
 */
int32 devicePowerPeriph (int32 modNum)
{
    /* If the input value is < 0 there is nothing to power up */
    if (modNum < 0)
        return (0);


    if (modNum >= TARGET_PWR_MAX_MOD)
        return (-1);

    return ((int32)pscEnableModule(modNum));
        
}


/**
 *  @brief  Enable the pass through version of the nand controller
 *
 *  @details  On the evm the nand controller is enabled by setting 
 *            gpio 14 high
 */
int32 deviceConfigureForNand(void)
{
	hwGpioSetDirection(NAND_MODE_GPIO, GPIO_OUT);
	hwGpioSetOutput(NAND_MODE_GPIO);
    return (0);

}


/**
 *  @brief
 *    The e-fuse mac address is loaded
 */
void deviceLoadDefaultEthAddress (uint8 *maddr)
{
    uint32 macA, macB;

    /* Read the e-fuse mac address */
    macA = *((uint32 *)0x2880834);
    macB = *((uint32 *)0x2880838);

    maddr[0] = (macB >>  8) & 0xff;
    maddr[1] = (macB >>  0) & 0xff;
    maddr[2] = (macA >> 24) & 0xff;
    maddr[3] = (macA >> 16) & 0xff;
    maddr[4] = (macA >>  8) & 0xff;
    maddr[5] = (macA >>  0) & 0xff;
}





