#*************************************************************************************
#* FILE NAME: makefile
#*
#* DESCRIPTION: Makes the ethernet module for the IBL
#*
#*************************************************************************************/

ifndef IBL_ROOT
	export IBL_ROOT= ../../..
endif

ECODIR= $(IBL_ROOT)/ethboot

CSRC= ethboot.c

.PHONY: ethboot


include $(IBL_ROOT)/make/$(ARCH)/makeeco.mk



C6X_C_DIR+=  $(IBL_ROOT)
C6X_C_DIR+= ;$(IBL_ROOT)/arch/$(ARCH)
C6X_C_DIR+= ;$(IBL_ROOT)/device
C6X_C_DIR+= ;$(IBL_ROOT)/device/$(TARGET)
C6X_C_DIR+= ;$(IBL_ROOT)/ethboot
C6X_C_DIR+= ;$(IBL_ROOT)/driver/eth
C6X_C_DIR+= ;$(STDINC)
C6X_C_DIR+= ;$(IBL_ROOT)/hw/macs
C6X_C_DIR+= ;$(IBL_ROOT)/hw/mdio
C6X_C_DIR+= ;$(IBL_ROOT)/hw/sgmii
export C6X_C_DIR



ethboot: gen_cdefdep makefile $(OBJS)


$(OBJS): cdefdep

gen_cdefdep:
	@echo Checking command line dependencies
	@echo $(ENDIAN) $(TARGET) $(CDEFS) > cdefdep.tmp
	@sh -c 'if diff -q cdefdep.tmp cdefdep ; then echo same ; else $(CP) cdefdep.tmp cdefdep ; fi '


