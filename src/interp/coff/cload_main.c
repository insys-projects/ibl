/****************************************************************************/
/* MAIN.C  - Skeleton Driver for COFF Loader       Version 6.00 4/96        */
/****************************************************************************/
/*                                                                          */
/* General:  This module is a skeleton driver for a standalone COFF         */
/*           loader.  This file is NOT part of the generic loader, but      */
/*           provides a sample interface to it that can provide a basis     */
/*           for a specific loader.  YOU MUST CUSTOMIZE THIS PROGRAM        */
/*           FOR YOUR APPLICATION.  In particular, you must write the       */
/*           function 'mem_write()' which is the low-level function to      */
/*           load target memory.                                            */
/*                                                                          */
/*           Refer to the documentation provided with the loader for        */
/*           details on how to interface with it.                           */
/*                                                                          */
/* Usage:    cload <-options> filename                                      */
/* Options:     -b        clear .bss section                                */
/*              -q        quiet mode (no banner)                            */
/*              -r xxxx   relocate by xxxx                                  */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/* Functions defined in this module:                                        */
/*                                                                          */
/*    main()             - Main driver for loader.                          */
/*    mem_write()        - Load a buffer of raw data to the target.         */
/*    set_reloc_amount() - Determine relocation amount for each section.    */
/*    lookup_sym()       - Stub for symbol lookup routine.                  */
/*    myalloc()          - Application version of 'malloc()'.               */
/*    mralloc()          - Application version of 'realloc()'.              */
/*    load_msg()         - Called by loader to write debug messages.        */
/*                                                                          */
/****************************************************************************/
#include "header.h"

#define READ_BIN "rb"                     /* HOST-SPECIFIC                   */

unsigned int reloc = 0;                 /* RELOCATION AMOUNT               */

void mem_copy(unsigned char* dst, unsigned char* src, int nbytes)
{
	int i = 0;
	for(i = 0; i<nbytes; i++)
	*(dst+i) = *(src+i);	
} 

/****************************************************************************/
/*                                                                          */
/* MEM_WRITE() - Load a buffer of raw data to the target.                   */
/*                                                                          */
/*   THIS FUNCTION MUST BE CUSTOMIZED FOR YOUR APPLICATION !                */
/*                                                                          */
/*   (As supplied, this function simply dumps the data out to the screen.)  */
/*                                                                          */
/****************************************************************************/
int mem_write(unsigned char *buffer,        /* POINTER TO DATA BUFFER       */
              unsigned int   nbytes,        /* NUMBER OF 8-BIT BYTES        */
              T_ADDR         addr,          /* TARGET DESTINATION ADDRESS   */
              unsigned char  page)          /* TARGET DESTINATION PAGE      */
{
   /*-----------------------------------------------------------------------*/
   /* INSERT CUSTOM CODE HERE TO LOAD TARGET MEMORY.                        */
   /*-----------------------------------------------------------------------*/
	mem_copy((void*)addr, (void*)buffer, nbytes); 
   return 1;
}


/****************************************************************************/
/*                                                                          */
/* SET_RELOC_AMOUNT() - Determine relocation amount for each section.       */
/*                                                                          */
/****************************************************************************/
int set_reloc_amount()
{
  int i;

  for (i = 0; i<  n_sections; ++i) reloc_amount[i] = reloc;
  return 1;
}


/****************************************************************************/
/*                                                                          */
/* LOOKUP_SYM() - Stub for symbol lookup routine.                           */
/*                                                                          */
/****************************************************************************/
void lookup_sym(int indx, SYMENT *sym, AUXENT *aux)
{}


/****************************************************************************/
/*                                                                          */
/* LOAD_SYMS() - Stub for symbol load routine.  This routine is only called */
/*               if the global flag 'need_symbols' is TRUE.                 */
/*                                                                          */
/****************************************************************************/
int load_syms(int need_reloc)
{
   return 1;
}
