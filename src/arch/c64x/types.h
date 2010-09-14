#ifndef _TYPES_H
#define _TYPES_H
/*******************************************************************************
 * FILE PURPOSE:  DSP specific C type definitions.
 *******************************************************************************
 * FILE NAME: swpform.h  
 *
 * DESCRIPTION: Defines general use types for DSP.  
 *
 *  @file   types.h
 *
 *  @brief
 *      This file provides architecture specific typedefs
 *
 ******************************************************************************/


#include <stdlib.h>  /* Defines NULL */

/* a signed 16-bit integer */
typedef short int16;
typedef unsigned short uint16;

typedef int int32;
typedef unsigned int uint32;

typedef char char8;
typedef unsigned char uchar8;

typedef char int8;
typedef unsigned char uint8;


typedef unsigned char word;

typedef short BOOL;
typedef short bool;
typedef short Bool;
#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE  1
#endif


/* tistdtypes.h types used from the emac driver */
typedef unsigned char Uint8;
typedef char Int8;
typedef int Int32;
typedef unsigned short Uint16;
typedef unsigned int Uint32;

/* TI boot types */
typedef unsigned char  UINT8;
typedef unsigned short UINT16;
typedef unsigned int   UINT32;


/* Types from evm driver */
typedef volatile unsigned int  VUint32;
typedef volatile unsigned char VUint8;

/* Types from the ethernet driver */
typedef unsigned int  IPN;

#endif /* types.h */
