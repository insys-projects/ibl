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


#*******************************************************************************************
#* FILE PURPOSE: Top level make for the IBL
#*******************************************************************************************
#* FILE NAME: Makefile
#*
#* DESCRIPTION: Builds the Intermediate Boot Loader (IBL)
#*
#*  Usage:  make c6455 | c6472 | c6474 | c6457 | c661x [DEBUG=yes] [ETH=no] [NAND=no]  \
#*          [BIS=no] [COFF=no] [BLOB=no] [ELF=no] [ENDIAN= both | big | little] [I2C_BUS_ADDR= 0x50 | 0x51] \
#*			[COMPACT_I2C=yes]
#*
#* or to make a board specific configuraiton
#*
#*			make evm_c6455 | evm_c6472 | evm_c6474 | evm_c6457
#*
#* or to test the builds by making all the devices and testing excludes
#*
#*			make test_build
#*******************************************************************************************

IBLS_C6X= c6455 c6472 c6474 c6457 c661x
EVMS_C6X= evm_c6455 evm_c6472 evm_c6474


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
	@echo must specify a target [ $(IBLS_C6X) $(EVMS_C6X) ]


# The debug flag changes compiler options
ifndef DEBUG
 DEBUG=no
endif
export DEBUG


$(IBLS_C6X): 
	make -f makestg1 ARCH=c64x TARGET=$@ I2C_BUS_ADDR=$(I2C_BUS_ADDR) COMPACT_I2C=$(COMPACT_I2C) ENDIAN_MODE=$(ENDIAN_MODE) EXCLUDES='$(EXCLUDES)' $@


# Configurations for individual evms
# The c455 EVM has a 128k eeprom (64k at 0x50, 64k at 0x51), so both endians are built with full functionality
evm_c6455:
	make -f makestg1 ARCH=c64x TARGET=c6455 I2C_BUS_ADDR=0x50 COMPACT_I2C=no ENDIAN_MODE=both EXCLUDES= c6455

# The c6472 EVM has a 128k eeprom (64k at 0x50, 64k at 0x51), so both endians are built with full functionality
evm_c6472:
	make -f makestg1 ARCH=c64x TARGET=c6472 I2C_BUS_ADDR=0x50 COMPACT_I2C=no ENDIAN_MODE=both EXCLUDES= c6472

# The 6474 EVM has a 32k eeprom. A stripped down version is build with only one endian.
evm_c6474:
	make -f makestg1 ARCH=c64x TARGET=c6474 I2C_BUS_ADDR=0x50 COMPACT_I2C=yes ENDIAN_MODE=little EXCLUDES='ELF NAND BIS' I2C_SIZE_BYTES=0x8000 c6474
	cp ibl_c6474/i2crom.dat ibl_c6474/i2crom_0x50.dat
	make -f makestg1 ARCH=c64x TARGET=c6474 I2C_BUS_ADDR=0x51 COMPACT_I2C=yes ENDIAN_MODE=big EXCLUDES='ELF NAND BIS' I2C_SIZE_BYTES=0x8000 c6474
	cp ibl_c6474/i2crom.dat ibl_c6474/i2crom_0x51.dat

# The 6457 EVM
evm_c6457:
	make -f makestg1 ARCH=c64x TARGET=c6457 I2C_BUS_ADDR=0x50 COMPACT_I2C=yes ENDIAN_MODE=little EXCLUDES='ELF NAND BIS' I2C_SIZE_BYTES=0x8000 c6457
	cp ibl_c6457/i2crom.dat ibl_c6457/i2crom_0x50.dat
	make -f makestg1 ARCH=c64x TARGET=c6457 I2C_BUS_ADDR=0x51 COMPACT_I2C=yes ENDIAN_MODE=big EXCLUDES='ELF NAND BIS' I2C_SIZE_BYTES=0x8000 c6457
	cp ibl_c6457/i2crom.dat ibl_c6457/i2crom_0x51.dat

# Test - builds all the targets, with single component exclusion
test_build:
	make -f makestg1 ARCH=c64x TARGET=c6455 I2C_BUS_ADDR=0x50 COMPACT_I2C=no ENDIAN_MODE=both EXCLUDES=     c6455
	make -f makestg1 ARCH=c64x TARGET=c6455 I2C_BUS_ADDR=0x50 COMPACT_I2C=no ENDIAN_MODE=both EXCLUDES=ETH  c6455
	make -f makestg1 ARCH=c64x TARGET=c6455 I2C_BUS_ADDR=0x50 COMPACT_I2C=no ENDIAN_MODE=both EXCLUDES=NAND c6455
	make -f makestg1 ARCH=c64x TARGET=c6455 I2C_BUS_ADDR=0x50 COMPACT_I2C=no ENDIAN_MODE=both EXCLUDES=BIS  c6455
	make -f makestg1 ARCH=c64x TARGET=c6455 I2C_BUS_ADDR=0x50 COMPACT_I2C=no ENDIAN_MODE=both EXCLUDES=COFF c6455
	make -f makestg1 ARCH=c64x TARGET=c6455 I2C_BUS_ADDR=0x50 COMPACT_I2C=no ENDIAN_MODE=both EXCLUDES=ELF  c6455
	make -f makestg1 ARCH=c64x TARGET=c6455 I2C_BUS_ADDR=0x50 COMPACT_I2C=no ENDIAN_MODE=both EXCLUDES=BLOB c6455

	make -f makestg1 ARCH=c64x TARGET=c6457 I2C_BUS_ADDR=0x50 COMPACT_I2C=yes ENDIAN_MODE=both EXCLUDES=     c6457
	make -f makestg1 ARCH=c64x TARGET=c6457 I2C_BUS_ADDR=0x50 COMPACT_I2C=yes ENDIAN_MODE=both EXCLUDES=ETH  c6457
	make -f makestg1 ARCH=c64x TARGET=c6457 I2C_BUS_ADDR=0x50 COMPACT_I2C=yes ENDIAN_MODE=both EXCLUDES=NAND c6457
	make -f makestg1 ARCH=c64x TARGET=c6457 I2C_BUS_ADDR=0x50 COMPACT_I2C=yes ENDIAN_MODE=both EXCLUDES=BIS  c6457
	make -f makestg1 ARCH=c64x TARGET=c6457 I2C_BUS_ADDR=0x50 COMPACT_I2C=yes ENDIAN_MODE=both EXCLUDES=COFF c6457
	make -f makestg1 ARCH=c64x TARGET=c6457 I2C_BUS_ADDR=0x50 COMPACT_I2C=yes ENDIAN_MODE=both EXCLUDES=ELF  c6457
	make -f makestg1 ARCH=c64x TARGET=c6457 I2C_BUS_ADDR=0x50 COMPACT_I2C=yes ENDIAN_MODE=both EXCLUDES=BLOB c6457

	make -f makestg1 ARCH=c64x TARGET=c6472 I2C_BUS_ADDR=0x50 COMPACT_I2C=no ENDIAN_MODE=both EXCLUDES=     c6472
	make -f makestg1 ARCH=c64x TARGET=c6472 I2C_BUS_ADDR=0x50 COMPACT_I2C=no ENDIAN_MODE=both EXCLUDES=ETH  c6472
	make -f makestg1 ARCH=c64x TARGET=c6472 I2C_BUS_ADDR=0x50 COMPACT_I2C=no ENDIAN_MODE=both EXCLUDES=NAND c6472
	make -f makestg1 ARCH=c64x TARGET=c6472 I2C_BUS_ADDR=0x50 COMPACT_I2C=no ENDIAN_MODE=both EXCLUDES=BIS  c6472
	make -f makestg1 ARCH=c64x TARGET=c6472 I2C_BUS_ADDR=0x50 COMPACT_I2C=no ENDIAN_MODE=both EXCLUDES=COFF c6472
	make -f makestg1 ARCH=c64x TARGET=c6472 I2C_BUS_ADDR=0x50 COMPACT_I2C=no ENDIAN_MODE=both EXCLUDES=ELF  c6472
	make -f makestg1 ARCH=c64x TARGET=c6472 I2C_BUS_ADDR=0x50 COMPACT_I2C=no ENDIAN_MODE=both EXCLUDES=BLOB c6472

	make -f makestg1 ARCH=c64x TARGET=c6474 I2C_BUS_ADDR=0x50 COMPACT_I2C=yes ENDIAN_MODE=both EXCLUDES=     c6474
	make -f makestg1 ARCH=c64x TARGET=c6474 I2C_BUS_ADDR=0x50 COMPACT_I2C=yes ENDIAN_MODE=both EXCLUDES=ETH  c6474
	make -f makestg1 ARCH=c64x TARGET=c6474 I2C_BUS_ADDR=0x50 COMPACT_I2C=yes ENDIAN_MODE=both EXCLUDES=NAND c6474
	make -f makestg1 ARCH=c64x TARGET=c6474 I2C_BUS_ADDR=0x50 COMPACT_I2C=yes ENDIAN_MODE=both EXCLUDES=BIS  c6474
	make -f makestg1 ARCH=c64x TARGET=c6474 I2C_BUS_ADDR=0x50 COMPACT_I2C=yes ENDIAN_MODE=both EXCLUDES=COFF c6474
	make -f makestg1 ARCH=c64x TARGET=c6474 I2C_BUS_ADDR=0x50 COMPACT_I2C=yes ENDIAN_MODE=both EXCLUDES=ELF  c6474
	make -f makestg1 ARCH=c64x TARGET=c6474 I2C_BUS_ADDR=0x50 COMPACT_I2C=yes ENDIAN_MODE=both EXCLUDES=BLOB c6474


# will need to add a line for each additional ARCH type added
clean:
	make -f makestg2 clean   ARCH=c64x TARGET=c6472
	make -f makestg2 cleant  ARCH=c64x TARGET=c6472
	make -f makestg2 cleant  ARCH=c64x TARGET=c6474
	make -f makestg2 cleant  ARCH=c64x TARGET=c6455
	make -f makestg2 cleant  ARCH=c64x TARGET=c6457
	make -C ../util/bconvert       clean
	make -C ../util/btoccs         clean
	make -C ../util/i2cConfig      clean    ARCH=c64x TARGET=c6455
	make -C ../util/i2cConfig      clean    ARCH=c64x TARGET=c6472
	make -C ../util/i2cConfig      clean    ARCH=c64x TARGET=c6474
	make -C ../util/i2cConfig      clean    ARCH=c64x TARGET=c6457
	make -C ../util/nandwriter     clean    ARCH=c64x TARGET=c6455
	make -C ../util/nandwriter     clean    ARCH=c64x TARGET=c6472
	make -C ../util/nandwriter     clean    ARCH=c64x TARGET=c6474
	make -C ../util/nandwriter     clean    ARCH=c64x TARGET=c6457
	make -C ../util/romparse       clean
	make -C ../test/test1		   clean
	make -C ../test/test2		   clean










