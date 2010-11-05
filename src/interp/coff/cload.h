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



/***************************************************************************
* FILENAME: cload.h
* VERSION:  2.4  5/2/96  13:12:48
* SCCS ID:  "@(#)cload.h	2.4  5/2/96"
***************************************************************************/
/*****************************************************************************/
/* CLOAD.H - Header file for Generic COFF Loader     Version 4.00 9/92       */
/*****************************************************************************/
#ifndef _CLOAD_H_
#define _CLOAD_H_

/*---------------------------------------------------------------------------*/
/* THIS MACRO IS USED TO FACILIATE ACCESS TO THE SECTION HEADERS             */
/*---------------------------------------------------------------------------*/
#define SECT_HDR(i) ((SCNHDR *)(sect_hdrs + (i) * SCNHSZ))

/*---------------------------------------------------------------------------*/
/* THIS MACRO IS USED TO FACILITATE BACKWARDS COMPATIBILITY FOR COFF-        */
/* DEPENDENT TOOLS THAT SUPPORT COFF VERSION 2.                              */
/*---------------------------------------------------------------------------*/
#define O_SECT_HDR(i) ((O_SCNHDR *)(o_sect_hdrs + (i)*SCNHSZ_IN(coff_version)))

/*----------------------------------------------------------------------------*/
/* STATIC COPY OF 8 CHARACTER SECTION NAME, GUARANTEED NULL TERMINATION WHEN  */
/* USED AS A STRING.                                                          */
/*----------------------------------------------------------------------------*/
static char stat_nm[SYMNMLEN+1]={'\0','\0','\0','\0','\0','\0','\0','\0','\0'};
#define SNAMECPY(s)	(strn_copy(stat_nm, (s), SYMNMLEN))

/*---------------------------------------------------------------------------*/
/* THESE MACROS ARE USED TO FIND CINIT AND BSS SECTIONS                      */
/*---------------------------------------------------------------------------*/
#define IS_BSS(sptr)    (!str_comp(sptr->s_name, ".bss"))
#define IS_CINIT(sptr)  ((sptr->s_flags & STYP_COPY) &&         \
                        !str_comp(sptr->s_name, CINIT) )

/*---------------------------------------------------------------------------*/
/* VARIABLES SET BY THE APPLICATION.                                         */
/*---------------------------------------------------------------------------*/
/* #define FILE_BASED 0   - moved to osal.h */
#define STRIP_RELOC 1
#define REMOVE_MALLOC 0

#if (FILE_BASED)
extern FILE   *fin;						/* INPUT FILE                         */
#else
extern unsigned char gRxBuffer[0x400040];
#endif

extern int     need_data;            /* READ IN RAW DATA                     */
extern int     need_symbols;         /* READ IN SYMBOL TABLE                 */
extern int     clear_bss;            /* CLEAR BSS SECTION                    */
extern int     big_e_config;         /* ENDIANNESS CONFIGURATION OF TARGET   */
#if TMS320C60 || RTC
extern int    fill_bss_value;       /* NUMBER FOR FILL VALUE                */
#endif

/*---------------------------------------------------------------------------*/
/* VARIABLES SET BY THE LOADER.                                              */
/*---------------------------------------------------------------------------*/
extern FILHDR  file_hdr;             /* FILE HEADER STRUCTURE                */
extern int     coff_version;         /* VERSION OF COFF USED BY FILE         */
extern AOUTHDR o_filehdr;            /* OPTIONAL (A.OUT) FILE HEADER         */
extern T_ADDR  entry_point;          /* ENTRY POINT OF MODULE                */
extern T_ADDR *reloc_amount;         /* AMOUNT OF RELOCATION PER SECTION     */
extern char   *sect_hdrs;            /* ARRAY OF SECTION HEADERS             */
extern char   *o_sect_hdrs;          /* ARRAY OF OLD COFF SECTION HEADERS    */
extern int     n_sections;           /* NUMBER OF SECTIONS IN THE FILE       */
extern int     big_e_target;         /* OBJECT DATA IS STORED MSB FIRST      */
extern int     byte_swapped;         /* BYTE ORDERING OPPOSITE OF HOST       */
extern int     curr_sect;            /* INDEX OF SECTION CURRENTLY LOADING   */
extern int     load_err;             /* ERROR CODE RETURNED IF LOADER FAILS  */
extern struct strtab *str_head;      /* HEAD OF STRING BUFFER LIST           */

/*--------------------------------------------------------------------------*/
/*			  CLOAD.C PROTOTYPES				    */
/*--------------------------------------------------------------------------*/
extern int	cload (void);
extern int	cload_headers (void);
extern int	cload_data (void);
extern int	cload_sect_data (struct scnhdr *);
extern int	cload_cinit (unsigned char *, int *, int *);
extern int	cload_symbols (void);
extern int      cload_strings (void);
extern void     str_free (struct strtab *);
extern int	sym_read (int, struct syment *, union auxent *);
extern char    *sym_name (struct syment *);
extern char    *sym_add_name (struct syment *);
extern int	reloc_add (int, struct syment *);
extern int 	relocate (RELOC *, unsigned char *, int);
extern int	reloc_size (int);
extern int	reloc_offset (int);
extern int      reloc_stop (int);
extern int	sym_reloc_amount (RELOC *);
extern unsigned	int unpack (unsigned char *, int, int, int);
extern void	repack (unsigned int, unsigned char *, int,int,int);
extern int	cload_lineno (int, int, struct lineno *, int);
extern void	swap4byte (void *);
extern void	swap2byte (void *);

#if FILE_BASED
int reloc_read(RELOC *rptr);
#else
int reloc_read(RELOC *rptr, unsigned int offset);
#endif

/*--------------------------------------------------------------------------*/
/*             PROTOTYPES FOR FUNCTIONS REQUIRED BY CLOAD.C 		    */
/*--------------------------------------------------------------------------*/
extern void   	lookup_sym(int indx, SYMENT *sym, AUXENT *aux);
/* extern int    	mem_write(unsigned char *, unsigned int, T_ADDR, unsigned char); - moved to osal.h */
extern void     load_msg(const char *);
extern int 	set_reloc_amount(void);
extern int 	load_syms(int);

/*---------------------------------------------------------------------------*/
/* VALUE OF big_e_config IF THERE IS NO TARGET AND SO IT'S A DON'T CARE.     */
/*---------------------------------------------------------------------------*/
#define DONTCARE    -1

/*---------------------------------------------------------------------------*/
/* ERROR CODES                                                               */
/*---------------------------------------------------------------------------*/
#define E_FILE      1                /* ERROR READING COFF FILE              */
#define E_MAGIC     2                /* WRONG MAGIC NUMBER                   */
#define E_RELOC     3                /* FILE IS NOT RELOCATABLE              */
#define E_SYM       4                /* LOAD_SYM RETURNED FALSE              */
#define E_ALLOC     5                /* MYALLOC OR MRALLOC RETURNED FALSE    */
#define E_SETRELOC  6                /* SET_RELOC_AMOUNT RETURNED FALSE      */
#define E_MEMWRITE  7                /* MEM_WRITE RETURNED FALSE             */
#define E_RELOCENT  8                /* RELOC ENTRY RULES VIOLATED           */
#define E_ENDIAN    9                /* OBJ ENDIANESS CONFLICTS WITH TARGET  */

#endif /* _CLOAD_H_  */
