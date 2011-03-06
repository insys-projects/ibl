/**
 * @file c661xinit.c
 *
 * @brief
 *		c661x functions used during the initial stage of the ibl load
 *
 */
#include "ibl.h"
#include "iblloc.h"
#include "device.h"
#include "pllapi.h"
#include "spi_api.h"
#include "spi_loc.h"
#include "tiboot_c661x.h"       


/**
 * @brief Configure the PLLs
 *
 * @details
 *   The three PLLs are enabled. Only the main PLL has the ability to configure
 *   the multiplier and dividers.
 */
void devicePllConfig (void)
{

    /* Unlock the chip registers and leave them unlocked */
    *((Uint32 *)0x2620038) = 0x83e70b13;
    *((Uint32 *)0x262003c) = 0x95a4f1e0;

    if (ibl.pllConfig[ibl_MAIN_PLL].doEnable == TRUE)
        hwPllSetPll (MAIN_PLL, 
                     ibl.pllConfig[ibl_MAIN_PLL].prediv,
                     ibl.pllConfig[ibl_MAIN_PLL].mult,
                     ibl.pllConfig[ibl_MAIN_PLL].postdiv);

    if (ibl.pllConfig[ibl_DDR_PLL].doEnable == TRUE)
        hwPllSetCfg2Pll (DEVICE_PLL_BASE(DDR_PLL),
                         ibl.pllConfig[ibl_DDR_PLL].prediv,
                         ibl.pllConfig[ibl_DDR_PLL].mult,
                         ibl.pllConfig[ibl_DDR_PLL].postdiv,
                         ibl.pllConfig[ibl_MAIN_PLL].pllOutFreqMhz,
                         ibl.pllConfig[ibl_DDR_PLL].pllOutFreqMhz);

    if (ibl.pllConfig[ibl_NET_PLL].doEnable == TRUE)
        hwPllSetCfgPll (DEVICE_PLL_BASE(NET_PLL),
                        ibl.pllConfig[ibl_NET_PLL].prediv,
                        ibl.pllConfig[ibl_NET_PLL].mult,
                        ibl.pllConfig[ibl_NET_PLL].postdiv,
                        ibl.pllConfig[ibl_MAIN_PLL].pllOutFreqMhz,
                        ibl.pllConfig[ibl_NET_PLL].pllOutFreqMhz);


}


/**
 * @brief
 *  Return the endian status of the device
 *
 * @details
 *  Returns true if the device is executing in little endian mode
 */
extern cregister volatile unsigned int CSR;

bool deviceIsLittleEndian (void)
{
    if ((CSR & (1 << 8)) == 0)    
        return (FALSE);

    return (TRUE);

}


/**
 *  @brief
 *      Return the device used for the second stage program load.
 *      For SPI NAND a second stage loader is required and this
 *      function must be changed to locate that fingerprint.
 */
int32 deviceReadBootDevice (void)
{
    uint32 v;
    int32  w;

    BOOT_PARAMS_COMMON_T *params;

#if  (defined(EXCLUDE_NOR_SPI) && defined(EXCLUDE_NAND_SPI) && !defined(EXCLUDE_I2C))

    return (BOOT_DEVICE_I2C);

#elif (defined(EXCLUDE_NOR_SPI) && !defined(EXCLUDE_NAND_SPI) && defined(EXCLUDE_I2C))

    return (BOOT_DEVICE_NAND_SPI);

#elif (!defined(EXCLUDE_NOR_SPI) && defined(EXCLUDE_NAND_SPI) && defined(EXCLUDE_I2C))

    return (BOOT_DEVICE_NOR_SPI);

#endif

    v = *((Uint32 *)DEVICE_JTAG_ID_REG);

    if (v == DEVICE_C6618_JTAG_ID_VAL)
        params = (BOOT_PARAMS_COMMON_T *)ROM_BOOT_PARAMS_ADDR_C6618;
    else
        params = (BOOT_PARAMS_COMMON_T *)ROM_BOOT_PARAMS_ADDR_C6616;

    switch (params->boot_mode)  {

#ifndef EXCLUDE_I2C
        case BOOT_MODE_I2C:   w = BOOT_DEVICE_I2C;
                              break;
#endif

#ifndef EXCLUDE_NOR_SPI
        case BOOT_MODE_SPI:   w = BOOT_DEVICE_SPI_NOR;
                              break;
#endif

        default:              w = BOOT_DEVICE_INVALID;
                              break;
    
    }

    return (w);
}

#define L1PEDCMD	    0x01846408
#define L2EDCEN		    0x01846030
#define L2EDCMD		    0x01846008
#define SMEDCC		    0x0BC00010
/**
 *  @brief
 *      Enable the EDC for the local memory 
 */
void iblEnableEDC ()
{
    /* Enable L1P EDC */
    *(volatile unsigned int *)(L1PEDCMD) = 0x1;	//Set EN(bit0)=1	

    /* Enable EDC L2EDCEN, set DL2CEN(bit0),PL2CEN(bit1),DL2SEN(bit2),PL2SEN(bit3),SDMAEN(bit4)=1 */
	*(volatile unsigned int *)(L2EDCEN) |= 0x1F;	

    /* Enalble L2 EDC */
    *(volatile unsigned int *)(L2EDCMD) = 0x1;

    /* Enalbe MSMC EDC */
    *(volatile unsigned int *)(SMEDCC) &= 0x7FFFFFFF;	//Clear SEN(bit31)=0	
	*(volatile unsigned int *)(SMEDCC) |= 0x40000000;	//Set ECM(bit30)=1	
}

#define FPGA_BM_GPI_STATUS_LO_REG           4   /* Boot Mode GPI Status (07-00 Low Byte) Register */
#define FPGA_BM_GPI_STATUS_HI_REG           5   /* Boot Mode GPI Status (15-08 High Byte) Register */
#define FPGA_READ_REG_CMD(x)                ((x | 0x80) << 8)
/**
 * @brief
 *      Enter the ROM boot loader if the FPGA boot register
 *      indicates it was not I2C address 0x51 boot, this is necessary
 *      to apply the PLL workaround for non-I2C boot modes
 */
void iblEnterRom ()
{
    uint32      reg =  DEVICE_REG32_R (DEVICE_REG_DEVSTAT);
    uint32      v, dev_stat, bm_lo, bm_hi;
    void        (*exit)();

    /* Power up the SPI */
    devicePowerPeriph (TARGET_PWR_SPI);

    /* Reset SPI */
    DEVICE_REG32_W (DEVICE_SPI_BASE(0) + SPI_REG_SPIGCR0, SPI_REG_VAL_SPIGCR0_RESET);

    /* Release Reset */
    DEVICE_REG32_W (DEVICE_SPI_BASE(0) + SPI_REG_SPIGCR0, SPI_REG_VAL_SPIGCR0_ENABLE);

    /* CS1, CLK, in and out are functional pins, FPGA uses SPI CS1 */
    DEVICE_REG32_W (DEVICE_SPI_BASE(0) + SPI_REG_SPIPC0, 0xe02);

    /* prescale=7, char len=16 */
    DEVICE_REG32_W (DEVICE_SPI_BASE(0) + SPI_REG_SPIFMT(0), 0x710);

    /* C2TDELAY=0x6, T2CDELAY=0x3 */
    DEVICE_REG32_W (DEVICE_SPI_BASE(0) + SPI_REG_SPIDELAY, 0x6030000);

    /* Clear the SPIDAT0 */
    //DEVICE_REG32_R (DEVICE_SPI_BASE(0) + SPI_REG_SPIDAT0);

    /* Master mode, enable SPI */
    DEVICE_REG32_W (DEVICE_SPI_BASE(0) + SPI_REG_SPIGCR1, 0x01000003);

    /* Read the BM status lo register */
	DEVICE_REG32_W(DEVICE_SPI_BASE(0) + 0x38, FPGA_READ_REG_CMD(FPGA_BM_GPI_STATUS_LO_REG));
    chipDelay32(10000);
    v = DEVICE_REG32_R(DEVICE_SPI_BASE(0) + SPI_REG_SPIFLG);
    if ( v & 0x100)
    {
        bm_lo = DEVICE_REG32_R(DEVICE_SPI_BASE(0) + SPI_REG_SPIBUF) & 0xff;
    }
    else
    {
        return;
    }

    /* Read the BM status hi register */
	DEVICE_REG32_W(DEVICE_SPI_BASE(0) + 0x38, FPGA_READ_REG_CMD(FPGA_BM_GPI_STATUS_HI_REG));
    chipDelay32(10000);
    v = DEVICE_REG32_R(DEVICE_SPI_BASE(0) + SPI_REG_SPIFLG);
    if ( v & 0x100)
    {
        bm_hi = DEVICE_REG32_R(DEVICE_SPI_BASE(0) + SPI_REG_SPIBUF) & 0xff;
    }
    else
    {
        return;
    }

    /* Reset SPI */
    DEVICE_REG32_W (DEVICE_SPI_BASE(0) + SPI_REG_SPIGCR0, SPI_REG_VAL_SPIGCR0_RESET);

    if ( (BOOT_READ_BITFIELD(bm_lo,3,1) != 0x5)     ||
         (BOOT_READ_BITFIELD(bm_hi,3,3) == 0x0) )    
    { 
        /* Not i2c boot or i2c boot with address 0x50 */

        /* Update the DEVSTAT to v1 */
        dev_stat = DEVICE_REG32_R(DEVICE_REG_DEVSTAT );
        dev_stat &= ~(0x0000080E);
        dev_stat |= ((bm_hi << 8) | bm_lo);
        
        /* Update the DEVSTAT register for the intended Boot Device and i2c Addr */
        DEVICE_REG32_W (DEVICE_REG_DEVSTAT, dev_stat);

        exit = (void (*)())BOOT_ROM_ENTER_ADDRESS;
        (*exit)();        
    }
}

#if (!defined(EXCLUDE_NOR_SPI) || !defined(EXCLUDE_NAND_SPI))
/**
 *  @brief
 *      Return the default hardware configuration for SPI. If this information
 *      is available in the boot ROM it is used, otherwise defaults are used.
 */
void deviceLoadInitSpiConfig (void *vcfg)
{
    uint32 v;

    spiConfig_t *cfg = (spiConfig_t *)vcfg;

    BOOT_PARAMS_COMMON_T *params;
    BOOT_PARAMS_SPI_T    *spip;

    v = *((Uint32 *)DEVICE_JTAG_ID_REG);

    if (v == DEVICE_C6618_JTAG_ID_VAL)
        params = (BOOT_PARAMS_COMMON_T *)ROM_BOOT_PARAMS_ADDR_C6618;
    else
        params = (BOOT_PARAMS_COMMON_T *)ROM_BOOT_PARAMS_ADDR_C6616;


    /* SPI_ROM is a constant defined during make which enables the use of the
     * parameters from the ROM boot loader */
    if ((SPI_ROM == 1) && (params->boot_mode == BOOT_MODE_SPI))  {

        spip = (BOOT_PARAMS_SPI_T *)params;

        cfg->port      = 0;
        cfg->mode      = spip->mode;
        cfg->addrWidth = spip->addrWidth;
        cfg->npin      = spip->nPins;
        cfg->csel      = spip->csel;
        cfg->c2tdelay  = spip->c2tdelay;

        v = (UINT32)spip->cpuFreqMhz * 1000;  /* CPU frequency in kHz */
        v = v / (DEVICE_SPI_MOD_DIVIDER * (((UINT32)(spip->busFreqMhz) * 1000) + spip->busFreqKhz));

        if (v > DEVICE_SPI_MAX_DIVIDER)
            v = DEVICE_SPI_MAX_DIVIDER;

        cfg->clkdiv = v;

    }  else  {

        cfg->port      = 0;
        cfg->mode      = SPI_MODE;
        cfg->addrWidth = SPI_ADDR_WIDTH;
        cfg->npin      = SPI_NPIN;
        cfg->csel      = SPI_CSEL;
        cfg->c2tdelay  = SPI_C2TDEL;
        cfg->clkdiv    = SPI_CLKDIV;

    }

}



#endif
