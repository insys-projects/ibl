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



/****************************************************************************/
/*  COFF_TRG.H                                                              */
/*      This file contains target dependent parameters for COFF files.      */
/****************************************************************************/
#ifndef COFF_TRG_H
#define COFF_TRG_H

#if defined(TOOL_ASSEMBLER)
   #error The COFF submodule should no longer be used by the assembler; use TICOFF04
#endif

/*--------------------------------------------------------------------------*/
/* TMS340 (GSP) Target Specific Parameters (bit-addressable)                */
/*--------------------------------------------------------------------------*/
#if TMS340
#define MAGIC         MAGIC_340          /* Magic number for GSP            */
#define TRG_MEM_WIDTH 1                  /* Bit address                     */
#endif

/*--------------------------------------------------------------------------*/
/* TMS370/C8 Target Specific Parameters (byte-addressable)                  */
/*--------------------------------------------------------------------------*/
#if TMS370
#define MAGIC         MAGIC_370          /* 370 Magic number                */
#define TRG_MEM_WIDTH 8                  /* Byte address                    */
#endif

#if TMS370C8
#define MAGIC         MAGIC_370          /* C8  Magic number                */
#define TRG_MEM_WIDTH 8                  /* Byte address                    */
#endif

#if TMS370C8P
#define MAGIC         MAGIC_C8P          /* C8  Magic number                */
#define TRG_MEM_WIDTH 8                  /* Byte address                    */
#endif

/*--------------------------------------------------------------------------*/
/* TMS37016 (C16) Target Specific Parameters (byte-addressable)             */
/*--------------------------------------------------------------------------*/
#if TMS37016
#define MAGIC         MAGIC_C16          /* 370/16 Magic number             */
#define TRG_MEM_WIDTH 8                  /* Byte address                    */
#endif

/*--------------------------------------------------------------------------*/
/* TMS32030 (C30) Target Specific Parameters (32-bit word-addressable)      */
/*--------------------------------------------------------------------------*/
#if TMS32030
#define MAGIC         MAGIC_C30          /* Magic number for C30            */
#define TRG_MEM_WIDTH 32                 /* 32-bit address                  */
#endif

/*--------------------------------------------------------------------------*/
/* TMS32025 (DSP) Target Specific Parameters (16-bit word-addressable)      */
/*--------------------------------------------------------------------------*/
#if TMS32025
#define MAGIC         MAGIC_DSP          /* Magic number for C1x/2x/5x      */
#define TRG_MEM_WIDTH 16                 /* 16-bit address                  */
#endif

/*--------------------------------------------------------------------------*/
/* TMS380 (EGL) Target Specific Parameters (byte-addressable)               */
/*--------------------------------------------------------------------------*/
#if TMS380
#define MAGIC         MAGIC_380          /* Magic number for TMS380         */
#define TRG_MEM_WIDTH 8                  /* Byte address                    */
#endif

/*--------------------------------------------------------------------------*/
/* TMS320C8x (MVP MP/PP) Target Specific Parameters (byte-addressable)      */
/*--------------------------------------------------------------------------*/
#if MVP_PP || MVP_MP
#define MAGIC         MAGIC_MVP          /* Magic number for PP             */
#define TRG_MEM_WIDTH 8                  /* Byte address                    */
#endif

/*--------------------------------------------------------------------------*/
/* TMS320C54x (LEAD) Target Specific Parameters (16-bit word-addressable)   */
/*--------------------------------------------------------------------------*/
#if LEAD
#define MAGIC          MAGIC_LEAD        /* Magic number for C5xx           */
#define TRG_MEM_WIDTH  16                /* 16-bit address                  */
#define TRG_INST_ALIGN 16                /* Instruction alignment           */
#endif

/*--------------------------------------------------------------------------*/
/* TMS470 (ARM) Target Specific Parameters (byte-addressable)               */
/*--------------------------------------------------------------------------*/
#if ARM
#define MAGIC                  MAGIC_ARM /* Magic number for ARM            */
#define TRG_MEM_WIDTH           8        /* Byte address                    */
#define TRG_ARM_MODE            0        /* ARM setting for curr_objmode    */
#define TRG_THUMB_MODE          1        /* Thumb setting for curr_objmode  */
#ifndef TRG_INST_ALIGN_THUMB
#define TRG_INST_ALIGN_THUMB   16
#endif

#ifndef TRG_INST_ALIGN_ARM
#define TRG_INST_ALIGN_ARM     32
#endif

#ifndef TRG_INST_ALIGN
#define TRG_INST_ALIGN         (curr_objmode == TRG_ARM_MODE ?     \
                                                TRG_INST_ALIGN_ARM \
                                                : TRG_INST_ALIGN_THUMB)
#endif
extern unsigned char curr_objmode;

/*------------------------------------------------------------------------*/
/*  Target device identification flags (bits 4-7 in file header flags)    */
/*------------------------------------------------------------------------*/
#define F_ARM9ABI		0x10	  /* 0x10 = ARM9ABI, 0x00 = TIABI   */
#define F_ARCH4			0x20	  /* ARCH4 = ARM7 		    */
#define F_ARCH5E		0x40	  /* ARCH5E = ARM9		    */
#define F_ARCH6 		0x60	  /* ARCH6  = ARM11		    */
#define F_DMODE			0x80	  /* DUAL MODE 			    */
#define ARCH_MASK		0x60

#define ARCH5E_FLAGS(flg)	((flg & ARCH_MASK) == F_ARCH5E)
#define ARCH6_FLAGS(flg)	((flg & ARCH_MASK) == F_ARCH6)
#define ARCH4_FLAGS(flg)	((flg & ARCH_MASK) == F_ARCH4) 

#undef  F_VERSION
#define F_VERSION		( F_ARM9ABI  | F_ARCH4 | \
				  F_ARCH5E | F_DMODE )
#endif


/*--------------------------------------------------------------------------*/
/* TMS320C6x Target Specific Parameters (byte-addressable)                  */
/*--------------------------------------------------------------------------*/
#if TMS32060
#define MAGIC          MAGIC_C60         /* Magic number for C60            */
#define TRG_MEM_WIDTH   8                /* Byte address                    */
#define TRG_INST_ALIGN 32                /* Instruction alignment           */
#endif

/*--------------------------------------------------------------------------*/
/* TARANTULA Target Specific Parameters (byte-addressable)                  */
/*--------------------------------------------------------------------------*/
#if TARANTULA
#define MAGIC         MAGIC_TARANTULA    /* Magic number for TARANTULA      */
#define TRG_MEM_WIDTH 8                  /* Byte address                    */
#endif

/*--------------------------------------------------------------------------*/
/* TMS320C2xxx (Ankoor) Target Specific Parameters (16-bit word-addressable)*/
/*--------------------------------------------------------------------------*/
#if RTC
#define MAGIC          MAGIC_ANKOOR      /* Magic number for Ankoor         */
#define TRG_MEM_WIDTH  16                /* 16-bit address                  */
#define TRG_INST_ALIGN 16                /* Instruction alignment           */
#endif

/*--------------------------------------------------------------------------*/
/* TMS320C55xx (LEAD3) Target Specific Parameters (byte-addressable code)   */
/*                                         (16-bit word-addressable data)   */
/*--------------------------------------------------------------------------*/
#if LEAD3
/*--------------------------------------------------------------------------*/
/* The C55x object consumers can accept either Laijin or Ryujin object      */
/* files, so ISMAGIC() becomes a target-specific macro, and MAGIC is not    */
/* defined.                                                                 */
/*--------------------------------------------------------------------------*/
#define ISMAGIC(x)	(((unsigned short)(x)) == MAGIC_LEAD3 || \
                         ((unsigned short)(x)) == MAGIC_LEAD3_R35)

#define TRG_MEM_WIDTH  8                 /* Byte address                    */
#define TRG_INST_ALIGN 8                 /* Instruction alignment           */
#endif

/*--------------------------------------------------------------------------*/
/* MSP430 Target Specific Parameters (byte-addressable)                     */
/*--------------------------------------------------------------------------*/
#if MSP
#define MAGIC                  MAGIC_MSP /* Magic number for MSP            */
#define TRG_MEM_WIDTH           8        /* Byte address                    */
#define TRG_INST_ALIGN         16
#endif

/*--------------------------------------------------------------------------*/
/* IF WE DIDN'T DEFINE ANY TARGET, DEFINE THEM TO SOMETHING...              */
/*--------------------------------------------------------------------------*/
#ifndef TRG_MEM_WIDTH
#define TRG_MEM_WIDTH 8                  /* ARBITRARY...                    */
#endif

#ifndef TRG_INST_ALIGN
#define TRG_INST_ALIGN 32                /* Instruction alignment           */
#endif

#ifndef MAGIC        
#define MAGIC         0                  /* ARBITRARY...                    */
#endif

#ifndef ISMAGIC
#define ISMAGIC(x)	(((unsigned short)(x)) == MAGIC)
#endif

#endif /* COFF_TRG_H */
