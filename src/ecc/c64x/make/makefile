#*************************************************************************************
#* FILE NAME: makefile
#*
#* DESCRIPTION: Makes the ECC module for the IBL
#*
#*************************************************************************************/

ifndef IBL_ROOT
	export IBL_ROOT= ../../..
endif

ECODIR= $(IBL_ROOT)/ecc

CSRC= 3byte_ecc.c

.PHONY: ecc


include $(IBL_ROOT)/make/$(ARCH)/makeeco.mk



C6X_C_DIR=   $(IBL_ROOT)
C6X_C_DIR+= ;$(IBL_ROOT)/arch/$(ARCH)
C6X_C_DIR+= ;$(ECODIR)
C6X_C_DIR+= ;$(STDINC)
export C6X_C_DIR

vpath % $(ECODIR)/3byte


ecc: gen_cdefdep makefile $(OBJS)


$(OBJS): cdefdep

gen_cdefdep:
	@echo Checking command line dependencies
	@echo $(ENDIAN) $(TARGET) > cdefdep.tmp
	@sh -c 'if diff -q cdefdep.tmp cdefdep ; then echo same ; else $(CP) cdefdep.tmp cdefdep ; fi '


