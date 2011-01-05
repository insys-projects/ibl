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
#endif

#ifndef EXCLUDE_NOR_SPI
        case BOOT_MODE_SPI:   w = BOOT_DEVICE_SPI_NOR;
#endif

        default:              w = BOOT_DEVICE_INVALID;
    
    }

    return (w);
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
