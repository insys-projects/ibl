#**************************************************************
#* FILE PURPOSE: Top level make for the i2c parameter writer
#**************************************************************
#* FILE NAME: Makefile
#*
#* DESCRIPTION: Builds the program to write the parameter table
#*  			to the i2c rom
#*
#***************************************************************

DEVICES= c6472

all:
	@echo must specify a target [ $(DEVICES) ]

# Default options that can be overwridden on the command line
ifndef ENDIAN
 ENDIAN= little
endif

export ENDIAN

$(DEVICES):
	make -f makestg2 ARCH=c64x TARGET=$@ $@


clean:
	make -f makestg2 clean2 ARCH=c64x





