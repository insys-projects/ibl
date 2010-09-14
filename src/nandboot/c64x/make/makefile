#*************************************************************************************
#* FILE NAME: makefile
#*
#* DESCRIPTION: Makes the nand module for the IBL
#*
#*************************************************************************************/

ifndef IBL_ROOT
	export IBL_ROOT= ../../..
endif

ECODIR= $(IBL_ROOT)/nandboot

CSRC= nandboot.c

.PHONY: nandboot


include $(IBL_ROOT)/make/$(ARCH)/makeeco.mk



C6X_C_DIR+=  $(IBL_ROOT)
C6X_C_DIR+= ;$(IBL_ROOT)/arch/$(ARCH)
C6X_C_DIR+= ;$(IBL_ROOT)/device
C6X_C_DIR+= ;$(IBL_ROOT)/device/$(TARGET)
C6X_C_DIR+= ;$(IBL_ROOT)/nandboot
C6X_C_DIR+= ;$(IBL_ROOT)/driver/nand
C6X_C_DIR+= ;$(STDINC)
export C6X_C_DIR



nandboot: gen_cdefdep makefile $(OBJS)


$(OBJS): cdefdep

gen_cdefdep:
	@echo Checking command line dependencies
	@echo $(ENDIAN) $(TARGET) > cdefdep.tmp
	@sh -c 'if diff -q cdefdep.tmp cdefdep ; then echo same ; else $(CP) cdefdep.tmp cdefdep ; fi '


