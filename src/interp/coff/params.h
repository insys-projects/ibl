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
/*  PARAMS.H  - Target dependent parameters.                                */
/****************************************************************************/

#if TMS340
typedef unsigned int  T_ADDR;          /* TYPE FOR TARGET ADDRESS          */
typedef unsigned short T_DATA;          /* TYPE FOR TARGET DATA WORD        */
typedef unsigned short T_SIZE;          /* TYPE FOR CINIT SIZE FIELD        */
typedef unsigned int  T_IADDR;         /* TYPE FOR CINIT ADDRESS FIELD     */
typedef unsigned short T_INSTRUCT;      /* TYPE FOR INSTRUCTION OPCODE      */

#define MAGIC         MAGIC_340
/* #define NEW_SYM_NDX   1 USE 32-bit SYMBOL INDEX, NO DISP */
#define BYTETOLOC(x)  ((T_ADDR)(x)<<3)  /* CONVERT ADDRESSES TO BYTES       */
#define LOCTOBYTE(x)  ((x)>>3)          /* CONVERT BYTES TO ADDRESSES       */
#define BIT_OFFSET(a) ((a)&7)           /* BIT OFFSET OF ADDR WITHIN BYTE   */

#ifdef OTIS
#define LOADBUFSIZE   (TRG_MAX_MBLK/8)  /* USE OTIS BUFFER SIZE             */
#else
#define LOADBUFSIZE   0x1000            /* 16K BUFFER FOR LOADING DATA      */
#endif

#define LOADWORDSIZE  2                 /* MINIMUM DIVISOR OF LOAD SIZE     */
#define CINIT         ".cinit"          /* NAME OF CINIT SECTION            */
#define INIT_ALIGN    1                 /* ALIGNMENT OF CINIT RECORDS       */
#define INIT_WSIZE    2                 /* SIZE IN BYTES OF INIT DATA ITEMS */
#endif

#if TMS32030 || TMS32040
typedef unsigned int  T_ADDR;          /* TYPE FOR TARGET ADDRESS          */
typedef unsigned int  T_DATA;          /* TYPE FOR TARGET DATA WORD        */
typedef unsigned int  T_SIZE;          /* TYPE FOR CINIT SIZE FIELD        */
typedef unsigned int  T_IADDR;         /* TYPE FOR CINIT ADDRESS FIELD     */
typedef unsigned int  T_INSTRUCT;      /* TYPE FOR INSTRUCTION OPCODE      */

#define MAGIC MAGIC_C30                 /* Magic number for C30             */

#define LOCTOBYTE(x)   ((x)<<2)         /* C30 word addrs to byte addrs     */
#define BYTETOLOC(x)   ((x)>>2)         /* byte addrs to word addrs         */
#define BIT_OFFSET(a)  (0)              /* BIT OFFSET OF ADDR WITHIN BYTE   */

#ifdef OTIS
#define LOADBUFSIZE   (TRG_MAX_MBLK/8)  /* USE OTIS BUFFER SIZE             */
#else
#define LOADBUFSIZE   0x0FF8            /* 4K-8 BUFFER FOR LOADING DATA     */
#endif

#define LOADWORDSIZE  2                 /* MINIMUM DIVISOR OF LOAD SIZE     */
#define CINIT         ".cinit"          /* NAME OF CINIT SECTION            */
#define INIT_ALIGN    1                 /* ALIGNMENT OF CINIT RECORDS       */
#define INIT_WSIZE    4                 /* SIZE IN BYTES OF INIT DATA ITEMS */
#endif

#if TMS32025 || TMS32050
typedef unsigned short T_ADDR;          /* TYPE FOR TARGET ADDRESS          */
typedef unsigned short T_DATA;          /* TYPE FOR TARGET DATA WORD        */
typedef unsigned short T_SIZE;          /* TYPE FOR CINIT SIZE FIELD        */
typedef unsigned short T_IADDR;         /* TYPE FOR CINIT ADDRESS FIELD     */
typedef unsigned short T_INSTRUCT;      /* TYPE FOR INSTRUCTION OPCODE      */

#define MAGIC MAGIC_DSP                 /* Magic number for C25            */

#define LOCTOBYTE(x)   ((x)<<1)         /* 16-bit word addrs to byte addrs */
#define BYTETOLOC(x)   ((x)>>1)         /* byte addrs to word addrs        */
#define BIT_OFFSET(a)  (0)              /* BIT OFFSET OF ADDR WITHIN BYTE   */

#ifdef OTIS
#define LOADBUFSIZE   (TRG_MAX_MBLK/8)  /* USE OTIS BUFFER SIZE             */
#else
#define LOADBUFSIZE   0x4000            /* 16K BUFFER FOR LOADING DATA      */
#endif /* ifdef OTIS */

#define LOADWORDSIZE  2                 /* MINIMUM DIVISOR OF LOAD SIZE     */
#define CINIT         ".cinit"          /* NAME OF CINIT SECTION            */
#define INIT_ALIGN    1                 /* ALIGNMENT OF CINIT RECORDS       */
#define INIT_WSIZE    2                 /* SIZE IN BYTES OF INIT DATA ITEMS */

#endif /* TMS32025 || TMS32050 */

#if TMS380
typedef unsigned short T_ADDR;          /* TYPE FOR TARGET ADDRESS          */
typedef unsigned short T_DATA;          /* TYPE FOR TARGET DATA WORD        */
typedef unsigned short T_SIZE;          /* TYPE FOR CINIT SIZE FIELD        */
typedef unsigned short T_IADDR;         /* TYPE FOR CINIT ADDRESS FIELD     */
typedef unsigned short T_INSTRUCT;      /* TYPE FOR INSTRUCTION OPCODE      */

#define MAGIC MAGIC_380                 /* Magic number for TMS380         */

#define LOCTOBYTE(x)   x                /* 16-bit byte addrs to byte addrs */
#define BYTETOLOC(x)   x                /* byte addrs to byte addrs        */
#define BIT_OFFSET(a)  (0)              /* BIT OFFSET OF ADDR WITHIN BYTE   */

#ifdef OTIS
#define LOADBUFSIZE   (TRG_MAX_MBLK/8)  /* USE OTIS BUFFER SIZE             */
#else
#define LOADBUFSIZE   0x4000            /* 16K BUFFER FOR LOADING DATA      */
#endif

#define LOADWORDSIZE  2                 /* MINIMUM DIVISOR OF LOAD SIZE     */
#define CINIT         ".cinit"          /* NAME OF CINIT SECTION            */
#define INIT_ALIGN    2                 /* ALIGNMENT OF CINIT RECORDS       */
#define INIT_WSIZE    1                 /* SIZE IN BYTES OF INIT DATA ITEMS */
#endif

#if TMS370 || TMS370C8
typedef unsigned short T_ADDR;          /* TYPE FOR TARGET ADDRESS          */
typedef unsigned char  T_DATA;          /* TYPE FOR TARGET DATA WORD        */
typedef unsigned short T_SIZE;          /* TYPE FOR CINIT SIZE FIELD        */
typedef unsigned short T_IADDR;         /* TYPE FOR CINIT ADDRESS FIELD     */
typedef unsigned char  T_INSTRUCT;      /* TYPE FOR INSTRUCTION OPCODE      */

#define MAGIC MAGIC_370                 /* 370 Magic Number                 */

#define LOCTOBYTE(x)   x                /* 370 addresses are same as bytes  */
#define BYTETOLOC(x)   x
#define BIT_OFFSET(a)  (0)              /* BIT OFFSET OF ADDR WITHIN BYTE   */

#ifdef OTIS
#define LOADBUFSIZE   (TRG_MAX_MBLK/8)  /* USE OTIS BUFFER SIZE             */
#else
#define LOADBUFSIZE   0x4000            /* 16K BUFFER FOR LOADING DATA      */
#endif

#define LOADWORDSIZE  2                 /* MINIMUM DIVISOR OF LOAD SIZE     */
#define CINIT         ".cinit"          /* NAME OF CINIT SECTION            */
#define INIT_ALIGN    1                 /* ALIGNMENT OF CINIT RECORDS       */
#define INIT_WSIZE    1                 /* SIZE IN BYTES OF INIT DATA ITEMS */
#endif

#if MVP_PP
typedef unsigned int  T_ADDR;          /* TYPE FOR TARGET ADDRESS          */
typedef unsigned int  T_DATA;          /* TYPE FOR TARGET DATA WORD        */
typedef unsigned int  T_SIZE;          /* TYPE FOR CINIT SIZE FIELD        */
typedef unsigned int  T_IADDR;         /* TYPE FOR CINIT ADDRESS FIELD     */

typedef struct {unsigned int ins_h; unsigned int ins_l; } T_INSTRUCT;      
					/* TYPE FOR INSTRUCTION OPCODE      */

#define MAGIC MAGIC_MVP                 /* PP  Magic Number                 */

#define LOCTOBYTE(x)   (x)              /* PP  addresses are same as bytes  */
#define BYTETOLOC(x)   (x)
#define BIT_OFFSET(a)  (0)              /* BIT OFFSET OF ADDR WITHIN BYTE   */

#ifdef OTIS
#define LOADBUFSIZE   (TRG_MAX_MBLK/8)  /* USE OTIS BUFFER SIZE             */
#else
#define LOADBUFSIZE   0x4000            /* 16K BUFFER FOR LOADING DATA      */
#endif

#define LOADWORDSIZE  8                 /* MINIMUM DIVISOR OF LOAD SIZE     */
#define CINIT         ".cinit"          /* NAME OF CINIT SECTION            */
#define INIT_ALIGN    8                 /* ALIGNMENT OF CINIT RECORDS       */
#define INIT_WSIZE    4                 /* SIZE IN BYTES OF INIT DATA ITEMS */
#endif

#if MVP_MP
typedef unsigned int  T_ADDR;          /* TYPE FOR TARGET ADDRESS          */
typedef unsigned int  T_DATA;          /* TYPE FOR TARGET DATA WORD        */
typedef unsigned int  T_SIZE;          /* TYPE FOR CINIT SIZE FIELD        */
typedef unsigned int  T_IADDR;         /* TYPE FOR CINIT ADDRESS FIELD     */
typedef unsigned int  T_INSTRUCT;      /* TYPE FOR INSTRUCTION OPCODE      */

#define FT_IEEE_FLT		1
#define FT_IEEE_DBL		2
#define FLT_PRECISION		4

typedef struct trg_fval {
	unsigned int	fval1;
	unsigned int	fval2;
} TRG_FVAL;

#define MAGIC MAGIC_MVP                 /* MP  Magic Number                 */

#define LOCTOBYTE(x)   (x)              /* MP  addresses are same as bytes  */
#define BYTETOLOC(x)   (x)
#define BIT_OFFSET(a)  (0)              /* BIT OFFSET OF ADDR WITHIN BYTE   */

#ifdef OTIS
#define LOADBUFSIZE   (TRG_MAX_MBLK/8)  /* USE OTIS BUFFER SIZE             */
#else
#define LOADBUFSIZE   0x4000            /* 16K BUFFER FOR LOADING DATA      */
#endif

#define LOADWORDSIZE  8                 /* MINIMUM DIVISOR OF LOAD SIZE     */
#define CINIT         ".cinit"          /* NAME OF CINIT SECTION            */
#define INIT_ALIGN    8                 /* ALIGNMENT OF CINIT RECORDS       */
#define INIT_WSIZE    4                 /* SIZE IN BYTES OF INIT DATA ITEMS */
#endif

#if LEAD
typedef unsigned short T_ADDR;          /* TYPE FOR TARGET ADDRESS          */
typedef unsigned short T_DATA;          /* TYPE FOR TARGET DATA WORD        */
typedef unsigned short T_SIZE;          /* TYPE FOR CINIT SIZE FIELD        */
typedef unsigned short T_IADDR;         /* TYPE FOR CINIT ADDRESS FIELD     */
typedef unsigned short T_INSTRUCT;      /* TYPE FOR INSTRUCTION OPCODE      */

#define MAGIC MAGIC_LEAD                /* Magic number for C25            */

#define LOCTOBYTE(x)   ((x)<<1)         /* 16-bit word addrs to byte addrs */
#define BYTETOLOC(x)   ((x)>>1)         /* byte addrs to word addrs        */
#define BIT_OFFSET(a)  (0)              /* BIT OFFSET OF ADDR WITHIN BYTE   */

#ifdef OTIS
#define LOADBUFSIZE   (TRG_MAX_MBLK/8)  /* USE OTIS BUFFER SIZE             */
#else
#define LOADBUFSIZE   0x4000            /* 16K BUFFER FOR LOADING DATA      */
#endif /* ifdef OTIS */

#define LOADWORDSIZE  2                 /* MINIMUM DIVISOR OF LOAD SIZE     */
#define CINIT         ".cinit"          /* NAME OF CINIT SECTION            */
#define INIT_ALIGN    1                 /* ALIGNMENT OF CINIT RECORDS       */
#define INIT_WSIZE    2                 /* SIZE IN BYTES OF INIT DATA ITEMS */

#endif /* LEAD */

#if LEAD3
typedef unsigned int  T_ADDR;          /* TYPE FOR TARGET ADDRESS          */
typedef unsigned short T_DATA;          /* TYPE FOR TARGET DATA WORD        */
typedef unsigned short T_SIZE;          /* TYPE FOR CINIT SIZE FIELD        */
typedef unsigned int  T_IADDR;         /* TYPE FOR CINIT ADDRESS FIELD     */
typedef unsigned int  T_INSTRUCT;      /* TYPE FOR INSTRUCTION OPCODE      */

#define MAGIC MAGIC_LEAD3               /* Magic number for C55             */

#define LOCTOBYTE(x)   (x)              /* LEAD3 is byte addressable        */
#define BYTETOLOC(x)   (x)              /* LEAD3 is byte addressable        */
#define BIT_OFFSET(a)  (0)              /* BIT OFFSET OF ADDR WITHIN BYTE   */

#ifdef OTIS
#define LOADBUFSIZE   (TRG_MAX_MBLK/8)  /* USE OTIS BUFFER SIZE             */
#else
#define LOADBUFSIZE   0x4000            /* 16K BUFFER FOR LOADING DATA      */
#endif /* ifdef OTIS */

#define LOADWORDSIZE  2                 /* MINIMUM DIVISOR OF LOAD SIZE     */
#define CINIT         ".cinit"          /* NAME OF CINIT SECTION            */
#define INIT_ALIGN    1                 /* ALIGNMENT OF CINIT RECORDS       */
#define INIT_WSIZE    2                 /* SIZE IN BYTES OF INIT DATA ITEMS */

#endif /* LEAD3 */

#if ARM
typedef unsigned int T_ADDR;           /* TYPE FOR TARGET ADDRESS          */
typedef unsigned int T_DATA;           /* TYPE FOR TARGET DATA WORD        */
typedef unsigned int T_SIZE;           /* TYPE FOR CINIT SIZE FIELD        */
typedef unsigned int T_IADDR;          /* TYPE FOR CINIT ADDRESS FIELD     */
typedef unsigned int T_INSTRUCT;       /* TYPE FOR INSTRUCTION OPCODE      */

#define MAGIC MAGIC_ARM                 /* Magic number for ARM             */

#define LOCTOBYTE(x)   (x)              /* ARM addresses are same as bytes  */
#define BYTETOLOC(x)   (x)
#define BIT_OFFSET(a)  (0)              /* BIT OFFSET OF ADDR WITHIN BYTE   */

#ifdef OTIS
#define LOADBUFSIZE   (TRG_MAX_MBLK/8)  /* USE OTIS BUFFER SIZE             */
#else
#define LOADBUFSIZE   0x4000            /* 16K BUFFER FOR LOADING DATA      */
#endif

#define LOADWORDSIZE  4                 /* MINIMUM DIVISOR OF LOAD SIZE     */
#define CINIT         ".cinit"          /* NAME OF CINIT SECTION            */
#define INIT_ALIGN    4                 /* ALIGNMENT OF CINIT RECORDS       */
#define INIT_WSIZE    4                 /* SIZE IN BYTES OF INIT DATA ITEMS */

#endif /* ARM */

#if TMS32060
typedef unsigned int  T_ADDR;          /* TYPE FOR TARGET ADDRESS          */
typedef unsigned int  T_DATA;          /* TYPE FOR TARGET DATA WORD        */
typedef unsigned int  T_SIZE;          /* TYPE FOR CINIT SIZE FIELD        */
typedef unsigned int  T_IADDR;         /* TYPE FOR CINIT ADDRESS FIELD     */
typedef unsigned int  T_INSTRUCT;      /* TYPE FOR INSTRUCTION OPCODE      */

#define FT_IEEE_FLT		1
#define FT_IEEE_DBL		2
#define FLT_PRECISION		4

typedef struct trg_fval {
	unsigned int	fval1;
	unsigned int	fval2;
} TRG_FVAL;

#define MAGIC MAGIC_C60                 /* C60 Magic Number                 */

#define LOCTOBYTE(x)   (x)              /* C60 addresses are same as bytes  */
#define BYTETOLOC(x)   (x)
#define BIT_OFFSET(a)  (0)              /* BIT OFFSET OF ADDR WITHIN BYTE   */

#ifdef OTIS
#define LOADBUFSIZE   (TRG_MAX_MBLK/8)  /* USE OTIS BUFFER SIZE             */
#else
#define LOADBUFSIZE   0x4000            /* 16K BUFFER FOR LOADING DATA      */
//#define UNBUFFERED		1
#endif

#define LOADWORDSIZE  8                 /* MINIMUM DIVISOR OF LOAD SIZE     */
#define CINIT         ".cinit"          /* NAME OF CINIT SECTION            */
#define INIT_ALIGN    8                 /* ALIGNMENT OF CINIT RECORDS       */
#define INIT_WSIZE    4                 /* SIZE IN BYTES OF INIT DATA ITEMS */
#endif

#if VIVID
typedef unsigned int  T_ADDR;          /* TYPE FOR TARGET ADDRESS          */
typedef unsigned int  T_DATA;          /* TYPE FOR TARGET DATA WORD        */
typedef unsigned int  T_SIZE;          /* TYPE FOR CINIT SIZE FIELD        */
typedef unsigned int  T_IADDR;         /* TYPE FOR CINIT ADDRESS FIELD     */
typedef unsigned int  T_INSTRUCT;      /* TYPE FOR INSTRUCTION OPCODE      */

#define FT_IEEE_FLT		1
#define FT_IEEE_DBL		2
#define FLT_PRECISION		4

typedef struct trg_fval {
	unsigned int	fval1;
	unsigned int	fval2;
} TRG_FVAL;

#define MAGIC MAGIC_VIVID               /* Vivid Magic Number               */

#define LOCTOBYTE(x)   (x)              /* Vivid addresses are same as bytes*/
#define BYTETOLOC(x)   (x)
#define BIT_OFFSET(a)  (0)              /* BIT OFFSET OF ADDR WITHIN BYTE   */

#ifdef OTIS
#define LOADBUFSIZE   (TRG_MAX_MBLK/8)  /* USE OTIS BUFFER SIZE             */
#else
#define LOADBUFSIZE   0x4000            /* 16K BUFFER FOR LOADING DATA      */
#endif

#define LOADWORDSIZE  8                 /* MINIMUM DIVISOR OF LOAD SIZE     */
#define CINIT         ".cinit"          /* NAME OF CINIT SECTION            */
#define INIT_ALIGN    8                 /* ALIGNMENT OF CINIT RECORDS       */
#define INIT_WSIZE    4                 /* SIZE IN BYTES OF INIT DATA ITEMS */
#endif

