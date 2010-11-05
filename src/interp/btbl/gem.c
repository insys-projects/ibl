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



/**********************************************************************************************************
 * FILE PURPOSE: Support functions specific to gem
 **********************************************************************************************************
 * FILE NAME: gem.c
 *
 * DESCRIPTION: Contains functions common to all gem devices.
 *
 **********************************************************************************************************/
#include "types.h"
#include "btblwrap.h"

extern volatile cregister unsigned int CSR;
#define C64X_REG_CSR_BIT_EN (1 << 8)

/************************************************************************************
 * FUNCTION PURPOSE: Copy data from the boot table to the destination locations
 ************************************************************************************
 * DESCRIPTION: Copy data from the boot table to the destination. 
 *
 *              The input data is interpreted as 32 bit values, made up of two 16 
 *              bit values in big endian format. This is done to acheive endian
 *              independence on the c64x. 
 *
 *              If the size is not evenly divisible by 4 then the final write does a
 *              read first, and writes the most significant bytes back. 
 ************************************************************************************/
UINT16 coreCopyData (UINT32 dest_addr, UINT16 *p_data, UINT32 sizeBytes, UINT16 start_vector)
{
  UINT32  value;
  UINT32  insert;
  UINT32  i;
  UINT32  j;
  UINT32  n32;
  UINT32  rb;

  UINT32  *restrict rdest;
  UINT16  *restrict rsrc;

  chipDummy ((void *)start_vector);

  n32 = sizeBytes >> 2;


  /* If the destination address is 32 bit aligned an unaligned copy is not requird */
  if ((dest_addr & 0x3) == 0)  {

    rdest = (UINT32 *)dest_addr;
    rsrc  = p_data;

    for (i = j = 0; i < n32; i++, j += 2)  {

      value = (((UINT32)rsrc[j]) << 16) | (rsrc[j+1]);
      rdest[i] = value;

    }

    dest_addr += (n32 << 2);


  }  else  {

    /* Do whole 32 bit writes first */
    for (i = j = 0; i < n32; i++, j += 2)  {

      value = (((UINT32)p_data[j]) << 16) | (p_data[j+1]);
      chipStoreWord ((UINT32 *)dest_addr, value);
      dest_addr += 4;

    }

  }

  /* Handle any remaining values based on the endian of the device */
  rb = sizeBytes - (n32 << 2);

  if (rb != 0)  {

    value = chipReadWord ((UINT32 *)dest_addr);

    if ((CSR & C64X_REG_CSR_BIT_EN) != 0)  {

      /* Little endian. Put remaining bytes in the least significant value */
      if (rb >= 1)  {
        insert = (p_data[j] >> 8) & 0x00ff;
        value = (value & 0xffffff00) | insert;

      } 

      if (rb >= 2)  {
        insert = p_data[j] & 0x00ff;
        value = (value & 0xffff00ff) | (insert << 8);

      } 
      
      if (rb >= 3)  {
        insert = (p_data[j+1] >> 8) & 0x00ff;
        value = (value & 0xff00ffff) | (insert << 16);
      }

    } else  {

      /* Bit endian. Put remaining bytes in the most significant value */
      if (rb == 1)  {
        insert = (p_data[j] >> 8) & 0xff;
        value = (value & 0x00ffffff) | (insert << 24);
      
      } else if (rb == 2)  {
        insert = p_data[j] & 0xffff;
        value = (value & 0x0000ffff) | (insert << 16);

      } else if (rb == 3)  {
        insert = ((p_data[j] << 8) | (p_data[j+1] >> 8)) & 0x00ffffff;
        value = (value & 0x000000ff) | (insert << 8);
      }

    }

   chipStoreWord ((UINT32 *)dest_addr, value);

  }


  return (CORE_NOERR);

} /* coreCopyData */



