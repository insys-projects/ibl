#*******************************************************************************************
#* FILE PURPOSE: Top level make for the IBL
#*******************************************************************************************
#* FILE NAME: Makefile
#*
#* DESCRIPTION: Builds the Intermediate Boot Loader (IBL)
#*
#*******************************************************************************************

IBLS_C6X= c6455 c6472 c6474 c6474l c6457


all:
	@echo must specify a target [ $(IBLS_C6X) ]


# Default options that can be overridden on the command line
ifndef ENDIAN
 ENDIAN=little
endif
export ENDIAN

# The debug flag changes compiler options
ifndef DEBUG
 DEBUG=no
endif
export DEBUG

# The value of I2C_PARAM_BUS_ADDR determines the I2C bus address used
# to read the ibl configuration table from. The default values are
# 0x50 for little endian and 0x51 for big endian.

ifndef I2C_PARAM_BUS_ADDR
 ifeq ($(ENDIAN),little)
   I2C_PARAM_BUS_ADDR=0x50
 else
   I2C_PARAM_BUS_ADDR=0x51
 endif
endif

export I2C_PARAM_BUS_ADDR


$(IBLS_C6X):
	make -f makestg2 ARCH=c64x TARGET=$@ $@ 


# will need to add a line for each additional ARCH type added
clean:
	make -f makestg2 clean   ARCH=c64x
	make -f makestg2 cleant  ARCH=c64x TARGET=c6472
	make -f makestg2 cleant  ARCH=c64x TARGET=c6474
	make -f makestg2 cleant  ARCH=c64x TARGET=c6474l
	make -f makestg2 cleant  ARCH=c64x TARGET=c6457
	make -C ../util/bconvert       clean
	make -C ../util/btoccs         clean
	make -C ../util/i2cConfig      clean
	make -C ../util/nandWriter     clean
	make -C ../util/romparse       clean
	make -C ../test/test1		   clean
	make -C ../test/test2		   clean










