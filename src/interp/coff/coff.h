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



/**************************************************************************/
/*  COFF.H - Definition of COFF structures and definitions.               */
/*                                                                        */
/*  This file defines all standard COFF definitions, used by any program  */
/*  which reads or writes COFF files.                                     */
/*                                                                        */
/*  HISTORY                                                               */
/*       --/--/83     - Original (lost in the mists of time)              */
/*       --/--/91     - Tektronics relocation entry kludge                */
/*       01/07/94 RET - Reworked file header and relocation entries, add  */
/*                      COFF version support.   (Removed Tek kludge)      */
/*       12/24/95 TMS - Reworked section header structures for COFF 2     */
/*                                                                        */
/**************************************************************************/
#ifndef COFF_H
#define COFF_H

#if defined(TOOL_ASSEMBLER)
   #error The COFF submodule should no longer be used by the assembler; use TICOFF04
#endif

/*------------------------------------------------------------------------*/
/*  COFF VERSION FLAGS                                                    */
/*------------------------------------------------------------------------*/
#if !defined(COFF_VERSION_0) && \
    !defined(COFF_VERSION_1) && \
    !defined(COFF_VERSION_2)
#define COFF_VERSION_0     0
#define COFF_VERSION_1     0
#define COFF_VERSION_2     1
#endif

#ifndef COFF_VERSION_0 
#define COFF_VERSION_0     0
#endif

#ifndef COFF_VERSION_1
#define COFF_VERSION_1     0
#endif

#ifndef COFF_VERSION_2
#define COFF_VERSION_2     0
#endif

/*------------------------------------------------------------------------*/
/*  COFF MAGIC NUMBERS                                                    */
/*------------------------------------------------------------------------*/
#define COFF_MAGIC_0    0xc0      /* ORIGINAL VERSION OF COFF             */
#define COFF_MAGIC_1    0xc1
#define COFF_MAGIC_2    0xc2

/*------------------------------------------------------------------------*/
/*  COFF TARGET ID's (FORMERLY MAGIC NUMBERS)                             */
/*  NOTE!!! DEFINE THE MACRO "MAGIC" TO BE ONE OF THESE MACROS.           */
/*------------------------------------------------------------------------*/
#define MAGIC_340	0x90
#define MAGIC_370	0x91
#define MAGIC_DSP	0x92
#define MAGIC_C30	0x93
#define MAGIC_380	0x94
#define MAGIC_MVP	0x95
#define MAGIC_C16	0x96
#define MAGIC_ARM	0x97
#define MAGIC_LEAD	0x98
#define MAGIC_C60	0x99
#define MAGIC_C8P	0x9a
#define MAGIC_unused1   0x9b
#define MAGIC_LEAD3	0x9c
#define MAGIC_ANKOOR	0x9d
#define MAGIC_unused	0x9e
#define MAGIC_TARANTULA	0x9f
#define MAGIC_MSP       0xA0
#define MAGIC_LEAD3_R35	0xA1

#define MAGIC_MIN       0x90      /* MINIMUM VALID MAGIC NUMBER           */
#define MAGIC_MAX       0xA1      /* MAXIMUM VALID MAGIC NUMBER           */

/*------------------------------------------------------------------------*/
/*  Macros to recognize magic numbers                                     */
/*  NOTE: ISMAGIC() is now a target-specific macro defined in coff_trg.h  */
/*------------------------------------------------------------------------*/
#define ISCOFF(x)       (ISCOFF_0(x)||ISCOFF_1(x)||ISCOFF_2(x)||ISMAGIC(x))
#define BADMAGIC(x)     (((unsigned short)(x) & 0x8080) && !ISMAGIC(x))

#define ISEXE(x)        ((x >> 1) & 0x0001)

#if COFF_VERSION_2
#define ISCOFF_0(x)     ((unsigned short)(x) == COFF_MAGIC_0)
#define ISCOFF_1(x)     ((unsigned short)(x) == COFF_MAGIC_1)
#define ISCOFF_2(x)	((unsigned short)(x) == COFF_MAGIC_2)
#elif COFF_VERSION_1
#define ISCOFF_0(x)     ((unsigned short)(x) == COFF_MAGIC_0)
#define ISCOFF_1(x)     ((unsigned short)(x) == COFF_MAGIC_1)
#define ISCOFF_2(x)	FALSE
#else
#define ISCOFF_0(x)     FALSE
#define ISCOFF_1(x)     FALSE
#define ISCOFF_2(x)	FALSE
#endif

#define ISMAGIC_ANY(x)  (((unsigned short)(x)) >= MAGIC_MIN &&   \
                         ((unsigned short)(x)) <= MAGIC_MAX)
#define ISCOFF_ANY(x)   (ISCOFF_0(x) || ISCOFF_1(x) || \
			 ISCOFF_2(x) || ISMAGIC_ANY(x))

#include "coffdefs.h"

/*------------------------------------------------------------------------*/
/*  COFF FILE HEADER                                                      */
/*------------------------------------------------------------------------*/
struct filehdr
{
   unsigned short  f_magic;        /* magic number */
   unsigned short  f_nscns;        /* number of sections */
   unsigned int    f_timdat;       /* time & date stamp */
   unsigned int    f_symptr;       /* file pointer to symtab */
   unsigned int    f_nsyms;        /* number of symtab entries */
   unsigned short  f_opthdr;       /* sizeof(optional hdr) */
   unsigned short  f_flags;        /* flags */
   unsigned short  f_target_id;    /* target architecture id */
};

#define FILHDR  struct filehdr
#define FILHSZ             (COFF_VERSION_0 ? 20 : 22)
#define FILHSZ_IN(version) (version >= COFF_MAGIC_1 ? 22 : 20)

/*------------------------------------------------------------------------*/
/*  File header flags                                                     */
/*------------------------------------------------------------------------*/
#define  F_RELFLG   0x01       /* relocation info stripped from file      */
#define  F_EXEC     0x02       /* file is executable (no unresolved refs) */
#define  F_LNNO     0x04       /* line nunbers stripped from file         */
#define  F_LSYMS    0x08       /* local symbols stripped from file        */

 /*------------------------------------------------------------------------*/
/*  Target device identification flags (bits 4-7 in file header flags)    */
/*------------------------------------------------------------------------*/
#define  F_VERS0    0x0        /* 0th generation CPU                      */
#define  F_VERS1    0x10       /* 1st generation CPU                      */
#define  F_VERS2    0x20       /* 2nd generation CPU                      */
#define  F_VERS3    0x40       /* 3rd generation CPU                      */
#define  F_VERS4    0x80       /* 4th generation CPU                      */
#define  F_VERSION  (F_VERS1 | F_VERS2 | F_VERS3 | F_VERS4)

/*------------------------------------------------------------------------*/
/*  Target device raw data byte ordering flags (bits 8-9)                 */
/*------------------------------------------------------------------------*/
#define  F_LITTLE   0x100      /* object code is LSB first                */
#define  F_BIG      0x200      /* object code is MSB first                */
#define  F_BYTE_ORDER (F_LITTLE | F_BIG)

#define  F_SYMMERGE 0x1000     /* Tags, etc were merged - no duplicates   */

#define  F_ICODE    0x2000     /* This file contains embedded I-Code      */


/*------------------------------------------------------------------------*/
/*  OPTIONAL FILE HEADER                                                  */
/*------------------------------------------------------------------------*/
typedef struct aouthdr
{
   short   magic;          /* optional file header magic number    */
   short   vstamp;         /* version stamp                        */
   int     tsize;          /* text size in bytes, padded to FW bdry*/
   int     dsize;          /* initialized data "  "                */
   int     bsize;          /* uninitialized data "   "             */
   int     entrypt;        /* entry pt.                            */
   int     text_start;     /* base of text used for this file      */
   int     data_start;     /* base of data used for this file      */
} AOUTHDR;

#define AOUTSZ     sizeof(AOUTHDR)
#define AOUT1MAGIC 0x108


/*------------------------------------------------------------------------*/
/*  COMMON ARCHIVE FILE STRUCTURES                                        */
/*                                                                        */
/*       ARCHIVE File Organization:                                       */
/*       +---------------------------------------------+                  */
/*       |          ARCHIVE_MAGIC_STRING               |                  */
/*       +---------------------------------------------+                  */
/*       |          ARCHIVE_FILE_MEMBER_1              |                  */
/*       +- - - - - - - - - - - - - - - - - - - - - - -+                  */
/*       |   Archive File Header "ar_hdr"              |                  */
/*       |   Contents (Ext symbol direct, text file)   |                  */
/*       +---------------------------------------------+                  */
/*       |          ARCHIVE_FILE_MEMBER_2              |                  */
/*       +- - - - - - - - - - - - - - - - - - - - - - -+                  */
/*       |   Archive File Header "ar_hdr"              |                  */
/*       |   Contents (long file member name table)    |                  */
/*       +---------------------------------------------+                  */
/*       |          ARCHIVE_FILE_MEMBER_3              |                  */
/*       +- - - - - - - - - - - - - - - - - - - - - - -+                  */
/*       |   Archive File Header "ar_hdr"              |                  */
/*       |   Contents (.o or text file)                |                  */
/*       +---------------------------------------------+                  */
/*       |       .               .               .     |                  */
/*       |       .               .               .     |                  */
/*       |       .               .               .     |                  */
/*       +---------------------------------------------+                  */
/*       |          ARCHIVE_FILE_MEMBER_n              |                  */
/*       +- - - - - - - - - - - - - - - - - - - - - - -+                  */
/*       |   Archive File Header "ar_hdr"              |                  */
/*       |   Contents (.o or text file)                |                  */
/*       +---------------------------------------------+                  */
/*                                                                        */
/*------------------------------------------------------------------------*/

#define ARMAG   "!<arch>\n"
#define SARMAG  8
#define ARFMAG  "`\n"
#define ARFMAG_SIZE   2

struct ar_hdr           /* archive file member header - printable ascii */
{
   char    ar_name[16];    /* file member name - `/' terminated         */
   char    ar_date[12];    /* file member date - decimal                */
   char    ar_offset[6];   /* file member offset - decimal              */
   char    ar_gid[6];      /* file member group id - decimal            */
   char    ar_mode[8];     /* file member mode - octal                  */
   char    ar_size[10];    /* file member size - decimal                */
   char    ar_fmag[2];     /* ARFMAG - string to end header             */
};

#define ARHDR     struct ar_hdr
#define ARHSZ     sizeof(ARHDR)
#define AR_HDR_SZ sizeof(ARHDR)
#define AR_FNAME_SIZE 16
#define MAGIC_LONGFILENAME 1
#define LONGFILENAME       "<filenames>"
#define SYMDIRNAME         "/               " 


/*------------------------------------------------------------------------*/
/*  SECTION HEADER                                                        */
/*------------------------------------------------------------------------*/
#define  SYMNMLEN   8      /*  Number of characters in a symbol name      */

/*------------------------------------------------------------------------*/
/* THE OLD COFF VERSION TYPE DEFINITION FOR SECTION HEADERS TO PROVIDE    */
/* BACKWARDS COMPATIBILITY FOR COFF-DEPENDENT TOOLS THAT SUPPORT COFF 2.  */
/*------------------------------------------------------------------------*/
struct o_scnhdr 
{
	char            os_name[8];      /* section name                  */
	int            os_paddr;        /* physical address              */
	int            os_vaddr;        /* virtual address               */
	int            os_size;         /* section size                  */
	int            os_scnptr;       /* file ptr to raw data for scn  */
	int            os_relptr;       /* file ptr to relocation        */
	int            os_lnnoptr;      /* file ptr to line numbers      */
	unsigned short  os_nreloc;       /* number of relocation entries  */
	unsigned short  os_nlnno;        /* number of line number entries */
	unsigned short  os_flags;        /* flags                         */
	char            os_reserved;     /* reserved byte                 */
	unsigned char   os_page;         /* memory page id                */
};

/*------------------------------------------------------------------------*/
/* THE NEW COFF VERSION TYPE DEFINITION FOR SECTION HEADERS.  THIS        */
/* REVISION ALLOWS FOR UNRESTRICTED SECTION NAME LENGTH.                  */
/*------------------------------------------------------------------------*/
struct scnhdr
{
   union
   {
      char            _s_name[SYMNMLEN];   /* old COFF version name fld   */
      struct
      {
         int    _s_zeroes;                /* new == 0                    */
         int    _s_offset;                /* offset into string table    */
      } _s_s;
      char      *_s_nptr[2];               /* allows for overlaying       */
   } _s;

   int            s_paddr;        /* physical address                    */
   int            s_vaddr;        /* virtual address                     */
   int            s_size;         /* section size                        */
   int            s_scnptr;       /* file ptr to raw data for section    */
   int            s_relptr;       /* file ptr to relocation              */
   int            s_lnnoptr;      /* file ptr to line numbers            */
   unsigned int   s_nreloc;       /* number of relocation entries        */
   unsigned int   s_nlnno;        /* number of line number entries       */
   unsigned int   s_flags;        /* flags                               */
   unsigned char   s_mwidth;       /* memory width in bits                */
                                   /* (0 == target memory width)          */
   unsigned char   s_reserved;     /* reserved byte                       */
   unsigned short  s_page;         /* memory page id                      */
};

#define s_name          _s._s_name
#define s_nptr          _s._s_nptr[1]
#define s_zeroes        _s._s_s._s_zeroes
#define s_offset        _s._s_s._s_offset

#define O_SCNHDR	struct o_scnhdr
#define SCNHDR		struct scnhdr
#define O_SCNHSZ	sizeof(O_SCNHDR)
#define SCNHSZ		sizeof(SCNHDR)
#define SCNHSZ_IN(version) (version == COFF_MAGIC_2 ? SCNHSZ : O_SCNHSZ)

/*------------------------------------------------------------------------*/
/* Define constants for names of "special" sections                       */
/*------------------------------------------------------------------------*/
#define _TEXT    ".text"
#define _DATA    ".data"
#define _BSS     ".bss"
#define _REG	 ".reg"
#define _BINIT   ".binit"
#define _CINIT   ".cinit"
#define _PINIT   ".pinit"
#define _ICODE   ".icode"

#define _OVLY_PREFIX	".ovly:"

#define IS_DWARF_SECTION(name) (!strncmp(name, ".debug_", 7))

/*------------------------------------------------------------------------*/
/* Bits 0-8 of s_flags are used as a section "type"                       */
/*------------------------------------------------------------------------*/
#define STYP_REG    0x00  /* "regular" : allocated, relocated, loaded     */
#define STYP_DSECT  0x01  /* "dummy"   : !allocated, relocated, !loaded   */
#define STYP_NOLOAD 0x02  /* "noload"  : allocated, relocated, !loaded    */
#define STYP_XXX1   0x04  /* not used - was STYP_GROUP                    */
#define STYP_XXX2   0x08  /* not used - was STYP_PAD                      */
#define STYP_COPY   0x10  /* "copy"    : used for C init tables - 
                                           not allocated, relocated,
                                           loaded;  reloc & lineno
                                           entries processed normally     */
#define STYP_TEXT   0x20  /* section contains text only                   */
#define STYP_DATA   0x40  /* section contains data only                   */
#define STYP_BSS    0x80  /* section contains bss only                    */

/*------------------------------------------------------------------------*/
/* Bits 8-11 specify an alignment.  The alignment is (2 ** x), where x is */
/* is the encoded value                                                   */
/*------------------------------------------------------------------------*/
#define ALIGN_MASK         0xF00  /* mask for alignment factor            */
#define ALIGN_SIZE(s_flag) (1 << (((unsigned)s_flag & ALIGN_MASK) >> 8))
#define ALIGN_GET(header)     (((header).s_flags & ALIGN_MASK) >> 8)
#define ALIGN_SET(header,amt) (header).s_flags = \
               (((header).s_flags & ~ALIGN_MASK) | ((amt << 8) & ALIGN_MASK))

/*------------------------------------------------------------------------*/
/* bit 12-31                                                              */
/*------------------------------------------------------------------------*/
#define STYP_BLOCK      0x01000   /* use alignment as blocking factor     */
#define STYP_PASS       0x02000   /* Pass section through unchanged       */
#define STYP_CLINK      0x04000   /* Conditionally link section           */

#define STYP_VECTOR     0x08000   /* section contains vector table        */
                                  /* (Used in addition to STYP_TEXT)      */

#define STYP_PADDED     0x10000   /* section HAS BEEN padded.  Do not pad */
                                  /* again if partial linking.            */
#define STYP_ICODE      0x20000   /* section has ICODE associated with it */

#define STYP_LGT	0x40000   /* linker generated table section       */
#define STYP_COMMON     0x80000   /* comdat section - remove duplicates   */

#define STYP_BASIC_MASK 0xFF      /* basic section types                  */
#define STYP_TIEXT_MASK 0xFF000   /* TI section type extensions           */

/*------------------------------------------------------------------------*/
/* A mask for all the flags we support - anything else is garbage.        */
/*------------------------------------------------------------------------*/
#define STYP_KNOWN_MASK (ALIGN_MASK | STYP_BASIC_MASK | STYP_TIEXT_MASK)


/*------------------------------------------------------------------------*/
/*  RELOCATION ENTRIES                                                    */
/*  WE SUPPORT TWO TYPES OF RELOCATION ENTRIES:                           */
/*     1) OLD STYLE, WITH 16 BIT SYMBOL TABLE INDEX.                      */
/*     2) NEW STYLE, WITH 32 BIT SYMBOL TABLE INDEX.                      */
/*  FOR ANY GIVEN INPUT FILE, THE FILE HEADER FLAG "F_RELOC_12" INDICATES */
/*  THE TYPE OF RELOCATION ENTRY IN THE FILE.                             */
/*  THE TARGET-SPECIFIC FLAG RELOC_ENTRY_12 DETERMINES WHETHER THE NEW    */
/*  STYLE IS SUPPORTED ON A GIVEN TARGET.                                 */
/*------------------------------------------------------------------------*/
typedef struct reloc_old
{
   int            r_vaddr;       /* (virtual) address of reference       */

   union {
	struct {
		unsigned char	 _offset;   /* bit offset of rel fld      */
		unsigned char	 _fieldsz;  /* size of rel fld            */
		unsigned char	 _wordsz;   /* # bytes containing rel fld */
		unsigned char	 _dum1;

		unsigned short	 _type;
	} _r_field;

	struct {
		unsigned int	 _spc;      /* section relative PC        */
   		unsigned short   _type;     /* relocation type            */
	} _r_spc;

	struct {
		unsigned int	 _uval;      /* constant value           */
   		unsigned short   _type;      /* relocation type          */
	} _r_uval;

	struct {
		short		 _symndx;   /* 32-bit sym tbl index       */
   		unsigned short	 _disp;     /* extra addr encode data     */
   		unsigned short   _type;     /* relocation type            */
	} _r_sym;
   } _u_reloc;
} RELOC_OLD;

/*------------------------------------------------------------------------*/
/* MAKE SURE THE RELOCATION STRUCTURE IS TIGHTLY PACKED TO AVOID HOST     */
/* ALIGNMENT MECHANISMS.                                                  */
/*------------------------------------------------------------------------*/
typedef struct reloc
{
   int            r_vaddr;       /* (virtual) address of reference       */

   union {
	struct {
		unsigned char	 _offset;   /* bit offset of rel fld      */
		unsigned char	 _fieldsz;  /* size of rel fld            */
		unsigned char	 _wordsz;   /* # bytes containing rel fld */
		unsigned char	 _dum1;
		unsigned short	 _dum2;

		unsigned short	 _type;
	} _r_field;

	struct {
		unsigned int	 _spc;      /* section relative PC        */
		unsigned short	 _dum;
   		unsigned short   _type;     /* relocation type            */
	} _r_spc;

	struct {
		unsigned int	 _uval;      /* constant value           */
		unsigned short	 _dum;
   		unsigned short   _type;      /* relocation type          */
	} _r_uval;

	struct {
		int		 _symndx;   /* 32-bit sym tbl index       */
   		unsigned short	 _disp;     /* extra addr encode data     */
   		unsigned short   _type;     /* relocation type            */
	} _r_sym;
   } _u_reloc;
} RELOC;

#define RELSZ              (COFF_VERSION_0 ? 10 : 12)
#define RELSZ_IN(version)  ((version >= COFF_MAGIC_1) ? 12 : 10)

/*------------------------------------------------------------------------*/
/* Macros for accessing fields in relocation entry data structure.        */
/*------------------------------------------------------------------------*/
#define r_offset	_u_reloc._r_field._offset
#define r_fieldsz	_u_reloc._r_field._fieldsz
#define r_wordsz	_u_reloc._r_field._wordsz

#define r_spc		_u_reloc._r_spc._spc
#define r_uval		_u_reloc._r_uval._uval

#define r_symndx	_u_reloc._r_sym._symndx
#define r_disp		_u_reloc._r_sym._disp
#define r_type		_u_reloc._r_sym._type

/*------------------------------------------------------------------------*/
/*   define all relocation types                                          */
/*------------------------------------------------------------------------*/
#define R_ABS          0x0        /* absolute address - no relocation     */
#define R_DIR16        0x1        /* UNUSED                               */
#define R_REL16        0x2        /* UNUSED                               */
#define R_DIR24        0x4        /* UNUSED                               */
#define R_REL24        0x5        /* 24 bits, direct                      */
#define R_DIR32        0x6        /* UNUSED                               */
#define R_RRRELREG     0xe        /* RR: 8 bit relocatable register       */

#define R_RELBYTE      0xf        /* 8 bits, direct                       */
#define R_RELWORD      0x10       /* 16 bits, direct                      */
#define R_RELLONG      0x11       /* 32 bits, direct                      */
#define R_PCRBYTE      0x12       /* 8 bits, PC-relative                  */
#define R_PCRWORD      0x13       /* 16 bits, PC-relative                 */
#define R_PCRLONG      0x14       /* 32 bits, PC-relative                 */
                                  /* (See unsigned (LD3) versions below.) */
#define R_PCR24        0x15       /* 24 bits, PC-relative                 */
#define R_PCR23H       0x16       /* 23 bits, PC-rel in halfwords(x>>1)   */
#define R_PCR24W       0x17       /* 24 bits, PC-rel in words (x >> 2)    */
#define R_OCRLONG      0x18       /* GSP: 32 bits, one's compl direct     */
#define R_GSPPCR16     0x19       /* GSP: 16 bits, PC relative (in words) */
#define R_GSPOPR32     0x1a       /* GSP: 32 bits, direct big-endian      */
#define R_GSPPCA16     0x1b       /* GSP: same as GSPPCR16, but PC const  */
#define R_OCBD32       0x1c       /* GSP: 32 bits, 1's compl,big-endian   */
#define R_RRNREG       0x1d       /* RR: 8 bit reloc. reg. w/ neg off     */
#define R_PARTLS16     0x20       /* Brahma: 16 bit offset of 24 bit addr */
#define R_PARTMS8      0x21       /* Brahma: 8 bit page of 24 bit addr    */
#define R_PARTLS7      0x28       /* DSP: 7 bit offset of 16 bit addr     */
#define R_PARTMS9      0x29       /* DSP: 9 bit page of 16 bit addr       */
#define R_REL13        0x2a       /* DSP: 13 bits, direct                 */
#define R_REL23        0x2b       /* DSP,C54X: 23 bits, direct (ext addr) */
#define R_RELXPC       0x2c       /* DSP,C54X: 16 bits, relative to XPC   */
#define R_HIEXT        0x2d       /* C54X: Hi word of extended prog addr  */
#define R_HIWORD       0x31	  /* RR: 8 bit reloc. hi byte of word     */
#define R_LABCOD       0x32       /* C16 16-bit code address relocation   */

#define R_PPBASE       0x34       /* PP: Global Base address type         */
#define R_PPLBASE      0x35       /* PP: Local Base address type          */
#define R_PP15	       0x38       /* PP: Global 15 bit offset             */
#define R_PP15W        0x39       /* PP: Global 15 bit offset / 4         */
#define R_PP15H	       0x3a       /* PP: Global 15 bit offset / 2         */
#define R_PP16B        0x3b       /* PP: Global 16 bit offset for bytes   */
#define R_PPL15	       0x3c       /* PP: Local 15 bit offset              */
#define R_PPL15W       0x3d       /* PP: Local 15 bit offset divided by 4 */
#define R_PPL15H       0x3e       /* PP: Local 15 bit offset divided by 2 */
#define R_PPL16B       0x3f       /* PP: Local 16 bit offset for bytes    */
#define R_PPN15	       0x40       /* PP: Global 15 bit neg offset         */
#define R_PPN15W       0x41       /* PP: Global 15 bit neg offset / 4     */
#define R_PPN15H       0x42       /* PP: Global 15 bit neg offset / 2     */
#define R_PPN16B       0x43       /* PP: Global 16 bit neg byte offset    */
#define R_PPLN15       0x44       /* PP: Local 15 bit neg offset          */
#define R_PPLN15W      0x45       /* PP: Local 15 bit neg offset / 4      */
#define R_PPLN15H      0x46       /* PP: Local 15 bit neg offset / 2      */
#define R_PPLN16B      0x47       /* PP: Local 16 bit neg byte offset     */

#define R_MPPCR	       0x4f       /* MP: 32-bit PC-relative / 4           */

#define R_C60BASE      0x50	  /* C60: Data Page Pointer Based Offset  */
#define R_C60DIR15     0x51       /* C60: LD/ST long Displacement         */
#define R_C60PCR21     0x52       /* C60: 21-bit PC Relative              */
#define R_C60PCR10     0x53       /* C60: 10-bit PC Relative              */
#define R_C60LO16      0x54       /* C60: MVK Low Half Register           */
#define R_C60HI16      0x55       /* C60: MVKH/MVKLH High Half Register   */
#define R_C60SECT      0x56       /* C60: Section-Based Offset            */
#define R_C60S16       0x57       /* C60: Signed 16-bit                   */
#define R_C60PCR7      0x70       /* C60: 7-bit PC Relative               */
#define R_C60PCR12     0x71       /* C60: 12-bit PC Relative              */
#define R_C60PCR12H    0x72       /* C60: 12-bit PC Relative (Half)       */
#define R_C60PCR7J     0x73       /* C60: 7-bit PC Relative (-BNOP)       */
#define R_C60PCR8J     0x74       /* C60: 8-bit PC Relative (-BNOP)       */
#define R_C60PCR10J    0x75       /* C60: 10-bit PC Relative (-CALLP)     */
#define R_C60ALIGN     0x76       /* C60: Alignment info for compressor   */
#define R_C60FPHEAD    0x77       /* C60: Explicit assembly directive     */
#define R_C60NOCMP    0x100       /* C60: Don't compress this code scn    */

#define R_C8PHIBYTE    0x58	  /* C8+: High byte of 24-bit address.    */
#define R_C8PMIDBYTE   0x59	  /* C8+: Middle byte of 24-bit address.  */
#define R_C8PVECADR    0x5a	  /* C8+: Vector address (0xFFnnnn)       */
#define R_C8PADR24     0x5b	  /* C8+: 24-bit address (rev byte order) */

#define R_PARTLS6      0x5d       /* ANKOOR: 6 bit offset of 22 bit addr  */
#define R_PARTMID10    0x5e       /* ANKOOR: Middle 10 bits of 22 bit addr*/ 
#define R_REL22        0x5f       /* ANKOOR: 22 bits, direct              */
#define R_PARTMS6      0x60       /* ANKOOR: Upper 6 bits of 22 bit addr  */
#define R_PARTMS16     0x61       /* ANKOOR: Upper 16 bits of 22 bit addr */
#define R_ANKPCR16     0x62       /* ANKOOR: PC relative 16 bit           */
#define R_ANKPCR8      0x63       /* ANKOOR: PC relatvie 8 bit            */
#define R_ANKPTR       0x64       /* ANKOOR: 22 bit pointer               */
#define R_ANKHI16      0x65       /* ANKOOR: HI 16 bits of address + data */
#define R_ANKLOPTR     0x66       /* ANKOOR: Pointer to low 64K           */
#define R_ANKNWORD     0x67       /* ANKOOR: 16 bit negated relocation    */
#define R_ANKNBYTE     0x68       /* ANKOOR:  8 bit negated relocation    */
#define R_ANKHIBYTE    0x69       /* ANKOOR: High byte of a word          */
#define R_REL13_SE16   0x6a       /* MANTRA: 13 bit sign extend to 16 bit */

#define R_LD3_DMA      0x78	  /* LEAD3: 7 most sig bits of a byte,    */
				  /* unsigned value; used in DMA address  */
				  /*                                      */
				  /* 5432109876543210                     */
				  /* xxxxxxxxFFFFFFFx                     */

#define R_LD3_MDP      0x7a	  /* LEAD3: 7 bits spanning two bytes,    */
				  /* unsigned value; used as MDP reg value*/
				  /*                                      */
				  /* 321098765432109876543210             */
				  /* xxxxxxxxxxxxxFFFFFFFxxxx             */

#define R_LD3_PDP      0x7b	  /* LEAD3: 9 bits spanning two bytes,    */
				  /* unsigned value; used as PDP reg value*/
				  /*                                      */
				  /* 321098765432109876543210             */
				  /* xxxxxxxxxxxFFFFFFFFFxxxx             */

#define R_LD3_REL23    0x7c	  /* LEAD3: 23 bit unsigned value in a    */
				  /* 24-bit-wide field			  */


#define R_TARBASE      0x80	  /* TARANTULA: Data Page Pointer Based Off */
#define R_TARDIR12     0x81       /* TARANTULA: LD/ST long Displacement     */
#define R_TARLO16      0x82       /* TARANTULA: MVK Low Half Register       */
#define R_TARHI16      0x83       /* TARANTULA: MVKH/MVKLH High Half Reg    */
#define R_TARSECT      0x84       /* TARANTULA: Section-based offset        */

#define R_LD3_k8       0x88       /* LD3: Unsigned 8 bit, direct          */
#define R_LD3_k16      0x89       /* LD3: Unsigned 16 bit, direct         */
#define R_LD3_K8       0x8a       /* LD3: Signed 8 bit, direct            */
#define R_LD3_K16      0x8b       /* LD3: Signed 16 bit, direct           */

#define R_LD3_l8       0x8c       /* LD3: Unsigned 8 bit, PC-relative     */
#define R_LD3_l16      0x8d       /* LD3: Unsigned 16 bit, PC-relative    */
#define R_LD3_L8       0x8e       /* LD3: Signed 8 bit, PC-relative       */
#define R_LD3_L16      0x8f       /* LD3: Signed 16 bit, PC-relative      */

#define R_LD3_k4       0x90       /* LD3: Unsigned 4 bit shift immediate  */
#define R_LD3_k5       0x91       /* LD3: Unsigned 5 bit shift immediate  */
#define R_LD3_K5       0x92       /* LD3: Signed 5 bit shift immediate    */
#define R_LD3_k6       0x93       /* LD3: Unsigned 6 bit immediate        */
#define R_LD3_k12      0x94       /* LD3: Unsigned 12 bit immediate       */
#define R_LD3_ABS16    0x95       /* LD3: abs16(addr) truncated k16       */

#define R_NONE	         0x96     /* Encode dependencies between sections */

#define R_MSP_PCR20_SRC  0x97     /* MSP: PC-rel 20-bit src ext encoding     */
#define R_MSP_PCR20_DST  0x98     /* MSP: PC-rel 20-bit dst ext encoding     */
#define R_MSP_PCR20_ODST 0x99     /* MSP: PC-rel 20-bit offset dst ext enc   */
#define R_MSP_EXT20_SRC  0x9A     /* MSP: Relative 20-bit src ext encoding   */
#define R_MSP_EXT20_DST  0x9B     /* MSP: Relative 20-bit dst ext encoding   */
#define R_MSP_EXT20_ODST 0x9C     /* MSP: Rel 20-bit offset dst ext encoding */
#define R_MSP_REL20_SRC  0x9D     /* MSP: Relative 20-bit src opnd           */
#define R_MSP_REL20_DST  0x9E     /* MSP: Relative 20-bit dst opnd           */
#define R_MSP_PCR16      0x9F     /* MSP: PC-rel 16-bit encoding             */
#define R_MSP_PCR20_CALL 0xA0     /* MSP: PC-rel 20-bit call operand         */
#define R_MSP_REL16      0xA1     /* MSP: Relative 16-bit (20bit MSPx device)*/

#define R_T2_PCR24H    0xA2    /* THUMB2: 24 bits, PC-rel in halfwords(x>>1) */
#define R_T2_PCR20H    0xA3    /* THUMB2: 20 bits, PC-rel in halfwords(x>>1) */

#define R_LD3_ABS24    0xA4    /* C55x+: LD_R_ABS, ST_R_ABS -- *(#k24)       */
#define R_LD3_SP_K9    0xA5    /* C55x+: SP += K9 w/ K9 encoded as (K9 >> 1) */

/*---------------------------------------------------------------------------*/
/* GENERIC relocation types for complex relocation expressions.              */
/* *** NOTE: This range of relocation types exists above 0x4000 ***          */
/* *** NOTE: Top bit of relocation type field used as SEGVALUE flag ***      */
/*---------------------------------------------------------------------------*/
#define RE_ADD		0x4000	/* Operator Instructions: +                  */
#define RE_SUB		0x4001	/*                        -                  */
#define RE_NEG		0x4002	/*                        unary -            */
#define RE_MPY		0x4003	/*                        *                  */
#define RE_DIV		0x4004	/*                        /                  */
#define RE_MOD		0x4005	/*                        %                  */

#define RE_SR		0x4006	/*                        >>u                */
#define RE_ASR		0x4007	/*                        >>s                */
#define RE_SL		0x4008	/*                        <<                 */

#define RE_AND		0x4009	/*                        &                  */
#define RE_OR		0x400a	/*                        |                  */
#define RE_XOR		0x400b	/*                        ^                  */
#define RE_NOTB		0x400c	/*                        ~                  */
 
#define RE_ULDFLD	0x400d	/* unsigned relocation field load            */
#define RE_SLDFLD	0x400e	/* signed relocation field load              */
#define RE_USTFLD	0x400f	/* unsigned relocation field store           */
#define RE_SSTFLD	0x4010	/* signed relocation field store             */
#define RE_XSTFLD	0x4016	/* signedness is irrelevant                  */

#define RE_PUSH		0x4011	/* push symbol on the stack                  */
#define RE_PUSHSV	0xc011	/* push symbol: SEGVALUE flag set            */
#define RE_PUSHSK	0x4012	/* push signed constant on the stack         */
#define RE_PUSHUK	0x4013	/* push unsigned constant on the stack       */
#define RE_PUSHPC	0x4014	/* push current section PC on the stack      */
#define RE_DUP          0x4015  /* duplicate tos and push copy               */

/*---------------------------------------------------------------------------*/
/* Other useful generic relocation types                                     */
/*---------------------------------------------------------------------------*/
#define RM_FIRST_MARKER   0x5000    /* First non-relocatable type marker    */
#define RM_RANGE          0x5000    /* Raw data marker - no relocation      */
#define RM_OBJ            0x5001    /* Raw data marker - no relocation      */
#define RM_DWARF_LENGTH   0x5002    /* Raw data marker - no relocation      */
#define RM_LAST_MARKER    0x5002    /* Last non-relocatable type marker     */

/*---------------------------------------------------------------------------*/
/* Macro to determine whether relocation entry has a symbol on the section's */
/* symbol list.  Operator relocation types do not.                           */
/*---------------------------------------------------------------------------*/
#define isunary(x)	((x) == RE_NEG || (x) == RE_NOTB)
#define isbinary(x)	(ismathrel(x) && !isunary(x))
#define issymrel(x)	((x) == RM_OBJ    || (x) == RE_PUSH || \
                         (x) == RE_PUSHSV || (x) < 0x4000)
#define ismathrel(x)	((x) >= RE_ADD && (x) <= RE_NOTB)
#define ispushrel(x)	(((x) >= RE_PUSH && (x) <= RE_PUSHPC) || ((x) == RE_DUP))
#define isldfldrel(x)	((x) == RE_ULDFLD || (x) == RE_SLDFLD)
#define isstfldrel(x)	((x) == RE_USTFLD || (x) == RE_SSTFLD || (x) == RE_XSTFLD )
#define ismarker(x)     ((x) >= RM_FIRST_MARKER && (x) <= RM_LAST_MARKER)

/*---------------------------------------------------------------------------*/
/* Macro to determine if current relocation entry is a field instruction.    */
/*---------------------------------------------------------------------------*/
#define isfldrel(x)	(((x) >= RE_ULDFLD && (x) <= RE_SSTFLD) || ((x) == RE_XSTFLD))


/*------------------------------------------------------------------------*/
/*  LINE NUMBER ENTRIES                                                   */
/*------------------------------------------------------------------------*/
struct lineno
{
   union
   {
      int         l_symndx; /* sym index of fcn name iff l_lnno == 0     */
      int         l_paddr;  /* (physical) address of line number         */
   }  l_addr;
   unsigned short  l_lnno;   /* line number                               */
};

#define LINENO  struct lineno
#define LINESZ  6       /* sizeof(LINENO)                                 */


/*------------------------------------------------------------------------*/
/*  SYMBOL TABLE ENTRIES                                                  */
/*------------------------------------------------------------------------*/

#define  FILNMLEN   14     /*  Number of characters in a file name        */
#define  DIMNUM     4      /*  Number of array dimensions in aux entry    */

typedef unsigned short DSTYPE;

struct syment
{
    union
    {
       char            _n_name[SYMNMLEN];   /* old COFF version           */
       struct
       {
          int    _n_zeroes;                /* new == 0                   */
          int    _n_offset;                /* offset into string table   */
       } _n_n;
       const char     *_n_nptr[2];          /* allows for overlaying      */
    } _n;

    int                    n_value;        /* value of symbol            */
    short                   n_scnum;        /* section number             */
    DSTYPE                  n_type;         /* type and derived type      */
    char                    n_sclass;       /* storage class              */
    char                    n_numaux;       /* number of aux. entries     */
};

#define n_name          _n._n_name
#define n_nptr          _n._n_nptr[1]
#define n_zeroes        _n._n_n._n_zeroes
#define n_offset        _n._n_n._n_offset

/*------------------------------------------------------------------------*/
/* Relocatable symbols have a section number of the                       */
/* section in which they are defined.  Otherwise, section                 */
/* numbers have the following meanings:                                   */
/*------------------------------------------------------------------------*/
#define  N_UNDEF  0                     /* undefined symbol               */
#define  N_ABS    -1                    /* value of symbol is absolute    */
#define  N_DEBUG  -2                    /* special debugging symbol       */


/*------------------------------------------------------------------------*/
/*  AUXILIARY SYMBOL ENTRY                                                */
/*------------------------------------------------------------------------*/
#define SPACE(len, name)    char  name[len]

union auxent
{
        struct
	{
	    SPACE(4, _0_3);
            int    x_fsize;        /* size of struct in bits.            */
	    SPACE(4, _8_11);
            int    x_endndx;       /* ptr to next sym beyond .eos        */
	    SPACE(2, _16_17);
	} x_tag;

        struct
	{
	    int    x_tagndx;       /* ptr to beginning of struct         */
	    int    x_fsize;        /* size of struct in bits.            */
	    SPACE(10, _8_17);
        } x_eos;

	struct 
	{
	    int    x_tagndx;       /* ptr to tag for function            */
	    int    x_fsize;        /* size of function in bytes          */
	    int    x_lnnoptr;      /* file ptr to fcn line #             */
	    int    x_endndx;       /* ptr to next sym beyond .ef         */
	    SPACE(2, _16_17);
        } x_func;

        struct
	{
            int    x_regmask;      /* Mask of regs use by func           */
	    unsigned short  x_lnno; /* line number of block begin         */
	    unsigned short  x_lcnt; /* # line number entries in func      */
	    int    x_framesize;    /* size of func local vars            */
	    int    x_endndx;       /* ptr to next sym beyond .eb         */
	    SPACE(2, _16_17);
	} x_block;

        struct
	{
	    int    x_tagndx;       /* ptr to tag for array type          */
	    int    x_fsize;        /* Size of array in bits.             */
            unsigned short  x_dimen[DIMNUM];
	    SPACE(2, _16_17);
	} x_array;

	struct
	{
            int    x_tagndx;       /* str, un, or enum tag indx          */
	    int    x_fsize;        /* Size of symbol                     */
	    SPACE(10, _10_17);
	} x_sym;

        struct
        {
	    char    x_fname[FILNMLEN];
        } x_file;

        struct
        {
            int            x_scnlen;  /* section length                  */
            unsigned short  x_nreloc;  /* number of reloc entries         */
            unsigned short  x_nlinno;  /* number of line numbers          */
	    SPACE(8, _10_17);
        } x_scn;
};

#define SYMENT  struct syment
#define SYMESZ  18      /* sizeof(SYMENT) */

#define AUXENT  union auxent
#define AUXESZ  18      /* sizeof(AUXENT) */

/*------------------------------------------------------------------------*/
/*  NAMES OF "SPECIAL" SYMBOLS                                            */
/*------------------------------------------------------------------------*/
#define  _BF            ".bf"
#define  _EF            ".ef"
#define _STEXT          ".text"
#define _ETEXT          "etext"
#define _SDATA          ".data"
#define _EDATA          "edata"
#define _SBSS           ".bss"
#define _END            "end"
#define _C_ARGS   	".args"
#define _BINITPTR	"binit"
#define _CINITPTR       "cinit"
#define _PINITPTR       "pinit"
#define _ASM_SRC_NAME   "$ASM$"    /* SPECIAL SYMBOL FOR ASSY SRC DEBUG */

/*------------------------------------------------------------------------*/
/* HIGH LEVEL LANGUAGE ACCESSIBLE VERSIONS OF THE ABOVE SPECIAL SYMBOLS.  */
/*------------------------------------------------------------------------*/
#define _STEXT_HLL      "___text__"
#define _ETEXT_HLL      "___etext__"
#define _SDATA_HLL      "___data__"
#define _EDATA_HLL      "___edata__"
#define _SBSS_HLL       "___bss__"
#define _END_HLL        "___end__"
#define _C_ARGSPTR_HLL  "___c_args__"
#define _BINITPTR_HLL	"___binit__"
#define _CINITPTR_HLL   "___cinit__"
#define _PINITPTR_HLL   "___pinit__"

/*------------------------------------------------------------------------*/
/*  ENTRY POINT SYMBOLS                                                   */
/*------------------------------------------------------------------------*/
#define _START          "_start"
#define _MAIN           "_main"

/*------------------------------------------------------------------------*/
/*  SYMBOLS USED FOR C++ LOWERING                                         */
/*------------------------------------------------------------------------*/
#define _TYPEINFO_ID_PREFIX    "__TID_"        /* Unique type id for RTTI */
#define _TEMPLATE_MANGLED_ROOT "__tm__"        /* mangled name root for   */
					       /* template symbols.       */              
#define _TEMPLATE_SIGNATURES  "__pt__"         /* when distinct_template_ */
					       /* signatures is FALSE,    */
                                               /* template name is mangled*/
                                               /* with this string.       */
#define _PARTIAL_SPEC         "__ps__"         /* for the first argument  */
                                               /* list of a partial       */
                                               /* specialization, this    */
                                               /* string is used to mangle*/
                                               /* the name.               */
#endif
