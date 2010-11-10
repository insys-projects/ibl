#*******************************************************************************************
#* FILE PURPOSE: Top level make for the IBL
#*******************************************************************************************
#* FILE NAME: Makefile
#*
#* DESCRIPTION: Builds the Intermediate Boot Loader (IBL)
#*
#*  Usage:  make c6455 | c6472 | c6474 | c6474l | c6457 [DEBUG=yes] [ETH=no] [NAND=no]  \
#*          [BIS=no] [COFF=no] [BLOB=no] [ELF=no] [ENDIAN= both | big | little] [I2C_BUS_ADDR= 0x50 | 0x51] \
#*			[COMPACT_I2C=yes]
#*******************************************************************************************

IBLS_C6X= c6455 c6472 c6474 c6474l c6457

# Excluding functions from the build reduces the I2C eeprom memory used and
# speeds the initial boot time. Note that boot table cannot be excluded
# because it is required for the two stage I2C load process

EXCLUDES=

ifeq ($(ETH),no)
 EXCLUDES+= ETH
endif

ifeq ($(NAND),no)
 EXCLUDES+= NAND
endif

ifeq ($(BIS),no)
 EXCLUDES+= BIS
endif

ifeq ($(COFF),no)
 EXCLUDES+= COFF
endif

ifeq ($(BLOB),no)
 EXCLUDES+= BLOB
endif

ifeq ($(ELF),no)
 EXCLUDES+= ELF
endif
 
export EXCLUDES




# The endian of the build. The default target builds a single ROM image with both endians present
ifeq ($(ENDIAN),big)
 ENDIAN_MODE=big
else
 ifeq ($(ENDIAN),little)
  ENDIAN_MODE=little
 else
  ENDIAN_MODE=both
 endif
endif

# The i2c ROM bus address. The code will advance accross I2C bus address boundaries (the code must
# be blocked so that a single block doesn't cross a boundary, however). 
ifndef I2C_BUS_ADDR
 I2C_BUS_ADDR=0x50
endif



all:
	@echo must specify a target [ $(IBLS_C6X) ]


# The debug flag changes compiler options
ifndef DEBUG
 DEBUG=no
endif
export DEBUG


$(IBLS_C6X): 
	make -f makestg1 ARCH=c64x TARGET=$@ I2C_BUS_ADDR=$(I2C_BUS_ADDR) COMPACT=$(COMPACT) ENDIAN_MODE=$(ENDIAN_MODE) $@


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










