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
#CC=egcs

OBJDIR=obj

BASE_CFLAGS=-Dstricmp=strcasecmp -D_strnicmp=strncasecmp -Dstrnicmp=strncasecmp -Dstrcmpi=strcasecmp -DLINUX -Damx_Init=amx_InitAMX


#safe optimization
#CFLAGS=$(BASE_CFLAGS) -m486 -O6 -ffast-math -funroll-loops \
# -fexpensive-optimizations -malign-loops=2  -malign-jumps=2 \
# -malign-functions=2 -Wall

# full debug flags
# CFLAGS=$(BASE_CFLAGS) -g  -Wall

#small optimization flags
# CFLAGS=$(BASE_CFLAGS) -O2 -m486

#full optimization (WONT WORK WITH omit-frame-pointer !!!!!)
CFLAGS=$(BASE_CFLAGS) -m486 -O6 -ffast-math -funroll-loops \
  -fomit-frame-pointer -fexpensive-optimizations -malign-loops=2 \
  -malign-jumps=2 -malign-functions=2

#use these when debugging 
CFLAGS+=  -ggdb

INCLUDEDIRS=-I../linux -I.

LDFLAGS=-ldl


OBJADD=   

SHLIBEXT=so
SHLIBCFLAGS=-fPIC
SHLIBLDFLAGS=-shared

DO_CC=$(CC) -w $(CFLAGS) $(SHLIBCFLAGS) $(INCLUDEDIRS) -o $@ -c $<

#############################################################################
# SETUP AND BUILD
# GAME
#############################################################################

$(OBJDIR)/%.o: %.c
	$(DO_CC)

SRC = \
amx.c amxcons.c amxcore.c amxdll.c amxstring.c

OBJ=$(addprefix $(OBJDIR)/,$(subst .c,.o,$(SRC)))

$(MODNAME)_$(ARCH).$(SHLIBEXT) : neat $(OBJ)
	$(CC) $(CFLAGS) $(SHLIBLDFLAGS) $(LDFLAGS) -o $@ $(OBJ) $(OBJADD) 

neat:
	- if [ ! -d $(OBJDIR) ]; then mkdir $(OBJDIR); fi
clean:
	-rm -f $(OBJ)
	-rm -f $(MODNAME)_$(ARCH).$(SHLIBEXT)
spotless: clean
	-rmdir $(OBJDIR)
	-rm -f *~ *.bak
depend:
	$(CC) -MM $(INCLUDEDIRS) $(SRC)
