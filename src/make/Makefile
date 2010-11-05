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










