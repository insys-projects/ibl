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
 * File	Name:	evmc64x_i2c_uart.h
 *
 * Description:	This contains UART specific structure, typedefs, function
 *		prototypes.
 *
 * History:
 *		AUG/11/2009, Amit Solanki,	Created	the file
 * 		AUG/8/2010, Deepika Makhija, Modified for Dual UART
 *
 ******************************************************************************/
#ifndef	_EVM64X_I2C_UART_H_
#define	_EVM64X_I2C_UART_H_

/************************
 * Include Files
 ************************/
#include "types.h"

/************************
 * Defines and Macros
 ************************/
 // UART I2C Slave Address
#define	I2C_UART_ADDR	(0x4D)

#define UARTA_SELECT	(0x0 << 1)
#define UARTB_SELECT	(0x1 << 1)

// UART	register addresses
#define	THR			(0x00 << 3)		// transmit FIFO, write	only
#define	RHR			(0x00 << 3)		// receive FIFO, read oly
#define	IER			(0x01 << 3)		// interrup enable reg., R/W
#define	FCR			(0x02 << 3)		// FIFO	control, write only
#define	IIR			(0x02 << 3)		// interrupt status, read only
#define	LCR			(0x03 << 3)		// line	control, R/W
#define	MCR			(0x04 << 3)		// modem control, R/W
#define	LSR			(0x05 << 3)		// Line	status,	R/W
#define	MSR			(0x06 << 3)		// Modem status, R/W
#define	SPR			(0x07 << 3)		// scratch pad,	R/W
#define	TXFIFO		(0x08 << 3)		// TX FIFO, R
#define	RXFIFO		(0x09 << 3)		// RX FIFO, R

#define	IODIR		(0x0A << 3)		// IO Direction	Control	R/W
#define	IOSTATE		(0x0B << 3) 	// IO State R/W	
#define	IOINTMSK	(0x0C << 3)		// IO Interrupt	Mask R/W
#define	IOCTRL		(0x0E << 3)		// IO Control R/W
#define	EFCR		(0x0F << 3)		//  Enhancede Function Reg R/W

// Special registers, LCR.7 must be set	
// to 1 to	R/W to these registers
#define	DLL			(0x00 << 3)		// Baud	rate divisor, LSB, R/W
#define	DLM			(0x01 << 3)		// Baud	rate divisor, MSB, R/W
#define	EFR			(0x02 << 3)		// Enhanced register, R/W
#define	XON1		(0x04 << 3)		// Flow	control	
#define	XON2		(0x05 << 3)		// Flow	control	
#define	XOFF1		(0x06 << 3)		// Flow	control	
#define	XOFF2		(0x07 << 3)		// Flow	control	

// Mask	to enable DLL and DLM
#define	DLAB		(0x80)			// Way to swap mem banks

// for 19200 baudrate for crystal clock	14.7456	MHz
#define	DLL_VAL		(0x30)
#define	DLM_VAL		(0x00)

// Macros to be	used for setting baudrate
#define	BAUD_RATE_9600		(0x0060)
#define	BAUD_RATE_19200		(0x0030)
#define	BAUD_RATE_56000		(0x0010)
#define	BAUD_RATE_115200	(0x0008)

/************************
 * Structures and Enums
 ************************/
typedef enum _UART_device
{
	I2C_UART_A = 0,
	I2C_UART_B
}UART_device;

/************************
 * Function declarations
************************/
void i2cUartInit(UART_device eUartDevice);
void i2cUartSetBaudRate(UART_device eUartDevice, UINT16 uiBaudRate);
UINT16 i2cUartReadBaudRate(UART_device eUartDevice);
UINT8 i2cUartRead(UART_device eUartDevice, UINT8 uchAddress);
void i2cUartWrite(UART_device eUartDevice, UINT8 uchAddress, 
					  UINT8 uchByte);
void i2cUartLedSet(UINT32 uiLedNum);
void i2cUartLedClear(UINT32 uiLedNum);

#endif // _EVM64X_I2C_UART_H_

