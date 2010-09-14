/****************************************************************************/
/*									    */
/*  File: version.h		Last change: 25-June-90 RET       	    */
/*									    */
/*  All portability flags and version information is to appear HERE ONLY.   */
/*  Host specific flags are defined automatically by this file.             */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/* TARGET SPECIFIC FLAGS                                                    */
/****************************************************************************/
#define TMS340      0			     /* TMS340                      */
#define TMS34020    0			     /* TMS340 340XX, 0 for 10 only */
#define TMS370      0			     /* Roadrunner                  */
#define TMS32030    0			     /* Brahma                      */
#define TMS32025    0			     /* Shiva, Himiko               */
#define TMS32050    0                        /* Mosaic                      */
#define TMS380      0			     /* Eagle                       */
#define TMS37016    0                        /* Prism                       */
#define MVP         0			     /* MVP                         */
#define MVP_MP      0			     /* MVP MP                      */
#define MVP_PP      0			     /* MVP PP                      */
#define TMS32060    1			     /* TMS320C60                   */
#define VIVID       0			     /* VIVID                       */
#define LEAD        0                        /* LEAD Fixed-Point DSP        */
#define LEAD3       0                        /* LEAD Fixed-Point DSP        */
#define ARM         0                        /* ARM                         */
#define TMS370C8    0			     /* C8                          */
#define TMS370C8P   0			     /* C8+                         */
#define RTC         0                        /* ANKOOR                      */

#define VERSION     "Version 0.01"
#define COPYRITE    "Copyright (c) 1997 Texas Instruments Incorporated"
#define BANNER_PAD  30
#define VSTAMP      001		        /* version stamp for output file*/


/****************************************************************************/
/* OPERATING SYSTEM SPECIFIC FLAGS: MSDOS, UNIX, VAX_VMS, MPW.              */
/* THESE FLAGS ARE SET AUTOMATICALLY BY THE HOST COMPILER DEPENDING ON      */
/* PREDEFINED HOST COMPILER SYMBOLS.   WHEN THE TOOLS ARE PORTED TO A NEW   */
/* HOST OR A NEW COMPILER, THIS FILE MAY NEED MODIFICATION.                 */
/****************************************************************************/

/****************************************************************************/
/*         MSDOS   : If system is DOS based. (MS-DOS, PC-DOS, etc)          */
/****************************************************************************/
#ifdef MSDOS                            /* ALWAYS defed IN MSC 5.00+        */
#undef MSDOS
#define MSDOS       1
#else
#define MSDOS       0
#endif

/****************************************************************************/
/*         UNIX    : Any unix environment                                   */
/****************************************************************************/
#if unix || __unix                      /* ALWAYS defined by UNIX compilers */
#define UNIX        1
#else
#define UNIX        0
#endif

/****************************************************************************/
/*         NT : If target is Windows NT                                     */
/****************************************************************************/
#ifdef __NT__                     /* DEFINED WITH /bt=NT on WATCOM COMPILER */
#define WIN_NT     1
#else
#define WIN_NT     0
#endif

/****************************************************************************/
/*         VAX_VMS : If system is a VAX, running VMS                        */
/****************************************************************************/
#if VAX && VMS                          /* BOTH DEFINED BY VAX C COMPILER   */
#define VAX_VMS     1	
#else
#define VAX_VMS     0
#endif

/****************************************************************************/
/*         MPW     : Macintosh, particularly the MPW environment            */
/****************************************************************************/
#if macintosh || mcu68000               /* DEFINED BY MPW COMPILER          */
#define MPW         1
#else
#define MPW         0
#endif


/****************************************************************************/
/* HOST_ENFORCES_ALIGNMENT is used by the tools to determine if the         */
/* host allows memory accesses on unaligned boundaries.   This flag is      */
/* used in the compilers for reading I-CODE, and in the assemblers for      */
/* building memory images of object files.                                  */
/*--------------------------------------------------------------------------*/
/* This implementation is PARANOID - that is, if I don't specifically know  */
/* that a host can handle non-aligned memory accesses, I assume it can't.   */
/****************************************************************************/
#if MSDOS || VAX_VMS || MPW || WIN_NT
#define HOST_ENFORCES_ALIGNMENT 0        /* NON-ALIGNED ACCESSES LEGAL */
#else
#define HOST_ENFORCES_ALIGNMENT 1        /* SPARC, MIPS, HP...         */
#endif
