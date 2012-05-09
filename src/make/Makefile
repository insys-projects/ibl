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
#*  Usage:  make c6455 | c6472 | c6474 | c6457 | c66x 
#*          [DEBUG=yes] 						/* Compiles for debug */
#*          [ETH=no] 							/* Disables ethernet support */
#*          [BIS=no] 							/* Disables BIS interpreter */
#*          [COFF=no] 							/* Disables COFF interpreter */
#*          [BLOB=no] 							/* Disables BLOB interpreter */
#*          [ELF=no] 							/* Disables ELF interpreter */
#*          [NAND=no]  							/* Disables NAND support through EMIF/SPI/GPIO */
#*          [NAND_SPI=no] 						/* Disables NAND support through SPI */
#*			[NAND_EMIF=no]						/* Disables NAND support through EMIF */
#*			[NAND_GPIO=no]						/* Disables NAND support through GPIO */
#*			[NOR=no]							/* Disables NOR through EMIF/SPI */
#*			[NOR_SPI=no]						/* Disables NOR support through SPI */
#*          [NOR_EMIF=no] 						/* Disables NOR support through EMIF */
#*			[SPI=no]							/* Disables SPI */
#*			[I2C=no]							/* Disables I2C */
#*			[EMIF=no]							/* Disables EMIF */
#*			[MULTI_BOOT=no]						/* Disables Multi-boot feature */
#*			[SPI_MODE=<0,1,2,3>]				/* Selects the SPI operating mode */
#*			[SPI_ADDR_WIDTH=<16,24>]			/* Selects the SPI address width */
#*			[SPI_NPIN=<4,5>]					/* Selects the number of pins on the interface */
#*			[SPI_CSEL=<1,2>]					/* Sets the SPI CSEL value in 5 pin mode */
#*			[SPI_C2TDEL=x]						/* Sets the SPI C to T delay value */
#*			[SPI_CLKDIV=x]						/* Sets the SPI module clock divider */
#*			[SPI_USE_ROM=yes]					/* Uses SPI interface parameters from boot ROM, if available */
#*          [ENDIAN= both | big | little] 		/* Selects the endian of the build */
#*          [I2C_BUS_ADDR= 0x50 | 0x51] 		/* The initial I2C bus address */
#*          [COMPACT_I2C=yes]					/* Mimimizes the size of the I2C */
#*          [I2C_MAP_ADDR= 0x500 | 0x800] 		/* The IBL configuration parameter offset */
#*			
#*
#* or to make a board specific configuraiton
#*
#*			make evm_c6455 | evm_c6472 | evm_c6474 | evm_c6457 | evm_c66x
#*
#* or to test the builds by making all the devices and testing excludes
#*
#*			make test_build
#*******************************************************************************************

IBLS_C6X= c6455 c6472 c6474 c6474l c6457 c66x c665x
EVMS_C6X= evm_c6455 evm_c6472 evm_c6474 evm_c6474l


# Excluding functions from the build reduces the I2C eeprom memory used and
# speeds the initial boot time. Note that boot table cannot be excluded
# because it is required for the two stage I2C load process

CEXCLUDES=

ifeq ($(ETH),no)
 CEXCLUDES+= ETH
endif

ifeq ($(BIS),no)
 CEXCLUDES+= BIS
endif

ifeq ($(COFF),no)
 CEXCLUDES+= COFF
endif

ifeq ($(BLOB),no)
 CEXCLUDES+= BLOB
endif

ifeq ($(ELF),no)
 CEXCLUDES+= ELF
endif

ifeq ($(NAND),no)
 CEXCLUDES+= NAND_SPI
 CEXCLUDES+= NAND_EMIF
 CEXCLUDES+= NAND_GPIO

else

 ifeq ($(NAND_SPI),no)
  CEXCLUDES+= NAND_SPI
 endif

 ifeq ($(NAND_EMIF),no)
  CEXCLUDES+= NAND_EMIF
 endif

 ifeq ($(NAND_GPIO),no)
  CEXCLUDES+= NAND_GPIO
 endif

endif


ifeq ($(NOR),no)
 CEXCLUDES+= NOR_SPI
 CEXCLUDES+= NOR_EMIF

else

 ifeq ($(NOR_SPI),no)
  CEXCLUDES+= NOR_SPI
 endif

 ifeq ($(NOR_EMIF),no)
  CEXCLUDES+= NOR_EMIF
 endif

endif


ifeq ($(I2C),no)
 CEXCLUDES+= I2C
endif


ifeq ($(SPI),no)

 ifeq (,$(findstring NAND_SPI, $(CEXCLUDES)))
  CEXCLUDES+= NAND_SPI
 endif

 ifeq (,$(findstring NOR_SPI, $(CEXCLUDES)))
  CEXCLUDES+= NOR_SPI
 endif

endif


ifeq ($(EMIF),no)

 ifeq (,$(findstring NAND_EMIF, $(CEXCLUDES)))
  CEXCLUDES+= NAND_EMIF
 endif

 ifeq (,$(findstring NOR_EMIF, $(CEXCLUDES)))
  CEXCLUDES+= NOR_EMIF
 endif

endif

ifeq ($(MULTI_BOOT),no)
  CEXCLUDES+= MULTI_BOOT
endif

# The endian of the build. The default target builds a single ROM image with both endians present
ifeq ($(ENDIAN),big)
 ENDIAN_MODE=big
 ENDIAN_SFX=be
else
 ifeq ($(ENDIAN),little)
  ENDIAN_MODE=little
 ENDIAN_SFX=le
 else
  ENDIAN_MODE=both
 ENDIAN_SFX=le-be
 endif
endif

# The i2c ROM bus address. The code will advance accross I2C bus address boundaries (the code must
# be blocked so that a single block doesn't cross a boundary, however). 
ifndef I2C_BUS_ADDR
 I2C_BUS_ADDR=0x50
endif

ifndef I2C_MAP_ADDR
  I2C_MAP_ADDR=0x500
endif


# The SPI configuration parameters
ifndef SPI_MODE
 SPI_MODE=1
endif

ifndef SPI_ADDR_WIDTH
 SPI_ADDR_WIDTH=24
endif

ifndef SPI_NPIN
 SPI_NPIN=5
endif

ifndef SPI_CSEL
 SPI_CSEL=2
endif

ifndef SPI_C2TDEL
 SPI_C2TDEL=4
endif

ifndef SPI_CLKDIV
 SPI_CLKDIV=8
endif

ifneq ($(SPI_USE_ROM),0)
 SPI_ROM=1
else
 SPI_ROM=0
endif

SPI_DEFS=  SPI_MODE=$(SPI_MODE)
SPI_DEFS+= SPI_ADDR_WIDTH=$(SPI_ADDR_WIDTH) 
SPI_DEFS+= SPI_NPIN=$(SPI_NPIN) 
SPI_DEFS+= SPI_CSEL=$(SPI_CSEL) 
SPI_DEFS+= SPI_C2TDEL=$(SPI_C2TDEL) 
SPI_DEFS+= SPI_CLKDIV=$(SPI_CLKDIV) 
SPI_DEFS+= SPI_ROM=$(SPI_ROM)


.PHONY: all $(IBLS_C6X) evm_c6455 evm_c6472 evm_c6474 evm_c6474l evm_c6457 evm_c6678_i2c evm_c6670_i2c evm_c6657_i2c
.PHONY: test_c66x test_c665x test_c6455 test_c6472 test_c6474 test_c6457 clean


all:
	@echo must specify a target [ $(IBLS_C6X) $(EVMS_C6X) ]


# The debug flag changes compiler options
ifndef DEBUG
 DEBUG=no
endif
export DEBUG


$(IBLS_C6X): 
	make -f makestg1 ARCH=c64x TARGET=$@ I2C_BUS_ADDR=$(I2C_BUS_ADDR) I2C_MAP_ADDR=$(I2C_MAP_ADDR) COMPACT_I2C=$(COMPACT_I2C) ENDIAN_MODE=$(ENDIAN_MODE) CEXCLUDES='$(CEXCLUDES)' SPI_DEFS='$(SPI_DEFS)' $@

# Configurations for individual evms
# The c6455 EVM has a 128k eeprom (64k at 0x50, 64k at 0x51), so both endians are built with full functionality
evm_c6455:
	make -f makestg1 ARCH=c64x TARGET=c6455 EVM=c6455 I2C_BUS_ADDR=0x50 I2C_MAP_ADDR=0x500 \
    COMPACT_I2C=no ENDIAN_MODE=$(ENDIAN) CEXCLUDES='ELF COFF BIS NAND_GPIO MULTI_BOOT' c6455
	cp -f ibl_c6455/i2crom.dat bin/i2crom_0x50_c6455_$(ENDIAN_SFX).dat
	cp -f ibl_c6455/i2crom.bin bin/i2crom_0x50_c6455_$(ENDIAN_SFX).bin
	cp -f ../util/i2cConfig/i2cparam_c6455_$(ENDIAN_SFX).out bin/i2cparam_0x50_c6455_$(ENDIAN_SFX)_0x500.out

# The c6472 EVM has a 128k eeprom (64k at 0x50, 64k at 0x51), so both endians are built with full functionality
evm_c6472:
	make -f makestg1 ARCH=c64x TARGET=c6472 EVM=c6472 I2C_BUS_ADDR=0x50 I2C_MAP_ADDR=0x500 \
    COMPACT_I2C=yes ENDIAN_MODE=$(ENDIAN) CEXCLUDES='ELF COFF BIS MULTI_BOOT' c6472
	cp -f ibl_c6472/i2crom.dat bin/i2crom_0x50_c6472_$(ENDIAN_SFX).dat
	cp -f ibl_c6472/i2crom.bin bin/i2crom_0x50_c6472_$(ENDIAN_SFX).bin
	cp -f ../util/i2cConfig/i2cparam_c6472_$(ENDIAN_SFX).out bin/i2cparam_0x50_c6472_$(ENDIAN_SFX)_0x500.out

# The 6474 EVM has a 32k eeprom. A stripped down version is build with only one endian.
evm_c6474:
	make -f makestg1 ARCH=c64x TARGET=c6474 EVM=c6474 I2C_BUS_ADDR=0x50 I2C_MAP_ADDR=0x200 \
    COMPACT_I2C=yes ENDIAN_MODE=$(ENDIAN) CEXCLUDES='ELF NAND_GPIO COFF BIS MULTI_BOOT' I2C_SIZE_BYTES=0x8000 c6474
	cp -f ibl_c6474/i2crom.dat bin/i2crom_0x50_c6474_$(ENDIAN_SFX).dat
	cp -f ibl_c6474/i2crom.bin bin/i2crom_0x50_c6474_$(ENDIAN_SFX).bin
	cp -f ../util/i2cConfig/i2cparam_c6474_$(ENDIAN_SFX).out bin/i2cparam_0x50_c6474_$(ENDIAN_SFX)_0x200.out

evm_c6474l:
	make -f makestg1 ARCH=c64x TARGET=c6474l EVM=c6474l I2C_BUS_ADDR=0x50 I2C_MAP_ADDR=0x200 \
    COMPACT_I2C=yes ENDIAN_MODE=$(ENDIAN) CEXCLUDES='ELF COFF BIS MULTI_BOOT' I2C_SIZE_BYTES=0x8000 c6474l
	cp -f ibl_c6474l/i2crom.dat bin/i2crom_0x50_c6474l_$(ENDIAN_SFX).dat
	cp -f ibl_c6474l/i2crom.bin bin/i2crom_0x50_c6474l_$(ENDIAN_SFX).bin
	cp -f ../util/i2cConfig/i2cparam_c6474l_$(ENDIAN_SFX).out bin/i2cparam_0x50_c6474l_$(ENDIAN_SFX)_0x200.out
# The 6457 EVM
evm_c6457:
	make -f makestg1 ARCH=c64x TARGET=c6457 EVM=c6457 I2C_BUS_ADDR=0x50 I2C_MAP_ADDR=0x200 \
    COMPACT_I2C=yes ENDIAN_MODE=$(ENDIAN) CEXCLUDES='ELF COFF BIS MULTI_BOOT' c6457
	cp -f ibl_c6457/i2crom.dat bin/i2crom_0x50_c6457_$(ENDIAN_SFX).dat
	cp -f ibl_c6457/i2crom.bin bin/i2crom_0x50_c6457_$(ENDIAN_SFX).bin
	cp -f ../util/i2cConfig/i2cparam_c6457_$(ENDIAN_SFX).out bin/i2cparam_0x50_c6457_$(ENDIAN_SFX)_0x200.out

# The 6608 EVM
EVM_6608_SPI_DEFS= SPI_MODE=1 SPI_ADDR_WIDTH=24 SPI_NPIN=5 SPI_CSEL=2 SPI_C2TDEL=1 SPI_CLKDIV=8 SPI_ROM=1

evm_c6608:
	make -f makestg1 ARCH=c64x TARGET=c66x I2C=no I2C_BUS_ADDR=0x50 I2C_MAP_ADDR=0x500 ENDIAN_MODE=little CEXCLUDES=I2C SPI_DEFS='$(EVM_6608_SPI_DEFS)' c66x

# The 667x EVM SPI/NOR Boot
EVM_667x_SPI_DEFS= SPI_MODE=1 SPI_ADDR_WIDTH=24 SPI_NPIN=5 SPI_CSEL=2 SPI_C2TDEL=1 SPI_CLKDIV=8 SPI_ROM=1

evm_c667x_spi:
	make -f makestg1 ARCH=c64x TARGET=c66x I2C=no I2C_BUS_ADDR=0x51 I2C_MAP_ADDR=0x500 ENDIAN_MODE=little CEXCLUDES=I2C SPI_DEFS='$(EVM_667x_SPI_DEFS)' c66x

evm_c6678_i2c:
	make -f makestg1 I2C_BUS_ADDR=$(I2C_BUS_ADDR) I2C_MAP_ADDR=$(I2C_MAP_ADDR) \
ENDIAN_MODE=$(ENDIAN) ARCH=c64x TARGET=c66x EVM=c6678l SPI=no SPI_DEFS='$(EVM_667x_SPI_DEFS)' \
CEXCLUDES='COFF BIS NAND_GPIO' c66x
	cp -f ibl_c66x/i2crom.dat bin/i2crom_$(I2C_BUS_ADDR)_c6678_$(ENDIAN_SFX).dat
	cp -f ibl_c66x/i2crom.bin bin/i2crom_$(I2C_BUS_ADDR)_c6678_$(ENDIAN_SFX).bin
	cp -f ../util/i2cConfig/i2cparam_c66x_$(ENDIAN_SFX).out bin/i2cparam_$(I2C_BUS_ADDR)_c6678_$(ENDIAN_SFX)_$(I2C_MAP_ADDR).out
evm_c6670_i2c:
	make -f makestg1 I2C_BUS_ADDR=$(I2C_BUS_ADDR) I2C_MAP_ADDR=$(I2C_MAP_ADDR) \
ENDIAN_MODE=$(ENDIAN) ARCH=c64x TARGET=c66x EVM=c6670l SPI=no SPI_DEFS='$(EVM_667x_SPI_DEFS)' \
CEXCLUDES='COFF BIS NAND_EMIF' c66x
	cp -f ibl_c66x/i2crom.dat bin/i2crom_$(I2C_BUS_ADDR)_c6670_$(ENDIAN_SFX).dat
	cp -f ibl_c66x/i2crom.bin bin/i2crom_$(I2C_BUS_ADDR)_c6670_$(ENDIAN_SFX).bin
	cp -f ../util/i2cConfig/i2cparam_c66x_$(ENDIAN_SFX).out bin/i2cparam_$(I2C_BUS_ADDR)_c6670_$(ENDIAN_SFX)_$(I2C_MAP_ADDR).out

test_c66x:
	make -f makestg1 ARCH=c64x TARGET=c66x ENDIAN_MODE=both CEXCLUDES='NOR_SPI' SPI_DEFS='SPI_ROM=1 SPI_MODE=3 SPI_ADDR_WIDTH=24 SPI_NPIN=5 SPI_CSEL=2 SPI_C2TDEL=8 SPI_CLKDIV=0x20' I2C_BUS_ADDR=0x50 I2C_MAP_ADDR=$(I2C_MAP_ADDR) COMPACT_I2C=no c66x
	make -f makestg1 ARCH=c64x TARGET=c66x ENDIAN_MODE=both CEXCLUDES='NAND_SPI' SPI_DEFS='SPI_ROM=1 SPI_MODE=3 SPI_ADDR_WIDTH=24 SPI_NPIN=5 SPI_CSEL=2 SPI_C2TDEL=8 SPI_CLKDIV=0x20' I2C_BUS_ADDR=0x50 I2C_MAP_ADDR=$(I2C_MAP_ADDR) COMPACT_I2C=no c66x
	make -f makestg1 ARCH=c64x TARGET=c66x ENDIAN_MODE=both CEXCLUDES='NOR_EMIF' SPI_DEFS='SPI_ROM=1 SPI_MODE=3 SPI_ADDR_WIDTH=24 SPI_NPIN=5 SPI_CSEL=2 SPI_C2TDEL=8 SPI_CLKDIV=0x20' I2C_BUS_ADDR=0x50 I2C_MAP_ADDR=$(I2C_MAP_ADDR) COMPACT_I2C=no c66x
	make -f makestg1 ARCH=c64x TARGET=c66x ENDIAN_MODE=both CEXCLUDES='NAND_EMIF' SPI_DEFS='SPI_ROM=1 SPI_MODE=3 SPI_ADDR_WIDTH=24 SPI_NPIN=5 SPI_CSEL=2 SPI_C2TDEL=8 SPI_CLKDIV=0x20' I2C_BUS_ADDR=0x50 I2C_MAP_ADDR=$(I2C_MAP_ADDR) COMPACT_I2C=no c66x
	make -f makestg1 ARCH=c64x TARGET=c66x ENDIAN_MODE=both CEXCLUDES='NAND_EMIF NOR_EMIF' SPI_DEFS='SPI_ROM=1 SPI_MODE=3 SPI_ADDR_WIDTH=24 SPI_NPIN=5 SPI_CSEL=2 SPI_C2TDEL=8 SPI_CLKDIV=0x20' I2C_BUS_ADDR=0x50 I2C_MAP_ADDR=$(I2C_MAP_ADDR) COMPACT_I2C=no c66x
	make -f makestg1 ARCH=c64x TARGET=c66x ENDIAN_MODE=both CEXCLUDES='SPI NOR_SPI NAND_SPI' I2C_BUS_ADDR=0x50 I2C_MAP_ADDR=$(I2C_MAP_ADDR) COMPACT_I2C=no c66x
	make -f makestg1 ARCH=c64x TARGET=c66x ENDIAN_MODE=both CEXCLUDES=I2C SPI_DEFS='SPI_ROM=1 SPI_MODE=3 SPI_ADDR_WIDTH=24 SPI_NPIN=5 SPI_CSEL=2 SPI_C2TDEL=8 SPI_CLKDIV=0x20' c66x

evm_c6657_spi:
	make -f makestg1 ARCH=c64x TARGET=c665x I2C=no I2C_BUS_ADDR=0x51 I2C_MAP_ADDR=0x500 ENDIAN_MODE=little CEXCLUDES=I2C SPI_DEFS='$(EVM_667x_SPI_DEFS)' c66x

evm_c6657_i2c:
	make -f makestg1 I2C_BUS_ADDR=$(I2C_BUS_ADDR) I2C_MAP_ADDR=$(I2C_MAP_ADDR) \
ENDIAN_MODE=$(ENDIAN) ARCH=c64x TARGET=c665x EVM=c6657l SPI=no SPI_DEFS='$(EVM_667x_SPI_DEFS)' \
CEXCLUDES='COFF BIS NAND_GPIO' c665x
	cp -f ibl_c665x/i2crom.dat bin/i2crom_$(I2C_BUS_ADDR)_c6657_$(ENDIAN_SFX).dat
	cp -f ibl_c665x/i2crom.bin bin/i2crom_$(I2C_BUS_ADDR)_c6657_$(ENDIAN_SFX).bin
	cp -f ../util/i2cConfig/i2cparam_c665x_$(ENDIAN_SFX).out bin/i2cparam_$(I2C_BUS_ADDR)_c6657_$(ENDIAN_SFX)_$(I2C_MAP_ADDR).out

test_c665x:
	make -f makestg1 ARCH=c64x TARGET=c665x ENDIAN_MODE=both CEXCLUDES='NOR_SPI' SPI_DEFS='SPI_ROM=1 SPI_MODE=3 SPI_ADDR_WIDTH=24 SPI_NPIN=5 SPI_CSEL=2 SPI_C2TDEL=8 SPI_CLKDIV=0x20' I2C_BUS_ADDR=0x50 I2C_MAP_ADDR=$(I2C_MAP_ADDR) COMPACT_I2C=no c665x
	make -f makestg1 ARCH=c64x TARGET=c665x ENDIAN_MODE=both CEXCLUDES='NAND_SPI' SPI_DEFS='SPI_ROM=1 SPI_MODE=3 SPI_ADDR_WIDTH=24 SPI_NPIN=5 SPI_CSEL=2 SPI_C2TDEL=8 SPI_CLKDIV=0x20' I2C_BUS_ADDR=0x50 I2C_MAP_ADDR=$(I2C_MAP_ADDR) COMPACT_I2C=no c665x
	make -f makestg1 ARCH=c64x TARGET=c665x ENDIAN_MODE=both CEXCLUDES='NOR_EMIF' SPI_DEFS='SPI_ROM=1 SPI_MODE=3 SPI_ADDR_WIDTH=24 SPI_NPIN=5 SPI_CSEL=2 SPI_C2TDEL=8 SPI_CLKDIV=0x20' I2C_BUS_ADDR=0x50 I2C_MAP_ADDR=$(I2C_MAP_ADDR) COMPACT_I2C=no c665x
	make -f makestg1 ARCH=c64x TARGET=c665x ENDIAN_MODE=both CEXCLUDES='NAND_EMIF' SPI_DEFS='SPI_ROM=1 SPI_MODE=3 SPI_ADDR_WIDTH=24 SPI_NPIN=5 SPI_CSEL=2 SPI_C2TDEL=8 SPI_CLKDIV=0x20' I2C_BUS_ADDR=0x50 I2C_MAP_ADDR=$(I2C_MAP_ADDR) COMPACT_I2C=no c665x
	make -f makestg1 ARCH=c64x TARGET=c665x ENDIAN_MODE=both CEXCLUDES='NAND_EMIF NOR_EMIF' SPI_DEFS='SPI_ROM=1 SPI_MODE=3 SPI_ADDR_WIDTH=24 SPI_NPIN=5 SPI_CSEL=2 SPI_C2TDEL=8 SPI_CLKDIV=0x20' I2C_BUS_ADDR=0x50 I2C_MAP_ADDR=$(I2C_MAP_ADDR) COMPACT_I2C=no c665x
	make -f makestg1 ARCH=c64x TARGET=c665x ENDIAN_MODE=both CEXCLUDES='SPI NOR_SPI NAND_SPI' I2C_BUS_ADDR=0x50 I2C_MAP_ADDR=$(I2C_MAP_ADDR) COMPACT_I2C=no c665x
	make -f makestg1 ARCH=c64x TARGET=c665x ENDIAN_MODE=both CEXCLUDES=I2C SPI_DEFS='SPI_ROM=1 SPI_MODE=3 SPI_ADDR_WIDTH=24 SPI_NPIN=5 SPI_CSEL=2 SPI_C2TDEL=8 SPI_CLKDIV=0x20' c665x

# Test - builds all the targets, with single component exclusion

test_c6455:
	make -f makestg1 ARCH=c64x TARGET=c6455 I2C_BUS_ADDR=0x50 I2C_MAP_ADDR=$(I2C_MAP_ADDR) COMPACT_I2C=no ENDIAN_MODE=both CEXCLUDES=          c6455
	make -f makestg1 ARCH=c64x TARGET=c6455 I2C_BUS_ADDR=0x50 I2C_MAP_ADDR=$(I2C_MAP_ADDR) COMPACT_I2C=no ENDIAN_MODE=both CEXCLUDES=ETH       c6455
	make -f makestg1 ARCH=c64x TARGET=c6455 I2C_BUS_ADDR=0x50 I2C_MAP_ADDR=$(I2C_MAP_ADDR) COMPACT_I2C=no ENDIAN_MODE=both CEXCLUDES=NAND_GPIO c6455
	make -f makestg1 ARCH=c64x TARGET=c6455 I2C_BUS_ADDR=0x50 I2C_MAP_ADDR=$(I2C_MAP_ADDR) COMPACT_I2C=no ENDIAN_MODE=both CEXCLUDES=BIS       c6455
	make -f makestg1 ARCH=c64x TARGET=c6455 I2C_BUS_ADDR=0x50 I2C_MAP_ADDR=$(I2C_MAP_ADDR) COMPACT_I2C=no ENDIAN_MODE=both CEXCLUDES=COFF      c6455
	make -f makestg1 ARCH=c64x TARGET=c6455 I2C_BUS_ADDR=0x50 I2C_MAP_ADDR=$(I2C_MAP_ADDR) COMPACT_I2C=no ENDIAN_MODE=both CEXCLUDES=ELF       c6455
	make -f makestg1 ARCH=c64x TARGET=c6455 I2C_BUS_ADDR=0x50 I2C_MAP_ADDR=$(I2C_MAP_ADDR) COMPACT_I2C=no ENDIAN_MODE=both CEXCLUDES=BLOB      c6455

test_c6457:
	make -f makestg1 ARCH=c64x TARGET=c6457 I2C_BUS_ADDR=0x50 I2C_MAP_ADDR=$(I2C_MAP_ADDR) COMPACT_I2C=yes ENDIAN_MODE=both CEXCLUDES=          c6457
	make -f makestg1 ARCH=c64x TARGET=c6457 I2C_BUS_ADDR=0x50 I2C_MAP_ADDR=$(I2C_MAP_ADDR) COMPACT_I2C=yes ENDIAN_MODE=both CEXCLUDES=ETH       c6457
	make -f makestg1 ARCH=c64x TARGET=c6457 I2C_BUS_ADDR=0x50 I2C_MAP_ADDR=$(I2C_MAP_ADDR) COMPACT_I2C=yes ENDIAN_MODE=both CEXCLUDES=NAND_GPIO c6457
	make -f makestg1 ARCH=c64x TARGET=c6457 I2C_BUS_ADDR=0x50 I2C_MAP_ADDR=$(I2C_MAP_ADDR) COMPACT_I2C=yes ENDIAN_MODE=both CEXCLUDES=BIS       c6457
	make -f makestg1 ARCH=c64x TARGET=c6457 I2C_BUS_ADDR=0x50 I2C_MAP_ADDR=$(I2C_MAP_ADDR) COMPACT_I2C=yes ENDIAN_MODE=both CEXCLUDES=COFF      c6457
	make -f makestg1 ARCH=c64x TARGET=c6457 I2C_BUS_ADDR=0x50 I2C_MAP_ADDR=$(I2C_MAP_ADDR) COMPACT_I2C=yes ENDIAN_MODE=both CEXCLUDES=ELF       c6457
	make -f makestg1 ARCH=c64x TARGET=c6457 I2C_BUS_ADDR=0x50 I2C_MAP_ADDR=$(I2C_MAP_ADDR) COMPACT_I2C=yes ENDIAN_MODE=both CEXCLUDES=BLOB      c6457

test_c6472:
	make -f makestg1 ARCH=c64x TARGET=c6472 I2C_BUS_ADDR=0x50 I2C_MAP_ADDR=$(I2C_MAP_ADDR) COMPACT_I2C=no ENDIAN_MODE=both CEXCLUDES=          c6472
	make -f makestg1 ARCH=c64x TARGET=c6472 I2C_BUS_ADDR=0x50 I2C_MAP_ADDR=$(I2C_MAP_ADDR) COMPACT_I2C=no ENDIAN_MODE=both CEXCLUDES=ETH       c6472
	make -f makestg1 ARCH=c64x TARGET=c6472 I2C_BUS_ADDR=0x50 I2C_MAP_ADDR=$(I2C_MAP_ADDR) COMPACT_I2C=no ENDIAN_MODE=both CEXCLUDES=NAND_GPIO c6472
	make -f makestg1 ARCH=c64x TARGET=c6472 I2C_BUS_ADDR=0x50 I2C_MAP_ADDR=$(I2C_MAP_ADDR) COMPACT_I2C=no ENDIAN_MODE=both CEXCLUDES=BIS       c6472
	make -f makestg1 ARCH=c64x TARGET=c6472 I2C_BUS_ADDR=0x50 I2C_MAP_ADDR=$(I2C_MAP_ADDR) COMPACT_I2C=no ENDIAN_MODE=both CEXCLUDES=COFF      c6472
	make -f makestg1 ARCH=c64x TARGET=c6472 I2C_BUS_ADDR=0x50 I2C_MAP_ADDR=$(I2C_MAP_ADDR) COMPACT_I2C=no ENDIAN_MODE=both CEXCLUDES=ELF       c6472
	make -f makestg1 ARCH=c64x TARGET=c6472 I2C_BUS_ADDR=0x50 I2C_MAP_ADDR=$(I2C_MAP_ADDR) COMPACT_I2C=no ENDIAN_MODE=both CEXCLUDES=BLOB      c6472

test_c6474:
	make -f makestg1 ARCH=c64x TARGET=c6474 I2C_BUS_ADDR=0x50 I2C_MAP_ADDR=$(I2C_MAP_ADDR) COMPACT_I2C=yes ENDIAN_MODE=both CEXCLUDES=          c6474
	make -f makestg1 ARCH=c64x TARGET=c6474 I2C_BUS_ADDR=0x50 I2C_MAP_ADDR=$(I2C_MAP_ADDR) COMPACT_I2C=yes ENDIAN_MODE=both CEXCLUDES=ETH       c6474
	make -f makestg1 ARCH=c64x TARGET=c6474 I2C_BUS_ADDR=0x50 I2C_MAP_ADDR=$(I2C_MAP_ADDR) COMPACT_I2C=yes ENDIAN_MODE=both CEXCLUDES=NAND_GPIO c6474
	make -f makestg1 ARCH=c64x TARGET=c6474 I2C_BUS_ADDR=0x50 I2C_MAP_ADDR=$(I2C_MAP_ADDR) COMPACT_I2C=yes ENDIAN_MODE=both CEXCLUDES=BIS       c6474
	make -f makestg1 ARCH=c64x TARGET=c6474 I2C_BUS_ADDR=0x50 I2C_MAP_ADDR=$(I2C_MAP_ADDR) COMPACT_I2C=yes ENDIAN_MODE=both CEXCLUDES=COFF      c6474
	make -f makestg1 ARCH=c64x TARGET=c6474 I2C_BUS_ADDR=0x50 I2C_MAP_ADDR=$(I2C_MAP_ADDR) COMPACT_I2C=yes ENDIAN_MODE=both CEXCLUDES=ELF       c6474
	make -f makestg1 ARCH=c64x TARGET=c6474 I2C_BUS_ADDR=0x50 I2C_MAP_ADDR=$(I2C_MAP_ADDR) COMPACT_I2C=yes ENDIAN_MODE=both CEXCLUDES=BLOB      c6474


test_build: test_c6455 test_c6457 test_c6472 test_c6474 test_c66x test_c665x


# will need to add a line for each additional ARCH type added
clean:
	make -f makestg2 clean   ARCH=c64x TARGET=c6472
	make -f makestg2 cleant  ARCH=c64x TARGET=c6472
	make -f makestg2 cleant  ARCH=c64x TARGET=c6474
	make -f makestg2 cleant  ARCH=c64x TARGET=c6474l
	make -f makestg2 cleant  ARCH=c64x TARGET=c6455
	make -f makestg2 cleant  ARCH=c64x TARGET=c6457
	make -f makestg2 cleant  ARCH=c64x TARGET=c66x
	make -f makestg2 cleant  ARCH=c64x TARGET=c665x
	make -C ../util/bconvert       clean
	make -C ../util/btoccs         clean
	make -C ../util/i2cConfig      clean    ARCH=c64x TARGET=c6455
	make -C ../util/i2cConfig      clean    ARCH=c64x TARGET=c6472
	make -C ../util/i2cConfig      clean    ARCH=c64x TARGET=c6474
	make -C ../util/i2cConfig      clean    ARCH=c64x TARGET=c6474l
	make -C ../util/i2cConfig      clean    ARCH=c64x TARGET=c6457
	make -C ../util/i2cConfig      clean    ARCH=c64x TARGET=c66x
	make -C ../util/i2cConfig      clean	ARCH=c64x TARGET=c665x	
	make -C ../util/romparse       clean
	make -C ../util/iblConfig/build    clean
	make -C ../test/test1		   clean
	make -C ../test/test2		   clean
	rm -f bin/*.out
	rm -f bin/*.dat
	rm -f bin/*.bin
	find ../ -name *.oc | xargs rm -f
	find ../ -name *.dc | xargs rm -f
	find ../ -name *.oa | xargs rm -f
	find ../ -name *.da | xargs rm -f
	find ../ -name *.tmp | xargs rm -f
	find ../ -name cdefdep | xargs rm -f


