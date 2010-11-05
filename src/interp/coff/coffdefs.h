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
/*  COFFDEFS.H                                                            */
/*     Definitions of COFF symbol type and storage class fields.          */
/**************************************************************************/
#ifndef COFFDEFS_H
#define COFFDEFS_H

#if defined(TOOL_ASSEMBLER)
   #error The COFF submodule should no longer be used by the assembler; use TICOFF04
#endif

/*------------------------------------------------------------------------*/
/*   STORAGE CLASSES                                                      */
/*------------------------------------------------------------------------*/
#define  C_NULL          0
#define  C_AUTO          1     /* AUTOMATIC VARIABLE                      */
#define  C_EXT           2     /* EXTERNAL SYMBOL                         */
#define  C_STAT          3     /* STATIC                                  */
#define  C_REG           4     /* REGISTER VARIABLE                       */
#define  C_EXTREF        5     /* EXTERNAL DEFINITION                     */
#define  C_LABEL         6     /* LABEL                                   */
#define  C_ULABEL        7     /* UNDEFINED LABEL                         */
#define  C_MOS           8     /* MEMBER OF STRUCTURE                     */
#define  C_ARG           9     /* FUNCTION ARGUMENT                       */
#define  C_STRTAG        10    /* STRUCTURE TAG                           */
#define  C_MOU           11    /* MEMBER OF UNION                         */
#define  C_UNTAG         12    /* UNION TAG                               */
#define  C_TPDEF         13    /* TYPE DEFINITION                         */
#define  C_USTATIC       14    /* UNDEFINED STATIC                        */
#define  C_ENTAG         15    /* ENUMERATION TAG                         */
#define  C_MOE           16    /* MEMBER OF ENUMERATION                   */
#define  C_REGPARM       17    /* REGISTER PARAMETER                      */
#define  C_FIELD         18    /* BIT FIELD                               */
#define  C_UEXT          19    /* TENTATIVE EXTERNAL DEFINITION           */
#define  C_STATLAB       20    /* STATIC LOAD-TIME LABEL                  */
#define  C_EXTLAB        21    /* EXTERNAL LOAD-TIME LABEL                */
#define  C_VREG          22    /* VIRTUAL REGISTER VARIABLE               */
#define  C_SYSTEM        23    /* SYSTEM-WIDE SYMBOL                      */
#define  C_STATREG       24    /* STATIC REGISTER VARIABLE                */
#define  C_EXTREG        25    /* EXTERNAL REGISTER VARIABLE              */
#define  C_EXTREFREG     26    /* EXTERNAL REGISTER VARIABLE REFERENCE    */
#define  C_VARARG        27    /* LAST DECLARED PARAMETER OF VARARG FN    */
#define  C_EXTDEF        28    /* C_EXT DEFINED IN DIFFERENT FILE         */
#define  C_USTATREG      29    /* UNDEFINED STATIC REGISTER VARIABLE      */

#define  C_BLOCK         100   /* ".BB" OR ".EB"                          */
#define  C_FCN           101   /* ".BF" OR ".EF"                          */
#define  C_EOS           102   /* END OF STRUCTURE                        */
#define  C_FILE          103   /* FILE NAME                               */
#define  C_LINE          104   /* DUMMY SCLASS FOR LINE NUMBER ENTRY      */
#define  C_ALIAS         105   /* DUPLICATE TAG                           */
#define  C_PREF          106   /* DUMMY SCLASS FOR REF PRAGMA TABLE ENTRY */

#define  C_GRPTAG        107   /* TAG FOR GROUPED GLOBAL VARIABLES        */
#define  C_SMOG          108   /* STATIC MEMBER OF GROUP                  */
#define  C_EMOG          109   /* EXTERN MEMBER OF GROUP                  */
#define  C_EOG           110   /* END OF GROUP                            */

/*------------------------------------------------------------------------*/
/* STORAGE CLASS QUALIFIERS                                               */
/*------------------------------------------------------------------------*/
#define CQ_NEAR          0x0001
#define CQ_FAR           0x0002
#define CQ_INLINE        0x0004        /* INLINE THIS FUNCTION DEFINITION   */
#define CQ_SUPPRESS      0x0008        /* INLINE AND EMIT NO ICODE OUTPUT   */
#define CQ_CONTROL       0x0010        /* SYMBOL IS A "CONTROL REGISTER"    */
#define CQ_INTERRUPT     0x0020        /* INTERRUPT FUNCTION                */
#define CQ_TRAP          0x0040        /* (MVP CODE GENERATOR)              */
#define CQ_GREGISTER     0x0080        /* GLOBAL REGISTER SYMBOL            */
#define CQ_PORT          0x0100        /* 370 PORT VARIABLE                 */
#define CQ_SYSTEM        0x0200        /* MVP SHARED SYMBOL (MP AND PPs)    */
#define CQ_SYSTEMPP      0x0400        /* MVP SHARED SYMBOL (PPs only)      */
#define CQ_REENTRANT     0x0800        /* FUNCTION IS REENTRANT/NEEDS STACK */
#define CQ_NMI_INTERRUPT 0x1000        /* NMI INTERRUPT FUNCTION            */
 
#define CQ_ALIASED       0x2000        /* ADDRESS OF SYMBOL TAKEN SOMEWHERE */
#define CQ_WRITTEN       0x4000        /* VARIABLE IS MODIFIED BY CODE      */
#define CQ_TOUCHED       0x8000        /* VARIABLE IS REFERENCED BY CODE    */

#define CQ_ICODE (  CQ_NEAR | CQ_INLINE   | CQ_CONTROL   | CQ_TRAP       \
		  | CQ_FAR  | CQ_SUPPRESS | CQ_INTERRUPT | CQ_GREGISTER  \
		  | CQ_PORT | CQ_SYSTEM   | CQ_SYSTEMPP  | CQ_REENTRANT  \
		  | CQ_NMI_INTERRUPT)
							
/*------------------------------------------------------------------------*/
/* STORAGE CLASS MACROS                                                   */
/*------------------------------------------------------------------------*/
#define ISLOCAL(c) ((c) == C_AUTO   || (c) == C_REG     || (c) == C_VREG)
#define ISPARM(c)  ((c) == C_ARG    || (c) == C_REGPARM || (c) == C_VARARG)
#define ISAUTO(c)  ((c) == C_AUTO   || (c) == C_ARG     || (c) == C_VARARG)
#define ISREG(c)   ((c) == C_REG    || (c) == C_REGPARM || (c) == C_VREG || \
                    (c) == C_EXTREG || (c) == C_STATREG || (c) == C_EXTREFREG)
#define ISTAG(c)   ((c) == C_STRTAG || (c) == C_UNTAG   || (c) == C_ENTAG)
#define ISGROUP(c) ((c) == C_GRPTAG)

#define ISMOS(c)   ((c) == C_MOS    || (c) == C_MOU     || \
		    (c) == C_MOE    || (c) == C_FIELD)

#define ISXDEF(c)  ((c) == C_STAT   || (c) == C_STATREG || \
                    (c) == C_EXT    || (c) == C_EXTDEF  || (c) == C_EXTREG)

#define ISEXT(c)   ((c) == C_USTATIC || (c) == C_STAT   || (c) == C_STATREG  ||\
                    (c) == C_EXTREF  || (c) == C_UEXT   || (c) == C_EXTREFREG||\
                    (c) == C_EXT     || (c) == C_EXTDEF || (c) == C_EXTREG   ||\
		    (c) == C_EXTLAB  || (c) == C_SYSTEM)

#define ISGLOB(c)  ((c) == C_EXTREF  || (c) == C_UEXT   || (c) == C_EXTREFREG||\
                    (c) == C_EXT     || (c) == C_EXTDEF || (c) == C_EXTREG   ||\
		    (c) == C_EXTLAB  || (c) == C_SYSTEM)

#define ISNEAR(cq)          (((cq) & CQ_NEAR) != 0)
#define ISFAR(cq)           (((cq) & CQ_FAR) != 0)
#define ISCONTROL(cq)       (((cq) & CQ_CONTROL) != 0)
#define ISGREGISTER(cq)     (((cq) & CQ_GREGISTER) != 0)
#define ISPORT(cq)          (((cq) & CQ_PORT)    != 0)
#define ISINTERRUPT(cq)     (((cq) & CQ_INTERRUPT) != 0)
#define ISNMIINTERRUPT(cq)  (((cq) & CQ_NMI_INTERRUPT) != 0)
#define ISREENTRANT(cq)     (((cq) & CQ_REENTRANT) != 0)
#define ISTRAP(cq)          (((cq) & CQ_TRAP) != 0)
#define ISINT_OR_TRAP(cq)   (((cq) & (CQ_TRAP | CQ_INTERRUPT)) != 0)

/*-------------------------------------------------------------------------*/
/* COFF BASIC TYPES - PACKED INTO THE LOWER 4 BITS OF THE TYPE FIELD       */
/*-------------------------------------------------------------------------*/
#define  CT_NULL     0x80       /* UNDEFINED OR ERROR TYPE (NO TYPE INFO)  */
#define  CT_VOID     0          /* VOID TYPE                               */
#define  CT_SCHAR    1          /* CHARACTER (EXPLICITLY "signed")         */
#define  CT_CHAR     2          /* CHARACTER (IMPLICITLY SIGNED)           */
#define  CT_SHORT    3          /* SHORT INTEGER                           */
#define  CT_INT      4          /* INTEGER                                 */
#define  CT_LONG     5          /* LONG INTEGER                            */
#define  CT_FLOAT    6          /* SINGLE PRECISION FLOATING POINT         */
#define  CT_DOUBLE   7          /* DOUBLE PRECISION FLOATING POINT         */
#define  CT_STRUCT   8          /* STRUCTURE                               */
#define  CT_UNION    9          /* UNION                                   */
#define  CT_ENUM     10         /* ENUMERATION                             */
#define  CT_LDOUBLE  11         /* LONG DOUBLE FLOATING POINT              */
#define  CT_UCHAR    12         /* UNSIGNED CHARACTER                      */
#define  CT_USHORT   13         /* UNSIGNED SHORT                          */
#define  CT_UINT     14         /* UNSIGNED INTEGER                        */
#define  CT_ULONG    15         /* UNSIGNED LONG                           */

/*-------------------------------------------------------------------------*/
/* COFF DERIVED TYPES: 2 BITS EACH                                         */
/*-------------------------------------------------------------------------*/
#define  DCT_NON     0x0        /* NO DERIVED TYPE                         */
#define  DCT_PTR     0x1        /* POINTER                                 */
#define  DCT_FCN     0x2        /* FUNCTION                                */
#define  DCT_ARY     0x3        /* ARRAY                                   */

/*-------------------------------------------------------------------------*/
/* COFF TYPE FIELD MASKS AND SIZES                                         */
/*-------------------------------------------------------------------------*/
#define  N_BCTMASK     0xF      /* MASK FOR BASIC TYPE                     */
#define  N_CTMASK      0x30     /* MASK FOR FIRST DERIVED TYPE             */
#define  N_DCTMAX      12       /* MAXIMUM DERIVED TYPES                   */
#define  N_BCTSHFT     4        /* SHIFT AMOUNT (WIDTH) FOR BASIC TYPE     */
#define  N_CTSHIFT     2        /* SHIFT AMOUNT (WIDTH) FOR DERIVED TYPES  */

/*-------------------------------------------------------------------------*/
/* COFF TYPE MANIPULATION MACROS                                           */
/*                                                                         */
/*    BCTYPE(t)    - Return basic type from t                              */
/*    DCTYPE(t)    - Return all derived types from t                       */
/*    DCTYPE1(t)   - Return 1st derived type from t                        */
/*    CTQUAL(t,q)  - Return qualification of type                          */
/*    CTUNQUAL(t,q)- Return unqualified version of type                    */
/*                                                                         */
/*    MKCTYPE()    - Build a type from basic and several derived types     */
/*    CDERIVE(d,t) - Build a type from basic and one derived type          */
/*    CINCREF(t)   - Convert 't' into pointer to 't'                       */
/*    CDECREF(t)   - Remove first derviation from t                        */
/*                                                                         */
/*    ISCINT(t)    - TRUE if t is an integral type                         */
/*    ISCSGN(t)    - TRUE if t is a signed type                            */
/*    ISCUNS(t)    - TRUE if t is an unsigned type                         */
/*    ISCFLT(t)    - TRUE if t is a floating point type                    */
/*    ISCDBL(t)    - TRUE if t is a double or long double type             */
/*    ISCPTR(t)    - TRUE if t is a pointer                                */
/*    ISCFCN(t)    - TRUE if t is a function                               */
/*    ISCARY(t)    - TRUE if t is an array                                 */
/*    ISCSTR(t)    - TRUE if t is a struct, union, or enum type            */
/*    ISCAGG(t)    - TRUE if t is an array, struct, or union               */
/*                                                                         */
/*    CITOU(t)     - convert signed type to unsigned equivalent            */
/*    CUTOI(t)     - convert unsigned type to signed equivalent            */
/*    CNOSIGN(t)   - convert signed or unsigned type to "plain" equivalent */
/*-------------------------------------------------------------------------*/
#define BCTYPE(t)    ((int)((t) & N_BCTMASK))  
#define DCTYPE(t)    ((t) & ~N_BCTMASK)
#define DCTYPE1(t)   (((t) & N_CTMASK) >> N_BCTSHFT)

#define CTQUAL(t,q)  ((int)(DCTYPE(t) ? DCTYPE1(q) : BCTYPE(q)))
#define CTUNQUAL(t,q) (ISCPTR(t) ? ((q) & ~N_CTMASK) : ((q) & ~N_BCTMASK))

#define MKCTYPE(basic, d1,d2,d3,d4,d5,d6) \
                    ((basic) | ((d1) <<  4) | ((d2) <<  6) | ((d3) <<  8) |\
                               ((d4) << 10) | ((d5) << 12) | ((d6) << 14))
#define CDERIVE(d,t) ((DCTYPE(t) << N_CTSHIFT) | ((d) << N_BCTSHFT) | BCTYPE(t))
#define CINCREF(t)   ((DCTYPE(t) << N_CTSHIFT) | (DCT_PTR<<N_BCTSHFT)|BCTYPE(t))
#define CDECREF(t)   (DCTYPE((t) >> N_CTSHIFT) | BCTYPE(t))

#define ISCSGN(t)    (((t) >= CT_SCHAR && (t) <= CT_LONG)  || (t) == CT_ENUM)
#define ISCUNS(t)    ((t) >= CT_UCHAR  && (t) <= CT_ULONG)
#define ISCINT(t)    (((t) >= CT_SCHAR && (t) <= CT_LONG)  || (t) == CT_ENUM ||\
                      ((t) >= CT_UCHAR  && (t) <= CT_ULONG))
#define ISCFLT(t)    ((t) == CT_FLOAT || (t) == CT_DOUBLE || (t) == CT_LDOUBLE)
#define ISCDBL(t)    ((t) == CT_DOUBLE || (t) == CT_LDOUBLE)
#define ISCPTR(t)    (((t) & N_CTMASK) == (DCT_PTR << N_BCTSHFT)) 
#define ISCFCN(t)    (((t) & N_CTMASK) == (DCT_FCN << N_BCTSHFT))
#define ISCARY(t)    (((t) & N_CTMASK) == (DCT_ARY << N_BCTSHFT))
#define ISCSTR(t)    ((t) == CT_STRUCT || (t) == CT_UNION || (t) == CT_ENUM)
#define ISCAGG(t)    (ISCARY(t) || (t) == CT_STRUCT || (t) == CT_UNION)
#define ISCCHAR(t)   ((t) == CT_CHAR || (t) == CT_SCHAR || (t) == CT_UCHAR)
#define ISCSHORT(t)  ((t) == CT_SHORT || (t) == CT_USHORT)
#define ISCLONG(t)   ((t) == CT_LONG || (t) == CT_ULONG)

#define CITOU(t)     ((t) + (CT_UCHAR - ((t) == CT_SCHAR ? CT_SCHAR : CT_CHAR)))
#define CUTOI(t)     ((t) - (CT_UCHAR - CT_CHAR))
#define CNOSIGN(t)   (ISCUNS(t) ? CUTOI(t) : (t) == CT_SCHAR ? CT_CHAR : (t))

/*------------------------------------------------------------------------*/
/* ILLEGAL COFF TYPES USED TO MARK SPECIAL OBJECTS.                       */
/*                                                                        */
/* CT_VENEER  - DENOTES A VENEER OF A FUNCTION.                           */
/*------------------------------------------------------------------------*/
#define CT_VENEER     (MKCTYPE(CT_VOID, DCT_FCN, DCT_FCN, 0, 0, 0, 0))
#define ISCVENEER(t)  ((t) == CT_VENEER)

#endif /* COFFDEFS_H */

