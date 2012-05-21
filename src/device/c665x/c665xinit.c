/**
 * @file c665xinit.c
 *
 * @brief
 *		c66x functions used during the initial stage of the ibl load
 *
 */
#include "ibl.h"
#include "iblloc.h"
#include "device.h"
#include "pllapi.h"
#include "spi_api.h"
#include "spi_loc.h"
#include "tiboot_c665x.h"       


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
    v &= DEVICE_JTAG_ID_MASK;

    if (v == DEVICE_C6657_JTAG_ID_VAL)
	params = (BOOT_PARAMS_COMMON_T *)ROM_BOOT_PARAMS_ADDR_C6657; 	

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

#ifdef IBL_ENABLE_PCIE_WORKAROUND

/* undocumented register in data manual 
 * Bit 0 of this register is supposed to give the status of PCIe PLL lock*/
#define PCIE_STS_REG    0x262015C

/* Workaround for PCIe boot mode support for C6678/C6670 */
/* This is a temporary workaround should be removed once fixed in RBL */

/* PCIe Config register base on C6678/C6670 */
#define PCIE_BASE_ADDR 0x21800000

/* PCIe Application registers */
#define PCIE_APP_CMD_STATUS  0x4
#define PCIE_APP_OB_SIZE     0x30
#define PCIE_APP_SERDES_CFG0 0x390
#define PCIE_APP_SERDES_CFG1 0x394

/* PCIe Local Configuration registers */
#define PCIE_VENDER_DEVICE_ID   0x1000
#define PCIE_STATUS_CMD         0x1004
#define PCIE_CLASSCODE_REVID    0x1008
#define PCIE_BAR0               0x1010
#define PCIE_BAR1               0x1014
#define PCIE_BAR2               0x1018
#define PCIE_BAR3               0x101c
#define PCIE_DEVICE_CAP         0x1074
#define PCIE_DEV_STAT_CTRL   	0x1078
#define PCIE_LINK_STAT_CTRL     0x1080
#define PCIE_ACCR	        0x1118
#define PCIE_DEBUG0             0x1728
#define PCIE_PL_GEN2            0x180C

/* SERDES Configuration registers */
#define PCIE_SERDES_CFG_PLL 0x2620358

void waitForBoot(UINT32 MAGIC_ADDR)
{
    void (*exit)();
    UINT32 i, entry_addr;
	
    while(1)
    {
        entry_addr = DEVICE_REG32_R(MAGIC_ADDR);
        if (entry_addr != 0)
        {
            /* jump to the exit point, which will be the entry point for the full IBL */
            exit = (void (*)())entry_addr;
            (*exit)();
        }
        for (i=0; i < 100; i++)
            asm("nop");
	}
}

void iblPCIeWorkaround()
{
    UINT32  v, flag_6678 = 0, flag_6670 = 0, flag_6657 = 0, MAGIC_ADDR;
    UINT32  i;

     /* Power up PCIe */
    devicePowerPeriph (TARGET_PWR_PCIE);
    for(i=0; i<1000; i++) asm (" NOP");

    DEVICE_REG32_W ((PCIE_BASE_ADDR + PCIE_APP_SERDES_CFG0), 0x00062320);  /* ss clock */
    DEVICE_REG32_W ((PCIE_BASE_ADDR + PCIE_APP_SERDES_CFG1), 0x00022320);  /* ss clock */
      
    /* Wait for PCIe PLL lock */
    while(!(DEVICE_REG32_R(PCIE_STS_REG) & 1));

	/* Determine 6670 or 6678 */
    v = *((Uint32 *)DEVICE_JTAG_ID_REG);
    v &= DEVICE_JTAG_ID_MASK;
	
    if (v == DEVICE_C6678_JTAG_ID_VAL) {
		MAGIC_ADDR = 0x87fffc;
		flag_6678 = 1;
	} 
	if (v == DEVICE_C6670_JTAG_ID_VAL) {
        MAGIC_ADDR = 0x8ffffc;
		flag_6670 = 1;
	}
	if (v == DEVICE_C6657_JTAG_ID_VAL) {
                MAGIC_ADDR = 0x8ffffc;
                flag_6657 = 1;
        }


    DEVICE_REG32_W ((PCIE_BASE_ADDR + PCIE_CLASSCODE_REVID), 0x04800001);  /* class 0x04, sub-class 0x80, Prog I/F 0x00, Other multimedia device */ 
    DEVICE_REG32_W ((PCIE_BASE_ADDR + PCIE_LINK_STAT_CTRL), 0x10110080);  /* extended sync, slot_clk_cfg = 1 */

    DEVICE_REG32_W ((PCIE_BASE_ADDR + PCIE_VENDER_DEVICE_ID), 0xb005104c);  /* Vendor and Device ID */
    DEVICE_REG32_W ((PCIE_BASE_ADDR + PCIE_DEVICE_CAP), 0x288701); /* L0 = 4, L1 = 3 */

	DEVICE_REG32_W ((PCIE_BASE_ADDR + PCIE_APP_OB_SIZE), 0x00000003);     /* OB_SIZE = 8M */ 
	DEVICE_REG32_W ((PCIE_BASE_ADDR + PCIE_PL_GEN2), 0x0000000F);   /* num_fts = 0xF*/

    DEVICE_REG32_W ((PCIE_BASE_ADDR + PCIE_APP_CMD_STATUS), 0x0020); /* Set dbi_cs2 to allow access to the BAR registers */ 
 
	if (flag_6678)  {
		/* 6678 */
		DEVICE_REG32_W ((PCIE_BASE_ADDR + PCIE_BAR0), 0x00000FFF);   /* 4K */
		DEVICE_REG32_W ((PCIE_BASE_ADDR + PCIE_BAR1), 0x0007FFFF);   /* 512K */
		DEVICE_REG32_W ((PCIE_BASE_ADDR + PCIE_BAR2), 0x003FFFFF);   /* 4M */
		DEVICE_REG32_W ((PCIE_BASE_ADDR + PCIE_BAR3), 0x00FFFFFF);   /* 16M */
	} 

	if (flag_6670)  {
		/* 6670 */
		DEVICE_REG32_W ((PCIE_BASE_ADDR + PCIE_BAR0), 0x00000FFF);   /* 4K */
		DEVICE_REG32_W ((PCIE_BASE_ADDR + PCIE_BAR1), 0x000FFFFF);   /* 1M */
		DEVICE_REG32_W ((PCIE_BASE_ADDR + PCIE_BAR2), 0x001FFFFF);   /* 2M */
		DEVICE_REG32_W ((PCIE_BASE_ADDR + PCIE_BAR3), 0x00FFFFFF);   /* 16M */
   	 }	
	if (flag_6657)  {
                /* 6657 */
                DEVICE_REG32_W ((PCIE_BASE_ADDR + PCIE_BAR0), 0x00000FFF);   /* 4K */
                DEVICE_REG32_W ((PCIE_BASE_ADDR + PCIE_BAR1), 0x000FFFFF);   /* 1M */
                DEVICE_REG32_W ((PCIE_BASE_ADDR + PCIE_BAR2), 0x000FFFFF);   /* 1M */
                DEVICE_REG32_W ((PCIE_BASE_ADDR + PCIE_BAR3), 0x00FFFFFF);   /* 16M */
        }
    DEVICE_REG32_W ((PCIE_BASE_ADDR + PCIE_APP_CMD_STATUS), 0x0);    /* dbi_cs2=0 */

    DEVICE_REG32_W ((PCIE_BASE_ADDR + PCIE_STATUS_CMD), 0x00100146); /* ENABLE mem access */
    DEVICE_REG32_W ((PCIE_BASE_ADDR + PCIE_DEV_STAT_CTRL), 0x0000281F); /* Error control */
    DEVICE_REG32_W ((PCIE_BASE_ADDR + PCIE_ACCR), 0x000001E0); /* Error control */
    DEVICE_REG32_W ((PCIE_BASE_ADDR + PCIE_BAR0), 0); /* non-prefetch, 32-bit, mem bar */

    DEVICE_REG32_W ((PCIE_BASE_ADDR + PCIE_APP_CMD_STATUS), 0x0000007);    /* enable LTSSM, IN, OB */
    while((DEVICE_REG32_R(PCIE_BASE_ADDR + PCIE_DEBUG0) & 0x11)!=0x11);    /* Wait for training to complete */
 
    /* Wait for the Boot from Host */
    DEVICE_REG32_W(MAGIC_ADDR, 0);
	waitForBoot(MAGIC_ADDR);

    /* Will never reach here */
    return;
}

#endif  

#define FPGA_BM_GPI_STATUS_LO_REG           4   /* Boot Mode GPI Status (07-00 Low Byte) Register */
#define FPGA_BM_GPI_STATUS_HI_REG           5   /* Boot Mode GPI Status (15-08 High Byte) Register */
#define FPGA_ICS557_SEL_CTRL_REG            0x50 /* ICS 557 Clock Selection
                                                    Control Register*/
#define FPGA_READ_REG_CMD(x)                ((x | 0x80) << 8)
#define FPGA_WRITE_REG_CMD(addr,byte)       (((addr & 0x7f) << 8) | (byte & 0xff))

/**
 * @brief
 *      Enter the ROM boot loader if the FPGA boot register
 *      indicates it was not I2C address 0x51 boot, this is necessary
 *      to apply the PLL workaround for non-I2C boot modes
 */
void iblEnterRom ()
{
    uint32      v, dev_stat, bm_lo, bm_hi;
    void        (*exit)();

	
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
	DEVICE_REG32_W(DEVICE_SPI_BASE(0) + SPI_REG_SPIDAT0, FPGA_READ_REG_CMD(FPGA_BM_GPI_STATUS_LO_REG));
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
	DEVICE_REG32_W(DEVICE_SPI_BASE(0) + SPI_REG_SPIDAT0, FPGA_READ_REG_CMD(FPGA_BM_GPI_STATUS_HI_REG));
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

#ifdef IBL_ENABLE_PCIE_WORKAROUND
#define BOOT_DEVICE_MASK    0xE
#define DEVSTAT_BOOTDEVICE_SHIFT    1
#define PCI_BOOT_MODE   0x4

        if (((dev_stat & BOOT_DEVICE_MASK)>>DEVSTAT_BOOTDEVICE_SHIFT) == PCI_BOOT_MODE) {
            /* Write ICS 557 Clock Selection Control Register in the FPGA */
            /* 1 : FPGA_ICS557_SEL s driven high */
	        DEVICE_REG32_W(DEVICE_SPI_BASE(0) + SPI_REG_SPIDAT0,
                           FPGA_WRITE_REG_CMD(FPGA_ICS557_SEL_CTRL_REG,1));
            chipDelay32(10000);
            /* Reset SPI */
            DEVICE_REG32_W (DEVICE_SPI_BASE(0) + SPI_REG_SPIGCR0, SPI_REG_VAL_SPIGCR0_RESET);

            iblPCIeWorkaround();
            /* Will never reach here */
        }
#endif
        /* Reset SPI */
        DEVICE_REG32_W (DEVICE_SPI_BASE(0) + SPI_REG_SPIGCR0, SPI_REG_VAL_SPIGCR0_RESET);

        exit = (void (*)())BOOT_ROM_ENTER_ADDRESS;
        (*exit)();        
    }
    else
    {
        /* Update the DEVSTAT register for the actual boot configuration */
        DEVICE_REG32_W (DEVICE_REG_DEVSTAT, ((bm_hi << 8) | bm_lo));
    }

    /* Reset SPI */
    DEVICE_REG32_W (DEVICE_SPI_BASE(0) + SPI_REG_SPIGCR0, SPI_REG_VAL_SPIGCR0_RESET);
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
    v &= DEVICE_JTAG_ID_MASK;

    if (v == DEVICE_C6657_JTAG_ID_VAL)
        params = (BOOT_PARAMS_COMMON_T *)ROM_BOOT_PARAMS_ADDR_C6657;


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

	/* TODO:cpufreqMhz need update */
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

