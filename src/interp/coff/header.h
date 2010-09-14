/************************************************************************/
/* HEADER.H - STANDARD HEADER FILE FOR COFF LOADERS                     */
/************************************************************************/

/*-----------------------------------------------------------------------*/
/* INCLUDE STANDARD HEADER FILES                                         */
/*-----------------------------------------------------------------------*/
//#include <string.h>
//#include <stdlib.h>
#include <stdarg.h>
#include "version.h"
#include "params.h"
#include "coff.h"
#include "osal.h"   /* must preceed cload.h. */
#include "cload.h"
#include "proto.h"
#include "types.h"
#include "coff_trg.h"

/*-----------------------------------------------------------------------*/
/* CONSTANTS, MACROS, VARIABLES, AND STRUCTURES FOR THE LOADER.          */
/*-----------------------------------------------------------------------*/
#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))

