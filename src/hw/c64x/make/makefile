#**************************************************************************
#* FILE NAME: makefile
#*
#* DESCRIPTION: Makes the low level drivers for the hardware modules
#*
#**************************************************************************

ifndef IBL_ROOT
	export IBL_ROOT=../../..
endif

ECODIR= $(IBL_ROOT)/hw

ifeq ($(TARGET),c6472)
 CSRC= t64.c cpmacdrv.c pll.c psc.c emif31.c mdio.c gpio.c nandgpio.c i2c.c
endif

.PHONY: hw


include $(IBL_ROOT)/make/$(ARCH)/makeeco.mk


C6X_C_DIR=   $(IBL_ROOT)
C6X_C_DIR+= ;$(IBL_ROOT)/arch/$(ARCH)
C6X_C_DIR+= ;$(ECODIR)
C6X_C_DIR+= ;$(STDINC)
C6X_C_DIR+= ;$(IBL_ROOT)/cfg/$(TARGET)
C6X_C_DIR+= ;$(IBL_ROOT)/hw/timer
C6X_C_DIR+= ;$(IBL_ROOT)/hw/macs/cpmac
C6X_C_DIR+= ;$(IBL_ROOT)/driver/eth
C6X_C_DIR+= ;$(IBL_ROOT)/device
C6X_C_DIR+= ;$(IBL_ROOT)/device/$(TARGET)
C6X_C_DIR+= ;$(IBL_ROOT)/hw/plls
C6X_C_DIR+= ;$(IBL_ROOT)/hw/pscs
C6X_C_DIR+= ;$(IBL_ROOT)/hw/mdio
C6X_C_DIR+= ;$(IBL_ROOT)/hw/gpio
C6X_C_DIR+= ;$(IBL_ROOT)/hw/nands
C6X_C_DIR+= ;$(IBL_ROOT)/hw/nands/gpio
C6X_C_DIR+= ;$(IBL_ROOT)/hw/i2c
C6X_C_DIR+= ;$(IBL_ROOT)/ecc
export C6X_C_DIR


vpath % $(ECODIR)/timer/timer64

ifeq ($(TARGET),c6472)
 vpath % $(ECODIR)/macs/cpmac
 vpath % $(ECODIR)/plls/pllxx1p8
 vpath % $(ECODIR)/pscs/psc2
 vpath % $(ECODIR)/ddrs/emif31
 vpath % $(ECODIR)/mdio
 vpath % $(ECODIR)/gpio
 vpath % $(ECODIR)/nands/gpio
 vpath % $(ECODIR)/i2c
endif


hw: gen_cdefdep makefile $(OBJS)

$(OBJS): cdefdep


gen_cdefdep:
	@echo Checking command line dependencies
	@echo $(ENDIAN) $(TARGET) $(ARCH) > cdefdep.tmp
	@sh -c 'if diff -q cdefdep.tmp cdefdep ; then echo same ; else $(CP) cdefdep.tmp cdefdep ; fi '








