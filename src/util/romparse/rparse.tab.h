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



#ifndef YYSTYPE
#define YYSTYPE int
#endif
#define	LBRACE	258
#define	RBRACE	259
#define	SECTION	260
#define	EQU	261
#define	VALUE	262
#define	STRING	263
#define	BOOT_MODE	264
#define	PARAM_INDEX	265
#define	OPTIONS	266
#define	MULTI_I2C_ID	267
#define	MY_I2C_ID	268
#define	CORE_FREQ_MHZ	269
#define	I2C_CLK_FREQ_KHZ	270
#define	EXE_FILE	271
#define	PCI_PARMS	272
#define	NEXT_DEV_ADDR	273
#define	NEXT_DEV_ADDR_EXT	274
#define	ADDRESS_DELAY	275
#define	SWPLL	276
#define	DEV_ADDR_EXT	277
#define	DEV_ADDR	278
#define	LAYOUT	279
#define	ALIGN	280
#define	PAD	281
#define	LENGTH	282
#define	PAD_FILE_ID	283


extern YYSTYPE yylval;
