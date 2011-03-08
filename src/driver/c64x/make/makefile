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


#*******************************************************************
#* FILE NAME: makefile
#*
#* DESCRIPTION: Makes the peripheral driver modules
#*
#*******************************************************************

ifndef IBL_ROOT
	export IBL_ROOT=../../..
endif

ECODIR= $(IBL_ROOT)/driver


# The ethernet driver
ETHDIR= eth
ETHSRC= net.c arp.c ip.c udp.c bootp.c tftp.c

# The stream driver
STRMDIR= stream
STRMSRC= stream.c

# The timer driver
TIMDIR= timer
TIMSRC= timer.c

# The nand driver
NANDDIR= nand
NANDSRC= nand.c

# The nor dirver
NORDIR= nor
NORSRC= nor.c

CSRC= $(ETHSRC) $(STRMSRC) $(TIMSRC) $(NANDSRC) $(NORSRC)


.PHONY: driver


include $(IBL_ROOT)/make/$(ARCH)/makeeco.mk


C6X_C_DIR=   $(IBL_ROOT)
C6X_C_DIR+= ;$(IBL_ROOT)/arch/$(ARCH)
C6X_C_DIR+= ;$(ECODIR)
C6X_C_DIR+= ;$(STDINC)
C6X_C_DIR+= ;$(IBL_ROOT)/driver/timer
C6X_C_DIR+= ;$(IBL_ROOT)/driver/stream
C6X_C_DIR+= ;$(IBL_ROOT)/driver/nand
C6X_C_DIR+= ;$(IBL_ROOT)/driver/nor
C6X_C_DIR+= ;$(IBL_ROOT)/cfg/$(TARGET)
C6X_C_DIR+= ;$(IBL_ROOT)/hw/timer
C6X_C_DIR+= ;$(IBL_ROOT)/hw/nands
C6X_C_DIR+= ;$(IBL_ROOT)/device
C6X_C_DIR+= ;$(IBL_ROOT)/device/$(TARGET)
export C6X_C_DIR

vpath % $(ECODIR)/$(ETHDIR)
vpath % $(ECODIR)/$(STRMDIR)
vpath % $(ECODIR)/$(TIMDIR)
vpath % $(ECODIR)/$(NANDDIR)
vpath % $(ECODIR)/$(NORDIR)

driver: gen_cdefdep makefile $(OBJS)

$(OBJS): cdefdep


gen_cdefdep:
	@echo Checking command line dependencies
	@echo $(TARGET) $(CDEFS) $(DEBUG) > cdefdep.tmp
	@sh -c 'if diff -q cdefdep.tmp cdefdep ; then echo same ; else $(CP) cdefdep.tmp cdefdep ; fi '


