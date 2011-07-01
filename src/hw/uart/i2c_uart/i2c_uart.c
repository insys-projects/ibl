/******************************************************************************
 * Copyright (c) 2010 Texas Instruments Incorporated - http://www.ti.com
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
 *****************************************************************************/

/******************************************************************************
 *
 * File Name:  	evmc64x_i2c_uart.c
 *
 * Description:	This file contains APIs for I2C UART. 
 * 
 * History:		
 * 		AUG/11/2009, Amit Solanki,	Created the file
 * 		SEP/01/2009, Amit Solanki,  Updated the documentation
 * 
 ******************************************************************************/

/************************
 * Include Files
 ************************/
#include "evmc64x.h"
#include "evmc64x_i2c.h"
#include "evmc64x_i2c_uart.h"

/******************************************************************************
 * 
 * Function:	i2cUartInit  
 *
 * Description:	This function initializes the UART. 
 *
 * Parameters:	void
 *
 * Return Value: void
 * 
 ******************************************************************************/
void i2cUartInit(UART_device eUartDevice)
{
	// Enable the Divisor lathc bit in LCR for accessing special register(DLL,DLH)
	i2cUartWrite (eUartDevice, LCR, 0x80); 	
	// Set the baudrate,for accessing LCR[7] should be enable 
	i2cUartWrite (eUartDevice, DLL, DLL_VAL); 	
	i2cUartWrite (eUartDevice, DLM, DLM_VAL);

	// 8-bits
	i2cUartWrite (eUartDevice, LCR, 0x03);  
		 
	// Disable THR and RHR interrupt
    i2cUartWrite (eUartDevice, IER, 0x00);
    
    // set all GPIO as 1
    i2cUartWrite (eUartDevice, IOSTATE, 0xFF);
    // Set GPIO direction as ouput
    i2cUartWrite (eUartDevice, IODIR, 0xFF);
}

/******************************************************************************
 * 
 * Function:	i2cUartSetBaudRate  
 *
 * Description:	This function sets the UART baudrate. 
 *
 * Parameters:	UINT16 uiBaudRate - baudrate to set
 *
 * Return Value: void
 * 
 ******************************************************************************/
void i2cUartSetBaudRate(UART_device eUartDevice, UINT16 uiBaudRate)
{
	UINT8 uiDLLVal = 0;
	UINT8 uiDLHVal = 0;
	
	// Enable the Divisor lathc bit in LCR for 
	// accessing special register(DLL,DLH)
	i2cUartWrite (eUartDevice, LCR, 0x80);
	uiDLLVal = (UINT8 )(0x00FF & uiBaudRate); 	
	uiDLHVal = (UINT8 )(0xFF00 & uiBaudRate) >> 8;
	
	// Set the baudrate,for accessing LCR[7] should be enable 
	i2cUartWrite (eUartDevice, DLL, uiDLLVal); 	
	i2cUartWrite (eUartDevice, DLM, uiDLHVal);

	// 8-bits
	i2cUartWrite (eUartDevice, LCR, 0x03);  
}

/******************************************************************************
 * 
 * Function:	i2cUartReadBaudRate  
 *
 * Description:	This function reads the UART baudrate. 
 *
 * Parameters:	UART_device eUartDevice - Uart Device
 *
 * Return Value: UINT16 - 16 bit Baudrate read from UART
 * 
 ******************************************************************************/
UINT16 i2cUartReadBaudRate(UART_device eUartDevice)
{
	UINT16 ushBaudrate = 0; 
	UINT16 ushTemp = 0;
	
	// Enable the Divisor lathc bit in LCR for accessing special register(DLL,DLH)
	i2cUartWrite (eUartDevice, LCR, 0x80);
	
	// Read the baudrate 
	ushBaudrate = i2cUartRead(eUartDevice, DLL);
	ushTemp = i2cUartRead(eUartDevice, DLM);
	ushBaudrate = (ushBaudrate & 0xFF) | ((ushTemp & 0xFF) << 8); 

	// 8-bits
	i2cUartWrite (eUartDevice, LCR, 0x03);
	
	return ushBaudrate;
}

/******************************************************************************
 * 
 * Function:	i2cUartRead  
 *
 * Description:	This function reads a byte of data from I2C UART device  
 * 
 * Parameters:	UART_device eUartDevice - Uart Device	
 * 				UINT8 uchAddress - Address of 8-bit register
 *
 * Return Value: UINT8 - 8-bit value read from the register
 ******************************************************************************/
UINT8 i2cUartRead(UART_device eUartDevice, UINT8 uchAddress)
{
	UINT8 uChAdd;
	UINT8 uRcvChar = 0;
	
	if(I2C_UART_A == eUartDevice)
		uChAdd = (uchAddress | UARTA_SELECT);
	else
		uChAdd = (uchAddress | UARTB_SELECT);

	uRcvChar = i2cReadByte(uChAdd, I2C_UART_ADDR);
	
	return uRcvChar;
}

/******************************************************************************
 * 
 * Function:	i2cUartWrite  
 *
 * Description:	This function writes a byte of data to I2C UART device  
 * 
 * Parameters:	UART_device eUartDevice - Uart Device	
 * 				UINT8 uchAddress - Address of 8-bit register
 * 				UINT8 uchByte	-  8-bit data to write
 *
 * Return Value: UINT8 - 8-bit value read from the register
 ******************************************************************************/
void i2cUartWrite(UART_device eUartDevice, UINT8 uchAddress, 
					  UINT8 uchByte)
{
	UINT8 uChAdd;
	
	if(I2C_UART_A == eUartDevice)
		uChAdd = (uchAddress | UARTA_SELECT);
	else
		uChAdd = (uchAddress | UARTB_SELECT);
	
	i2cWriteByte(uChAdd, uchByte, I2C_UART_ADDR);
}

/******************************************************************************
 * 
 * Function:	i2cUartLedSet  
 *
 * Description:	This function Sets ON a user LED. 
 *
 * Parameters:	UINT32 uiLedNum - Led number (0-7)
 *
 * Return Value: void
 * 
 ******************************************************************************/
void i2cUartLedSet(UINT32 uiLedNum)
{
	UINT8 uchTemp = 0;
	uchTemp = i2cUartRead(I2C_UART_A, IOSTATE);
	uchTemp &= ~( 1 << (uiLedNum & 0x07) );
	i2cUartWrite (I2C_UART_A, IOSTATE, uchTemp);
}

/******************************************************************************
 * 
 * Function:	i2cUartLedClear  
 *
 * Description:	This function clears OFF a user LED. 
 *
 * Parameters:	UINT32 uiLedNum - Led number (0-7)
 *
 * Return Value: void
 * 
 ******************************************************************************/
void i2cUartLedClear(UINT32 uiLedNum)
{
	UINT8 uchTemp = 0;
	uchTemp = i2cUartRead(I2C_UART_A, IOSTATE);
	uchTemp |= (1 << (uiLedNum & 0x07));
	i2cUartWrite (I2C_UART_A, IOSTATE, uchTemp);
}
