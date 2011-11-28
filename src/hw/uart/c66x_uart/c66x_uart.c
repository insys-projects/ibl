/******************************************************************************
 * Copyright (c) 2011 Texas Instruments Incorporated - http://www.ti.com
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

#include "c66x_uart.h"

static void uart_delay_cycles(uint32_t cycles)
{
    while (cycles--) {
        asm ("NOP");
    }
}

/******************************************************************************
 *
 * Function:    uart_init
 *
 * Description: This function initializes UART peripheral for C66x
 *
 * Parameters: str    - string to print
 *             length - length of the string to print
 *
 * Return Value: none
 ******************************************************************************/
void uart_init(void)
{
    uint8_t DLL_val = 0;
    uint8_t DLH_val = 0;

    /* Setting baud rate to 115200 */
    DLL_val = (uint8_t )(0x00FF & BAUD_RATE_115200);
    DLH_val = (uint8_t )(0x00FF & (BAUD_RATE_115200  >> 8));

    /* Allows access to the divisor latches of the baud generator during a
       read or write operation (DLL and DLH) */

    uart_registers->LCR = 0x80;

    /* Set the baudrate,for accessing LCR[7] should be enable */
    uart_registers->DLL  = DLL_val;
    uart_registers->DLH  = DLH_val;

    /* Allows access to the receiver buffer register (RBR),
       the transmitter holding register (THR), and the
       interrupt enable register (IER) selected. */
    uart_registers->LCR = 0x18;

    /* Disable THR, RHR, Receiver line status interrupts */
    uart_registers->IER = 0;

    /* If autoflow control is desired,
     * write appropriate values to the modem
     * control register (MCR). Note that all UARTs
     * do not support autoflow control, see
     * the device-specific data manual for supported features.
     *
     * MCR
     * ====================================================
     * Bit  Field   Value   Description
     * 5    AFE     0       Autoflow control is disabled
     * 4    LOOP    0       Loop back mode is disabled.
     * 1    RTS     0       RTS control (UARTn_RTS is disabled,
     *                      UARTn_CTS is only enabled.)
     * =====================================================
     *
     *
     */

    uart_registers->MCR = 0;

    /* Choose the desired response to
     * emulation suspend events by configuring
     * the FREE bit and enable the UART by setting
     * the UTRST and URRST bits in the power and
     * emulation management register (PWREMU_MGMT).
     *
     *
     * PWREMU_MGMT
     * =================================================
     * Bit  Field   Value   Description
     * 14   UTRST   1       Transmitter is enabled
     * 13   URRST   1       Receiver is enabled
     * 0    FREE    1       Free-running mode is enabled
     * ===================================================
     *
     */
    uart_registers->PWREMU_MGMT = 0x6001;

    /* Cleanup previous data (rx trigger is also set to 0)*/
    uart_registers->FCR = 0xC1;

    /* Setting the baud rate */
    uart_registers->LCR = 0x80;

    /* Set the baudrate,for accessing LCR[7] should be enable */
    uart_registers->DLL  = DLL_val;
    uart_registers->DLH  = DLH_val;
    uart_registers->LCR  = 0x03;

    return;
}

/******************************************************************************
 *
 * Function:    uart_write_byte
 *
 * Description: This function writes a byte of data to UART device
 *
 * Parameters:  byte    -  8-bit data to write to THR
 *
 * Return Value: none
 ******************************************************************************/
static inline void uart_write_byte(uint8_t byte)
{
    while (!(uart_registers->LSR & UART_LSR_THRE_MASK)) {
        uart_delay_cycles(10000);
    }
    uart_registers->THR = (UART_THR_DATA_MASK & byte);
    return;
}

/******************************************************************************
 *
 * Function:    uart_write_string
 *
 * Description: This function writes a string of data to UART device
 *
 * Parameters: str    - string to print
 *             length - length of the string to print, maximum is 80
 *
 * Return Value: none
 ******************************************************************************/
void uart_write_string(char * str, uint32_t length)
{
    uint32_t i;
    if (length==0)
    {
    	/*Maximum length is 80 */
    	length=80;
    }
    for(i = 0; i < length; i++) {
        if(str[i]=='\0') break;
        uart_write_byte((uint8_t)str[i]);
    }
    uart_write_byte((uint8_t)0x0D);
    uart_write_byte((uint8_t)0x0A);
}

