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
 * File Name:  	gpio.c
 *
 * Description:	This file contains the lower level function to access GPIO 
 * 				
 * History:		
 * 		JUL/22/2009, Amit Solanki,	Created the file
 * 		SEP/01/2009, Amit Solanki,  Updated the documentation
 * 
 *****************************************************************************/
/****************
 * Include Files
 ****************/
#include "types.h"
#include "gpio.h"
#include "target.h"

/******************************************************************************
 * 
 * Function:		hwGpioSetDirection  
 *
 * Description:		This function configures the specified GPIO's direction  
 *
 * Parameters:		uiNumber - 	GPIO number to configure
 * 					direction - GPIO_OUT or GPIO_IN
 *
 * Return Value: 	void
 * 
 *****************************************************************************/
void hwGpioSetDirection( Uint32 uiNumber, GpioDirection direction )
{
    Uint32* puiGpioDir = ( Uint32*)GPIO_DIR_REG;
    
    if ( direction == GPIO_OUT )
    	*puiGpioDir =  *puiGpioDir & ~(1 << uiNumber);  // Set to OUTPUT
    else
    	*puiGpioDir =  *puiGpioDir | (1 << uiNumber);  // Set to INTPUT
}

/******************************************************************************
 * 
 * Function:		hwGpioSetDataBusDirection  
 *
 * Description:		This function configures the GPIO[7:0]'s direction  
 *
 * Parameters:		direction - GPIO_OUT or GPIO_IN
 * 					
 * Return Value: 	void
 * 
 *****************************************************************************/
void hwGpioSetDataBusDirection( GpioDirection direction )
{
    Uint32* puiGpioDir = ( Uint32*)GPIO_DIR_REG;

    if ( direction == GPIO_OUT )
    	*puiGpioDir =  *puiGpioDir & 0xffffff00;  // Set to OUTPUT
    else
    	*puiGpioDir =  *puiGpioDir | 0x000000ff;  // Set to INTPUT
}

/******************************************************************************
 * 
 * Function:		hwGpioSetOutput  
 *
 * Description:		This function sets the specified GPIO's pin state to 1
 *
 * Parameters:		uiNumber - 	GPIO number to configure
 *
 * Return Value: 	void
 * 
 * Pre-Condition:	The specified GPIO should be configured as output 
 * 
 *****************************************************************************/
void hwGpioSetOutput( Uint32 uiNumber)
{
    Uint32* puiGpioSet = (Uint32*) GPIO_SET_DATA_REG;    
    *puiGpioSet = ( 1 << (uiNumber % 32) );  // Set to 1
}

/******************************************************************************
 * 
 * Function:		hwGpioClearOutput  
 *
 * Description:		This function Clears the specified GPIO's pin state to 0  
 *
 * Parameters:		uiNumber - 	GPIO number to configure
 *
 * Return Value: 	void
 * 
 * Pre-Condition:	The specified GPIO should be configured as output 
 * 
 *****************************************************************************/
void hwGpioClearOutput( Uint32 uiNumber)
{
    Uint32* puiGpioClear = (Uint32*) GPIO_CLEAR_DATA_REG;    
 	*puiGpioClear = ( 1 << (uiNumber % 32) );   // Clear to 0
}

/******************************************************************************
 * 
 * Function:		hwGpioReadInput  
 *
 * Description:		This function gets the specified GPIO's pin state  
 *
 * Parameters:		uiNumber - 	GPIO number to configure
 *
 * Return Value: 	Uint32 - Input state of GPIO if success
 * 							  	- else GPIO status
 * 
 * Pre-Condition:	The specified GPIO should be configured as input
 * 
 *****************************************************************************/
Uint32 hwGpioReadInput( Uint32 uiNumber )
{
	Uint32* puiGpioInput = (Uint32*)GPIO_IN_DATA_REG;    
    if(uiNumber > GPIO_MAX_NUMBER)
    	return INVALID_GPIO_NUMBER;
    
    if( ( (*puiGpioInput >> uiNumber) & GPIO_HIGH) == GPIO_HIGH)
    	return GPIO_HIGH;
    else
    	return GPIO_LOW;
}

/******************************************************************************
 * 
 * Function:		hwGpioWriteDataBus  
 *
 * Description:		This function sets the GPIO[7:0] pins state  
 *
 * Parameters:		uchValue - 	Value to set as output
 *
 * Return Value: 	void
 * 
 * Pre-Condition:	The GPIO[7:0] should be configured as output
 * 
 *****************************************************************************/
void hwGpioWriteDataBus ( Uint8 uchValue )
{
    Uint32* puiGpioOutput = (Uint32*) GPIO_OUT_DATA_REG;
	*puiGpioOutput = ( (*puiGpioOutput & 0xffffff00) | uchValue );
}

/******************************************************************************
 * 
 * Function:		hwGpioClearDataBus  
 *
 * Description:		This function sets the value to the Clear data register
 *
 * Parameters:		uchValue - 	Value to set as output
 *
 * Return Value: 	void
 * 
 * Pre-Condition:	None
 * 
 *****************************************************************************/
void hwGpioClearDataBus(Uint8 uchValue)
{
    Uint32* puiGpioOutput = (Uint32*) GPIO_CLEAR_DATA_REG;
	*puiGpioOutput = uchValue;
}

/******************************************************************************
 * 
 * Function:		hwGpioSetDataBus  
 *
 * Description:		This function sets the value to the Set data register
 *
 * Parameters:		uchValue - 	Value to set as output
 *
 * Return Value: 	void
 * 
 * Pre-Condition:	None
 * 
 *****************************************************************************/
void hwGpioSetDataBus(Uint8 uchValue)
{
    Uint32* puiGpioOutput = (Uint32*) GPIO_SET_DATA_REG;
	*puiGpioOutput = uchValue;
}

/******************************************************************************
 * 
 * Function:		hwGpioReadDataBus  
 *
 * Description:		This function gets the GPIO[7:0] pins state
 * 
 * Parameters:		void
 *
 * Return Value: 	Uint8 - Input state of GPIO[7:0]
 * 
 * Pre-Condition:	The GPIO[7:0] should be configured as input
 * 
 *****************************************************************************/
Uint8 hwGpioReadDataBus( void )
{
	Uint32 temp;
	Uint32* puchGpioInput = (Uint32*)GPIO_IN_DATA_REG;
	temp = *puchGpioInput;
	temp = temp & 0xff;
	return ((Uint8)temp);
}

/******************************************************************************
 * 
 * Function:		hwGpioEnableGlobalInterrupt  
 *
 * Description:		This function Enables GPIO interrupts to CPU
 * 
 * Parameters:		void
 *
 * Return Value: 	void
 * 
 *****************************************************************************/
void hwGpioEnableGlobalInterrupt( void )
{
	Uint32* puiGpioInterrupt = (Uint32*)GPIO_BINTEN_REG;
    *puiGpioInterrupt = 0x01;
}

/******************************************************************************
 * 
 * Function:		hwGpioDisableGlobalInterrupt 
 *
 * Description:		This function Disables GPIO interrupts to CPU
 * 
 * Parameters:		void
 *
 * Return Value: 	void
 * 
 *****************************************************************************/
void hwGpioDisableGlobalInterrupt( void )
{
	Uint32* puiGpioInterrupt = (Uint32*)GPIO_BINTEN_REG;
    *puiGpioInterrupt = 0x00;
}

/******************************************************************************
 * 
 * Function:		hwGpioSetRisingEdgeInterrupt 
 *
 * Description:		This function sets specified GPIO's rising edge interrupt
 * 
 * Parameters:		uiNumber - 	GPIO number to configure
 *
 * Return Value: 	void
 * 
 *****************************************************************************/
void hwGpioSetRisingEdgeInterrupt( Uint32 uiNumber )
{
	Uint32* puiGpioSetRisingEdge = (Uint32*)GPIO_SET_RIS_TRIG_REG;
    *puiGpioSetRisingEdge = (1 << uiNumber);
}

/******************************************************************************
 * 
 * Function:		hwGpioClearRisingEdgeInterrupt 
 *
 * Description:		This function clears specified GPIO's rising edge interrupt
 * 
 * Parameters:		uiNumber - 	GPIO number to configure
 *
 * Return Value: 	void
 * 
 *****************************************************************************/
void hwGpioClearRisingEdgeInterrupt( Uint32 uiNumber )
{
	Uint32* puiGpioClearRisingEdge = (Uint32*)GPIO_CLR_RIS_TRIG_REG;
    *puiGpioClearRisingEdge = (1 << uiNumber);
}

/******************************************************************************
 * 
 * Function:		hwGpioSetFallingEdgeInterrupt 
 *
 * Description:		This function sets specified GPIO's falling edge interrupt
 * 
 * Parameters:		uiNumber - 	GPIO number to configure
 *
 * Return Value: 	void
 * 
 *****************************************************************************/
void hwGpioSetFallingEdgeInterrupt( Uint32 uiNumber )
{
	Uint32* puiGpioSetFallingEdge = (Uint32*)GPIO_SET_FAL_TRIG_REG;
    *puiGpioSetFallingEdge = (1 << uiNumber);
}

/******************************************************************************
 * 
 * Function:		hwGpioClearFallingEdgeInterrupt 
 *
 * Description:		This function clears specified GPIO's falling edge interrupt
 * 
 * Parameters:		uiNumber - 	GPIO number to configure
 *
 * Return Value: 	void
 * 
 *****************************************************************************/
void hwGpioClearFallingEdgeInterrupt( Uint32 uiNumber )
{
	Uint32* puiGpioClearFallingEdge = (Uint32*)GPIO_CLR_FAL_TRIG_REG;
    *puiGpioClearFallingEdge = (1 << uiNumber);
}
















