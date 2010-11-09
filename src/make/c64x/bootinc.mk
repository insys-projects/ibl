#************************************************************************************
#* FILE PURPOSE: Boot loader specific make options
#************************************************************************************
#* FILE NAME: bootinc.mk
#*
#* DESCRIPTION: Definitions common accross boot loader make files
#*
#************************************************************************************

# Adjust the cflags based on desired endianness

ifeq ($(ENDIANNES),big)
CFLAGS+= -me
AFLAGS+= -me
endif

ifneq ($(MV6400PLUS),no)
CFLAGS:=$(subst mv6400,mv6400+,$(CFLAGS))
AFLAGS:=$(subst mv6400,mv6400+,$(AFLAGS))
#CFLAGS:=$(subst o3,,$(CFLAGS))  # for debug builds
endif

# mdebug - remove this
# CFLAGS:=$(subst -o3,-o2,$(CFLAGS))
# end mdebug

CFLAGS+= -ms2

ifdef RTLIBS
RTLIBS = $(TOOLSC6X)/lib/rts6400.lib
endif

# Rule to run splint. Form the splint
SPLINTCHECK= -splint $(CDEFS) $(SPLINT_OPTS) $(SPLINT_IPATH) $(SPLINT_DEFS) $<
SPLINT_OBJ_EXT= sobj

ALLDIR= $(C6X_C_DIR) $(SPLINT_PATH)

SPLINT_I0= $(subst \\\\\,!,$(ALLDIR))
SPLINT_I1= $(subst \\\\,!,$(SPLINT_I0))
SPLINT_I2= $(subst \\\,!,$(SPLINT_I1))
SPLINT_I3= $(subst \\,!,$(SPLINT_I2))
SPLINT_I4= $(subst \,!,$(SPLINT_I3))
SPLINT_I5= $(subst !,\,$(SPLINT_I4))
SPLINT_I6= $(subst ;, ,$(SPLINT_I5))
SPLINT_I7= $(addprefix -I,$(SPLINT_I6))
SPLINT_I8= $(subst /,\,$(SPLINT_I7))
SPLINT_I9= $(SPLINT_I8) -I..\..

SPLINT_IPATH= $(SPLINT_I9)

SPLINT_OPTS= -booltype BOOL +boolint -nullret -incondefs -exportlocal -Dcregister=



#MIKE_DASH= \\hello\\\\mike
#MIKE_RM= $(subst \\\\,;,$(MIKE_DASH))
#MIKE_R2= $(subst \\,;,$(MIKE_RM))


SPLINT_OBJS= $(patsubst %.c, %.$(SPLINT_OBJ_EXT), $(CFILES))

%.$(SPLINT_OBJ_EXT): %.c
	-$(SPLINTCHECK)

