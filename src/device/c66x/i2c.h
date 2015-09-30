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
I2C_RET hwI2cMasterWrite (UINT32 eeprom_i2c_id, UINT8 *eData, UINT32 nbytes, UINT32 endBusState, BOOL busIsMine);
I2C_RET hwI2cMasterRead (UINT32 byte_addr, UINT32 byte_len, UINT8 *p_packed_bytes, UINT32 eeprom_i2c_id, UINT32 address_delay);



