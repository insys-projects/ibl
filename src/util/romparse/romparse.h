#ifndef _ROMPARSE_H
#define _ROMPARSE_H
/*************************************************************************************
 * FILE PURPOSE: Define boot data structures
 *************************************************************************************
 * FILE NAME: romparse.h
 *
 * DESCRIPTION: Defines the structures used to create the boot parameter
 *				table
 *
 *************************************************************************************/
 
/* tiboot.h defines the boot parameters structure that will be created */
#if defined(c6472)
 #include "tiboot_c6472.h"
#elif defined(c6474)
 #include "tiboot_c6474.h"
#elif defined(c6455)
 #include "tiboot_c6455.h"
#else
 #error invalid or missing device specification
#endif

/* Define the number of boot parameter tables that will be put on the rom */
#define NUM_BOOT_PARAM_TABLES   8

/* Define the size reserved for the PCI configuration table */
#define PCI_EEAI_PARAM_SIZE    0x20

/* Define a structure mapping the boot parameter table number to a program file
 * to an eeprom byte address */
#define MAX_FNAME_LEN        132
#define MAX_DATA_LEN_32bit   32768
typedef struct {
  char fname[MAX_FNAME_LEN];
  int  sizeBytes;
  int  addressBytes;
  unsigned int data[MAX_DATA_LEN_32bit];
} progFile_t;

/* Define the PCI parameter structure */
#define PCI_DATA_LEN_32bit  8
typedef struct {
  char fname[MAX_FNAME_LEN];
  int sizeBytes;
  int addressBytes;
  unsigned int data[PCI_DATA_LEN_32bit];
} pciFile_t;




#endif /* ROMPARSE_H */
