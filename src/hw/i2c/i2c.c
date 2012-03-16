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



/****************************************************************************
 * FILE PURPOSE: An i2c driver for the rom boot loader
 ****************************************************************************
 * FILE NAME: i2c.c
 *
 * DESCRIPTION: Provides an i2c driver for the rom boot loader. The driver
 *              is adapted from the csl polling driver in the 64x csl.
 *
 ****************************************************************************/ 
#include "types.h"
#include "i2c.h"
#include "i2cloc.h"
#include "target.h"

#define DEVICE_REG32_W(x,y)   *(volatile unsigned int *)(x)=(y)
#define DEVICE_REG32_R(x)    (*(volatile unsigned int *)(x))

 /**************************************************************************
  * Global variables
  *************************************************************************/
 UINT32 i2cCoreFreqMhz;
 UINT32 i2cBitPeriodCycles; 


/***********************************************************************************
 * FUNCTION PURPOSE: Provide an approximate delay
 ***********************************************************************************
 * DESCRIPTION: A delay in units of CPU cycles is executed
 ***********************************************************************************/
static void chipDelay32 (uint32 del)
{
    volatile unsigned int i;

    for (i = 0; i < del/8; i++){
        asm(" NOP ");
    }
}


/***************************************************************************
 * FUNCTION PURPOSE: Generate a delay
 ***************************************************************************
 * DESCRIPTION: Creates a delay specified in usec
 ***************************************************************************/
void hw_i2c_delay_usec (UINT32 usec)
{
  UINT32 delayCycles;

  delayCycles = usec * i2cCoreFreqMhz;

  chipDelay32 (delayCycles);

} /* hw_i2c_delay_usec */
 
 
 /***************************************************************************
  * FUNCTION PURPOSE: Initialize the I2C.
  ***************************************************************************
  * DESCRIPTION: Puts the I2C into reset, sets up the device and takes
  *              the device out of reset in slave receiver mode.
  **************************************************************************/
void hwI2Cinit (UINT16 coreFreqMhz, UINT16 moduleDivisor, UINT16 clkFreqKhz, UINT16 ownAddr)
 {
   UINT32 psc;
   UINT32 clkx;
   UINT32 tmp;
   UINT32 moduleFreqMhzQ1;
   UINT32 trueModFreqHz;
   UINT32 trueModFreqkHz;
 
   /* Put the i2c peripheral into reset */
   DEVICE_REG32_W (DEVICE_I2C_BASE + I2C_REG_MDR, I2C_VAL_REG_MDR_RESET);
   
   /* To determine the module prescaler Q31.1 format will be used. This is just
    * handled simply since the input module frequency is an integer it will just
    * be multiplied by two. Dividing two Q31.1 result in a Q30.0, or the divider *
    * Calculate module prescalars to get as close to target frequencies as possible */
   moduleFreqMhzQ1 = (coreFreqMhz << 1) / moduleDivisor;
   psc = moduleFreqMhzQ1 / I2C_TARGET_MODULE_FREQ_MHZ_Q1;
   if (psc > 0)
     psc = psc - 1;
   trueModFreqHz = ((UINT32)coreFreqMhz * (UINT32)1000000) / (moduleDivisor * (psc+1));
   
   tmp = trueModFreqHz / (clkFreqKhz * 1000);
   if (tmp > 12) 
     clkx = (tmp - 12) >> 1;
   else
     clkx = 0;
     
   /* Store the core frequency for use in generating delay */
   i2cCoreFreqMhz = coreFreqMhz;
   
   /* Calculate the number of micro seconds for each bit. */
   trueModFreqkHz = trueModFreqHz / 1000;  /* Prevent 32 bit integer overlflow */
   i2cBitPeriodCycles = (UINT32)coreFreqMhz * 1000 * (2 * clkx + 12) / trueModFreqkHz;
   
   /* Initialize the registers */
   DEVICE_REG32_W(DEVICE_I2C_BASE + I2C_REG_PSC, (UINT32)psc); 
   DEVICE_REG32_W(DEVICE_I2C_BASE + I2C_REG_OAR, (UINT32)ownAddr);
   DEVICE_REG32_W(DEVICE_I2C_BASE + I2C_REG_IER, (UINT32)0);
   DEVICE_REG32_W(DEVICE_I2C_BASE + I2C_REG_STR, I2C_VAL_REG_STR_RESET);
   DEVICE_REG32_W(DEVICE_I2C_BASE + I2C_REG_CLKL, clkx);
   DEVICE_REG32_W(DEVICE_I2C_BASE + I2C_REG_CLKH, clkx); 
   DEVICE_REG32_W(DEVICE_I2C_BASE + I2C_REG_CNT, (UINT32)0);
   DEVICE_REG32_W(DEVICE_I2C_BASE + I2C_REG_SAR, (UINT32)0x3ff);
   DEVICE_REG32_W(DEVICE_I2C_BASE + I2C_REG_DXR, (UINT32)0);
   DEVICE_REG32_W(DEVICE_I2C_BASE + I2C_REG_PFUNC, (UINT32)0);
   
   /* Take the I2C out of reset in the slave receiver mode */
   DEVICE_REG32_W(DEVICE_I2C_BASE + I2C_REG_MDR, I2C_VAL_REG_MDR_SLVRCV);
   
   /* Read the receive register to clear it */
   tmp = DEVICE_REG32_R(DEVICE_I2C_BASE + I2C_REG_DRR);


} /* hwI2Cinit */


/****************************************************************************
 * FUNCTION PURPOSE: Perform a master write 
 ****************************************************************************
 * DESCRIPTION: Enters master transmitter mode, writes a specified number
 *              of bytes. The byte stream arrives packed in the 16 bit
 *              data stream, and are sent most significant byte first.
 ****************************************************************************/
I2C_RET hwI2cMasterWrite (UINT32 eeprom_i2c_id, UINT8 *eData, UINT32 nbytes, UINT32 endBusState, BOOL busIsMine)
{
  UINT32 timeoutCount;
  UINT32 polling;
  UINT32 value;
  UINT32 str;
  UINT16 i;

  /* If the byte length is 0 there is nothing to do */
  if (nbytes == 0)
    return (I2C_RET_OK);
   
  /* Check for the bus busy signal */
  if (busIsMine == FALSE)  {
    timeoutCount = 0;
    do  { 
      polling = I2C_REG_STR_FIELD_BB(DEVICE_REG32_R (DEVICE_I2C_BASE + I2C_REG_STR));
    
      if (polling != 0)  {
        hw_i2c_delay_usec (I2C_MASTER_TRANSMITTER_BUS_ACCESS_DELAY_US);
      
        timeoutCount += 1;
        if (timeoutCount >= I2C_MAX_MASTER_TRANSMITTER_BUS_ACCESS_TIMEOUT)  {
      
          /* Return to slave receiver, clear nack and bus busy */
          DEVICE_REG32_W (DEVICE_I2C_BASE + I2C_REG_MDR, I2C_VAL_REG_MDR_SLVRCV);
          DEVICE_REG32_W (DEVICE_I2C_BASE + I2C_REG_STR, I2C_VAL_REG_STR_ON_FAIL);
          return (I2C_RET_IDLE_TIMEOUT);
        }
      } else  { /* The bus is free */
        timeoutCount = 0;
      }
    }
    while (timeoutCount != 0);

  }
      
  /* Enter master transmitter mode, set the slave address register */
  DEVICE_REG32_W (DEVICE_I2C_BASE + I2C_REG_MDR, I2C_VAL_REG_MDR_MSTXMT);
  DEVICE_REG32_W (DEVICE_I2C_BASE + I2C_REG_SAR, (UINT32)eeprom_i2c_id);
  
  /* Put the first byte into the transmit register, set the start bit */
  value = *eData & 0x00ff;
  eData = eData + 1;
  DEVICE_REG32_W (DEVICE_I2C_BASE + I2C_REG_DXR, value);
  

  /* Some delay required */
  chipDelay32 (i2cBitPeriodCycles);

  /* Set the start bit */
  DEVICE_REG32_W (DEVICE_I2C_BASE + I2C_REG_MDR, I2C_VAL_REG_MDR_MSTXMTSTRT);
  
  for (i = 1; i < nbytes; i++)  {
    timeoutCount = 0;
  
    do  {
      /* Read status */
      str = DEVICE_REG32_R (DEVICE_I2C_BASE + I2C_REG_STR);
    
      /* On Nack return failure */
      if (I2C_REG_STR_FIELD_NACK(str) != 0)  {
      
        /* Return to slave receiver, clear nack and bus busy */
        DEVICE_REG32_W (DEVICE_I2C_BASE + I2C_REG_MDR, I2C_VAL_REG_MDR_SLVRCV);
        DEVICE_REG32_W (DEVICE_I2C_BASE + I2C_REG_STR, I2C_VAL_REG_STR_ON_FAIL);
        return (I2C_RET_NO_ACK);
      }
        
      /* Check for transmit ready */
      if (I2C_REG_STR_FIELD_XRDY(str) != 0)  {
        timeoutCount = 0;
        
        value = *eData & 0x00ff;
        eData =  eData + 1;
        
        DEVICE_REG32_W (DEVICE_I2C_BASE + I2C_REG_DXR, value);

        
      }  else  {   /* XRDY bit not set */
        chipDelay32 (i2cBitPeriodCycles);
        timeoutCount += 1;
        if (timeoutCount >= I2C_MAX_MASTER_TRANSMITTER_TIMEOUT)  {
          /* Return to slave receiver, clear nack and bus busy */
          DEVICE_REG32_W (DEVICE_I2C_BASE + I2C_REG_MDR, I2C_VAL_REG_MDR_SLVRCV);
          DEVICE_REG32_W (DEVICE_I2C_BASE + I2C_REG_STR, I2C_VAL_REG_STR_ON_FAIL);
          return (I2C_RET_IDLE_TIMEOUT);
        }
      }
          
    } while (timeoutCount != 0);

  } /* end for loop */

  
  /* If releasing the bus, send a stop bit */
  if (endBusState == I2C_RELEASE_BUS)  {
  
    /* Wait for the ardy bit to go high */
    timeoutCount = 0;
    do  {
      str = DEVICE_REG32_R (DEVICE_I2C_BASE + I2C_REG_STR);
      if (I2C_REG_STR_FIELD_ARDY(str) != 0)  {
        DEVICE_REG32_W (DEVICE_I2C_BASE + I2C_REG_MDR, I2C_VAL_REG_MDR_MSTXMTSTOP);
        DEVICE_REG32_W (DEVICE_I2C_BASE + I2C_REG_STR, I2C_VAL_REG_STR_CLR_BUSY);
        hw_i2c_delay_usec (10000);
        timeoutCount = 0;
        
      } else  { /* Registers not ready for access */
        timeoutCount += 1;
        
        if (timeoutCount >= I2C_MAX_MASTER_TRANSMITTER_ARDY_TIMEOUT)  {
           /* On timeout put the peripheral into reset, wait, then
            * take it out of reset */
           DEVICE_REG32_W (DEVICE_I2C_BASE + I2C_REG_MDR, I2C_VAL_REG_MDR_RESET);
           hw_i2c_delay_usec (10000);
           DEVICE_REG32_W (DEVICE_I2C_BASE + I2C_REG_MDR, I2C_VAL_REG_MDR_SLVRCV);
           return (I2C_RET_IDLE_TIMEOUT);
         }
        chipDelay32 (i2cBitPeriodCycles); 
      }
    } while (timeoutCount != 0);
    
  } /* end bus release */
  
  return (I2C_RET_OK);
  
} /* hwI2cMasterWrite */
   
  

/**************************************************************************
 * FUNCTION PURPOSE: Perform a master read from an I2C prom
 **************************************************************************
 * DESCRIPTION: Reads a fixed number of bytes from an I2C prom. The read
 *              consists of a master write of 2 bytes (forming a 16 bit address,
 *              msb transmitted first), followed by a master read of the
 *              input number of bytes. The bytes that are read are placed
 *              in p_packed_bytes in big endian format.
 **************************************************************************/
I2C_RET hwI2cMasterRead (
  UINT32           byte_addr,
  UINT32           byte_len,
  UINT8           *p_packed_bytes,
  UINT32           eeprom_i2c_id,
  UINT32           address_delay)
{

  UINT32  str;  
  UINT32  timeoutCount;
  UINT32  i;
  UINT8   eAddr[2];
  I2C_RET ret;

  /* If the byte length is 0, there is nothing to do */
  if (byte_len == 0)
    return (I2C_RET_OK);
    
  /* Write the byte address to the eeprom. Do not send a stop */
  eAddr[0] = (byte_addr >> 8) & 0xff;
  eAddr[1] = byte_addr & 0xff;

  ret = hwI2cMasterWrite (eeprom_i2c_id, eAddr, 2, I2C_DO_NOT_RELEASE_BUS, FALSE);
  if (ret != I2C_RET_OK)
    return (ret);
    
  /* Give the I2C prom some time to process the read command */
  chipDelay32 (((UINT32)address_delay)<<8);

  /* Set the start bit, begin the master read */
  DEVICE_REG32_W (DEVICE_I2C_BASE + I2C_REG_MDR, I2C_VAL_REG_MDR_MSTRCV);

  /* Read the requested number of bytes */
  for (i = 0; i < byte_len; i++)  {
    timeoutCount = 0;
  
    do  {
      /* Read status */
      str = DEVICE_REG32_R (DEVICE_I2C_BASE + I2C_REG_STR);
    
      /* On Nack return failure */
      if (I2C_REG_STR_FIELD_NACK(str) != 0)  {
      
        /* Return to slave receiver, clear nack and bus busy */
        DEVICE_REG32_W (DEVICE_I2C_BASE + I2C_REG_MDR, I2C_VAL_REG_MDR_SLVRCV);
        DEVICE_REG32_W (DEVICE_I2C_BASE + I2C_REG_STR, I2C_VAL_REG_STR_ON_FAIL);
        return (I2C_RET_NO_ACK);
      }
        
      /* Check for receive byte ready */
      if (I2C_REG_STR_FIELD_RRDY(str) != 0)  {
        *p_packed_bytes = (UINT16) (DEVICE_REG32_R (DEVICE_I2C_BASE + I2C_REG_DRR) & 0x00ff);
        timeoutCount = 0;
        p_packed_bytes = p_packed_bytes + 1;
        
        
      }  else  {   /* RRDY bit not set */
        chipDelay32 (i2cBitPeriodCycles);
        timeoutCount += 1;
        if (timeoutCount >= I2C_MAX_MASTER_RECEIVE_TIMEOUT)  {
          /* Return to slave receiver, clear nack and bus busy */
          DEVICE_REG32_W (DEVICE_I2C_BASE + I2C_REG_MDR, I2C_VAL_REG_MDR_SLVRCV);
          DEVICE_REG32_W (DEVICE_I2C_BASE + I2C_REG_STR, I2C_VAL_REG_STR_ON_FAIL);
          return (I2C_RET_IDLE_TIMEOUT);
        }
      }
          
    } while (timeoutCount != 0);

  } /* end for loop */

  /* The data block has been read. Send the stop bit */
  DEVICE_REG32_W (DEVICE_I2C_BASE + I2C_REG_MDR, I2C_VAL_REG_MDR_MSTRCVSTOP);
        

  /* Wait for the rrdy and read the dummy byte */
  timeoutCount = 0;
  do  {

    str = DEVICE_REG32_R (DEVICE_I2C_BASE + I2C_REG_STR);

    /* Check for receive byte ready */
    if (I2C_REG_STR_FIELD_RRDY(str) != 0)  {
      eAddr[0] = (UINT16) (DEVICE_REG32_R (DEVICE_I2C_BASE + I2C_REG_DRR) & 0x00ff);
      timeoutCount = 0;

    }  else  {  /* rrdy not set */

      chipDelay32 (i2cBitPeriodCycles);
      timeoutCount += 1;
      if (timeoutCount >= I2C_MAX_MASTER_RECEIVE_TIMEOUT)  {
        /* Return to slave receiver, clear nack and bus busy */
        DEVICE_REG32_W (DEVICE_I2C_BASE + I2C_REG_MDR, I2C_VAL_REG_MDR_SLVRCV);
        DEVICE_REG32_W (DEVICE_I2C_BASE + I2C_REG_STR, I2C_VAL_REG_STR_ON_FAIL);
        return (I2C_RET_IDLE_TIMEOUT);
      }
    }

  } while (timeoutCount != 0);
  
  return (I2C_RET_OK);
  
} /* hwI2cMasterRead */
        
