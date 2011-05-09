#ifndef _SPI_LOC_H
#define _SPI_LOC_H
/************************************************************************************************
 * FILE PURPOSE: LOCAL SPI definitions
 ************************************************************************************************
 * FILE NAME: spi_loc.h
 *
 * DESCRIPTION: Defines the SPI hardware operation
 *
 ************************************************************************************************/

/* Register offsets */
#define SPI_REG_SPIGCR0         0x00
#define SPI_REG_SPIGCR1         0x04
#define SPI_REG_SPIFLG          0x10
#define SPI_REG_SPIPC0          0x14
#define SPI_REG_SPIDAT0         0x38
#define SPI_REG_SPIDAT1         0x3c
#define SPI_REG_SPIBUF          0x40
#define SPI_REG_SPIDELAY        0x48
#define SPI_REG_SPIFMT(x)       (0x50 + ((x)*4))
    
/* Register values */    
#define SPI_REG_VAL_SPIGCR0_RESET           0x0
#define SPI_REG_VAL_SPIGCR0_ENABLE          0x1

#define SPI_REG_VAL_SPIGCR1_XFER            0x01000003
#define SPI_REG_VAL_SPIGCR1_XFER_DISABLE    0

#define SPI_REG_SPIFLG_TX_EMPTY(v)  ((v) & 0x0200)
#define SPI_REG_SPIFLG_RX_DATA(v)   ((v) & 0x0100)

#define SPI_REG_VAL_SPIPC0_4PIN     0x01010e01  /* 1 pin input, 1 pin output, clock, cs0 */
#define SPI_REG_VAL_SPIPC0_5PIN     0x01010e03  /* Same as 3 pin with cs1 as well */

#define SPI_REG_SPIFMT_SET_PRESCALE(v, dev)   (v) = BOOT_SET_BITFIELD((v),dev,  15,  8)
#define SPI_REG_SPIFMT_SET_CHARLEN(v, wid)    (v) = BOOT_SET_BITFIELD((v),wid,   4,  0)
#define SPI_REG_SPIFMT_SET_MODE(v, mode)      (v) = BOOT_SET_BITFIELD((v),mode, 17, 16)
#define SPI_REG_SPIFMT_SET_SHIFTDIR(v, dir)   (v) = BOOT_SET_BITFIELD((v),dir,  20, 20)

#define SPI_REG_VAL_SPIFMT_SHIFT_MSB_FIRST    0


#define SPI_REG_SPIDELAY_SET_C2T(v, delay)    (v) = BOOT_SET_BITFIELD((v), delay, 31, 24)


#define SPI_REG_SPIDAT1_SET_CSHOLD(v, hold)   (v) = BOOT_SET_BITFIELD((v), hold, 28, 28)
#define SPI_REG_SPIDAT1_SET_WDELAY(v, delay)  (v) = BOOT_SET_BITFIELD((v), delay, 26, 26)
#define SPI_REG_SPIDAT1_SET_CSNR(v, csel)     (v) = BOOT_SET_BITFIELD((v), csel, 23, 16)
#define SPI_REG_SPIDAT1_SET_DATA(v, data)     (v) = BOOT_SET_BITFIELD((v), data, 15,  0)


/* Commands */
#define SPI_COMMAND_WRITE           0x02
#define SPI_COMMAND_READ            0x03
#define SPI_COMMAND_READ_STATUS     0x05
#define SPI_COMMAND_WRITE_ENABLE    0x06
#define SPI_COMMAND_ERASE_SECTOR    0x20

SINT16 hw_spi_xfer (UINT32 nbytes, UINT8 *dataOut, UINT8 *dataIn, spiConfig_t *cfg, BOOL terminate);

#endif /* _SPI_LOC_H */
