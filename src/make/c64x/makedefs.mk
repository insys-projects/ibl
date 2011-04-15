#*******************************************************************************
#* FILE PURPOSE: Define makefile variables for C64X
#*******************************************************************************
#* FILE NAME: makedefs.mk
#*
#* DESCRIPTION: Make include file which defines variables used by all DSP
#*    Makefiles.
#*              
#* (C) Copyright 1999 TELOGY Networks, Inc.             
#*******************************************************************************

ifeq ($(ENDIAN),big)
 MEXT=be
else
 MEXT=le
endif

CC          = cl6x
AS          = cl6x
LD          = cl6x -z
ARIN        = ar6x -rq
AROUT       = ar6x -xq 
HEX6X	    = hex6x
MAKEDEP     = $(MAKEDEPPATH)
MAKEDEP_OPT = -q
COBJEXT     = $(MEXT).oc
LOBJEXT     = ol
AOBJEXT     = $(MEXT).oa
CDEPEXT     = dc
LDEPEXT     = dl
ADEPEXT     = da
ASMEXT      = s
LASMEXT     = sa
LIBEXT      = lib
RM          = rm -f
CP          = cp -f
MV          = mv -f
CPUDIR      = c64x

ifeq ($(OS),Linux)
    DIRSEP=/
    PATHSEP=:
else
#DIRSEP=$(subst /,\,/)
    DIRSEP=/
    PATHSEP=;
endif


# CPU Class flags
CPUFLAGS   = -mv6400+

ifeq ($(ENDIAN),big)
  ALLFARLIB       = $(TOOLSC6XDOS)/lib/rts64pluse.lib
else
  ALLFARLIB       = $(TOOLSC6XDOS)/lib/rts64plus.lib
endif

#RTLIBS     = $(subst /,$(DIRSEP),$(ALLFARLIB))
RTLIBS     = $(subst \,/,$(ALLFARLIB))

# Common compilation flags
# -c     : disable linker
# -k     : keep .asm file
# -mi200 : set imterrupt threshold to max of 200 cycles.
# -ml3   : far memory model
# -pm    : program level optimization
# -mv6400: Target processor is 64x
# -pden  : display diagnostic number along with messages
# -pdr   : display remarks
# -pds1111: shut off #pragma symbol warnings
# -pds827: shut off #pragma symbol warnings (symbol undefined)
# -pds824: shut off #pragma symbol warnings (can be applied to file level symbol only)
# -pds837: shut off #pragma symbol warnings (can be applied to file level symbol only)
# -pds97:  shut off no source in file warning
ifneq ($(UTIL),yes)
 ifeq ($(DEBUG),yes)
  CFLAGS  = -c -k -mi200 -ml3 -ms3 -o2 -pm -pds1111 -pds827 -pds824 -pds837 -pds1037 -pds195 -pds97 -pden -pdr $(CPUFLAGS)
 else
# Always compile with -o2 as -o3 can possibly break endian neutral code */
  CFLAGS  = -c -k -mi200 -ml0 -ms3 -o2 -pm -pds1111 -pds827 -pds824 -pds837 -pds1037 -pds195 -pds97 -pden -pdr $(CPUFLAGS)
 endif
else
# Always compile with -o2 as -o3 can possibly break endian neutral code */
 CFLAGS  = -c -k -mi200 -ml3 -ms3 -o2 -pm -pds1111 -pds827 -pds824 -pds837 -pds1037 -pds195 -pds97 -pden -pdr $(CPUFLAGS)
endif

ifeq ($(ENDIAN),big)
 CFLAGS+= -me
endif

# Debug flags
# -as : keep local symbols
# -g  : enable debugging
# -ss : interlist C source and optimizer with .asm source
ifneq ($(UTIL),yes)
 ifeq ($(DEBUG),yes)
  DCFLAGS = -g -as  -ss
 else
  DCFLAGS = -as  -ss
 endif
else
 DCFLAGS = -g -as  -ss
endif


# ASM options
# -s  : place all symbols in object file
# -l  : produce listing
# -pw : warn pipelne conflicts 
# AFLAGS = -s -l -g -mk -ms $(CPUFLAGS)
AFLAGS = -g -ea.s -c -k -mi200 -ml3 -pm -al $(CPUFLAGS)

ifeq ($(ENDIAN),big)
 AFLAGS+= -me
endif

PPDEP_FLAGS  = -ppd=$(patsubst %.$(CDEPEXT),%.$(CDEPEXT)_TMP,$@)

# Include files for standard (non siu) modules.  Also used by linker to find
# library files.
FSTDINC  =$(TOOLSC6XDOS)/include;$(TOOLSC6XDOS)/lib
STDINC =$(subst /,$(DIRSEP),$(FSTDINC))

# Include files for ASM in standard (non siu) modules
STDAINC =$(subst /,$(DIRSEP),$(GGROOT)/misc/$(CPUDIR))

# Include files for this platform.  Change this for test code.
PLATINC =$(subst /,$(DIRSEP),$(GGROOT)/hw/$(CPUDIR);$(ECODIR)/gg/$(CPUDIR);$(ECODIR)/$(CPUDIR))

# Nothing past this point
