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

CSRC= $(ETHSRC) $(STRMSRC) $(TIMSRC) $(NANDSRC)

.PHONY: driver


include $(IBL_ROOT)/make/$(ARCH)/makeeco.mk


C6X_C_DIR=   $(IBL_ROOT)
C6X_C_DIR+= ;$(IBL_ROOT)/arch/$(ARCH)
C6X_C_DIR+= ;$(ECODIR)
C6X_C_DIR+= ;$(STDINC)
C6X_C_DIR+= ;$(IBL_ROOT)/driver/timer
C6X_C_DIR+= ;$(IBL_ROOT)/driver/stream
C6X_C_DIR+= ;$(IBL_ROOT)/driver/nand
C6X_C_DIR+= ;$(IBL_ROOT)/cfg/$(TARGET)
C6X_C_DIR+= ;$(IBL_ROOT)/hw/timer
C6X_C_DIR+= ;$(IBL_ROOT)/hw/nands
export C6X_C_DIR

vpath % $(subst ;,$(PATHSEP), $(ECODIR)/$(ETHDIR); $(ECODIR)/$(STRMDIR); $(ECODIR)/$(TIMDIR); $(ECODIR)/$(NANDDIR) )


driver: gen_cdefdep makefile $(OBJS)

$(OBJS): cdefdep


gen_cdefdep:
	@echo Checking command line dependencies
	@echo $(ENDIAN) $(TARGET) > cdefdep.tmp
	@sh -c 'if diff -q cdefdep.tmp cdefdep ; then echo same ; else $(CP) cdefdep.tmp cdefdep ; fi '






