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
*/
/******************************************************************************
 *
 * File Name:  	gpio.h
 *
 * Description:	This file is the header file for GPIO module
 * 
 * History:		
 * 	JUL/22/2009, Amit Solanki,	Created the file
 * 
 ******************************************************************************/
#ifndef GPIO_H_
#define GPIO_H_
/****************
 * Include Files
 ****************/
#include "types.h"

/*********************************
 * Defines and Macros and globals
 *********************************/

#define GPIO_0	0
#define GPIO_1	1
#define GPIO_2	2
#define GPIO_3	3
#define GPIO_4	4
#define GPIO_5	5
#define GPIO_6	6
#define GPIO_7	7
#define GPIO_8	8
#define GPIO_9	9
#define GPIO_10	10
#define GPIO_11	11
#define GPIO_12	12
#define GPIO_13	13
#define GPIO_14	14
#define GPIO_15	15

#define GPIO_DATAMASK     0x000000FF

#define GPIO_MAX_NUMBER 		15

#define SUCCESS 	0
#define GPIO_LOW 	0
#define GPIO_HIGH	1
#define INVALID_GPIO_NUMBER 	2
#define INVALID_GPIO_DIRECTION 	3
#define INVALID_GPIO_STATE		4

// GPIO pins and the Nand flash controller signal mapping

// GPIO[7:0] are used as bi-directional data-bus
// between DSP and FPGA

// Command pins
#define DSP_FPGA_CMD0 		GPIO_8
#define DSP_FPGA_CMD1 		GPIO_9

// DSP to FPGA strobe for synchronization
#define DSP_FPGA_STROBE 	GPIO_10

// FPGA to DSP ready signal
#define FPGA_DSP_READY		GPIO_11

// LED signals
#define DSP_FPGA_LED_0		GPIO_12
#define DSP_FPGA_LED_1		GPIO_13

// NAND or non-UART FPGA interrupt
#define FPGA_DSP_NFC_INTR	GPIO_14

// Nand mode selection GPIO
#define NAND_MODE_GPIO		GPIO_14

// UART interrupt
#define FPGA_DSP_UART_INTR	GPIO_15

/*********************
 * Typedefs and Enums
 *********************/
typedef enum _GpioDirection
{
	GPIO_OUT = 0,  
	GPIO_IN 
}GpioDirection;

/************************
 * Function declarations
 ************************/
void hwGpioSetDirection( Uint32 uiNumber, GpioDirection direction );
void hwGpioSetDataBusDirection( GpioDirection direction );
void hwGpioSetOutput( Uint32 uiNumber);
void hwGpioClearOutput( Uint32 uiNumber);
Uint32 hwGpioReadInput( Uint32 uiNumber );
void hwGpioWriteDataBus ( Uint8 uchValue );
Uint8 hwGpioReadDataBus( void );
void hwGpioEnableGlobalInterrupt( void );
void hwGpioSetDataBus(Uint8 uchValue);
void hwGpioClearDataBus(Uint8 uchValue);
void hwGpioDisableGlobalInterrupt( void );
void hwGpioSetRisingEdgeInterrupt( Uint32 uiNumber );
void hwGpioClearRisingEdgeInterrupt( Uint32 uiNumber );
void hwGpioSetFallingEdgeInterrupt( Uint32 uiNumber );
void hwGpioClearFallingEdgeInterrupt( Uint32 uiNumber );

#endif // GPIO_H_

