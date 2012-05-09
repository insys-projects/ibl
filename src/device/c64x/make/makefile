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

ASMSRC= c64x.s

# For no target set CSRC to all sources for clean
ifeq ($(TARGET),c6472)
 CSRC= c6472.c c6472init.c
else 
 ifeq ($(TARGET),c6455)
  CSRC= c6455.c c6455init.c
 else
  ifeq ($(TARGET),c6474)
   CSRC= c6474.c c6474init.c
  else
   ifeq ($(TARGET),c6474l)
    CSRC= c6474l.c c6474linit.c
   else
    ifeq ($(TARGET),c6457)
     CSRC= c6457.c c6457init.c
	else
	 ifeq ($(TARGET),c66x)
	  CSRC= c66x.c c66xinit.c c66xutil.c
	 else
      ifeq ($(TARGET),c665x)
	   CSRC= c665x.c c665xinit.c c665xutil.c 
      else
       CSRC= c6472.c c6455.c c6474.c c6474l.c c6457.c c6472init.c c6455init.c c6474linit.c c6474init.c c66x.c c66xinit.c c66xutil.c c665x.c c665xinit.c c665xutil.c
      endif 
     endif
    endif 
   endif
  endif
 endif
endif

.PHONY: device


include $(IBL_ROOT)/make/$(ARCH)/makeeco.mk


C6X_C_DIR=   $(IBL_ROOT)/device/$(TARGET)
C6X_C_DIR+= ;$(IBL_ROOT)/cfg/$(TARGET)
C6X_C_DIR+= ;$(IBL_ROOT)/device
C6X_C_DIR+= ;$(IBL_ROOT)
C6X_C_DIR+= ;$(IBL_ROOT)/arch/$(ARCH)
C6X_C_DIR+= ;$(STDINC)
C6X_C_DIR+= ;$(IBL_ROOT)/hw/plls
C6X_C_DIR+= ;$(IBL_ROOT)/hw/pscs
C6X_C_DIR+= ;$(IBL_ROOT)/hw/gpio
C6X_C_DIR+= ;$(IBL_ROOT)/hw/ddrs/emif31
C6X_C_DIR+= ;$(IBL_ROOT)/hw/qm
C6X_C_DIR+= ;$(IBL_ROOT)/hw/cpdma
C6X_C_DIR+= ;$(IBL_ROOT)/hw/pa
C6X_C_DIR+= ;$(IBL_ROOT)/hw/serdes
C6X_C_DIR+= ;$(IBL_ROOT)/hw/nands
C6X_C_DIR+= ;$(IBL_ROOT)/hw/nors
C6X_C_DIR+= ;$(IBL_ROOT)/driver/eth
C6X_C_DIR+= ;$(IBL_ROOT)/hw/spi
C6X_C_DIR+= ;$(IBL_ROOT)/hw/uart
export C6X_C_DIR

vpath % $(IBL_ROOT)/device/$(TARGET)


device: gen_cdefdep makefile $(OBJS)


$(OBJS): cdefdep


gen_cdefdep:
	@echo Checking command line dependencies
	@echo $(TARGET) $(ARCH) $(CDEFS) $(DEBUG) > cdefdep.tmp
	@sh -c 'if diff -q cdefdep.tmp cdefdep ; then echo same ; else $(CP) cdefdep.tmp cdefdep ; fi '



