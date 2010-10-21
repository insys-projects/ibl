#**************************************************************
#* FILE PURPOSE: Top level make for the i2c parameter writer
#**************************************************************
#* FILE NAME: Makefile
#*
#* DESCRIPTION: Builds the program to write the parameter table
#*  			to the i2c rom
#*
#***************************************************************

DEVICES= c6455 c6472 c6474 c6474l c6457

all:
	@echo must specify a target [ $(DEVICES) ]

# Default options that can be overwridden on the command line
ifndef ENDIAN
 ENDIAN= little
endif

export ENDIAN

# The value of I2C_PARAM_BUS_ADDR determines the I2C bus address used
# to read the ibl configuration table from. The default values are
# 0x50 for little endian and 0x51 for big endian

ifndef I2C_PARAM_BUS_ADDR
 ifeq ($(ENDIAN),little)
   I2C_PARAM_BUS_ADDR=0x50
 else
   I2C_PARAM_BUS_ADDR=0x51
 endif
endif

export I2C_PARAM_BUS_ADDR

$(DEVICES):
	make -f makestg2 ARCH=c64x TARGET=$@ $@


clean:
	make -f makestg2 clean2 ARCH=c64x





