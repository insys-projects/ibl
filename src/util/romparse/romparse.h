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
#elif defined(c6474) || defined(c6474l)
 #include "tiboot_c6474.h"
#elif defined(c6455)
 #include "tiboot_c6455.h"
#elif defined(c6457)
 #include "tiboot_c6457.h"
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
  unsigned int  addressBytes;
  unsigned int data[MAX_DATA_LEN_32bit];
  int  tag[NUM_BOOT_PARAM_TABLES];          /* identifies boot parameter tables which use this file */
  int  align;                               /* alignment requirements for the file */
} progFile_t;

/* Define the PCI parameter structure */
#define PCI_DATA_LEN_32bit  8
typedef struct {
  char fname[MAX_FNAME_LEN];
  int sizeBytes;
  int addressBytes;
  unsigned int data[PCI_DATA_LEN_32bit];
} pciFile_t;


/* Define a layout table. A layout table is a block of data which contains the addresses
 * of data files. Each address is 32 bits, with the upper 16 bits specifying the i2c 
 * id, the lower address the byte address of the 1st block in the table */
#define MAX_LAYOUTS         2
#define MAX_LAYOUT_FILES    8
typedef struct  {
  int nFiles;                   /* Number of files in file list*/
  int file[MAX_LAYOUT_FILES];   /* Index of each file in progFile array */
  
  unsigned int address;         /* I2c data address of the table */
  unsigned int dev_addr;        /* I2c device address of the table */
  int align;
} layout_t;

/* The i2c address masking is chosen to be 20 bits. This allows eproms to be stacked to use addresses
 * from 0x50 - 0x5f */
#define I2C_ADDR_MASK   0x0fffff


/* Pad section. The pad section creates a gap in the i2c memory map */
#define MAX_PADS        8
typedef struct  {
  unsigned int address;
  unsigned int len;
} pad_t;


/* Layout/pad interleave. The rom specification must be in order, so this structure tracks
 * the arrangement of layouts and pads */
typedef struct
{
  int type;   /* Either PAD or LAYOUT */
  int index;  /* The array index for the pad/layout */
  
} padLayoutOrder_t;




#endif /* ROMPARSE_H */
