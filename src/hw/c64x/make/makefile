#*
#*
#* Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/ 
#* 
#* 
#*  Redistribution and use in source and binary forms, with or without 
#*  modification, are permitted provided that the following conditions 
#*  are met:
#*
#*    Redistributions of source code must retain the above copyright 
#*    notice, this list of conditions and the following disclaimer.
#*
#*    Redistributions in binary form must reproduce the above copyright
#*    notice, this list of conditions and the following disclaimer in the 
#*    documentation and/or other materials provided with the   
#*    distribution.
#*
#*    Neither the name of Texas Instruments Incorporated nor the names of
#*    its contributors may be used to endorse or promote products derived
#*    from this software without specific prior written permission.
#*
#*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
#*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
#*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
#*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
#*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
#*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
#*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
#*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
#*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
#*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
#*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#*


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

ifeq ($(TARGET),c6474l)
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

ifeq ($(TARGET),c6457)
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








