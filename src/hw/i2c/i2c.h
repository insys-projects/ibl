/*********************************************************************
 * FILE PURPOSE: I2C API definition
 *********************************************************************
 * FILE NAME: i2c.h
 *
 * DESCRIPTION: Defines the i2c API 
 *
 * @file i2c.h
 *
 * @brief
 *  Defines the i2c API
 *
 *********************************************************************/
#include "types.h"

typedef UINT16 I2C_RET;


#define I2C_RET_OK              0
#define I2C_RET_LOST_ARB        1
#define I2C_RET_NO_ACK          2
#define I2C_RET_IDLE_TIMEOUT    3
#define I2C_RET_BAD_REQUEST     4
#define I2C_RET_CLOCK_STUCK_LOW 5
#define I2C_RET_GEN_ERROR       99


enum {
  I2C_RELEASE_BUS,
  I2C_DO_NOT_RELEASE_BUS
};


void hwI2Cinit (UINT16 coreFreqMhz, UINT16 moduleDivisor, UINT16 clkFreqKhz, UINT16 ownAddr);
I2C_RET hwI2cMasterWrite (UINT16 eeprom_i2c_id, UINT8 *eData, UINT16 nbytes, UINT16 endBusState, BOOL busIsMine);
I2C_RET hwI2cMasterRead (UINT32 byte_addr, UINT32 byte_len, UINT8 *p_packed_bytes, UINT16 eeprom_i2c_id, UINT16 address_delay);



