#ifndef _I2CLOC_H
#define _I2CLOC_H
/***********************************************************************
 * FILE PURPOSE: Local definitions used to run the I2C peripheral
 ***********************************************************************
 * FILE NAME: i2cloc.h
 *
 * DESCRIPTION: Provides defintions and prototypes local to the i2c module
 *
 ************************************************************************/
#include "types.h"

#define BOOTBITMASK(x,y)      (   (   (  ((UINT32)1 << (((UINT32)x)-((UINT32)y)+(UINT32)1) ) - (UINT32)1 )   )   <<  ((UINT32)y)   )
#define BOOT_READ_BITFIELD(z,x,y)   (((UINT32)z) & BOOTBITMASK(x,y)) >> (y)
#define BOOT_SET_BITFIELD(z,f,x,y)  (((UINT32)z) & ~BOOTBITMASK(x,y)) | ( (((UINT32)f) << (y)) & BOOTBITMASK(x,y) )

/* Register addresses relative to a base address */
#define I2C_REG_OAR         0x00
#define I2C_REG_IER         0x04
#define I2C_REG_STR         0x08
#define I2C_REG_CLKL        0x0c 
#define I2C_REG_CLKH        0x10
#define I2C_REG_CNT         0x14
#define I2C_REG_DRR         0x18
#define I2C_REG_SAR         0x1c
#define I2C_REG_DXR         0x20
#define I2C_REG_MDR         0x24
#define I2C_REG_ISR         0x28
#define I2C_REG_EXMODE      0x2c
#define I2C_REG_PSC         0x30
#define I2C_REG_PID1        0x34
#define I2C_REG_PID2        0x38
#define I2C_REG_PFUNC       0x48

/* Individual register definitions */
#define I2C_VAL_REG_MDR_RESET       0x4000
#define I2C_VAL_REG_MDR_SLVRCV      0x40A0
#define I2C_VAL_REG_MDR_MSTRCV      0x64A0
#define I2C_VAL_REG_MDR_MSTRCVSTOP  0x4CA0
#define I2C_VAL_REG_MDR_MSTXMT      0x46A0
#define I2C_VAL_REG_MDR_MSTXMTSTRT  0x66A0
#define I2C_VAL_REG_MDR_MSTXMTSTOP  0x4CA0

#define I2C_VAL_REG_STR_RESET    0x0410
#define I2C_VAL_REG_STR_ON_FAIL  0x1002  /* Clear bus busy, clear nack */
#define I2C_VAL_REG_STR_CLR_BUSY 0x1000  /* Clear busy                 */
#define I2C_VAL_REG_STR_CLR_RRDY 0x003F

/* Bit field definitions */
#define I2C_REG_STR_FIELD_BB(x)    BOOT_READ_BITFIELD((x), 12, 12)
#define I2C_REG_STR_FIELD_NACK(x)  BOOT_READ_BITFIELD((x),  1,  1)
#define I2C_REG_STR_FIELD_ARDY(x)  BOOT_READ_BITFIELD((x),  2,  2)
#define I2C_REG_STR_FIELD_XRDY(x)  BOOT_READ_BITFIELD((x),  4,  4)
#define I2C_REG_STR_FIELD_RRDY(x)  BOOT_READ_BITFIELD((x),  3,  3)



/* Byte ordering */
enum {
  I2C_BYTE_LSB,
  I2C_BYTE_MSB
};

/************************************************************************
 * Definition: Desired frequency for module operation in Qx.1 format
 ************************************************************************/
#define I2C_TARGET_MODULE_FREQ_MHZ_Q1  27  /* 13.5 MHz */


/************************************************************************
 * Definition: Timeout limit for master receiver. The units are 
 *             in number of bits, so provide some overhead 
 ************************************************************************/
#define I2C_MAX_MASTER_RECEIVE_TIMEOUT     240   /* 30 bytes */ 

/************************************************************************
 * Definition: Timeout limit for master transmitter. The units are
 *             in number of bits, so provide some overhead
 ************************************************************************/
#define I2C_MAX_MASTER_TRANSMITTER_TIMEOUT  240  /* 30 bytes */

/************************************************************************
 * Definition: Timeout limit for the master transmitter to get access
 *             to the bus. In 10ms units.
 ************************************************************************/
#define I2C_MAX_MASTER_TRANSMITTER_BUS_ACCESS_TIMEOUT  100
#define I2C_MASTER_TRANSMITTER_BUS_ACCESS_DELAY_US     10000

/*************************************************************************
 * Definition: Timeout limit after a master transmitter operation is 
 *             complete, and waiting for access to the MMRs. This should
 *             be on the order of two bytes, for the last two that are
 *             being sent (one in the shift register, one in the dxr. The
 *             units are in bits.
 *************************************************************************/
#define I2C_MAX_MASTER_TRANSMITTER_ARDY_TIMEOUT   32  /* 4 bytes */


/*************************************************************************
 * Definition: Timeout limit in slave receiver mode. The unit is in 
 *             expected bit periods, but is long since the master
 *             may have a long delay before beginning transmission.
 *************************************************************************/
#define I2C_MAX_SLAVE_RECEIVE_TIMEOUT      5000000




#endif /* _I2CLOC_H */
