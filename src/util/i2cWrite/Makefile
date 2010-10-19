#***************************************************************
#* FILE PURPOSE: Top level make file for the I2C writer 
#***************************************************************
#* FILE NAME: Makefile
#*
#* DESCRIPTION: builds the i2c eeprom writer 
#*
#***************************************************************

DEVICES= c6455 c6472 c6474

all:
	@echo must specify a target [ $(DEVICE) ]

ifndef ENDIAN
 ENDIAN= little
endif

export ENDIAN

$(DEVICES):
	make -f makestg2 ARCH=c64x TARGET=$@ $@

clean:
	make -f makestg2 clean2 ARCH=c64x





