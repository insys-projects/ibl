/*************************************************************************************
 * FILE PURPOSE: Create an I2C rom with multiple boot parameter sections and
 *               programs
 *************************************************************************************
 * FILE NAME: romparse.c
 *
 * DESCRIPTION: Creates a ccs hex file which contains the i2c eprom boot parameter 
 *              tables as well as any code.
 *
 *************************************************************************************/
#include <stdio.h>
#include <string.h>
#include "rparse.tab.h"
#include "romparse.h"

/*************************************************************************************
 * Definition: fixed i2c map locations
 *************************************************************************************/
#define PCI_PARAM_BASE  (NUM_BOOT_PARAM_TABLES * 0x80)
#define DATA_BASE       (PCI_PARAM_BASE + PCI_EEAI_PARAM_SIZE)



/*************************************************************************************
 * Declaration: The flex input file is assigned based on the command line
 *************************************************************************************/
extern FILE *yyin;

/*************************************************************************************
 * Declaration: The boot parameter tables. The current table is copied into position
 *              when the section parse is complete.
 *************************************************************************************/
BOOT_PARAMS_T boot_params[NUM_BOOT_PARAM_TABLES];
BOOT_PARAMS_T current_table;
int           current_file;       /* Identifies the program file in the current table */
int           ctable_index = -1;  /* Destination of current table */
int           max_index    =  0;  /* maximum table index, used for compacting output */

/************************************************************************************
 * Declaration: The structure storing the program data files, and the number of
 *              programs used.
 ************************************************************************************/
progFile_t progFile[NUM_BOOT_PARAM_TABLES];
int        nProgFiles = 0;

/************************************************************************************
 * Declaration: The PCI parameter structure
 ************************************************************************************/
pciFile_t pciFile;
int       pciSet = 0;

/*************************************************************************************
 * Declaration: The next free address in the ROM for general data usage. For the
 *              start address there is room for the initial boot parameter tables, 
 *              plus room for the PCI eeai config.
 *************************************************************************************/
int romBase = DATA_BASE;


/*************************************************************************************
 * Declaration: Args passed in from the command line
 *************************************************************************************/
char *inputFile;
int   compact = 0;


/*************************************************************************************
 * FUNCTION PURPOSE: flex/bison required support functions.
 *************************************************************************************
 * DESCRIPTION: yyerror and yywrap are required to support flex and/or bison parsing
 *              of the parameter description file. yyerror directly reports the
 *              error, yywrap is unused.
 *************************************************************************************/
void yyerror (char *s)
{
  fprintf (stderr, "flex/bison error is %s\n", s);
} /* yyerror */

void yywrap (void)
{
} /* yywrap */

/*************************************************************************************
 * FUNCTION PURPOSE: Initialize a boot table
 *************************************************************************************
 * DESCRIPTION: Sets a boot parameter table to 0s
 *************************************************************************************/
void initTable (BOOT_PARAMS_T *current_table)
{
  memset (current_table, 0, sizeof(BOOT_PARAMS_T));
}

/*************************************************************************************
 * FUNCTION PURPOSE: Initialize the program data file table
 *************************************************************************************
 * DESCRIPTION: The size and tags are all setup
 *************************************************************************************/
void initProgFile (void)
{
  int i, j;

  for (i = 0; i < NUM_BOOT_PARAM_TABLES; i++)  {
    progFile[i].sizeBytes = 0;

    for (j = 0; j < NUM_BOOT_PARAM_TABLES; j++)
      progFile[i].tag[j] = -1;

  }

}

/*************************************************************************************
 * FUNCTION PURPOSE: Complete a section
 *************************************************************************************
 * DESCRIPTION: The parser has detected a complete section. Copy the section into
 *              it's correct table location.
 *************************************************************************************/
void section (void)
{
  int i;

  /* It's an error if no section value has been declared */
  if (ctable_index == -1)  {
    fprintf (stderr, "romparse: the section did not have a boot paramter index specified\n");
    exit (-1);
  }

  /* Make sure the table is in range */
  if (ctable_index >= NUM_BOOT_PARAM_TABLES)  {
    fprintf (stderr, "romparse: The section index is too large (max %d)\n", NUM_BOOT_PARAM_TABLES-1);
    exit (-1);
  }

  /* The length must be set. Currently this program only supports I2C mode, so the
   * length is fixed */
  current_table.common.length   = 30;
  current_table.common.checksum = 0;

  /* Copy the table */
  memcpy (&boot_params[ctable_index], &current_table, sizeof (BOOT_PARAMS_T));
  initTable (&current_table);

  /* Track the maximum table index */
  if (ctable_index > max_index)
    max_index = ctable_index;

  /* If the section referenced a data file, link the data file back to this section */
  if (current_file >= 0)  {
    for (i = 0; i < NUM_BOOT_PARAM_TABLES; i++)  {
      if (progFile[current_file].tag[i] < 0)
        progFile[current_file].tag[i] = ctable_index;
        break;
    }
  }

  ctable_index = -1;
  current_file = -1;

} /* section */

/***************************************************************************************
 * FUNCTION PURPOSE: Open a ccs hex file and read in the data.
 ***************************************************************************************
 * DESCRIPTION: Reads a ccs hex format data file, loads the file into the
 *              next program file structure. Returns the index of the just loaded
 *              table.
 ***************************************************************************************/
int openProgFile (char *fname)
{
  FILE *str;
  int a, b, c, d, e;
  int i;
  char iline[132];

  /* Store the file name */
  strcpy (progFile[nProgFiles].fname, fname);

  /* Open the data file */
  str = fopen (fname, "r");
  if (str == NULL)  {
    fprintf (stderr, "romparse: Could not open file %s\n", fname);
    exit (-1);
  }

  /* Put the section at the next available i2c rom address */
  /* progFile[nProgFiles].addressBytes = romBase; */

  /* Read the one line ccs header. The length field in terms of lines */
  fgets (iline, 132, str);
  sscanf (iline, "%x %x %x %x %x", &a, &b, &c, &d, &e);
  progFile[nProgFiles].sizeBytes = e * 4; /* Length was in 4 byte words */

  /* Read in the data */
  for (i = 0; i < e; i++)  {
    fgets (iline, 132, str);
    sscanf (&(iline[2]), "%x", &((progFile[nProgFiles]).data[i]));
  }

  fclose (str);

  /* Update the next free rom base */
  /* romBase = romBase + progFile[nProgFiles].sizeBytes; */

  i = nProgFiles;
  nProgFiles += 1;

  return (i);

} /* openProgFile */

/***************************************************************************************
 * FUNCTION PURPOSE: Load the PCI paramter section
 ***************************************************************************************
 * DESCRIPTION: Loads the PCI parameter section and stores it in the rom. */
int setPciParams (char *fname)
{
  FILE *str;
  int a, b, c, d, e;
  int i;
  char iline[132];
  char *z;

  if (pciSet)  {
    fprintf (stderr, "romparse: PCI parameters specified more then once\n");
    exit (-1);
  }

  /* the input string still contains the quotes. Remove them here */
  z = &fname[1];
  *strchr (z, '"') = '\0';

  /* Store the file name */
  strcpy (pciFile.fname, z);

  /* Open the data file */
  str = fopen (z, "r");
  if (str == NULL)  {
    fprintf (stderr, "romparse: Could not open pci file %s\n", fname);
    exit (-1);
  }

  /* The address of the pci params is currently fixed */
  pciFile.addressBytes = PCI_PARAM_BASE;

  /* Read the one line ccs header. The length field is in terms of lines */
  fgets (iline, 132, str);
  sscanf (iline, "%x %x %x %x %x", &a, &b, &c, &d, &e);
  pciFile.sizeBytes = e * 4;  /* Convert length to bytes */

  /* Read in the data */
  for (i = 0; i < e; i++)  {
    fgets (iline, 132, str);
    sscanf (&(iline[2]), "%x", &(pciFile.data[i]));
  }

  pciSet = 1;

  return (0);

} /* setPciParams */

 

/***************************************************************************************
 * FUNCTION PURPOSE: Store an assignment
 ***************************************************************************************
 * DESCRIPTION: Stores an assigned value into the current boot parameter table
 ***************************************************************************************/
void assignKeyVal (int field, int value)
{
  switch (field)  {

    case BOOT_MODE:        current_table.common.boot_mode = value;
                           break;

    case PARAM_INDEX:      ctable_index = value;
                           break;

    case OPTIONS:          current_table.i2c.options = value;
                           break;

    case MULTI_I2C_ID:     current_table.i2c.multi_i2c_id = value;
                           break;

    case MY_I2C_ID:        current_table.i2c.my_i2c_id = value;
                           break;

    case CORE_FREQ_MHZ:    current_table.i2c.core_freq_mhz = value;
                           break;

    case I2C_CLK_FREQ_KHZ: current_table.i2c.i2c_clk_freq_khz = value;
                           break;

    case NEXT_DEV_ADDR:    current_table.i2c.next_dev_addr = value;
                           break;

    case NEXT_DEV_ADDR_EXT: current_table.i2c.next_dev_addr_ext = value;
                            break;

    case ADDRESS_DELAY:    current_table.i2c.address_delay = value;
                           break;

#ifndef c6455         
    case SWPLL:            current_table.i2c.swPll = value;
                           break;
#endif

    case DEV_ADDR_EXT:     current_table.i2c.dev_addr_ext = value;
                           break;

    case DEV_ADDR:         current_table.i2c.dev_addr = value;
                           break;


  }

} /* assignKeyVal */


/*******************************************************************************
 * FUNCTION PURPOSE: Parse a string input.
 *******************************************************************************
 * DESCRIPTION: Takes a string input. Currently only the i2c exe file name can be 
 *              assigned a string. 
 *******************************************************************************/
void assignKeyStr (int value, char *y)
{
  int i;
  char *z;


  /* the input string still contains the quotes. Remove them here */
  z = &y[1];
  *strchr (z, '"') = '\0';

  /* Check if the file name is already open */
  for (i = 0; i < nProgFiles; i++)  {

    if (!strcmp (z, progFile[i].fname))  {
      /* Found a match - copy the address */
      current_file = i;
      /* current_table.i2c.dev_addr     = progFile[i].addressBytes & 0xffff; */
      if (current_table.i2c.dev_addr_ext == 0)
        current_table.i2c.dev_addr_ext = 0x50;  /* hard coded to i2c rom slave address */
      return;
    }

  }

  /* Open and read the ccs file, set the ROM address */
  i = openProgFile (z);
  if (i >= 0) {
    /* current_table.i2c.dev_addr     = progFile[i].addressBytes & 0xffff; */
    current_file = i;
    if (current_table.i2c.dev_addr_ext == 0)
        current_table.i2c.dev_addr_ext = 0x50;
  }

} /* assignKeyStr */


/************************************************************************************
 * FUNCTION PURPOSE: Opens and writes the output file
 ************************************************************************************
 * DESCRIPTION: Creates the output file in ccs format.
 ************************************************************************************/
void createOutput (void)
{
  FILE *str;
  int   totalLenBytes;
  int   i, j;
  int   nTables;
  unsigned int value, v1, v2;
  unsigned int base;

  str = fopen ("i2crom.ccs", "w");
  if (str == NULL)  {
    fprintf (stderr, "romparse: Could not open output file i2crom.ccs for writing\n");
    exit (-1);
  }

  /* Compact the i2c eeprom to use the minimum memory possible */
  base    = DATA_BASE;
  nTables = NUM_BOOT_PARAM_TABLES; 

  if ((compact != 0) && (pciSet == 0))  {
    nTables = max_index + 1;
    base    = nTables * 0x80;  /* The number of parameter tables * size of a parameter table */
  }


  for (i = 0; i < NUM_BOOT_PARAM_TABLES; i++)  {
    progFile[i].addressBytes = base;
    base = base + progFile[i].sizeBytes;
  }

  /* Setup the base program file addresses. If a parameter set has
   * been tagged it means that this is an i2c program load */
  for (i = 0; i < NUM_BOOT_PARAM_TABLES; i++)  {
    for (j = 0; j < NUM_BOOT_PARAM_TABLES; j++)  {
      if (progFile[i].tag[j] >= 0)
        boot_params[progFile[i].tag[j]].i2c.dev_addr = progFile[i].addressBytes;
    }
  }
      
  /* The total length of the i2c eeprom is now stored in base */
  /* Write out the ccs header */
  fprintf (str, "1651 1 10000 1 %x\n", base >> 2);

  /* Write out the boot parameter tables. 0x80 bytes will be written out.
   * There are 16 bits in every parameter field, which is why the index
   * is from 0 to 0x40 */
  for (i = 0; i < nTables; i++)  {
    for (j = 0; j < (0x80 >> 1); j += 2)  {
      v1 = boot_params[i].parameter[j];
      v2 = boot_params[i].parameter[j+1];
      value = (v1 << 16) | v2;
      fprintf (str, "0x%08x\n", value);
    }
  }

  /* Write out the PCI parameter base. If none was included then zeros will be
   * written out */
  if (pciSet)  {
    for (i = 0; i < PCI_DATA_LEN_32bit; i++)  {
      fprintf (str, "0x%08x\n", pciFile.data[i]);
    }
  }
                                

  /* Write out each of the program files */
  for (i = 0; i < nProgFiles; i++)
    for (j = 0; j < progFile[i].sizeBytes >> 2; j++)
      fprintf (str, "0x%08x\n", (progFile[i]).data[j]);


  /* Close the input file */
  fclose (str);

} /* createOutput  */

/************************************************************************************
 * FUNCTION PURPOSE: Initialize the pci paramter table
 ************************************************************************************
 * DESCRIPTION: Zeros the pci parameters
 ************************************************************************************/
void initPciParams (void)
{
  memset (&pciFile, 0, sizeof(pciFile_t));
} /* initPciParams */



/************************************************************************************
 * FUNCTION PURPOSE: Parse the input arguments.
 ************************************************************************************
 * DESCRIPTION: Returns -1 on invalid args
 ************************************************************************************/
int parseIt (int argc, char *argv[])
{
  int i;

  if (argc < 2)  {
     fprintf (stderr, "usage: %s [-compact] inputfile\n", argv[0]);
     return (-1);
  }

  inputFile = NULL;  

  for (i = 1; i < argc;  )  {

    if (!strcmp (argv[i], "-compact"))  {
      compact = 1;
      i += 1;

    } else  {

      if (inputFile != NULL)  {
        fprintf (stderr, "usage: %s [-compact] inputfile\n", argv[0]);
        return (-1);
      }

      inputFile = argv[i];
      i += 1;
    }
  }

  return (0);

}



/************************************************************************************
 * FUNCTION PURPOSE: main function
 ************************************************************************************
 * DESCRIPTION: Performs the processing sequence.
 ************************************************************************************/
int main (int argc, char *argv[])
{
  int i;

  /* Initialize the tables */
  for (i = 0; i < NUM_BOOT_PARAM_TABLES; i++)
    initTable(&boot_params[i]);

  initTable (&current_table);
  current_file = -1;

  /* Initialize the program file structure */
  initProgFile ();

  /* Initialize the PCI param table */
  initPciParams ();


  /* Parse the input parameters */
  if (parseIt (argc, argv))
    return (-1);

  
  yyin = fopen (inputFile, "r");
  if (yyin == NULL)  {
    fprintf (stderr, "%s: could not open file %s\n", argv[0], inputFile);
    return (-1);
  }

  /* Parse the input description file */
  yyparse();


  /* Create the output file */
  createOutput ();

  return (0);
}



