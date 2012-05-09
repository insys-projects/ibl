###############################################################################
# Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
###############################################################################

CC               = gcc 
AR               = ar

###############################################################################
# Set host specific default options
###############################################################################
LD_OPTS =
C_OPTS =

C_OPTS += -O2 -DEVM=$(EVM) -DI2C_MAP_ADDR=$(I2C_MAP_ADDR)

ifeq ($(EVM),c6455)
	DEVICE_ID = 1
endif
ifeq ($(EVM),c6474)
	DEVICE_ID = 2
endif
ifeq ($(EVM),c6474l)
	DEVICE_ID = 3
endif
ifeq ($(EVM),c6457)
	DEVICE_ID = 4
endif
ifeq ($(EVM),c6472)
	DEVICE_ID = 5
endif
ifeq ($(EVM),c6678l)
	DEVICE_ID = 6
endif
ifeq ($(EVM),c6670l)
	DEVICE_ID = 7
endif
ifeq ($(EVM),c6657l)
	DEVICE_ID = 8
endif
LNK_OPTS +=

###############################################################################
# List of modules and their files
###############################################################################

IBLCFG_EXE = iblConfig.out
FILES  = \
	 iblConfig.c \
	 device.c \
	 device.h \
	 types.h \

OBJ_DIR = ./obj
VPATH = ../src

HEADERS = $(filter %.h, $(FILES))
INCL = $(patsubst %,-I%,$(VPATH))
BOARD_TEMP = $(patsubst %,-D%,$(BOARD))
DEFINE_BOARD = $(patsubst %,%_BOARD,$(BOARD_TEMP))

OBJS = $(patsubst %.c,$(OBJ_DIR)/%.obj,$(filter %.c, $(FILES)))

###############################################################################
# Normal make process                                                         
###############################################################################

$(IBLCFG_EXE): $(OBJS) gen_input
	$(CC) $(C_OPTS) $(OBJS) $(LNK_OPTS) -o $@
	cp $@ $@.dbg 

$(OBJ_DIR)/%.obj: %.c $(HEADERS) $(OBJ_DIR)/.created
	$(CC) -c $(C_OPTS) $(INCL) $(DEFINE_BOARD) $< -o $@

$(OBJ_DIR)/.created:
	mkdir -p $(OBJ_DIR)
	touch $(OBJ_DIR)/.created

gen_input:
	touch input.txt
	echo "file_name = ibl.bin" > input.txt
	echo "device = $(DEVICE_ID)" >> input.txt
	echo "offset = $(I2C_MAP_ADDR)" >> input.txt
	echo "ethBoot-doBootp = " >> input.txt
	echo "ethBoot-bootFormat = " >> input.txt
	echo "ethBoot-ipAddr = " >> input.txt
	echo "ethBoot-serverIp = " >> input.txt
	echo "ethBoot-gatewayIp = " >> input.txt
	echo "ethBoot-netmask = " >> input.txt
	echo "ethBoot-fileName = " >> input.txt

###############################################################################
# Cleanup								      
###############################################################################
clean:
	-rm -rf ./obj/
	-rm -rf $(IBLCFG_EXE)
	-rm -rf *.dbg
	-rm -rf *.out
	-rm -rf *.bin
	-rm -rf *.txt

