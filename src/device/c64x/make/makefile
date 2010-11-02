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

# For no target set CSRC to all sources for clean
ifeq ($(TARGET),c6472)
 CSRC= c6472.c
else 
 ifeq ($(TARGET),c6455)
  CSRC= c6455.c
 else
  ifeq ($(TARGET),c6474)
   CSRC= c6474.c
  else
   ifeq ($(TARGET),c6474l)
    CSRC= c6474l.c
   else
    ifeq ($(TARGET),c6457)
     CSRC= c6457.c
    else
     CSRC= c6472.c c6455.c c6474.c c6474l.c c6457.c 
    endif
   endif 
  endif
 endif
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
	@echo Checking command line dependencies
	@echo $(ENDIAN) $(TARGET) $(ARCH) $(CDEFS) $(DEBUG) > cdefdep.tmp
	@sh -c 'if diff -q cdefdep.tmp cdefdep ; then echo same ; else $(CP) cdefdep.tmp cdefdep ; fi '



