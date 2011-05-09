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

#ifndef _EMIF25_LOC_H
#define _EMIF25_LOC_H

/* Register offsets */
#define EMIF25_ASYNC_CFG_REG(x)       (0x10 + (x)*4)
#define EMIF25_FLASH_CTL_REG          0x60
#define EMIF25_FLASH_STATUS_REG       0x64
#define EMIF25_FLASH_ECC_LOAD_REG     0xbc
#define EMIF25_FLASH_ECC_REG(x)       (0xc0 + (x)*4)
#define EMIF25_FLASH_ECC_LOAD_REG     0xbc
#define EMIF25_FLASH_ERR_ADDR_REG(x)  (0xd0 + (x)*4)
#define EMIF25_FLASH_ERR_VALUE_REG(x) (0xd8 + (x)*4)

/* Setting the bus width in the async config register */
#define EMIF25_SET_ASYNC_WID(x,v)     BOOT_SET_BITFIELD((x),(v),1,0)

/* Setting the wait in the async config register */
#define EMIF25_SET_ASYNC_WAIT(x,v)    BOOT_SET_BITFIELD((x),(v),30,30)

/* Setting the nand enable/disable in the flash control register */
#define EMIF25_SET_FLASH_CTL_NAND_ENABLE(x,v,cs)   BOOT_SET_BITFIELD((x),(v),((cs)-2),((cs)-2))

#endif /* _EMIF25_LOCK_H */

