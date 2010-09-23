#*******************************************************************************************
#* FILE PURPOSE: Top level make for the IBL
#*******************************************************************************************
#* FILE NAME: Makefile
#*
#* DESCRIPTION: Builds the Intermediate Boot Loader (IBL)
#*
#*******************************************************************************************

IBLS_C6X= c6455 c6472 


all:
	@echo must specify a target [ $(IBLS_C6X) ]


# Default options that can be overridden on the command line
ifndef ENDIAN
 ENDIAN=little
endif
export ENDIAN


$(IBLS_C6X):
	make -f makestg2 ARCH=c64x TARGET=$@ $@ 


# will need to add a line for each additional ARCH type added
clean:
	make -f makestg2 clean   ARCH=c64x
	make -f makestg2 cleant  ARCH=c64x TARGET=c6472
	make -C ../util/bconvert       clean
	make -C ../util/btoccs         clean
	make -C ../util/i2cConfig      clean
	make -C ../util/nandWriter     clean
	make -C ../util/romparse_c6472 clean
	make -C ../test/test1		   clean
	make -C ../test/test2		   clean










