#*************************************************************
#* FILE PURPOS: Top level make for the nand flash writer
#*************************************************************
#* FILE NAME: Makefile
#*
#* DESCRIPTION: Builds the program to write a file to flash
#*
#*************************************************************

DEVICES= c6472

all:
	@echo must specify a target [ $(DEVICES) ]


# Default options that can be overridden from the command line
ifndef ENDIAN
 ENDIAN= little
endif

export ENDIAN

$(DEVICES):
	make -f makestg2 ARCH=c64x TARGET=$@ $@

clean:
	make -f makestg2 clean2 ARCH=c64x











