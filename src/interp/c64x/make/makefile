#****************************************************************************
#* FILE NAME: makefile
#*
#* DESCRIPTION: Makes the data parsers for the IBL
#*
#****************************************************************************


ifndef IBL_ROOT
	export IBL_ROOT=../../..
endif

ECODIR= $(IBL_ROOT)/interp

# BIS 
CSRC= bis.c 

# COFF
CSRC += cload.c osal.c 

# TI BOOT TABLE
CSRC += btblpr.c btblwrap.c gem.c 

# Blob
CSRC += blob.c

# elf loader files
CSRC += dload.c elfwrap.c dlw_client.c dload_endian.c ArrayList.c


.PHONY: interp


include $(IBL_ROOT)/make/$(ARCH)/makeeco.mk


C6X_C_DIR=	 $(IBL_ROOT)
C6X_C_DIR+= ;$(IBL_ROOT)/arch/$(ARCH)
C6X_C_DIR+= ;$(ECODIR)
C6X_C_DIR+= ;$(STDINC)
C6X_C_DIR+= ;$(IBL_ROOT)/cfg/$(TARGET)
export C6X_C_DIR


vpath % $(ECODIR)/bis
vpath % $(ECODIR)/coff
vpath % $(ECODIR)/btbl
vpath % $(ECODIR)/blob
vpath % $(ECODIR)/elf

interp: gen_cdefdep makefile $(OBJS)


$(OBJS): cdefdep



gen_cdefdep:
	@echo Checking command line dependencies
	@echo $(ENDIAN) $(TARGET) $(ARCH) > cdefdep.tmp
	@sh -c 'if diff -q cdefdep.tmp cdefdep ; then echo same ; else $(CP) cdefdep.tmp cdefdep ; fi '


