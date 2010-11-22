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
* FILENAME: cload.c
* VERSION:  2.6  5/2/96  13:11:03
* SCCS ID:  "@(#)cload.c	2.6  5/2/96"
***************************************************************************/
/******************************************************************************/
/* CLOAD.C  - Generic COFF Loader                      Version 6.00 4/96      */
/******************************************************************************/
/*                                                                            */
/*  This module contains functions to read in and load a COFF object file.    */
/*  The following routines are defined here:                                  */
/*                                                                            */
/*    cload()           - Main driver for COFF loader.                        */
/*    cload_headers()   - Read in the various headers of the COFF file.       */
/*    cload_data()      - Read in the raw data and load it into memory.       */
/*    cload_sect_data() - Read, relocate, and write out one section.          */
/*    cload_cinit()     - Process one buffer of C initialization records.     */
/*    cload_symbols()   - Read in the symbol table.                           */
/*    cload_strings()   - Read in the string table.                           */
/*    str_free()        - Free a string table.                                */
/*    sym_read()        - Read and relocate a symbol and its aux entry.       */
/*    sym_name()        - Return a pointer to the name of a symbol.           */
/*    sym_add_name()    - Add a symbol name to the string table.              */
/*    reloc_add()       - Add a symbol to the relocation symbol table.        */
/*    relocate()        - Perform a single relocation.                        */
/*    reloc_read()      - Read in and swap one relocation entry.              */
/*    reloc_size()      - Return the field size of a relocation type.         */
/*    reloc_offset()    - Return the field offset of a relocation type.       */
/*    reloc_stop()      - Return the number of bits to read for a reloc type. */
/*    sym_reloc_amount()- Return relocation amount for a relocation entry.    */
/*    unpack()          - Extract a relocation field from object bytes.       */
/*    repack()          - Encode a relocated field into object bytes.         */
/*    cload_lineno()    - Read in & swap line number entries.                 */
/*    swap4byte()       - Swap the order of bytes in a int.                  */
/*    swap2byte()       - Swap the order of bytes in a short.                 */
/*                                                                            */
/*  The loader calls the following external functions to perform application  */
/*  specific tasks:                                                           */
/*                                                                            */
/*   set_reloc_amount() - Define relocation amounts for each section.         */
/*   mem_write()        - Load a buffer of data into memory.                  */
/*   lookup_sym()       - Look up a symbol in an external symbol table.       */
/*   load_syms()        - Build the symbol table for the application.         */
/*   load_msg()         - Write diagnostic messages during loading.           */
/*                                                                            */
/******************************************************************************/
#include "header.h"
#include "coff_trg.h"
#if ((FILE_BASED) && !defined(FILE))
#include <stdio.h>
#endif
/*----------------------------------------------------------------------------*/
/* CONSTANTS, MACROS, VARIABLES, AND STRUCTURES FOR THE LOADER.               */
/*----------------------------------------------------------------------------*/
#define TRUE 1
#define FALSE 0
#define WORDSZ sizeof(T_DATA)           /* SIZE OF DATA UNITS IN OBJ FILE     */

#ifdef TMS320C60
   extern int target_coff (unsigned short flags);
#endif

/*----------------------------------------------------------------------------*/
/* APPLICATION VARIABLES                                                      */
/*----------------------------------------------------------------------------*/
#if (FILE_BASED)
FILE   *fin;							/* INPUT FILE                         */
#else
extern unsigned char gRxBuffer[0x400040];
#endif
                            
int     need_data    = TRUE;            /* APPLICATION NEEDS RAW DATA         */
int     need_symbols = FALSE;           /* APPLICATION NEEDS SYMBOL TABLE     */
int     clear_bss    = FALSE;           /* CLEAR BSS SECTION                  */


/*----------------------------------------------------------------------------*/
/* FILL VALUES TO BE USED BY LOADERS                                          */
/*----------------------------------------------------------------------------*/
#if defined(OTIS)
extern int fill_bss;
extern int fill_bss_value;
#else
#define fill_bss 0
#define fill_bss_value 0
#endif

/*----------------------------------------------------------------------------*/
/* LOADER VARIABLES                                                           */
/*----------------------------------------------------------------------------*/
FILHDR  file_hdr;                       /* FILE HEADER STRUCTURE              */
int     coff_version;                   /* COFF VERSION USED IN FILE          */
AOUTHDR o_filehdr;                      /* OPTIONAL (A.OUT) FILE HEADER       */
T_ADDR  entry_point;                    /* ENTRY POINT OF MODULE              */
T_ADDR *reloc_amount = NULL;            /* AMOUNT OF RELOCATION PER SECTION   */
char   *sect_hdrs = NULL;               /* ARRAY OF SECTION HEADERS           */
char   *o_sect_hdrs = NULL;             /* ARRAY OF OLD COFF SECTION HEADERS  */
int     n_sections;                     /* NUMBER OF SECTIONS IN THE FILE     */
int     big_e_target;                   /* TARGET DATA IN BIG-ENDIAN FORMAT   */
int     byte_swapped;                   /* BYTE ORDERING OPPOSITE OF HOST     */
int     curr_sect;                      /* SECTION NUMBER CURRENTLY LOADING   */
int     load_err;                       /* ERROR CODE RETURNED IF LOADER FAILS*/
struct strtab *str_head = NULL;         /* LIST OF STRING TABLE BUFFERS       */

static T_SIZE  init_size = 0;           /* CURRENT SIZE OF C INITIALIZATION   */
static int     need_reloc;              /* RELOCATION REQUIRED                */

#if TMS320C60
int big_e_config = TRG_DEF_BIG_E;       /* ENDIANNESS CONFIGURATION           */
#else
int big_e_config = DONTCARE;            /* ENDIANNESS CONFIGURATION           */
#endif

/*----------------------------------------------------------------------------*/
/* THIS STRUCTURE IS USED TO STORE THE RELOCATION AMOUNTS FOR SYMBOLS.        */
/* EACH RELOCATABLE SYMBOL HAS A CORRESPONDING ENTRY IN THIS TABLE.           */
/* THE TABLE IS SORTED BY SYMBOL INDEX; LOOKUP USES A BINARY SEARCH.          */
/*----------------------------------------------------------------------------*/
typedef struct
{
   int rt_index;                       /* INDEX OF SYMBOL IN SYMBOL TABLE    */
   int rt_disp;                        /* AMOUNT OF RELOCATION               */
} RELOC_TAB;

/*----------------------------------------------------------------------------*/
/* THE RELOCATION SYMBOL TABLE IS ALLOCATED DYNAMICALLY, AND REALLOCATED      */
/* AS MORE SPACE IS NEEDED.                                                   */
/*----------------------------------------------------------------------------*/
#define RELOC_TAB_START 128             /* STARTING SIZE OF TABLE             */
#define RELOC_GROW_SIZE 128             /* REALLOC AMOUNT FOR TABLE           */

static RELOC_TAB *reloc_tab = NULL;     /* RELOCATION SYMBOL TABLE            */

static int        reloc_tab_size;       /* CURRENT ALLOCATED AMOUNT           */
static int        reloc_sym_index;      /* CURRENT SIZE OF TABLE              */

/*----------------------------------------------------------------------------*/
/* RUN-TIME RELOCATION (AS OPPOSED TO LOAD-TIME) RELOCATION IS DETERMINED     */
/* AS FOLLOWS:  IF THE SECTION'S RUNTIME ADDRESS == LOADTIME ADDRESS, USE     */
/* LOADTIME RELOCATION.  OTHERWISE, ASSUME LOADTIME RELOC ONLY (RUNTIME RELOC */
/* == 0).                                                                     */
/*----------------------------------------------------------------------------*/
#define RUN_RELOC_AMOUNT(i) ((SECT_HDR(i)->s_paddr == SECT_HDR(i)->s_vaddr) ?  \
				    reloc_amount[i] : 0)

/*----------------------------------------------------------------------------*/
/* DEFINE A STRUCTURE FOR STRING TABLE BUFFERS.  THESE BUFFERS ARE STORED     */
/* AS A LINKED LIST OF MEMORY PACKETS, EACH CONTAINING UP TO 64K OF THE       */
/* STRING TABLE.                                                              */
/*----------------------------------------------------------------------------*/
typedef struct strtab
{
   unsigned int  size;                 /* SIZE OF THIS PACKET                */
   unsigned int  offset;               /* STARTING OFFSET OF THIS PACKET     */
   unsigned int  index;                /* AMOUNT CURRENTLY FILLED            */
   struct strtab *next;                 /* NEXT BUFFER                        */
   char           buf[1];               /* STRING DATA (EXPAND AS ALLOCATED)  */
} STRTAB;

#define MAX_STRING_ALLOC (unsigned int)(0xffff-sizeof(STRTAB)+1)
                                       /* MAX STRING BUFFER: 64K (SMALL HOSTS)*/
#define MIN_STRING_ALLOC 0x0400        /* MIN STRING BUFFER: 1K               */

unsigned int unpack();

#define reloc_read(x)   TRUE

/* extern void mem_copy(unsigned char* dst, unsigned char* src, int nbytes); - defined in osal.h */
/******************************************************************************/
/*                                                                            */
/* CLOAD() - Main driver for COFF loader.                                     */
/*                                                                            */
/******************************************************************************/
int cload()
{
   int result;

   load_err = 0;

   result   = cload_headers() && (need_reloc == FALSE) && cload_data();

   if (reloc_tab) free(reloc_tab);
   reloc_tab = NULL;

   if(result == TRUE)
       return 0;
   else
       return -1;
}


/******************************************************************************/
/*                                                                            */
/* CLOAD_HEADERS() - Read in the various headers of the COFF file.            */
/*                                                                            */
/******************************************************************************/
int cload_headers()
{
   int i;

   byte_swapped = FALSE;
   need_reloc   = FALSE;
#if FILE_BASED
   if (fseek(fin, 0L, 0) != 0 || !fread(&file_hdr, FILHSZ, 1, fin)) 
      { load_err = E_FILE; return FALSE; }
#else
	mem_copy((void*)&file_hdr, gRxBuffer, FILHSZ);
#endif

   /*-------------------------------------------------------------------------*/
   /* MAKE SURE THIS IS REALLY A COFF FILE. CHECK FOR SWAPPED FILES.          */
   /* DETERMINE BYTE ORDERING OF OBJECT DATA.                                 */
   /*-------------------------------------------------------------------------*/
   if (!ISCOFF(file_hdr.f_magic))
   {
       swap2byte(&file_hdr.f_magic);

       if (!ISCOFF(file_hdr.f_magic)) { load_err = E_MAGIC; return FALSE; }

       byte_swapped = TRUE;

       swap2byte(&file_hdr.f_nscns);  swap4byte(&file_hdr.f_timdat);
       swap4byte(&file_hdr.f_symptr); swap4byte(&file_hdr.f_nsyms);
       swap2byte(&file_hdr.f_opthdr); swap2byte(&file_hdr.f_flags);
#if COFF_VERSION_1 || COFF_VERSION_2
       swap2byte(&file_hdr.f_target_id); 
#endif
   }

   /*-------------------------------------------------------------------------*/
   /* DETERMINE THE ENDIANNESS OF THE COFF FILE, AND ENSURE THE ENDIANNESS OF */
   /* THE FILE IS THE SAME AS THE TARGET, IF THERE IS A TARGET.               */
   /*-------------------------------------------------------------------------*/
   big_e_target = ((file_hdr.f_flags & F_BIG) != 0);
   if (big_e_config != DONTCARE && big_e_target != big_e_config) 
      { load_err = E_ENDIAN; return FALSE; }

   /*-------------------------------------------------------------------------*/
   /* DETERMINE VERSION OF COFF BEING USED, CHECK TARGET ID IF NEEDED.        */
   /*-------------------------------------------------------------------------*/
   if (ISCOFF_1(file_hdr.f_magic) || ISCOFF_2(file_hdr.f_magic))
   {
       if (!ISMAGIC(file_hdr.f_target_id)) { load_err = E_MAGIC; return FALSE; }
       coff_version = file_hdr.f_magic;
   } 
   else coff_version = COFF_MAGIC_0;

#ifdef TMS320C60
   /*-------------------------------------------------------------------------*/
   /* DETERMINE WHETHER THE RIGHT COFF IS BEING LOADED                        */
   /*-------------------------------------------------------------------------*/
   if ( !target_coff( file_hdr.f_flags) )
       { load_err = E_FILE; return FALSE; }
#endif
 
   /*-------------------------------------------------------------------------*/
   /* READ IN OPTIONAL HEADER, IF THERE IS ONE, AND SWAP IF NEEDED.           */
   /*-------------------------------------------------------------------------*/
   if (file_hdr.f_opthdr == AOUTSZ)
   {
#if FILE_BASED   	
      fseek(fin, (int)FILHSZ_IN(coff_version), 0); 
      if (fread(&o_filehdr, file_hdr.f_opthdr, 1, fin) != 1) 
	 { load_err = E_FILE; return FALSE; }
#else
	mem_copy((void*)&o_filehdr, &gRxBuffer[(int)FILHSZ_IN(coff_version)], file_hdr.f_opthdr);
#endif	 

      if (byte_swapped)
      {
	  swap2byte(&o_filehdr.magic);      swap2byte(&o_filehdr.vstamp);
	  swap4byte(&o_filehdr.tsize);      swap4byte(&o_filehdr.dsize);
	  swap4byte(&o_filehdr.bsize);      swap4byte(&o_filehdr.entrypt);
	  swap4byte(&o_filehdr.text_start); swap4byte(&o_filehdr.data_start);
      }
      entry_point = o_filehdr.entrypt;
   }

   /*-------------------------------------------------------------------------*/
   /* Read in string table so that we can see long section names, if needed.  */
   /* This used tobe read right before the symbol table was read, but now the */
   /* section headers use "flexname" method to specify section names and so   */
   /* might need access to a string table entry.                              */
   /*-------------------------------------------------------------------------*/
   //if (!cload_strings()) return FALSE;

   /*-------------------------------------------------------------------------*/
   /* READ IN SECTION HEADERS.                                                */
   /*-------------------------------------------------------------------------*/
   if (sect_hdrs)   { free(sect_hdrs);     sect_hdrs = NULL; }
   if (o_sect_hdrs) { free(o_sect_hdrs); o_sect_hdrs = NULL; }

   if (!(sect_hdrs = (char *)malloc((n_sections = file_hdr.f_nscns) * SCNHSZ)))
      { load_err = E_ALLOC;  return FALSE; }
      
#if FILE_BASED
   fseek(fin, (int)FILHSZ_IN(coff_version) + file_hdr.f_opthdr, 0); 
#endif
   /*-------------------------------------------------------------------------*/
   /* Depending on which version of COFF we are reading, set up the section   */
   /* headers or s copy that we can translate into the new version.           */
   /*-------------------------------------------------------------------------*/
   if (ISCOFF_2(coff_version))
   {
#if FILE_BASED
      if (fread(sect_hdrs,SCNHSZ_IN(coff_version),n_sections,fin) != n_sections)
         { load_err = E_FILE;   return FALSE; }
#else
	mem_copy((void*)sect_hdrs, &gRxBuffer[(int)FILHSZ_IN(coff_version) + file_hdr.f_opthdr], SCNHSZ_IN(coff_version)*n_sections );
#endif
   }
   else
   {
      if (!(o_sect_hdrs = (char *)malloc(n_sections * SCNHSZ_IN(coff_version))))
         { load_err = E_ALLOC;  return FALSE; }
#if FILE_BASED
      if (fread(o_sect_hdrs,SCNHSZ_IN(coff_version),n_sections,fin)!=n_sections)
         { load_err = E_FILE;   return FALSE; }
#else
	mem_copy((void*)o_sect_hdrs, &gRxBuffer[(int)FILHSZ_IN(coff_version) + file_hdr.f_opthdr], SCNHSZ_IN(coff_version)*n_sections );
#endif
   }

   if (reloc_amount) free(reloc_amount);

   	if (!(reloc_amount = (T_ADDR*)malloc(n_sections * sizeof(T_ADDR))))
   	{ 
    	load_err = E_ALLOC;  
      	return FALSE; 
	}

   /*-------------------------------------------------------------------------*/
   /* SWAP SECTION HEADERS IF REQUIRED.                                       */
   /*-------------------------------------------------------------------------*/
   for (i = 0; i < n_sections; i++)
   {
       SCNHDR   *sptr = SECT_HDR(i);
       O_SCNHDR *tptr = O_SECT_HDR(i);

       if (byte_swapped)
       {
          /*-----------------------------------------------------------------*/
	  /* Swap sections according to native COFF version.                 */
	  /*-----------------------------------------------------------------*/
	  if (ISCOFF_2(coff_version))
	  {
             if (sptr->s_zeroes == 0L) swap4byte(&sptr->s_offset);
             swap4byte(&sptr->s_paddr);  
	     swap4byte(&sptr->s_vaddr);
             swap4byte(&sptr->s_size);   
	     swap4byte(&sptr->s_scnptr);
             swap4byte(&sptr->s_relptr); 
	     swap4byte(&sptr->s_lnnoptr);
             swap4byte(&sptr->s_nreloc); 
	     swap4byte(&sptr->s_nlnno);
             swap4byte(&sptr->s_flags);  
             /* s_mwidth   - single byte */
             /* s_reserved - single byte */
	     swap2byte(&sptr->s_page);
	  }
	  else
	  {
             swap4byte(&tptr->os_paddr);  
	     swap4byte(&tptr->os_vaddr);
             swap4byte(&tptr->os_size);   
	     swap4byte(&tptr->os_scnptr);
             swap4byte(&tptr->os_relptr); 
	     swap4byte(&tptr->os_lnnoptr);
             swap2byte(&tptr->os_nreloc); 
	     swap2byte(&tptr->os_nlnno);
             swap2byte(&tptr->os_flags);
	     /* os_reserved - one byte */
	     /* os_page     - one byte */
	  }
       }

       /*---------------------------------------------------------------------*/
       /* Old COFF version section headers are now ready to be transferred.   */
       /*---------------------------------------------------------------------*/
       if (!ISCOFF_2(coff_version))
       {
          strn_copy(sptr->s_name, tptr->os_name, SYMNMLEN);
          sptr->s_paddr   = tptr->os_paddr;
          sptr->s_vaddr   = tptr->os_vaddr;
          sptr->s_size    = tptr->os_size;
          sptr->s_scnptr  = tptr->os_scnptr;
          sptr->s_relptr  = tptr->os_relptr;
          sptr->s_lnnoptr = tptr->os_lnnoptr;
          sptr->s_nreloc  = tptr->os_nreloc;
          sptr->s_nlnno   = tptr->os_nlnno;
          sptr->s_flags   = tptr->os_flags;
          sptr->s_page    = tptr->os_page;
       }

       reloc_amount[i] = 0;

#if 0    // not to search the string table
      /*---------------------------------------------------------------------*/
      /* Fix up section name if it is a pointer into the string table.       */
      /*---------------------------------------------------------------------*/
      if (sptr->s_zeroes == 0L)
      {
         STRTAB *packet = str_head;

	 if (packet == NULL) 
	 {
	     return -1;
	 }

	 if (str_head->offset > sptr->s_offset)
	 {
	     return -1;
	 }

         while (sptr->s_offset < (int)packet->offset) packet = packet->next;
         sptr->s_nptr = packet->buf + (sptr->s_offset - packet->offset);
      }
#endif
   } // for ends


   /*-------------------------------------------------------------------------*/
   /* CALL AN EXTERNAL ROUTINE TO DETERMINE THE RELOCATION AMOUNTS FOR        */
   /* EACH SECTION.                                                           */
   /*-------------------------------------------------------------------------*/
   if (!set_reloc_amount()) { load_err = E_SETRELOC; return FALSE; }
   for (i = 0; i < n_sections; i++) need_reloc |= (reloc_amount[i] != 0);
   if (!need_data) need_reloc = FALSE;

   if (need_reloc && (file_hdr.f_flags & F_RELFLG)) 
      { load_err = E_RELOC; return FALSE; }

   return TRUE;
}


/******************************************************************************/
/*                                                                            */
/* CLOAD_DATA() - Read in the raw data and load it into memory.               */
/*                                                                            */
/******************************************************************************/
int cload_data()
{
   int ok = TRUE;

   if (!need_data) return TRUE;

   /*-------------------------------------------------------------------------*/
   /* LOOP THROUGH THE SECTIONS AND LOAD THEM ONE AT A TIME.                  */
   /*-------------------------------------------------------------------------*/
   for (curr_sect = 0; curr_sect < n_sections && ok; curr_sect++)
   {
      SCNHDR *sptr = SECT_HDR(curr_sect);                        
      char   *sname = (sptr->s_zeroes == 0L) ? 
				sptr->s_nptr : SNAMECPY(sptr->s_name);

      /*----------------------------------------------------------------------*/
      /* IF THIS IS THE TEXT SECTION, RELOCATE THE ENTRY POINT.               */
      /*----------------------------------------------------------------------*/
      if ((sptr->s_flags & STYP_TEXT) && !str_comp(sname, ".text"))
	 entry_point += RUN_RELOC_AMOUNT(curr_sect);

      /*----------------------------------------------------------------------*/
      /* IGNORE EMPTY SECTIONS OR SECTIONS WHOSE FLAGS INDICATE THE           */
      /* SECTION IS NOT TO BE LOADED.  IF THE CLEAR_BSS FLAG IS SET, BSS      */
      /* IS "LOADED" EVEN THOUGH IT HAS NO DATA, AND DEFER THE CINIT          */
      /* SECTION UNTIL LATER TO ENSURE BSS IS LOADED FIRST.                   */
      /*----------------------------------------------------------------------*/
      if ((sptr->s_scnptr || ((clear_bss || fill_bss) && IS_BSS(sptr)))   &&
	  (sptr->s_size)                                              &&
          !(sptr->s_flags & (STYP_DSECT | STYP_COPY | STYP_NOLOAD)))
	  ok &= cload_sect_data(sptr);
   }

  /*-------------------------------------------------------------------------*/
  /* WE DEFERRED CINIT, LOAD IT/THEM NOW.                                    */
  /*-------------------------------------------------------------------------*/
  for (curr_sect = 0; curr_sect < n_sections && ok; curr_sect++)
  {
     SCNHDR *sptr = SECT_HDR(curr_sect);
     char   *sname = (sptr->s_zeroes == 0L) ? 
			       sptr->s_nptr : SNAMECPY(sptr->s_name);
     
     if (IS_CINIT(sptr))
     {
		ok &= cload_sect_data(sptr);
     }
  }

   return ok;
}


/******************************************************************************/
/*                                                                            */
/* CLOAD_SECT_DATA() - Read, relocate, and write out the data for one section.*/
/*                                                                            */
/******************************************************************************/
int cload_sect_data(SCNHDR *sptr)
{
   T_ADDR        addr    = sptr->s_vaddr; /* CURRENT ADDRESS IN SECTION       */
   unsigned int nbytes;                  /* BYTE COUNT WITHIN SECTION        */
   int           packet_size = 0;         /* SIZE OF CURRENT DATA BUFFER      */
   int           excess  = 0;             /* BYTES LEFT FROM PREVIOUS BUFFER  */
   unsigned int  n_reloc = 0;             /* COUNTER FOR RELOCATION ENTRIES   */
   RELOC         reloc;                   /* RELOCATION ENTRY                 */
   int           relsz   = RELSZ_IN(coff_version); 
   unsigned char *packet = NULL;          /* LOAD BUFFER                      */
   unsigned int section_length = (unsigned int)LOCTOBYTE(sptr->s_size);
   unsigned int buffer_size    = LOADBUFSIZE;

#if defined (UNBUFFERED) && UNBUFFERED
   /*-------------------------------------------------------------------------*/
   /* IF UNBUFFERED, THEN SET SIZE TO SECTION LENGTH ROUNDED UP TO MULTIPLE   */
   /* 32 BYTES.  WE MAINTAIN A MINIMIUM OF LOADBUFSIZE IN THE EVENT SOMEONE   */
   /* CONTINUES TO USE THAT MACRO AS A SIZE LIMIT.                            */
   /*-------------------------------------------------------------------------*/
   buffer_size = MAX(buffer_size, (section_length + 32) & ~31ul); 
#endif

   /*-------------------------------------------------------------------------*/
   /* ENSURE LOADBUFSIZE IS A MULTIPLE OF 2                                   */
   /*-------------------------------------------------------------------------*/
   if (LOADBUFSIZE % 2) 
   {
       return -1;
   }

   /*-------------------------------------------------------------------------*/
   /* READ THE FIRST RELOCATION ENTRY, IF THERE ARE ANY.                      */
   /* IF THIS IS A BSS SECTION, CLEAR THE LOAD BUFFER.                        */
   /*-------------------------------------------------------------------------*/
#if FILE_BASED   
   if (need_reloc && sptr->s_nreloc &&
       (fseek(fin, sptr->s_relptr, 0) != 0 || !reloc_read(&reloc)))
      { load_err = E_FILE; return FALSE; }
#else
	// need_reloc is not used....
	if (need_reloc && sptr->s_nreloc && !reloc_read(&reloc, sptr->s_relptr))
      { load_err = E_FILE; return FALSE; }
#endif
   /*-------------------------------------------------------------------------*/
   /* ALLOCATE THE PACKET BUFFER                                              */
   /*-------------------------------------------------------------------------*/
   packet = (unsigned char *) malloc(buffer_size);
   if (!packet) { load_err = E_ALLOC; return FALSE; }

#ifdef OTIS
   if (IS_BSS(sptr))
   {
      TRG_MVAL filval = fill_bss_value;

      free (packet);

      if (!mem_fill(filval, LOCTOBYTE(sptr->s_size), addr, sptr->s_page))
         { load_err = E_MEMWRITE; return FALSE; }
      return TRUE;
   }
#else
   /*-------------------------------------------------------------------------*/
   /* Always want to clear memory for cases where memsize is not a multiple   */
   /* of the data size being written out.  If we do not clear the array, the  */
   /* last byte or so can be corrupted with data from the last buffer read.   */
   /*-------------------------------------------------------------------------*/
   for (nbytes = 0; nbytes < buffer_size; ++nbytes) packet[nbytes] = 0;
#endif
       
   /*-------------------------------------------------------------------------*/
   /* COPY ALL THE DATA IN THE SECTION.                                       */
   /*-------------------------------------------------------------------------*/

   for (nbytes = 0; nbytes < section_length; nbytes += packet_size)
   {
      int j;

      /*----------------------------------------------------------------------*/
      /* READ IN A BUFFER OF DATA.  IF THE PREVIOUS RELOCATION SPANNED        */
      /* ACROSS THE END OF THE LAST BUFFER, COPY THE LEFTOVER BYTES INTO      */
      /* THE BEGINNING OF THE NEW BUFFER.                                     */
      /*----------------------------------------------------------------------*/
      for (j = 0; j < excess; ++j) packet[j] = packet[packet_size + j];

      packet_size = (int)MIN(LOCTOBYTE(sptr->s_size) - nbytes, buffer_size);

      if (sptr->s_scnptr)
#if FILE_BASED   
	  if ((fseek(fin, sptr->s_scnptr + (int)(nbytes + excess), 0) != 0) ||
	      (fread(packet + excess, packet_size - excess, 1, fin) != 1))
	   { 
	      load_err = E_FILE; 
              free (packet);
	      return FALSE; 
	  }
#else
	mem_copy((void*)(packet + excess), (void*)&gRxBuffer[sptr->s_scnptr + (int)(nbytes + excess)], packet_size - excess);
#endif


      excess = 0;

      /*----------------------------------------------------------------------*/
      /* Clear out end of packet in case we write padding.                    */
      /*----------------------------------------------------------------------*/
      if (excess + packet_size < buffer_size)
	  for(j = excess + packet_size; j < buffer_size; j++)
	      packet[j] = 0;

      /*----------------------------------------------------------------------*/
      /* READ AND PROCESS ALL THE RELOCATION ENTRIES THAT AFFECT DATA         */
      /* CURRENTLY IN THE BUFFER.                                             */
      /*----------------------------------------------------------------------*/
      if (need_reloc)
	 while (n_reloc < sptr->s_nreloc && 
		(T_ADDR)reloc.r_vaddr < addr + BYTETOLOC(packet_size))
	 {
	    int i       = (int)LOCTOBYTE(reloc.r_vaddr - addr);   /* BYTE NDX */
	    int fieldsz = reloc_size(reloc.r_type);               /* IN BITS  */
            int wordsz  = MAX(fieldsz, reloc_stop(reloc.r_type)); /* IN BITS  */

	    /*----------------------------------------------------------------*/
	    /* IF THIS RELOCATION SPANS PAST THE END OF THE BUFFER,           */
	    /* SET 'EXCESS' TO THE NUMBER OF REMAINING BYTES AND FLUSH THE    */
	    /* BUFFER.  AT THE NEXT FILL, THESE BYTES WILL BE COPIED FROM     */
	    /* THE END OF THE BUFFER TO THE BEGINNING AND THEN RELOCATED.     */
	    /*----------------------------------------------------------------*/
	    if (i + (wordsz >> 3) > packet_size)
	    { 
	       i          -= i % LOADWORDSIZE;   /* DON'T BREAK WITHIN A WORD */
	       excess      = packet_size - i; 
	       packet_size = i;
	       break;
	    }

	    /*----------------------------------------------------------------*/
	    /* PERFORM THE RELOCATION AND READ IN THE NEXT RELOCATION ENTRY.  */
	    /*----------------------------------------------------------------*/
#if 0
	    if (!relocate(&reloc, packet + i, curr_sect)) 
                { free (packet); return FALSE; }
#endif

#if FILE_BASED  
	    if (n_reloc++ < sptr->s_nreloc                                    &&
	       (fseek(fin, sptr->s_relptr + ((int)n_reloc * relsz), 0) != 0  ||
		 !reloc_read(&reloc)))
#else
		if (n_reloc++ < sptr->s_nreloc && !reloc_read(&reloc, sptr->s_relptr + ((int)n_reloc * relsz)));
#endif		 
               { load_err = E_FILE; free (packet); return FALSE; }
	 }

      /*----------------------------------------------------------------------*/
      /* WRITE OUT THE RELOCATED DATA TO THE TARGET DEVICE.  IF THIS IS A     */
      /* CINIT SECTION, CALL A SPECIAL FUNCTION TO HANDLE IT.                 */
      /*----------------------------------------------------------------------*/
      if (!(IS_CINIT(sptr) ?
	      cload_cinit(packet, &packet_size, &excess) :
              (int) mem_write(packet, packet_size, addr + reloc_amount[curr_sect], 
						            sptr->s_page)))
         { load_err = E_MEMWRITE; free (packet); return FALSE; }

      /*----------------------------------------------------------------------*/
      /* KEEP TRACK OF THE ADDRESS WITHIN THE SECTION.                        */
      /*----------------------------------------------------------------------*/
      addr += BYTETOLOC(packet_size);

   }

   free (packet);
   return TRUE;
}


/******************************************************************************/
/*                                                                            */
/* CLOAD_CINIT() - Process one buffer of C initialization records.            */
/*                                                                            */
/******************************************************************************/
int cload_cinit(unsigned char *packet, int *packet_size, int *excess)
{
   int           i;                      /* BYTE COUNTER                      */
   int           init_packet_size;       /* SIZE OF CURRENT INITIALIZATION    */
   static T_ADDR init_addr;              /* ADDRESS OF CURRENT INITIALIZATION */
   int           bss_page = 0;           /* BSS SECTION PAGE NUMBER           */
 
   /*-------------------------------------------------------------------------*/
   /* FIND THE BSS SECTION ASSOCIATED WITH THE THE CINIT SECTION CURRENTLY    */
   /* BEING LOADED.                                                           */
   /*-------------------------------------------------------------------------*/
   for (i = 0; i < n_sections; ++i)
      if (IS_BSS(SECT_HDR(i))) 
          { bss_page = SECT_HDR(i)->s_page; break; }

   /*-------------------------------------------------------------------------*/
   /*  PROCESS ALL THE INITIALIZATION RECORDS IN THE BUFFER.                  */
   /*-------------------------------------------------------------------------*/
   for (i = 0; i < *packet_size; i += init_packet_size)
   {
      /*----------------------------------------------------------------------*/
      /* IF STARTING A NEW INITIALIZATION, READ THE SIZE AND ADDRESS FROM     */
      /* THE TABLE.                                                           */
      /*----------------------------------------------------------------------*/
      if (init_size == 0)
      {
	 T_SIZE temp;
	 int    align;

	 /*-------------------------------------------------------------------*/
	 /* POSITION THE BYTE INDEX ON THE NEXT INIT RECORD.                  */
	 /*-------------------------------------------------------------------*/
         if (align = (i % INIT_ALIGN)) i += (INIT_ALIGN - align);

	 /*-------------------------------------------------------------------*/
	 /* IF THE SIZE AND ADDRESS ARE NOT FULLY CONTAINED IN THIS BUFFER,   */
	 /* STOP HERE.  SET THE 'EXCESS' COUNTER TO THE NUMBER OF UNPROCESSED */
	 /* BYTES - THESE WILL BE COPIED TO THE HEAD OF THE NEXT BUFFER.      */
	 /*-------------------------------------------------------------------*/
	 if ((int)(i + sizeof(T_SIZE)) > *packet_size)
	    { *excess += *packet_size - i;  *packet_size = i;  break; }

	 /*-------------------------------------------------------------------*/
	 /* IF THE NEXT SIZE FIELD IS ZERO, BREAK.                            */
	 /*-------------------------------------------------------------------*/
	 temp = unpack(packet + i, sizeof(T_SIZE)*8, sizeof(T_SIZE)*8, 0);
	 if (temp == 0) break;

	 /*-------------------------------------------------------------------*/
	 /* READ THE ADDRESS FIELD ,IF IT'S ALL HERE.                         */
	 /*-------------------------------------------------------------------*/
         if ((int)(i + sizeof(T_SIZE) + sizeof(T_IADDR)) > *packet_size)
             { *excess += *packet_size - i;  *packet_size = i;  break; }

         i         += sizeof(T_SIZE);
         init_size  = temp;
         init_addr  = unpack(packet+i,sizeof(T_IADDR)*8,sizeof(T_IADDR)*8,0);
         i         += sizeof(T_IADDR);
      }

      /*----------------------------------------------------------------------*/
      /* WRITE OUT THE CURRENT PACKET, UP TO THE END OF THE BUFFER.           */
      /*----------------------------------------------------------------------*/
      if (init_packet_size = MIN(*packet_size-i, (int)(init_size * INIT_WSIZE)))
      {
	
	 if (!mem_write(packet + i, init_packet_size, init_addr, bss_page))
            return FALSE;

	 init_addr += BYTETOLOC(init_packet_size);
	 init_size -= init_packet_size / INIT_WSIZE;
      }
   }
   return TRUE;
}

#if 0
/******************************************************************************/
/*                                                                            */
/* CLOAD_SYMBOLS() - Read in the symbol table.                                */
/*                                                                            */
/******************************************************************************/
int cload_symbols()
{
   SYMENT sym;
   AUXENT aux;
   int first, next;

   if (file_hdr.f_nsyms == 0 || (!need_symbols && !need_reloc)) return TRUE;

   /*------------------------------------------------------------------------*/
   /* ALLOCATE THE RELOCATION SYMBOL TABLE.                                  */
   /*------------------------------------------------------------------------*/
   if (need_reloc)
   {
      reloc_sym_index = 0;
      reloc_tab_size  = MIN(RELOC_TAB_START, (int)file_hdr.f_nsyms);

      if (!(reloc_tab = (RELOC_TAB *)malloc(reloc_tab_size*sizeof(RELOC_TAB))))
          { load_err = E_ALLOC; return FALSE; }
   }

   /*------------------------------------------------------------------------*/
   /* IF THE APPLICATION NEEDS THE SYMBOL TABLE, LET IT READ IT IN.          */
   /* PASS NEED_RELOC TO THE APPLICATION SO THAT IT CAN CALL RELOC_ADD().    */
   /*------------------------------------------------------------------------*/
   if (need_symbols) 
   {
      if (load_syms(need_reloc)) return TRUE;
      else { load_err = E_SYM; return FALSE; }
   }

   /*------------------------------------------------------------------------*/
   /*  READ THE SYMBOL TABLE AND BUILD THE RELOCATION SYMBOL TABLE           */
   /*  FOR SYMBOLS THAT CAN BE USED IN RELCOATION, STORE THEM IN A           */
   /*  SPECIAL SYMBOL TABLE THAT CAN BE SEARCHED QUICKLY DURING              */
   /*  RELOCATION.                                                           */
   /*------------------------------------------------------------------------*/
   for (first = 0; first < file_hdr.f_nsyms; first = next)
   {
       if (!(next = sym_read(first, &sym, &aux))) 
          { load_err = E_FILE; return FALSE; }

       if (sym.n_sclass == C_EXT     || sym.n_sclass == C_EXTREF  ||
           sym.n_sclass == C_STAT    || sym.n_sclass == C_LABEL   ||
           sym.n_sclass == C_SYSTEM  || sym.n_sclass == C_BLOCK   || 
	   sym.n_sclass == C_FCN     || sym.n_sclass == C_STATLAB || 
	   sym.n_sclass == C_EXTLAB) 
	  if (!reloc_add(first, &sym)) return FALSE;
   }
   return TRUE;
}


/******************************************************************************/
/*                                                                            */
/* CLOAD_STRINGS() - Read in the string table.                                */
/*                                                                            */
/******************************************************************************/
int cload_strings()
{
   unsigned int str_size;              /* SIZE OF STRING TABLE              */
   unsigned int bufsize;               /* SIZE OF CURRENT BUFFER            */
   unsigned int ntoread;                /* AMOUNT TO READ FROM FILE          */
   int excess;                          /* AMOUNT LEFT OVER FROM LAST BUFFER */
   STRTAB *packet;                      /* STRING TABLE BUFFER PACKET        */
   unsigned int i = 0;

   /*----------------------------------------------------------------------*/
   /* Do not load if not loading/saving the symbol tables.                 */
   /* This is because the string table is an extension of the symbol table */
   /* and if you free and rebuild it, the symbol table's str_head will be  */
   /* incorrect.                                                           */
   /*----------------------------------------------------------------------*/
   if (!need_symbols) return TRUE;

   /*------------------------------------------------------------------------*/
   /* FREE ANY PREVIOUS STRING BUFFERS                                       */
   /*------------------------------------------------------------------------*/
   str_free(str_head); str_head = NULL;

   /*------------------------------------------------------------------------*/
   /* SEEK TO THE END OF THE SYMBOL TABLE AND READ IN THE SIZE OF THE STRING */
   /* TABLE.                                                                 */
   /*------------------------------------------------------------------------*/
#if FILE_BASED
   if ((file_hdr.f_nsyms == 0) ||
   	   fseek(fin, file_hdr.f_symptr + (file_hdr.f_nsyms * SYMESZ), 0) != 0 ||
       fread(&str_size, sizeof(int), 1, fin) != 1)
       return TRUE;
#else
   mem_copy(&str_size, &gRxBuffer[file_hdr.f_symptr + (file_hdr.f_nsyms * SYMESZ)], sizeof(int));	
   if ((file_hdr.f_nsyms == 0))
       return TRUE; 
#endif
   if (byte_swapped) swap4byte(&str_size);

   /*------------------------------------------------------------------------*/
   /* THE STRING TABLE IS READ IN AS A LINKED LIST OF BUFFERS.  TO           */
   /* PREVENT NAMES FROM BEING SPLIT ACROSS MULTIPLE BUFFERS, ANY PARTIAL    */
   /* NAME AT THE END OF A BUFFER IS COPIED INTO THE BEGINNING OF THE        */
   /* NEXT BUFFER.  THE VARIABLE 'EXCESS' KEEPS TRACK OF HOW MUCH NEEDS      */
   /* TO BE COPIED FROM THE PREVIOUS BUFFER.                                 */
   /*------------------------------------------------------------------------*/
   str_size -= 4;                       /* SUBTRACT OFF 4 BYTES ALREADY READ */
   excess    = 0;                       /* INITIALIZE LAST-BUFFER OVERFLOW   */


   /*------------------------------------------------------------------------*/
   /* READ STRING BUFFERS UNTIL THE WHOLE STRING TABLE IS READ.              */
   /*------------------------------------------------------------------------*/
   while (str_size)
   {
      /*---------------------------------------------------------------------*/
      /* ALLOCATE A NEW BUFFER.  ON 16-BIT MACHINES, RESTRICT THE            */
      /* BUFFER SIZE TO THE MAXIMUM THAT CAN BE ALLOCATED AT ONCE.           */
      /*---------------------------------------------------------------------*/
      bufsize = str_size + excess;

      if (sizeof(int) < 4  && bufsize > MAX_STRING_ALLOC)
	 bufsize = MAX_STRING_ALLOC;

      if (!(packet = (struct strtab *)malloc(sizeof(STRTAB) + 
                                             (unsigned int)bufsize - 1)))
         { load_err = E_ALLOC; return FALSE; }

      /*---------------------------------------------------------------------*/
      /* COPY ANY PARTIAL STRING FROM THE LAST BUFFER INTO THIS ONE.         */
      /* THEN FILL THE REST OF THE BUFFER BY READING FROM THE FILE.          */
      /*---------------------------------------------------------------------*/
      if (excess)
	 strn_copy(packet->buf, str_head->buf + str_head->size, excess);

      ntoread = (unsigned int)(bufsize - excess);
#if FILE_BASED
      if (fread(packet->buf + excess, ntoread, 1, fin) != 1) 
	 { load_err = E_FILE; return FALSE; }
#else
	mem_copy(packet->buf + excess, &gRxBuffer[file_hdr.f_symptr + (file_hdr.f_nsyms * SYMESZ) +sizeof(int)+ i++*ntoread], ntoread);
#endif	 
      str_size -= ntoread;

      /*---------------------------------------------------------------------*/
      /* IF THE BUFFER ENDS IN A PARTIAL STRING (DOESN'T END IN NULL),       */
      /* KEEP TRACK OF HOW MANY CHARACTERS ARE IN THE PARTIAL STRING         */
      /* SO THEY CAN BE COPIED INTO THE NEXT BUFFER.                         */
      /*---------------------------------------------------------------------*/
      for (excess = 0; packet->buf[bufsize - 1]; --bufsize, ++excess) ;

      /*---------------------------------------------------------------------*/
      /* LINK THE NEW BUFFER INTO THE HEAD OF THE LIST.                      */
      /*---------------------------------------------------------------------*/
      packet->size   = 
      packet->index  = bufsize;
      packet->next   = str_head;
      packet->offset = str_head ? (str_head->offset + str_head->size) : 4;
      str_head       = packet;
   }
   return TRUE;
}


/******************************************************************************/
/*                                                                            */
/* STR_FREE() - Free the list of string table buffers.                        */
/*                                                                            */
/******************************************************************************/
void str_free(STRTAB *head)
{
   STRTAB *this_one, *next;
   for (this_one = head; this_one; this_one = next)
   {
      next = this_one->next;
      free(this_one);
   }
}



/******************************************************************************/
/*                                                                            */
/* SYM_READ() - Read and relocate a symbol and its aux entry.  Return the     */
/*              index of the next symbol.                                     */
/*                                                                            */
/******************************************************************************/
int sym_read(int index, SYMENT *sym, AUXENT *aux)
{
    /*------------------------------------------------------------------------*/
    /* READ IN A SYMBOL AND ITS AUX ENTRY.                                    */
    /*------------------------------------------------------------------------*/
#if FILE_BASED
    if (fseek(fin, file_hdr.f_symptr + (index * SYMESZ), 0) != 0 ||
        fread(sym, SYMESZ, 1, fin) != 1                                ||
        (sym->n_numaux && fread(aux, SYMESZ, 1, fin) != 1)) 
      { load_err = E_FILE; return FALSE; }
#else
	mem_copy((void*)sym, (void*)&gRxBuffer[file_hdr.f_symptr + (index * SYMESZ)], SYMESZ); 
    if (sym->n_numaux) 
    { 
      	mem_copy((void*)aux, (void*)&gRxBuffer[file_hdr.f_symptr + ( (index+1) * SYMESZ)], SYMESZ);
      	load_err = E_FILE; 
      	return FALSE; 
    }
#endif
    if (byte_swapped)
    {
        /*--------------------------------------------------------------------*/
	/* SWAP THE SYMBOL TABLE ENTRY.                                       */
        /*--------------------------------------------------------------------*/
        if (sym->n_zeroes == 0) swap4byte(&sym->n_offset);
        swap4byte(&sym->n_value);
        swap2byte(&sym->n_scnum);
        swap2byte(&sym->n_type);

        /*--------------------------------------------------------------------*/
	/* SWAP THE AUX ENTRY, BASED ON THE STORAGE CLASS.                    */
        /*--------------------------------------------------------------------*/
        if (sym->n_numaux) switch(sym->n_sclass)
        {
          case C_FILE    : break;

          case C_STRTAG  :
          case C_UNTAG   :
          case C_ENTAG   : swap4byte(&aux->x_tag.x_fsize);
                           swap4byte(&aux->x_tag.x_endndx);
                           break;

          case C_FCN     : if (!str_comp(sym->n_name, ".bf"))
			   {
		               swap2byte(&aux->x_block.x_lcnt);
			       swap4byte(&aux->x_block.x_regmask); 
			       swap4byte(&aux->x_block.x_framesize);
			   }
			        
          case C_BLOCK   : swap2byte(&aux->x_block.x_lnno);
			   swap4byte(&aux->x_block.x_endndx);
                           break;

          case C_EOS     : swap4byte(&aux->x_eos.x_fsize);
                           swap4byte(&aux->x_eos.x_tagndx);
                           break;

          default        : /*-------------------------------------------------*/
                           /* HANDLE FUNCTION DEFINITION SYMBOL               */
                           /*-------------------------------------------------*/
                           if (((sym->n_type >> 4) & 3) == DCT_FCN)
                           {
                               swap4byte(&aux->x_func.x_tagndx);
                               swap4byte(&aux->x_func.x_fsize);
                               swap4byte(&aux->x_func.x_lnnoptr);
                               swap4byte(&aux->x_func.x_endndx);
                           }

                           /*-------------------------------------------------*/
                           /* HANDLE ARRAYS.                                  */
                           /*-------------------------------------------------*/
                           else if (((sym->n_type >> 4) & 3) == DCT_ARY)
                           {
                               swap4byte(&aux->x_array.x_tagndx);
                               swap4byte(&aux->x_array.x_fsize);
                               swap2byte(&aux->x_array.x_dimen[0]);
                               swap2byte(&aux->x_array.x_dimen[1]);
                               swap2byte(&aux->x_array.x_dimen[2]);
                               swap2byte(&aux->x_array.x_dimen[3]);
                           }

                           /*-------------------------------------------------*/
                           /* HANDLE SECTION DEFINITIONS                      */
                           /*-------------------------------------------------*/
                           else if (sym->n_type == 0)
                           {
                               swap4byte(&aux->x_scn.x_scnlen);
                               swap2byte(&aux->x_scn.x_nreloc);
                               swap2byte(&aux->x_scn.x_nlinno);
                           }

                           /*-------------------------------------------------*/
                           /* HANDLE MISC SYMBOL RECORD                       */
                           /*-------------------------------------------------*/
                           else
                           {
                               swap4byte(&aux->x_sym.x_fsize);
                               swap4byte(&aux->x_sym.x_tagndx);
                           }
        }
    }

    /*------------------------------------------------------------------------*/
    /* RELOCATE THE SYMBOL, BASED ON ITS STORAGE CLASS.                       */
    /*------------------------------------------------------------------------*/
    switch(sym->n_sclass)
    {
       case C_SYSTEM  :
       case C_EXT     :
       case C_EXTREF  :
       case C_STAT    :
       case C_LABEL   :
       case C_BLOCK   :
       case C_FCN     : 
       case C_STATLAB :
       case C_EXTLAB  :
	  /*------------------------------------------------------------------*/
	  /* IF THE SYMBOL IS UNDEFINED, CALL AN APPLICATION ROUTINE TO LOOK  */
	  /* IT UP IN AN EXTERNAL SYMBOL TABLE.  IF THE SYMBOL IS DEFINED,    */
	  /* RELOCATE IT ACCORDING TO THE SECTION IT IS DEFINED IN.           */
	  /*------------------------------------------------------------------*/
          if (sym->n_scnum == 0) 
	     lookup_sym((short)index, sym, aux);
          else if (sym->n_scnum > 0) 
	  {
	     if (sym->n_sclass == C_STATLAB || sym->n_sclass == C_EXTLAB)
	          sym->n_value += reloc_amount[sym->n_scnum - 1];
	     else sym->n_value += RUN_RELOC_AMOUNT(sym->n_scnum - 1);
          }
    }

    return (index + sym->n_numaux + 1);
}


/******************************************************************************/
/*                                                                            */
/* SYM_NAME() - Return a pointer to the name of a symbol in either the symbol */
/*              entry or the string table.                                    */
/*                                                                            */
/******************************************************************************/
char *sym_name(SYMENT *symptr)
{
    static char temp[9];

    if (symptr->n_zeroes == 0)
    {
       STRTAB *packet = str_head;

       /*---------------------------------------------------------------------*/
       /* Return the empty string if this symbol has no name (offset == 0)    */
       /*---------------------------------------------------------------------*/
       if (symptr->n_offset < 4) /* Anything below 4 isn't valid */
       {
          temp[0] = 0;
          return temp;
       }

       /*---------------------------------------------------------------------*/
       /* Otherwise, return the string in the string table.                   */
       /*---------------------------------------------------------------------*/
       while (packet && symptr->n_offset < (int)packet->offset)
          packet = packet->next;

       /*---------------------------------------------------------------------*/
       /* Return the empty string if packet NULL (invalid offset)             */
       /*---------------------------------------------------------------------*/
       if (!packet)
       {
          temp[0] = 0;
          return temp;
       }

       return packet->buf + (symptr->n_offset - packet->offset);
    }
    
    strn_copy(temp, symptr->n_name, 8);
    temp[8] = 0;
    return temp;
}


/******************************************************************************/
/*                                                                            */
/* SYM_ADD_NAME() - Given a symbol table entry, return a pointer to the       */
/*                  symbol's name in the string table.  Add the name to the   */
/*                  table if it's not already there.                          */
/*                                                                            */
/******************************************************************************/
char *sym_add_name(SYMENT *symptr)
{
    char *dest;
    char *result;
    int i;

    /*------------------------------------------------------------------------*/
    /* IF THE SYMBOL'S NAME WAS IN THE COFF STRING TABLE, LOOK THROUGH THE    */
    /* LIST OF STRING TABLE BUFFERS UNTIL FINDING THE ONE THE NAME IS IN,     */
    /* AND SIMPLY POINT INTO THE BUFFER.                                      */
    /*------------------------------------------------------------------------*/
    if (symptr->n_zeroes == 0)
       return sym_name(symptr);

    /*------------------------------------------------------------------------*/
    /* OTHERWISE ADD THE STRING TO THE STRING TABLE.                          */
    /* ALLOCATE AND LINK IN A NEW PACKET IF NECESSARY.  NEW PACKETS ARE       */
    /* LINKED TO THE HEAD OF THE LIST TO EASE ADDING NEW SYMBOLS.             */
    /*------------------------------------------------------------------------*/
    if (!str_head || str_head->index + SYMNMLEN + 1 > str_head->size)
    {
       STRTAB *packet;

       if (!(packet = (STRTAB *)malloc(sizeof(STRTAB) + MIN_STRING_ALLOC - 1)))
          { load_err = E_ALLOC; return NULL; }

       packet->size   = MIN_STRING_ALLOC;
       packet->index  = 0;
       packet->next   = str_head;
       packet->offset = str_head ? (str_head->offset + str_head->size) : 4;
       str_head       = packet;
    }

    /*------------------------------------------------------------------------*/
    /* COPY THE NAME INTO THE STRING TABLE.                                   */
    /*------------------------------------------------------------------------*/
    result = dest = str_head->buf + str_head->index; 
    for (i = 0; *dest++ = symptr->n_name[i++]; )
       if (i == SYMNMLEN) { *dest++ = '\0'; ++i; break; }

    symptr->n_zeroes = 0;
    symptr->n_offset = str_head->offset + str_head->index;
    str_head->index += i;
    return result;
}
#endif

#if 0
/******************************************************************************/
/*                                                                            */
/* RELOC_ADD() - Add an entry to the relocation symbol table.  This table     */
/*               stores relocation information for each relocatable symbol.   */
/*                                                                            */
/******************************************************************************/
int reloc_add(int index, SYMENT *sym)
{
   int disp;                            /* RELOCATION AMOUNT FOR THIS SYMBOL */

   if (!need_reloc) return TRUE;

   /*-------------------------------------------------------------------------*/
   /* USE THE SYMBOL VALUE TO CALCULATE THE RELOCATION AMOUNT:                */
   /* 1) IF THE SYMBOL WAS UNDEFINED (DEFINED IN SECTION 0), USE THE          */
   /*    SYMBOL'S VALUE.                                                      */
   /* 2) IF THE SYMBOL HAS A POSITIVE SECTION NUMBER, USE THE RELOCATION      */
   /*    AMOUNT FOR THE SECTION IN WHICH THE SYMBOL IS DEFINED.               */
   /* 3) OTHERWISE, THE SYMBOL IS ABSOLUTE, SO THE RELOCATION AMOUNT IS 0.    */
   /*-------------------------------------------------------------------------*/
   if (sym->n_scnum == 0)
      disp = sym->n_value;
   else if (sym->n_scnum > 0)
   {
      if (sym->n_sclass == C_STATLAB || sym->n_sclass == C_EXTLAB)
	   disp = reloc_amount[sym->n_scnum - 1];
      else disp = RUN_RELOC_AMOUNT(sym->n_scnum - 1);
   }
   else disp = 0;

   /*-------------------------------------------------------------------------*/
   /* IF THERE IS A NON-ZERO RELOCATION AMOUNT, ADD THE SYMBOL TO THE TABLE.  */
   /*-------------------------------------------------------------------------*/
   if (disp == 0) return TRUE;

   if (reloc_sym_index >= reloc_tab_size)
   {
      reloc_tab_size += RELOC_GROW_SIZE;
      reloc_tab = (RELOC_TAB *)realloc((char *)reloc_tab, 
                                       reloc_tab_size*sizeof(RELOC_TAB));

      if (!reloc_tab) { load_err = E_ALLOC; return FALSE; }
   }
   reloc_tab[reloc_sym_index  ].rt_index = index;
   reloc_tab[reloc_sym_index++].rt_disp  = disp;

   return TRUE;
}



/******************************************************************************/
/*                                                                            */
/* RELOCATE() - Perform a single relocation by patching the raw data.         */
/*                                                                            */
/******************************************************************************/
int relocate(RELOC *rp, unsigned char *data, int s)
{
/*   RELOC         *rp;                    RELOCATION ENTRY                   */
/*   unsigned char *data;                  DATA BUFFER                        */
/*   int            s;                     INDEX OF CURRENT SECTION           */
   int fieldsz = reloc_size(rp->r_type);     /* SIZE OF ACTUAL PATCH VALUE    */
   int offset  = reloc_offset(rp->r_type);   /* OFFSET OF ACTUAL PATCH VALUE  */
   int wordsz  = MAX(fieldsz, reloc_stop(rp->r_type)); /* SIZE CONTAINING FLD */
   int objval;                              /* FIELD TO BE PATCHED           */
   int reloc_amt;                           /* AMOUNT OF RELOCATION          */

   int pp_shift_cnt = 0;
   int pp_local     = FALSE;

   if (rp->r_type == R_ABS) return TRUE;          /* NOTHING TO DO   */

   /*-------------------------------------------------------------------------*/
   /* DETERMINE THE RELOCATION AMOUNT FROM THE RELOCATION SYMBOL TABLE.       */
   /*-------------------------------------------------------------------------*/
   reloc_amt = (rp->r_symndx == -1) ? RUN_RELOC_AMOUNT(s) 
				    : sym_reloc_amount(rp);

   /*-------------------------------------------------------------------------*/
   /* EXTRACT THE RELOCATABLE FIELD FROM THE OBJECT DATA.                     */
   /*-------------------------------------------------------------------------*/
   objval = unpack(data, fieldsz, wordsz, offset + BIT_OFFSET(rp->r_vaddr));

   /*-------------------------------------------------------------------------*/
   /* MODIFY THE FIELD BASED ON THE RELOCATION TYPE.                          */
   /*-------------------------------------------------------------------------*/
   switch (rp->r_type)
   {
      /*----------------------------------------------------------------------*/
      /* NORMAL RELOCATIONS: ADD IN THE RELOCATION AMOUNT.                    */
      /*----------------------------------------------------------------------*/
      case R_RELBYTE:
      case R_RELWORD:
      case R_REL24:
      case R_RELLONG:
      case R_DIR32:
      case R_PARTLS16:
	 objval += reloc_amt;
	 break;

      /*--------------------------------------------------------------------*/
      /* ADD IN THE RELOCATION AMOUNT, BUT MAKE SURE WE'RE STILL IN THE     */
      /* 370'S REGISTER FILE.                                               */
      /*--------------------------------------------------------------------*/
      case R_RRNREG:
      case R_RRRELREG:
	 if (rp->r_type == R_RRNREG)
	    objval = ((char)objval + reloc_amt);
	 else objval += reloc_amt;
 
         if (objval & ((-1L >> 8*fieldsz) << 8*fieldsz))
         {
	    /* ERROR */
         }
 
         break;
 
      /*--------------------------------------------------------------------*/
      /* PP UNSCALED 15-BIT OFFSET RELOCATION.                              */
      /*--------------------------------------------------------------------*/
      case R_PP15    :
      case R_PPL15   :
      case R_PPN15   :
      case R_PPLN15  :
	 {
	    int  bit;
	    char *sname = (SECT_HDR(s)->s_zeroes == 0L) ?
			  SECT_HDR(s)->s_nptr : SNAMECPY(SECT_HDR(s)->s_name);

	    pp_local = (rp->r_type == R_PPL15) || (rp->r_type == R_PPLN15);

	    /*--------------------------------------------------------------*/
	    /* IF NEGATIVE RELOC TYPE, THEN TREAT CONST OFFSET AS A NEGATIVE*/
	    /*--------------------------------------------------------------*/
	    if (rp->r_type == R_PPN15 || rp->r_type == R_PPLN15)
	    {
		objval      = -objval;
                rp->r_type -= 010;           /* CHANGE TYPE TO NON NEG.     */
            }

	    objval += reloc_amt;

	    /*--------------------------------------------------------------*/
            /* IF THE ADDRESS STILL FALLS WITHIN AN APPROPRIATE RANGE       */
	    /*--------------------------------------------------------------*/
	    if ((objval >= 0x00000000 && objval <= 0x00007fff) || 
	        (objval >= 0x01000000 && objval <= 0x010007ff) )
	        break;

	    /*--------------------------------------------------------------*/
            /* IF THE ADDRESS FALLS OUTSIDE AN APPROPRIATE RANGE, BUT CAN   */
	    /* BE SCALED BY SIZE TO GET BACK INTO RANGE, THEN READ THE UPPER*/
	    /* BIT OF THE SIZE FIELD.  IF IT IS A 1, THEN WE CAN SCALE THIS */
	    /* OFFSET BY 4, IF IT IS 0, THEN WE CAN SCALE THIS OFFSET BY 2. */ 
	    /*--------------------------------------------------------------*/
	    bit = unpack(data, 1, 64, pp_local ? 30 : 8);
 
	    /*--------------------------------------------------------------*/
	    /* DETERMINE IF THE OFFSET IS ALIGNED FOR SCALING.  IF SO,      */
	    /* THEN PACK THE SCALED OFFSET INTO INSTRUCTION, CHANGE THE     */
	    /* RELOC TYPE TO SCALED, AND TURN ON SCALE BIT IN INSTRUCT.     */
	    /*--------------------------------------------------------------*/
	    if (!(objval & ((2<<bit)-1)) && 
		 (objval >>= (bit+1)) >= 0 && objval <= 0x7fff)
	    {
		rp->r_type = pp_local ? (bit ? R_PPL15W : R_PPL15H) : 
					(bit ? R_PP15W : R_PP15H);
		repack(1, data, 1, 64, pp_local ? 28 : 6);
	        break;
	    }
	    
	    /*--------------------------------------------------------------*/
	    /* ERROR, THE OFFSET WILL NOT FIT SCALED OR UNSCALED.           */
	    /*--------------------------------------------------------------*/
	    
	    load_err = E_RELOCENT;
	    return FALSE;
         }

      /*--------------------------------------------------------------------*/
      /* PP SCALED 15-BIT OFFSET RELOCATION. FOR R_PP15W THE RELOC_AMT IS   */
      /* DIVIDED BY 4.  FOR R_PP15H THE RELOC_AMT IS DIVIDED BY 2.          */ 
      /*--------------------------------------------------------------------*/
      case R_PP15W   :
      case R_PPL15W  :
      case R_PPN15W  :
      case R_PPLN15W :	pp_shift_cnt++;   /* FALL THROUGH */

      case R_PP15H   :
      case R_PPL15H  :
      case R_PPN15H  :
      case R_PPLN15H :  pp_shift_cnt++;   /* FALL THROUGH */
	 {
	    int obj_addr_x;
	    char *sname = (SECT_HDR(s)->s_zeroes == 0) ? 
			  SECT_HDR(s)->s_nptr : SNAMECPY(SECT_HDR(s)->s_name);

	    /*--------------------------------------------------------------*/
	    /* NOTE THAT THIS IS DEPENDENT ON THE NUMBERING OF THESE RELOC  */
	    /* VALUES.                                                      */
	    /*--------------------------------------------------------------*/
	    pp_local = (rp->r_type & 4);

	    /*--------------------------------------------------------------*/
	    /* IF NEGATIVE RELOC TYPE, THEN TREAT CONST OFFSET AS NEGATIVE  */
	    /*--------------------------------------------------------------*/
	    if (rp->r_type >= R_PPN15) 
	    {
		objval      = -objval;
                rp->r_type -= 010;           /* CHANGE TYPE TO NON NEG.     */
            }

	    obj_addr_x = (objval << pp_shift_cnt) + reloc_amt;

	    /*--------------------------------------------------------------*/
            /* LINK TIME ADDRESS VIOLATES THE SCALING FACTOR WE ARE USING   */
	    /* FOR THIS OPERAND. UNSCALE THE VALUE IF IT WILL FIT IN 15 BITS*/
	    /* BY CHANGING RELOC TYPE TO UNSCALED, AND CHANGING SCALE BIT   */
	    /* IN THE INSTRUCTION.                                          */
	    /*--------------------------------------------------------------*/
	    if (pp_shift_cnt && (reloc_amt & ((1<<pp_shift_cnt)-1)))
	    {
		objval     = obj_addr_x;
		rp->r_type = (pp_local ? R_PPL15 : R_PP15);
	        repack(0, data, 1, 64, pp_local ? 28 : 6);
	    }
	    else objval = obj_addr_x >> pp_shift_cnt;

	    /*--------------------------------------------------------------*/
            /* IF THE ADDRESS STILL FALLS WITHIN AN APPROPRIATE RANGE       */
	    /*--------------------------------------------------------------*/
	    if ((objval     >= 0x00000000  && objval     <= 0x00007fff) || 
	        (obj_addr_x >= 0x01000000  && obj_addr_x <= 0x010007ff) )
	        break;

	    /*--------------------------------------------------------------*/
	    /* ERROR, THE OFFSET WILL NOT FIT SCALED OR UNSCALED.           */
	    /*--------------------------------------------------------------*/
	    
	    load_err = E_RELOCENT;
	    return FALSE;
         }

      /*--------------------------------------------------------------------*/
      /* PP 16-bit byte offset relocation. For R_PP16B the lower 15-bits    */
      /* are handled just like R_PP15, and the upper bit is placed in the   */
      /* scale indicator bit of the field.                                  */
      /*--------------------------------------------------------------------*/
      case R_PP16B   :
      case R_PPL16B  :
      case R_PPN16B  :
      case R_PPLN16B :
	 {
	    char *sname = (SECT_HDR(s)->s_zeroes == 0) ? 
		          SECT_HDR(s)->s_nptr : SNAMECPY(SECT_HDR(s)->s_name);

	    pp_local = (rp->r_type == R_PPL16B) || (rp->r_type == R_PPLN16B);

            /*--------------------------------------------------------------*/
	    /* READ THE SCALE BIT (16th BIT) AND CREATE 16 BIT CONSTANT OFF */
            /*--------------------------------------------------------------*/
	    objval |= (unpack(data, 1, 64, pp_local ? 28 : 6) << 15);

	    /*--------------------------------------------------------------*/
	    /* IF NEGATIVE RELOC TYPE, THEN TREAT CONST OFFSET AS NEGATIVE  */
	    /*--------------------------------------------------------------*/
	    if (rp->r_type == R_PPN16B || rp->r_type == R_PPLN16B)
	    {
	       objval      = - objval;
               rp->r_type -= 010; /* CHANGE THE TYPE TO A NON NEG TYPE.     */
            }

	    objval += reloc_amt;

	    /*--------------------------------------------------------------*/
            /* IF THE ADDRESS STILL FALLS WITHIN AN APPROPRIATE RANGE       */
	    /*--------------------------------------------------------------*/
	    if ((objval >= 0x00000000 && objval <= 0x0000ffff) || 
		(objval >= 0x01000000 && objval <= 0x010007ff) )
	    {
	       /*-----------------------------------------------------------*/
	       /* RELOCATE THE 16TH BIT OF THE ADDRESS.                     */
	       /*-----------------------------------------------------------*/
	       repack(((objval&0x8000) >> 15), data, 1, 64, pp_local ? 28 : 6);
	       break;
            }

	    /*--------------------------------------------------------------*/
            /* ADDRESS IS OUT OF RANGE.                                     */
	    /*--------------------------------------------------------------*/
	    
	    load_err = E_RELOCENT;
	    return FALSE;
         }

      /*--------------------------------------------------------------------*/
      /* PP BASE ADDRESS RELOCATION.  BIT 0 IS 0 IF IN DATA RAM, 1 IF IN    */
      /* PARAMETER RAM.  THIS CODE ASSUMES THAT WE DO NOT RELOCATE FROM     */
      /* PRAM TO DRAM OR FROM DRAM TO PRAM AT LOAD TIME.                    */
      /*--------------------------------------------------------------------*/
      case R_PPLBASE: pp_local = TRUE;
      case R_PPBASE:
         {
	   /*---------------------------------------------------------------*/
	   /* IF WAS DRAM AND RELOC_AMT IS GREAT ENOUGH TO MOVE INTO PRAM,  */
	   /* CHANGE TO PRAM                                                */
	   /*---------------------------------------------------------------*/
	   if (!objval && reloc_amt > (int)(0x01000000 - 0xC000)) objval = 1;

	   /*---------------------------------------------------------------*/
	   /* IF WAS PRAM AND RELOC_AMT IS NEGATIVE AND CAN MOVE INTO DRAM, */
	   /* CHANGE TO DRAM                                                */
	   /*---------------------------------------------------------------*/
	   else if (objval && (-reloc_amt) > (int)(0x01000000 - 0xC000))
	       objval = 0;

	   break;
         }

      /*----------------------------------------------------------------------*/
      /* 34010 ONE'S COMPLEMENT RELOCATION.  SUBTRACT INSTEAD OF ADD.         */
      /*----------------------------------------------------------------------*/
      case R_OCRLONG:
	 objval -= reloc_amt;
	 break;

      /*----------------------------------------------------------------------*/
      /* 34020 WORD-SWAPPED RELOCATION.  SWAP BEFORE RELOCATING.              */
      /*----------------------------------------------------------------------*/
      case R_GSPOPR32:
      case R_OCBD32:
          objval  = ((objval >> 16) & 0xFFFF) | (objval << 16); 
          objval += (rp->r_type == R_GSPOPR32) ? reloc_amt : -reloc_amt; 
          objval  = ((objval >> 16) & 0xFFFF) | (objval << 16);
          break; 

      /*----------------------------------------------------------------------*/
      /* PC-RELATIVE RELOCATIONS.  IN THIS CASE THE RELOCATION AMOUNT         */
      /* IS ADJUSTED BY THE PC DIFFERENCE.   IF THIS IS AN INTERNAL           */
      /* RELOCATION TO THE CURRENT SECTION, NO ADJUSTMENT IS NEEDED.          */
      /*----------------------------------------------------------------------*/
      case R_PCRBYTE:
      case R_PCRWORD:
      case R_GSPPCR16:
      case R_GSPPCA16:
      case R_PCRLONG:
      case R_PCR24:
      case R_ANKPCR16:
      case R_ANKPCR8:
         {
            int           shift = 8 * (4 - fieldsz);
	    unsigned int pcdif = RUN_RELOC_AMOUNT(s);

	    /*----------------------------------------------------------------*/
	    /* HANDLE SPECIAL CASES OF JUMPING FROM ABSOLUTE SECTIONS (SPECIAL*/
	    /* RELOC TYPE) OR TO ABSOLUTE DESTINATION (SYMNDX == -1).  IN     */
	    /* EITHER CASE, SET THE APPROPRIATE RELOCATION AMOUNT TO 0.       */
	    /*----------------------------------------------------------------*/
	    if( rp->r_symndx == -1 )      reloc_amt = 0;
	    if( rp->r_type == R_GSPPCA16) pcdif = 0;

            /*----------------------------------------------------------------*/
            /* Handle ankoor's offset where upper 14 (PCR8) and upper 6(PCR16)*/
            /* bits of 22 bit address is held in r_disp field of reloc entry  */
            /*----------------------------------------------------------------*/
            if(rp->r_type == R_ANKPCR8 || rp->r_type == R_ANKPCR16)
            {
                shift = 10;
                objval |= rp->r_disp << ((rp->r_type == R_ANKPCR8) ? 8 : 16);
            }

	    reloc_amt -= pcdif;

            if (rp->r_type == R_GSPPCR16 || rp->r_type == R_GSPPCA16)
	       reloc_amt >>= 4;                              /* BITS TO WORDS */

	    objval  = (int)(objval << shift) >> shift;      /* SIGN EXTEND   */
	    objval += reloc_amt;
            break;
         }

      /*--------------------------------------------------------------------*/
      /* Ankoor page-addressing.  Calculate address from the 22-bit         */
      /* value in the relocation field plus the relocation amount. Shift    */
      /* out the lower 16 bits.                                             */
      /*--------------------------------------------------------------------*/
      case R_PARTMS6:
          objval = (objval & 0xC0) |
                   (((rp->r_disp += reloc_amt) >> 16) & 0x3F);
          break;

      /*----------------------------------------------------------------------*/
      /* 320C30 PAGE-ADDRESSING RELOCATION.  CALCULATE THE ADDRESS FROM       */
      /* THE 8-BIT PAGE VALUE IN THE FIELD, THE 16-BIT OFFSET IN THE RELOC    */
      /* ENTRY, AND THE RELOCATION AMOUNT.  THEN, STORE THE 8-BIT PAGE        */
      /* VALUE OF THE RESULT BACK IN THE FIELD.                               */
      /*----------------------------------------------------------------------*/
      case R_PARTMS8:
	  objval = (int)((objval << 16) + rp->r_disp + reloc_amt) >> 16;
	  break;

      /*----------------------------------------------------------------------*/
      /* DSP(320) PAGE-ADDRESSING.  CALCULATE ADDRESS FROM THE 16-BIT         */
      /* VALUE IN THE RELOCATION FIELD PLUS THE RELOCATION AMOUNT.  OR THE    */
      /* TOP 9 BITS OF THIS RESULT INTO THE RELOCATION FIELD.                 */
      /*----------------------------------------------------------------------*/
      case R_PARTMS9:
	  objval = (objval & 0xFE00) | 
		   (((int)(rp->r_disp + reloc_amt) >> 7) & 0x1FF);
          break;

      /*----------------------------------------------------------------------*/
      /* DSP(320) PAGE-ADDRESSING.  CALCULATE ADDRESS AS ABOVE, AND OR THE    */
      /* 7-BIT DISPLACEMENT INTO THE FIELD.                                   */
      /*----------------------------------------------------------------------*/
      case R_PARTLS7:
	  objval = (objval & 0x80) | ((rp->r_disp + reloc_amt) & 0x7F);
	  break;

      /*--------------------------------------------------------------------*/
      /* Ankoor page-addressing.  Calculate address from the 22-bit         */
      /* value in the relocation field plus the relocation amount. Mask off */
      /* bits [21:16] and [5:0] to extract a data page within a 64K page    */
      /*--------------------------------------------------------------------*/
      case R_PARTMID10:
          objval = (objval & 0xFC00) |
                   (((rp->r_disp += reloc_amt) >> 6) & 0x3FF);
          break;

      /*--------------------------------------------------------------------*/
      /* RR(370) MSB RELOCATION.  CALCULATE ADDRESS FROM THE 16-BIT VALUE   */
      /* IN THE RELOCATION ENTRY PLUS THE RELOCATION AMOUNT.  PATCH THE     */
      /* MSB OF THE RESULT INTO THE RELOCATION FIELD.                       */
      /*--------------------------------------------------------------------*/
      case R_HIWORD:
	  objval += (rp->r_disp += (unsigned short)reloc_amt) >> 8;
	  break;

      /*--------------------------------------------------------------------*/
      /* C8+ High byte of 24-bit address.  Calculate address from 24-bit    */
      /* value in the relocation entry plus the relocation amount.  Patch   */
      /* the MSB of the result into the relocation field.                   */
      /*--------------------------------------------------------------------*/
      case R_C8PHIBYTE:
          objval = (int)((objval << 16) + rp->r_disp + reloc_amt) >> 16;
          break;

      /*--------------------------------------------------------------------*/
      /* C8+ Middle byte of 24-bit address.  Calculate address from 24-bit  */
      /* value in the relocation entry plus the relocation amount.  Patch   */
      /* the middle byte of the result into the relocation field.           */
      /*--------------------------------------------------------------------*/
      case R_C8PMIDBYTE:
          objval = (int)((objval << 16) + rp->r_disp + reloc_amt) >> 8;
          break;

      /*--------------------------------------------------------------------*/
      /* C8+ Vector Address.  Calculate address from 24-bit value in the    */
      /* relocation entry plus the relocation amount.  MSB must be 0xFF     */
      /* since interrupt and trap handlers must be programmed in the top-   */
      /* most segment of memory.  Patch bottom 16-bits of the result into   */
      /* the relocation field.                                              */
      /*--------------------------------------------------------------------*/
      case R_C8PVECADR:
          objval += reloc_amt;
          if ((objval & 0xFF0000) != 0xFF0000)
          {
	     /* ERROR */
          }
          objval &= 0xFFFF;
          break;

      /*----------------------------------------------------------------------*/
      /* C8+ 24-bit Address.  The byte ordering for 24-bit addresses on the   */
      /* C8+ is reversed (low, middle, high).  Needs to be unordered, add     */
      /* in reloc_amt, then re-ordered.                                       */
      /*----------------------------------------------------------------------*/
      case R_C8PADR24:
          objval = ((objval>>16) | (objval&0xff00) | ((objval&0xff)<<16));
          objval += reloc_amt;
          objval = ((objval>>16) | (objval&0xff00) | ((objval&0xff)<<16));
          break;

      /*----------------------------------------------------------------------*/
      /* DSP(320) 13-BIT CONSTANT.  RELOCATE ONLY THE LOWER 13 BITS OF THE    */
      /* FIELD.                                                               */
      /*----------------------------------------------------------------------*/
      case R_REL13:
          objval = (objval & 0xE000) | ((objval + reloc_amt) & 0x1FFF);
          break;

      /*--------------------------------------------------------------------*/
      /* ANKOOR 22-bit extended address.  Calculate address by masking      */
      /* off the opcode and OR in the lower 16-bit constant + the           */
      /* amount.                                                            */
      /*--------------------------------------------------------------------*/
      case R_REL22:
         {
            unsigned int obj_hi = unpack(data  , 16, 16, 0);
            unsigned int obj_lo = unpack(data+2, 16, 16, 0);

            /*--------------------------------------------------------------*/
            /* BUILD THE OFFSET TO BE RELOCATED.                            */
            /*--------------------------------------------------------------*/
            objval = ((obj_hi & 0x003F) << 16) | (obj_lo & 0xFFFF);

            /*--------------------------------------------------------------*/
            /* Calculate displacement                                       */
            /*--------------------------------------------------------------*/
            objval += reloc_amt;

            /*--------------------------------------------------------------*/
            /* REPACK THE RELOCATED VALUE BACK IN THE OBJECT VALUE.         */
            /*--------------------------------------------------------------*/
            obj_hi = (obj_hi & 0xFFC0) | ((objval & 0x3F0000) >> 16);
            obj_lo = objval & 0xFFFF;
            repack(obj_hi, data  , 16, 16, 0);
            repack(obj_lo, data+2, 16, 16, 0);

            return TRUE;
         }

      /*--------------------------------------------------------------------*/
      /* DSP, LEAD 23-bit extended address.  Calculate address by masking   */
      /* off the opcode and OR in the lower 16-bit constant + the           */
      /* amount.                                                            */
      /*--------------------------------------------------------------------*/
      case R_REL23:
         {
            unsigned int obj_hi = unpack(data  , 16, 16, 0);
            unsigned int obj_lo = unpack(data+2, 16, 16, 0);

            /*--------------------------------------------------------------*/
            /* BUILD THE OFFSET TO BE RELOCATED.                            */
            /*--------------------------------------------------------------*/
            objval = ((obj_hi & 0x007F) << 16) | (obj_lo & 0xFFFF);

            /*--------------------------------------------------------------*/
            /* Calculate displacement                                       */
            /*--------------------------------------------------------------*/
            objval += reloc_amt;

            /*--------------------------------------------------------------*/
            /* REPACK THE RELOCATED VALUE BACK IN THE OBJECT VALUE.         */
            /*--------------------------------------------------------------*/
            obj_hi = (obj_hi & 0xFF80) | ((objval & 0x7F0000) >> 16);
            obj_lo = objval & 0xFFFF;
            repack(obj_hi, data  , 16, 16, 0);
            repack(obj_lo, data+2, 16, 16, 0);

            return TRUE;
         }


      /*----------------------------------------------------------------------*/
      /* PRISM (370/16) code label relocation.  Convert word address to byte  */
      /* address, add in relocation, convert back to word address.            */
      /*----------------------------------------------------------------------*/
      case R_LABCOD:
	  objval = ((objval << 1) + reloc_amt) >> 1;
	  break;
   }

   /*-------------------------------------------------------------------------*/
   /* PACK THE RELOCATED FIELD BACK INTO THE OBJECT DATA.                     */
   /*-------------------------------------------------------------------------*/
   repack(objval, data, fieldsz, wordsz, offset + BIT_OFFSET(rp->r_vaddr));
   return TRUE;
} 


/******************************************************************************/
/*                                                                            */
/* RELOC_READ() - Read a single relocation entry.                             */
/*                                                                            */
/******************************************************************************/
#if FILE_BASED
int reloc_read(RELOC *rptr)
#else
int reloc_read(RELOC *rptr, unsigned int offset)
#endif
{
#if COFF_VERSION_1 || COFF_VERSION_2
   /*------------------------------------------------------------------------*/
   /* THE FOLLOWING UNION IS USED TO READ IN VERSION 0 OR 1 RELOC ENTRIES    */
   /*------------------------------------------------------------------------*/
   /* THE FORMAT OF RELOCATION ENTRIES CHANGED BETWEEN COFF VERSIONS 0 AND 1.*/
   /* VERSION 0 HAS A 16 BIT SYMBOL INDEX, WHILE VERSION 1 HAS A 32 BIT INDX.*/
   /*------------------------------------------------------------------------*/
   union { RELOC new_c; RELOC_OLD old; } input_entry;
#if FILE_BASED
   if (fread(&input_entry, RELSZ_IN(coff_version), 1, fin) != 1)
      { load_err = E_FILE; return FALSE; }
#else
 	mem_copy((void*)&input_entry, (void*)&gRxBuffer[offset], RELSZ_IN(coff_version));
#endif
   /*------------------------------------------------------------------------*/
   /* IF LOADING A VERSION 0 FILE, TRANSLATE ENTRY TO VERSION 1 FORMAT.      */
   /* (THIS COULD BE SIMPLER - ALL THE SWAPS EXCEPT FOR THE SYMBOL INDEX     */
   /* COULD BE DONE AFTER THE TRANSLATION - BUT THIS SEEMS TO BE CLEARER)    */
   /*------------------------------------------------------------------------*/
   if (ISCOFF_0(coff_version))
   {
      if (byte_swapped)
      {
         swap4byte(&input_entry.old.r_vaddr);
	 swap2byte(&input_entry.old.r_type);

	 /* if a field reloc, fields are chars, so don't swap */
	 if (!isfldrel(input_entry.old.r_type))
	 {
	     swap2byte(&input_entry.old.r_symndx);
	     swap2byte(&input_entry.old.r_disp);
	 }
      }

      rptr->r_vaddr  = input_entry.old.r_vaddr;
      rptr->r_symndx = input_entry.old.r_symndx;
      rptr->r_disp   = input_entry.old.r_disp;
      rptr->r_type   = input_entry.old.r_type;
   }
   else
   {
      *rptr = input_entry.new_c;

      if (byte_swapped)
      {
         swap4byte(&rptr->r_vaddr); 
	 swap2byte(&rptr->r_type);

	 /* Again, if a field reloc, fields are chars, so don't swap */
	 if (!isfldrel(rptr->r_type))
	 {
	     swap4byte(&rptr->r_symndx);
	     swap2byte(&rptr->r_disp);  
	 }
      }
   }

#else
   /*-------------------------------------------------------------------------*/
   /* READ IN AND BYTE SWAP AN VERSION 0 RELOC ENTRY                          */
   /*-------------------------------------------------------------------------*/
   if (fread(rptr, RELSZ, 1, fin) != 1) { load_err = E_FILE; return FALSE; }

   if (byte_swapped)
   {
      swap4byte(&rptr->r_vaddr);
      swap2byte(&rptr->r_type);  

      /* If a field reloc, fields are chars, so don't swap */
      if (!isfldrel(rptr->r_type))
      {
	  swap2byte(&rptr->r_symndx);
	  swap2byte(&rptr->r_disp);
      }
   }
#endif

   return TRUE;
}


#endif
/*************************************************************************/
/*                                                                       */
/*   RELOC_SIZE()-                                                       */
/*      Return the field size of a relocation type.                      */
/*                                                                       */
/*************************************************************************/

int reloc_size(int type)
{
   switch (type)
   {
       case R_PPBASE:
       case R_PPLBASE:      return 1;

       case R_HIWORD:
       case R_C8PHIBYTE:
       case R_C8PMIDBYTE:
       case R_RELBYTE:
       case R_PCRBYTE:
       case R_RRNREG:
       case R_RRRELREG:
       case R_ANKPCR8:
       case R_PARTLS6:
       case R_PARTMS6:
       case R_PARTLS7:      return 8;

       case R_PP15:
       case R_PP15W:
       case R_PP15H:
       case R_PP16B:
       case R_PPN15:
       case R_PPN15W:
       case R_PPN15H:
       case R_PPN16B:
       case R_PPL15:
       case R_PPL15W:
       case R_PPL15H:
       case R_PPL16B:
       case R_PPLN15:
       case R_PPLN15W:
       case R_PPLN15H:
       case R_PPLN16B:      return 15;

       case R_LABCOD:
       case R_RELWORD:
       case R_PCRWORD:
       case R_ANKPCR16:
       case R_GSPPCR16:
       case R_GSPPCA16:
       case R_PARTLS16:
       case R_PARTMS8:
       case R_PARTMS9:
       case R_PARTMID10:
       case R_PARTMS16:
       case R_REL22:
       case R_REL13:        
       case R_C8PVECADR:    return 16;

       case R_REL24:
       case R_PCR24:
       case R_PCR24W:
      case R_C8PADR24:      return 24;

       case R_MPPCR:
       case R_GSPOPR32:
       case R_RELLONG:
       case R_PCRLONG:
       case R_OCBD32:
       case R_OCRLONG:
       case R_DIR32:        return 32;

       default:             return 0;
   }
}


/*************************************************************************/
/*                                                                       */
/*   RELOC_OFFSET()-                                                     */
/*      Return the offset of a relocation type field.  The value of      */
/*      offset should be the bit offset of the LSB of the field in       */
/*      little-endian mode.                                              */
/*                                                                       */
/*************************************************************************/

int reloc_offset(int type)
{
   switch (type)
   {
       case R_PP15    :
       case R_PP15W   :
       case R_PP15H   :
       case R_PP16B   :
       case R_PPN15   :
       case R_PPN15W  :
       case R_PPN15H  :
       case R_PPN16B  :
       case R_PPLBASE :     return 22;

       case R_PPL15   :
       case R_PPL15W  :
       case R_PPL15H  :
       case R_PPL16B  :
       case R_PPLN15  :
       case R_PPLN15W :
       case R_PPLN15H :
       case R_PPLN16B :
       case R_PPBASE  :     return 0;

       default        :     return 0;
   }
}


/*************************************************************************/
/*                                                                       */
/*   RELOC_STOP() -                                                      */
/*      Return the number of bits to read for a relocation type.         */
/*                                                                       */
/*************************************************************************/

int reloc_stop(int type)
{
   switch (type)
   {
       case R_PPBASE  :
       case R_PPLBASE :

       case R_PP15    :
       case R_PP15W   :
       case R_PP15H   :
       case R_PP16B   :

       case R_PPL15   :
       case R_PPL15W  :
       case R_PPL15H  :
       case R_PPL16B  :

       case R_PPN15   :
       case R_PPN15W  :
       case R_PPN15H  :
       case R_PPN16B  :

       case R_PPLN15  :
       case R_PPLN15W :
       case R_PPLN15H :
       case R_PPLN16B :   return 64;

       default       :    return WORDSZ * 8;
   }
}


#if 0
/******************************************************************************/
/*                                                                            */
/* SYM_RELOC_AMOUNT() - Determine the amount of relocation for a particular   */
/*                      relocation entry.  Search the relocation symbol table */
/*                      for the referenced symbol, and return the amount from */
/*                      the table.                                            */
/*                                                                            */
/******************************************************************************/
int sym_reloc_amount(RELOC *rp)
{
   int index = rp->r_symndx;

   int i = 0,
       j = reloc_sym_index - 1;

   /*-------------------------------------------------------------------------*/
   /* THIS IS A SIMPLE BINARY SEARCH (THE RELOC TABLE IS ALWAYS SORTED).      */
   /*-------------------------------------------------------------------------*/
   while (i <= j)
   {
      int m = (i + j) / 2;
      if      (reloc_tab[m].rt_index < index) i = m + 1;
      else if (reloc_tab[m].rt_index > index) j = m - 1;
      else return reloc_tab[m].rt_disp;                              /* FOUND */
   }

   /*-------------------------------------------------------------------------*/
   /* IF NOT FOUND, SYMBOL WAS NOT RELOCATED.                                 */
   /*-------------------------------------------------------------------------*/
   return 0;
}

#endif

/******************************************************************************/
/*                                                                            */
/*  UNPACK() - Extract a relocation field from object bytes and convert into  */
/*             a int so it can be relocated.                                 */
/*                                                                            */
/******************************************************************************/
unsigned int unpack(unsigned char *data, int fieldsz, int wordsz, int bit_offset)
{
   register int  i;
   unsigned int objval;
   int           start;                       /* MS byte with reloc data      */
   int           stop;                        /* LS byte with reloc data      */
   int           r  = bit_offset & 7;         /* num unused LS bits in stop   */
   int           l  = 8 - r;                  /* num used   MS bits in stop   */
   int           tr = ((bit_offset+fieldsz-1) & 7)+1; /* # LS bits in strt*/
   int           tl = 8 - tr;	              /* num unused MS bits in start  */

   start = (big_e_target ? (wordsz-fieldsz-bit_offset) : 
			   (bit_offset+fieldsz-1)) >>3;
   stop  = (big_e_target ? (wordsz-bit_offset-1) : bit_offset) >>3;

   if (start == stop) return (data[stop] >> r) & ((0x1 << fieldsz) - 0x1);

   objval = (unsigned)((data[start] << tl) & 0xFF) >> tl;
   
   if (big_e_target) 
	 for (i=start+1; i<stop; ++i) objval = (objval << 8) | data[i];
   else  for (i=start-1; i>stop; --i) objval = (objval << 8) | data[i];
   
   return (objval << l) | (data[stop] >> r);
}



/******************************************************************************/
/*                                                                            */
/* REPACK() - Encode a binary relocated field back into the object data.      */
/*                                                                            */
/******************************************************************************/
void repack(unsigned int objval, unsigned char *data, int fieldsz,
            int  wordsz, int bit_offset)
{
   register int  i;
   int           start;                      /* MS byte with reloc data       */
   int           stop;                       /* LS byte with reloc data       */
   int           r     = bit_offset & 7;     /* num unused LS bits in stop    */
   int           l     = 8 - r;              /* num used   MS bits in stop    */
   int           tr    = ((bit_offset+fieldsz-1) & 7)+1; /* # LS bits in strt */
   unsigned int mask  = (1ul << fieldsz) - 1ul;

   if (fieldsz < sizeof(objval)) objval &= mask;
   
   start = (big_e_target ? (wordsz-fieldsz-bit_offset) : 
			   (bit_offset+fieldsz-1)) >>3;
   stop  = (big_e_target ? (wordsz-bit_offset-1) : bit_offset) >>3;

   if (start == stop)
   {
       data[stop] &= ~(mask << r);
       data[stop] |= (objval << r); 
       return;
   }

   data[start] &= ~((1<<tr)-1);
   data[stop]  &=  ((1<< r)-1);
   data[stop]  |= (objval << r); 
   objval     >>= l;

   if (big_e_target) 
        for (i = stop - 1; i > start; objval >>= 8) data[i--] = objval;
   else for (i = stop + 1; i < start; objval >>= 8) data[i++] = objval;
   
   data[start] |= objval; 
}


/******************************************************************************/
/*                                                                            */
/* CLOAD_LINENO() - Read in, swap, and relocate line number entries.  This    */
/*                  function is not called directly by the loader, but by the */
/*                  application when it needs to read in line number entries. */
/*                                                                            */
/******************************************************************************/
int cload_lineno(int filptr, int count, LINENO *lptr, int scnum) 
{
/*   int    filptr;                      WHERE TO READ FROM                  */
/*   int     count;                       HOW MANY TO READ                    */
/*   LINENO *lptr;                        WHERE TO PUT THEM                   */
/*   int     scnum;                       SECTION NUMBER OF THESE ENTRIES     */

    int i;

    /*------------------------------------------------------------------------*/
    /* READ IN THE REQUESTED NUMBER OF LINE NUMBER ENTRIES AS A BLOCK.        */
    /*------------------------------------------------------------------------*/
#if FILE_BASED
    if (fseek(fin, filptr, 0) != 0) { load_err = E_FILE; return FALSE; }
    for (i = 0; i < count; i++)
        if (fread(lptr + i, 1, LINESZ, fin) != LINESZ) 
                                    { load_err = E_FILE; return FALSE; }
#else
	for (i = 0; i < count; i++)
   	mem_copy((void*)(lptr+i), (void*)&gRxBuffer[filptr + i*LINESZ], LINESZ);
#endif
    /*------------------------------------------------------------------------*/
    /* SWAP AND RELOCATE EACH ENTRY, AS NECESSARY.                            */
    /*------------------------------------------------------------------------*/
    if (byte_swapped || RUN_RELOC_AMOUNT(scnum - 1))
       for (i = 0; i < count; i++, lptr++)
       {
	  if (byte_swapped)
	  {
	      swap2byte(&lptr->l_lnno);
	      swap4byte(&lptr->l_addr.l_paddr);
	  }

	  if (lptr->l_lnno != 0) 
	     lptr->l_addr.l_paddr += RUN_RELOC_AMOUNT(scnum - 1);
       }
    
    return TRUE;
}


/******************************************************************************/
/*                                                                            */
/*  SWAP4BYTE() - Swap the order of bytes in a int.                          */
/*                                                                            */
/******************************************************************************/
void swap4byte(void *addr)
{
   unsigned int *value = (unsigned int *)addr;
   unsigned int temp1, temp2, temp3, temp4;

   temp1 = (*value)       & 0xFF;
   temp2 = (*value >> 8)  & 0xFF;
   temp3 = (*value >> 16) & 0xFF;
   temp4 = (*value >> 24) & 0xFF;

   *value = (temp1 << 24) | (temp2 << 16) | (temp3 << 8) | temp4;
}


/******************************************************************************/
/*                                                                            */
/*  SWAP2BYTE() - Swap the order of bytes in a short.                         */
/*                                                                            */
/******************************************************************************/
void swap2byte(void *addr)
{
   unsigned short *value = (unsigned short *)addr;
   unsigned short temp1,temp2;

   temp1 = temp2 = *value;
   *value = ((temp2 & 0xFF) << 8) | ((temp1 >> 8) & 0xFF);
}
