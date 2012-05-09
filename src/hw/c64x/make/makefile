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
 CSRC= t64.c cpmacdrv.c pll.c psc.c emif31.c mdio.c gpio.c nandgpio.c i2c.c null_uart.c
else
 ifeq ($(TARGET),c6474)
  CSRC= t64.c cpmacdrv.c pll.c psc.c emif31.c mdio.c gpio.c nandgpio.c i2c.c sgmii.c null_uart.c
 else
  ifeq ($(TARGET),c6474l)
   CSRC= t64.c cpmacdrv.c pll.c psc.c emif31.c mdio.c gpio.c nandgpio.c i2c.c sgmii.c null_uart.c
  else
   ifeq ($(TARGET),c6457)
    CSRC= t64.c cpmacdrv.c pll.c psc.c emif31.c mdio.c gpio.c nandgpio.c i2c.c sgmiicur.c null_uart.c
   else
    ifeq ($(TARGET),c6455)
     CSRC= t64.c cpmacdrv.c pll.c emif31.c mdio.c gpio.c nandgpio.c i2c.c null_uart.c
        else
         ifeq ($(TARGET),c66x)
           CSRC= t64.c pll.c cfgpll.c cfgpll2.c mdio.c i2c.c psc.c cpsw.c qm.c cpdma.c pa.c sgmii.c serdes.c gmacsl.c emif4.c gpio.c
           CSRC+= nandemif25.c nandgpio.c spi.c nandspi.c noremif25.c norspi.c emif25.c spiutil.c c66x_uart.c
             else
              ifeq ($(TARGET),c665x)
                CSRC= t64.c pll.c cfgpll.c cfgpll2.c mdio.c i2c.c psc.c mdio.c serdes.c sgmii.c emif4.c gpio.c cpmacdrv.c
                CSRC+= nandemif25.c nandgpio.c spi.c nandspi.c noremif25.c norspi.c emif25.c spiutil.c c66x_uart.c	
         else
          CSRC= t64.c cpmacdrv.c pll.c psc.c emif31.c mdio.c gpio.c nandgpio.c i2c.c sgmii.c cfgpll.c cfgpll2.c
          CSRC+= qm.c cpdma.c pa.c serdes.c gmacsl.c emif4.c nandemif25.c spi.c nandspi.c noremif25.c norspi.c emif25.c spiutil.c
      endif
     endif
    endif
   endif
  endif
 endif
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
C6X_C_DIR+= ;$(IBL_ROOT)/hw/macs
C6X_C_DIR+= ;$(IBL_ROOT)/hw/macs/cpmacsl
C6X_C_DIR+= ;$(IBL_ROOT)/driver/eth
C6X_C_DIR+= ;$(IBL_ROOT)/device
C6X_C_DIR+= ;$(IBL_ROOT)/device/$(TARGET)
C6X_C_DIR+= ;$(IBL_ROOT)/hw/plls
C6X_C_DIR+= ;$(IBL_ROOT)/hw/pscs
C6X_C_DIR+= ;$(IBL_ROOT)/hw/mdio
C6X_C_DIR+= ;$(IBL_ROOT)/hw/gpio
C6X_C_DIR+= ;$(IBL_ROOT)/hw/nands
C6X_C_DIR+= ;$(IBL_ROOT)/hw/nands/gpio
C6X_C_DIR+= ;$(IBL_ROOT)/hw/nands/emif25
C6X_C_DIR+= ;$(IBL_ROOT)/hw/nands/spi
C6X_C_DIR+= ;$(IBL_ROOT)/hw/i2c
C6X_C_DIR+= ;$(IBL_ROOT)/hw/sgmii
C6X_C_DIR+= ;$(IBL_ROOT)/hw/cpsw
C6X_C_DIR+= ;$(IBL_ROOT)/hw/cpdma
C6X_C_DIR+= ;$(IBL_ROOT)/hw/pa
C6X_C_DIR+= ;$(IBL_ROOT)/hw/spi
C6X_C_DIR+= ;$(IBL_ROOT)/hw/nors
C6X_C_DIR+= ;$(IBL_ROOT)/hw/emif25
C6X_C_DIR+= ;$(IBL_ROOT)/hw/uart
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
 vpath % $(ECODIR)/uart/null_uart
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
 vpath % $(ECODIR)/uart/null_uart
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
 vpath % $(ECODIR)/uart/null_uart
endif

ifeq ($(TARGET),c6457)
 vpath % $(ECODIR)/macs/cpmac
 vpath % $(ECODIR)/plls/pllcur
 vpath % $(ECODIR)/pscs/psccur
 vpath % $(ECODIR)/ddrs/emif31
 vpath % $(ECODIR)/mdio
 vpath % $(ECODIR)/gpio
 vpath % $(ECODIR)/nands/gpio
 vpath % $(ECODIR)/i2c
 vpath % $(ECODIR)/sgmii
 vpath % $(ECODIR)/uart/null_uart
endif

ifeq ($(TARGET),c6455)
 vpath % $(ECODIR)/macs/cpmac
 vpath % $(ECODIR)/plls/pllxx1p8
 vpath % $(ECODIR)/ddrs/emif31
 vpath % $(ECODIR)/mdio
 vpath % $(ECODIR)/gpio
 vpath % $(ECODIR)/nands/gpio
 vpath % $(ECODIR)/i2c
 vpath % $(ECODIR)/uart/null_uart
endif

ifeq ($(TARGET),c66x)
 vpath % $(ECODIR)/plls/pll014phi
 vpath % $(ECODIR)/mdio
 vpath % $(ECODIR)/i2c
 vpath % $(ECODIR)/pscs/psc2
 vpath % $(ECODIR)/cpsw
 vpath % $(ECODIR)/qm
 vpath % $(ECODIR)/cpdma
 vpath % $(ECODIR)/pa
 vpath % $(ECODIR)/sgmii
 vpath % $(ECODIR)/serdes
 vpath % $(ECODIR)/gpio
 vpath % $(ECODIR)/macs/cpmacsl
 vpath % $(ECODIR)/ddrs/emif4
 vpath % $(ECODIR)/nands/emif25
 vpath % $(ECODIR)/nands/gpio
 vpath % $(ECODIR)/nands/spi
 vpath % $(ECODIR)/spi
 vpath % $(ECODIR)/nors/emif25
 vpath % $(ECODIR)/nors/spi
 vpath % $(ECODIR)/emif25
 vpath % $(ECODIR)/uart/c66x_uart
endif

ifeq ($(TARGET),c665x)
 vpath % $(ECODIR)/plls/pll014phi
 vpath % $(ECODIR)/mdio
 vpath % $(ECODIR)/i2c
 vpath % $(ECODIR)/pscs/psc2
 vpath % $(ECODIR)/sgmii
 vpath % $(ECODIR)/serdes
 vpath % $(ECODIR)/gpio
 vpath % $(ECODIR)/macs/cpmac
 vpath % $(ECODIR)/ddrs/emif4
 vpath % $(ECODIR)/nands/emif25
 vpath % $(ECODIR)/nands/gpio
 vpath % $(ECODIR)/nands/spi
 vpath % $(ECODIR)/spi
 vpath % $(ECODIR)/nors/emif25
 vpath % $(ECODIR)/nors/spi
 vpath % $(ECODIR)/emif25
 vpath % $(ECODIR)/uart/c66x_uart
endif

hw: gen_cdefdep makefile $(OBJS)

$(OBJS): cdefdep


gen_cdefdep:
	@echo Checking command line dependencies
	@echo $(TARGET) $(ARCH) $(CDEFS) $(DEBUG) > cdefdep.tmp
	@sh -c 'if diff -q cdefdep.tmp cdefdep ; then echo same ; else $(CP) cdefdep.tmp cdefdep ; fi '

cleant:
	make -f makestg2 clean2 ARCH=c64x  






