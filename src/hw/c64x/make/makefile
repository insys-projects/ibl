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

# If no target specified, set CSRC to all files (used for clean)
ifeq ($(TARGET),c6472)
 CSRC= t64.c cpmacdrv.c pll.c psc.c emif31.c mdio.c gpio.c nandgpio.c i2c.c nandwrgpio.c
else
 ifeq ($(TARGET),c6474)
  CSRC= t64.c cpmacdrv.c pll.c psc.c emif31.c mdio.c gpio.c nandgpio.c i2c.c nandwrgpio.c sgmii.c
 else
  ifeq ($(TARGET),c6474l)
   CSRC= t64.c cpmacdrv.c pll.c psc.c emif31.c mdio.c gpio.c nandgpio.c i2c.c nandwrgpio.c sgmii.c
  else
   ifeq ($(TARGET),c6457)
    CSRC= t64.c cpmacdrv.c pll.c psc.c emif31.c mdio.c gpio.c nandgpio.c i2c.c nandwrgpio.c sgmii.c
   else
    ifeq ($(TARGET),c6455)
     CSRC= t64.c cpmacdrv.c pll.c emif31.c mdio.c gpio.c nandgpio.c i2c.c nandwrgpio.c
    else
     CSRC= t64.c cpmacdrv.c pll.c psc.c emif31.c mdio.c gpio.c nandgpio.c i2c.c nandwrgpio.c sgmii.c
    endif
   endif
  endif
 endif
endif

CDEFS += -DIBL_I2C_CFG_EEPROM_BUS_ADDR=$(I2C_PARAM_BUS_ADDR)

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
C6X_C_DIR+= ;$(IBL_ROOT)/hw/sgmii
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

ifeq ($(TARGET),c6474)
 vpath % $(ECODIR)/macs/cpmac
 vpath % $(ECODIR)/plls/pllxx1p8
 vpath % $(ECODIR)/pscs/psc2
 vpath % $(ECODIR)/ddrs/emif31
 vpath % $(ECODIR)/mdio
 vpath % $(ECODIR)/gpio
 vpath % $(ECODIR)/nands/gpio
 vpath % $(ECODIR)/i2c
 vpath % $(ECODIR)/sgmii
endif

ifeq ($(TARGET),c6455)
 vpath % $(ECODIR)/macs/cpmac
 vpath % $(ECODIR)/plls/pllxx1p8
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
	@echo $(ENDIAN) $(TARGET) $(ARCH) $(CDEFS) $(DEBUG) > cdefdep.tmp
	@sh -c 'if diff -q cdefdep.tmp cdefdep ; then echo same ; else $(CP) cdefdep.tmp cdefdep ; fi '








