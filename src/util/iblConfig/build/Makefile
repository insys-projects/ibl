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

#RT_LIB = $(C_DIR)/lib/libc.a
C_OPTS += -O2 -DEVM=$(EVM) -DI2C_MAP_ADDR=$(I2C_MAP_ADDR)

#LNK_OPTS += -i$(C_DIR)/lib -c $(RT_LIB)
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
CLIB_INC = $(C_DIR)/include/

HEADERS = $(filter %.h, $(FILES))
INCL = -I$(CLIB_INC) $(patsubst %,-I%,$(VPATH))
BOARD_TEMP = $(patsubst %,-D%,$(BOARD))
DEFINE_BOARD = $(patsubst %,%_BOARD,$(BOARD_TEMP))

OBJS = $(patsubst %.c,$(OBJ_DIR)/%.obj,$(filter %.c, $(FILES)))

###############################################################################
# Normal make process                                                         
###############################################################################

$(IBLCFG_EXE): $(OBJS)
	$(CC) $(C_OPTS) $(OBJS) $(LNK_OPTS) -o $@
	cp $@ $@.dbg 

$(OBJ_DIR)/%.obj: %.c $(HEADERS) $(OBJ_DIR)/.created
	$(CC) -c $(C_OPTS) $(INCL) $(DEFINE_BOARD) $< -o $@

$(OBJ_DIR)/.created:
	mkdir -p $(OBJ_DIR)
	touch $(OBJ_DIR)/.created


###############################################################################
# Cleanup								      
###############################################################################
clean:
	-rm -rf ./obj/
	-rm -rf $(IBLCFG_EXE)
	-rm -rf *.dbg
	-rm -rf *.out
	-rm -rf *.bin

