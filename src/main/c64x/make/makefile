#***********************************************************************
#* FILE PURPOSE: Build the IBL main module
#***********************************************************************
#* FILE NAME: makefile
#*
#* DESCRIPTION: Makes the main module for the IBL
#*
#***********************************************************************

ifndef IBL_ROOT
	export IBL_ROOT= ../../..
endif

ECODIR= $(IBL_ROOT)/main


CSRC= iblmain.c


CDEFS += -DIBL_I2C_CFG_EEPROM_BUS_ADDR=$(I2C_PARAM_BUS_ADDR)


.PHONY: main



include $(IBL_ROOT)/make/$(ARCH)/makeeco.mk


C6X_C_DIR=   $(IBL_ROOT)/main
C6X_C_DIR+= ;$(IBL_ROOT)
C6X_C_DIR+= ;$(IBL_ROOT)/interp/bis
C6X_C_DIR+= ;$(IBL_ROOT)/interp/coff
C6X_C_DIR+= ;$(IBL_ROOT)/interp/btbl
C6X_C_DIR+= ;$(IBL_ROOT)/interp/blob
C6X_C_DIR+= ;$(IBL_ROOT)/interp/elf
C6X_C_DIR+= ;$(IBL_ROOT)/arch/$(ARCH)
C6X_C_DIR+= ;$(IBL_ROOT)/device
C6X_C_DIR+= ;$(IBL_ROOT)/device/$(TARGET)
C6X_C_DIR+= ;$(IBL_ROOT)/ethboot
C6X_C_DIR+= ;$(IBL_ROOT)/driver/timer
C6X_C_DIR+= ;$(IBL_ROOT)/hw/i2c
C6X_C_DIR+= ;$(IBL_ROOT)/cfg/$(TARGET)
C6X_C_DIR+= ;$(STDINC)

# Paths to the kicker intermediate boot loader for devices which require them
ifeq ($(TARGET),c6455)
 C6X_C_DIR+= ;$(IBL_ROOT)/make/ibl_c6455
 C6X_C_DIR+= ;$(IBL_ROOT)/util/romparse
endif



export C6X_C_DIR




main: gen_cdefdep makefile $(OBJS)



$(OBJS): cdefdep

gen_cdefdep:
	@echo Checking command line dependencies
	@echo $(ENDIAN) $(TARGET) $(ARCH) $(CDEFS) $(DEBUG) > cdefdep.tmp
	@sh -c 'if diff -q cdefdep.tmp cdefdep ; then echo same ; else $(CP) cdefdep.tmp cdefdep ; fi '

