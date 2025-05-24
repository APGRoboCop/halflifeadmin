#
# Half-Life mp_i386.so Makefile for Linux
#
# August 1999 by Leon Hartwig (jehannum@planethalflife.com)
# Revised February 5, 2000
#
# Based on the Q2 Makefile by Zoid
#

MODNAME=amx

ARCH=admin

#make sure this is the correct compiler for your system
CC=gcc

OBJDIR=obj

BASE_CFLAGS= -DLINUX -m32 -Damx_Init=amx_InitAMX -DAMXDLL -DFIXEDPOINT \
	-std=gnu17 -shared-libgcc -Wall

#safe optimization
#CFLAGS=$(BASE_CFLAGS) -m486 -O6 -ffast-math -funroll-loops \
# -fexpensive-optimizations -malign-loops=2  -malign-jumps=2 \
# -malign-functions=2 -Wall

# full debug flags
# CFLAGS=$(BASE_CFLAGS) -g3 -ggdb3 -Wconversion

#small optimization flags
CFLAGS=$(BASE_CFLAGS) -O2 -march=i686 -mtune=generic -msse4.2 -mfpmath=sse -pipe

#full optimization (WONT WORK WITH omit-frame-pointer !!!!!)
#CFLAGS=$(BASE_CFLAGS)  -O3 -ffast-math -funroll-loops \
#  -malign-jumps=2 -malign-functions=2 -malign-loops=2 

#use these when debugging 
#CFLAGS+=  -ggdb

MODARCH=
MARCH = $(shell uname -m)
#ifeq "$(MARCH)" "x86_64"
#	CFLAGS += -DTARGET_64BIT
#	OBJDIR := $(OBJDIR)64
#	MODARCH = _amd64
#endif

#INCLUDEDIRS=-I../linux -I.
INCLUDEDIRS=

OBJADD=   

SHLIBEXT=so
SHLIBCFLAGS=-fPIC
SHLIBLDFLAGS=-shared

DO_CC=$(CC) $(CFLAGS) $(SHLIBCFLAGS) $(INCLUDEDIRS) -o $@ -c $<

#############################################################################
# SETUP AND BUILD
# GAME
#############################################################################

$(OBJDIR)/%.o: %.c
	$(DO_CC)

SRC = \
amx.c amxcons.c amxcore.c amxdll.c amxstring.c fixed.c

OBJ=$(addprefix $(OBJDIR)/,$(subst .c,.o,$(SRC)))

TARGET = $(MODNAME)_$(ARCH)$(MODARCH).$(SHLIBEXT)
$(TARGET) : neat $(OBJ)
	$(CC) $(CFLAGS) $(SHLIBLDFLAGS) $(LDFLAGS) -o $@ $(OBJ) $(OBJADD) 

amtest: amxadmintst.o amtest.o $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ amxadmintst.o amtest.o $(OBJ) $(OBJADD) 

neat:
	- if [ ! -d $(OBJDIR) ]; then mkdir $(OBJDIR); fi
clean:
	-rm -f $(OBJ)
	-rm -f $(TARGET)
spotless: clean
	-rmdir $(OBJDIR)
	-rm -f *~ *.bak
depend:
	$(CC) -MM $(INCLUDEDIRS) $(SRC)

test:
	-install -m 664 $(TARGET) /data3/Admin/dlls/
