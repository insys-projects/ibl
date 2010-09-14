#*************************************************************************
#* FILE PURPOSE: Build the IBL device specific module
#*************************************************************************
#* FILE NAME: makefile
#*
#* DESCRIPTION: Makes the device specific modules
#*
#*************************************************************************

ifndef IBL_ROOT
	export IBL_ROOT=../../..
endif

ECODIR= $(IBL_ROOT)/device

ifeq ($(TARGET),c6472)
 CSRC= c6472.c
endif

ifeq ($(TARGET),c6455)
 CSRC= c6455.c
endif

.PHONY: device


include $(IBL_ROOT)/make/$(ARCH)/makeeco.mk


C6X_C_DIR=   $(IBL_ROOT)/device/$(TARGET)
C6X_C_DIR+= ;$(IBL_ROOT)/device
C6X_C_DIR+= ;$(IBL_ROOT)
C6X_C_DIR+= ;$(IBL_ROOT)/arch/$(ARCH)
C6X_C_DIR+= ;$(STDINC)
C6X_C_DIR+= ;$(IBL_ROOT)/hw/plls
C6X_C_DIR+= ;$(IBL_ROOT)/hw/pscs
C6X_C_DIR+= ;$(IBL_ROOT)/hw/gpio
C6X_C_DIR+= ;$(IBL_ROOT)/hw/ddrs/emif31
export C6X_C_DIR

vpath % $(IBL_ROOT)/device/$(TARGET)


device: gen_cdefdep makefile $(OBJS)


$(OBJS): cdefdep


gen_cdefdep:
	@echo CSRC= $(CSRC)
	@echo Checking command line dependencies
	@echo $(ENDIAN) $(TARGET) $(ARCH) > cdefdep.tmp
	@sh -c 'if diff -q cdefdep.tmp cdefdep ; then echo same ; else $(CP) cdefdep.tmp cdefdep ; fi '



