#*******************************************************************************
#* FILE PURPOSE: Perform standard variable computation and targets for ECOs
#*******************************************************************************
#* FILE NAME: makeeco.mk
#*
#* DESCRIPTION: Make include file which computes some variables and defines
#*    standard targets for DSP ECOs.
#*
#*    Input variables:
#*         CSRC   : List of C Source files
#*         ASMSRC : List of assembly source files
#*    Output variables:
#*         OBJS   : List of all object files
#*
#*              
#* (C) Copyright 1999-2001 TELOGY Networks, Inc.             
#*******************************************************************************

# get definitions
include $(IBL_ROOT)/make/$(ARCH)/makedefs.mk

# If ECODIR is not defined, derive it from ECONAME
ifndef ECODIR
   ECODIR = $(IBL_ROOT)/$(ECONAME)
endif

# VPATH tells make how to find sources in other directories
vpath %.$(ASMEXT) $(ECODIR)/$(CPUDIR)
#The vpath below is needed to compile the dsp/memory folder properly
vpath %.c $(ECODIR)/$(CPUDIR)/$(CPUTYPEDIR)
vpath %.c $(ECODIR)/$(CPUDIR)
vpath %.sa $(ECODIR)/$(CPUDIR)
vpath %.s62 $(ECODIR)/$(CPUDIR)
vpath % $(ECODIR)

# Get filenames w/o paths

CFILES     = $(foreach file, $(CSRC), $(notdir $(file)))
AFILES     = $(foreach file, $(ASMSRC), $(notdir $(file)))
LINAFILES  = $(foreach file, $(LINASMSRC), $(notdir $(file)))
BIOSSFILES = $(foreach file, $(BIOSASMSRC), $(notdir $(file)))
EXCFILES   = $(foreach file, $(EXCSRC), $(notdir $(file)))
EXAFILES   = $(foreach file, $(EXASMSRC), $(notdir $(file)))
TESTCFILES = $(foreach file, $(TESTCSRC), $(notdir $(file)))
TESTAFILES = $(foreach file, $(TESTASMSRC), $(notdir $(file)))
TESTLINASMFILES = $(foreach file, $(TESTLINASMSRC), $(notdir $(file)))


DFULLPATH = $(patsubst %.s62, %.$(ADEPEXT),$(BIOSSFILES)  \
              $(patsubst %.c, %.$(CDEPEXT), $(CFILES) $(EXCFILES) $(TESTCFILES)) \
              $(patsubst %.$(ASMEXT), %.$(ADEPEXT), $(AFILES) $(EXAFILES) $(TESTAFILES) $(BIOSSFILES)) \
              $(patsubst %.sa, %.$(LDEPEXT), $(LINAFILES) $(TESTLINASMFILES)))
			

OBJS  = $(COBJS) $(AOBJS)
COBJS = $(patsubst %.c, %.$(COBJEXT), $(CFILES))
AOBJS = $(patsubst %.s62, %.$(AOBJEXT), $(BIOSSFILES) \
         $(patsubst %.$(ASMEXT), %.$(AOBJEXT), $(AFILES)) \
         $(patsubst %.sa, %.$(LOBJEXT), $(LINAFILES)))
REFTGT = 

DCS  = $(CDCS) $(ADCS)
CDCS = $(patsubst %.c, %.$(CDEPEXT), $(CFILES))
ADCS = $(patsubst %.s62, %.$(CDEPEXT), $(BIOSSFILES) \
         $(patsubst %.$(ASMEXT), %.$(CDEPEXT), $(AFILES)) \
         $(patsubst %.sa, %.$(CDEPEXT), $(LINAFILES)))


# extra objects (not built by builtin rules)
EXOBJS = $(patsubst %.s62, %.$(AOBJEXT), $(BIOSSFILES) \
           $(patsubst %.c, %.$(COBJEXT), $(EXCFILES)) \
           $(patsubst %.$(ASMEXT), %.$(AOBJEXT), $(EXAFILES)))
 
# extra objects (used for test code)
TESTOBJS = $(patsubst %.s62, %.$(AOBJEXT), $(BIOSSFILES) \
             $(patsubst %.c, %.$(COBJEXT), $(TESTCFILES)) \
             $(patsubst %.$(ASMEXT), %.$(AOBJEXT), $(TESTAFILES)) \
             $(patsubst %.$(LASMEXT), %.$(LOBJEXT), $(TESTLINASMFILES)))
 
# intermediate files
CLEAN_INTERMEDIATES = $(patsubst %.s62, %.lst, $(BIOSSFILES)  \
                        $(patsubst %.c, %.asm, $(CFILES) $(EXCFILES) $(TESTCFILES)) \
                        $(patsubst %.c, %.lst, $(CFILES) $(EXCFILES) $(TESTCFILES)) \
                        $(patsubst %.c, %.cif, $(CFILES) $(EXCFILES) $(TESTCFILES)) \
                        $(patsubst %.$(ASMEXT), %.lst, $(AFILES) $(EXAFILES) $(TESTAFILES)) \
						$(patsubst %.sa, %.asm, $(LINAFILES) $(TESTLINASMFILES)) \
						$(patsubst %.sa, %.lst, $(LINAFILES) $(TESTLINASMFILES)) \
                        $(DFULLPATH))

# Compute FLATFILES, if necessary
ifndef FLATFILES
  FLATFILES  =$(patsubst %,$(ECODIR)/%,$(CSRC) $(ASMSRC) $(HDR) $(LINASMSRC) $(BIOSASMSRC) )
endif

# If FLATGGDIR isnt set, and we make flat, then we are making flat for test prg 
ifndef FLATGGDIR
  # This has two magical effects: 1) causes FLATGGDIR to be set now, and
  # causes recursive makefiles to set the correct FLAT directory even during
  # recursion.
  FLATFILES +=$(patsubst %,$(ECODIR)/%,$(TESTCSRC) $(TESTASMSRC) $(TESTHDR))
  ifdef ECONAME
    export FLATGGDIR = $(ECONAME)/c64x/make/flat
  else
    export FLATGGDIR = $(notdir $(ECODIR))/c64x/make/flat
  endif
endif

FLATDIR    =$(ROMROOT)/$(FLATGGDIR)

ifneq ($(findstring $(FLAT),yes YES),)
  FLAT_TARGET =$(REFTGT) flat
else
  FLAT_TARGET =$(REFTGT)
endif

.PHONY: clean flat gen_flatdir 

# standard targets
clean:
	-$(RM) $(OBJS) $(EXOBJS) $(TESTOBJS) $(CLEAN_TARGETS) $(CLEAN_INTERMEDIATES) $(REFTGT)
	-$(RM) -r flat

ifneq ($(findstring $(FLAT),yes YES),)
flat: 
	-mkdir $(subst /,\,$(FLATDIR))
	-$(CP) -u $(FLATFILES) $(FLATDIR)

# Define command to copy targets to flat
CPFLATCMD = $(CP) $@ $(FLATDIR)

else

CPFLATCMD =

endif

# Copy targets to delivery directory
# The variables STDDLV and BOOTLD should be set by calling makefile
ifneq ($(BOOTLD),none)
   CPBOOTLD=$(CP) $(BOOTPATH)/$(BOOTLD) $(DELIVERY)/boot_$(OUTFILE).out
   CPBOOTLD2=
else
   ifeq ($(GGDSP_CPU),12)
     CPBOOTLD=$(CP) $(BOOTPATH)/boot5560_pg10_20.out $(DELIVERY)/bootpg10_20_$(OUTFILE).out
     CPBOOTLD2=
   else
     CPBOOTLD=
     CPBOOTLD2=
   endif
endif

CDEPINC = $(subst \,/,-I$(subst ;, -I,$(C6X_C_DIR)))
ADEPINC = $(subst \,/,-I$(subst ;, -I,$(C6X_A_DIR)))

CFLAGS+=$(CDEPINC)

# how to build .c and .s files on c64x
FNAMEDEF   = -dTNFNAME=$(notdir $<)
GGCCOMPILE = $(CC) $(CFLAGS) $(DCFLAGS) $(CDEFS) $(FNAMEDEF) $(FUNCOPTS) $< -eo.$(COBJEXT)
GGLCOMPILE = $(CC) $(CFLAGS) $(DCFLAGS) $(CDEFS) $(FNAMEDEF) $< -eo.$(LOBJEXT)
GGACOMPILE = $(AS) $(AFLAGS) $(ADEFS) $< -eo.$(AOBJEXT)
GGCMAKEDEP = $(CC) $(PPDEP_FLAGS) $(CFLAGS) $(DCFLAGS) $(CDEFS) $(FNAMEDEF) $(FUNCOPTS) $<
GGLMAKEDEP = $(CC) $(PPDEP_FLAGS) $(CFLAGS) $(DCFLAGS) $(CDEFS) $(FNAMEDEF) $< 
GGAMAKEDEP = $(MAKEDEP) $(MAKEDEP_OPT) $(ADEPINC) -a -e$(AOBJEXT) -o$$.$(ADEPEXT) $<
GGALMAKEDEP = $(MAKEDEP) $(MAKEDEP_OPT) $(ALDEPINC) -a -e$(ALOBJEXT) -o$$.$(ALDEPEXT) $<

#Two options of invoking blockexpn:
# 1)Passing NUMCH for implicit expansion 
# 2)Explicitly giving the number of expansions in the .bex file
GGBLOCKEXPN = $(BLOCKEXPN) $< $@
GGBLOCKEXPNNUMCH = $(BLOCKEXPN) $< $@ $(NUMCH)

%.$(COBJEXT): %.c 
	$(GGCCOMPILE)

# Compilation of Linear Assembly files
%.$(LOBJEXT): %.$(LASMEXT)
	$(GGLCOMPILE)

%.$(AOBJEXT): %.$(ASMEXT)
	$(GGACOMPILE)

# Special compilation for BIOS
%.$(AOBJEXT): %.s62
	$(GGACOMPILE)


ifeq ($(OS),Linux)
%.$(CDEPEXT): %.c
	$(GGCMAKEDEP)
	sed -e 's/$(notdir $*).obj[ ]*:[ ]*/$(notdir $*).oc $(subst /,\/,$@): /' < $(patsubst %.$(CDEPEXT),%.$(CDEPEXT)_TMP,$@) > $(patsubst %.$(CDEPEXT),%.$(CDEPEXT)_TMP_2,$@)
	sed -e 's/\\/\//g' < $(patsubst %.$(CDEPEXT),%.$(CDEPEXT)_TMP_2,$@) > $@
	@$(RM) $(patsubst %.$(CDEPEXT),%.$(CDEPEXT)_TMP,$@) $(patsubst %.$(CDEPEXT),%.$(CDEPEXT)_TMP_2,$@)
else
%.$(CDEPEXT): %.c
	$(GGCMAKEDEP)
	sed -e 's/$(notdir $*).obj[ ]*:[ ]*/$(notdir $*).oc $(subst /,\/,$@): /' < $(patsubst %.$(CDEPEXT),%.$(CDEPEXT)_TMP,$@) > $(patsubst %.$(CDEPEXT),%.$(CDEPEXT)_TMP_2,$@)
	sed -e "s/\\/\//g" < $(patsubst %.$(CDEPEXT),%.$(CDEPEXT)_TMP_2,$@) > $@
	@$(RM) $(patsubst %.$(CDEPEXT),%.$(CDEPEXT)_TMP,$@) $(patsubst %.$(CDEPEXT),%.$(CDEPEXT)_TMP_2,$@)

endif
# dependency for .sa files
%.$(LDEPEXT): %.sa
	$(GGLMAKEDEP)

%.$(ADEPEXT): %.s
	$(GGAMAKEDEP)

%.$(ADEPEXT): %.s62
	$(GGAMAKEDEP)

# Preprocessing via blockexpn.pl
# ggmbuf.bec and ggmheap.bec are not build specific but common.They need to use the
# NUMCH parameter
%.c: %.bec
	$(GGBLOCKEXPNNUMCH)

#Numch parameter not passed to linker command file / memmap as the number of expansion 
#required  may be different from num channels in the build
%.h: %.beh
	$(GGBLOCKEXPN)

%.cmd: %.becmd
	$(GGBLOCKEXPN)	

# Reflection page symbol tables
# %.oc is included so we inherit the dependancies from the .dc file
# Set GGREFDBG=-v# on command line to debug
%.ref: %.c %.oc $(REFPAGEDEP)
	$(REFPAGE) $(GGREFDBG) -iC6X_C_DIR $< -s$@ $(CDEFS) -dGG_REF_PAGE -d6416 -t$(REFSYM)
	
ifneq ($(DOMAKEDEP),no)
# include generated dependancies.
sinclude $(DFULLPATH)
endif

# Nothing past this point
