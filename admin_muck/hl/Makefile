#
# Half-Life mp_i386.so Makefile for Linux
#
# August 1999 by Leon Hartwig (jehannum@planethalflife.com)
# Revised February 5, 2000
#
# Based on the Q2 Makefile by Zoid
#

MODNAME=admin

ARCH=i386

# none or mysql
SQL=none

VERSION=2.50.07

BUILD=opt

#############################################################################################################
#make sure this is the correct compiler for your system
CC=gcc
CXX=g++
#CC=egcs



OBJDIR=obj
MMOBJDIR=objMM
#SDKSRC=$(HOME)/sdk2.1/SDK/SourceCode
#METADIR=$(HOME)/sdk2.1/SDK/SourceCode/metamod/metamod
#TESTDIR=/hdb3/halflife/cstrike/dlls
SDKSRC=../../hlsdk/SourceCode
METADIR=../../metamod/metamod
TESTDIR=../dlls

BASE_CFLAGS=-Dstricmp=strcasecmp -D_strnicmp=strncasecmp -Dstrnicmp=strncasecmp -Dstrcmpi=strcasecmp -DLINUX -DVERSION=\"$(VERSION)\"

ifeq "$(BUILD)" "opt"
#full optimization (WONT WORK WITH omit-frame-pointer !!!!!)
CFLAGS=$(BASE_CFLAGS) -march=i586 -O6 -ffast-math -funroll-loops \
	-fexpensive-optimizations -malign-loops=2 \
	-malign-jumps=2 -malign-functions=2 -Wall
else
#use these when debugging 
CFLAGS=$(BASE_CFLAGS)
CFLAGS+= -g -ggdb -Wall -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations
CFLAGS+= -fno-thread-jumps -fno-defer-pop -fno-delayed-branch -fno-omit-frame-pointer -fkeep-static-consts
endif

# uncomment for metamod support
#CFLAGS+= -DUSE_METAMOD

INCLUDEDIRS=-I$(SDKSRC)/engine -I$(SDKSRC)/common -I$(SDKSRC)/pm_shared -I$(SDKSRC)/dlls -I../linux -I.

ifeq "$(MM)" "mm"
	CFLAGS+= -DUSE_METAMOD
	INCLUDEDIRS+= -I$(METADIR)
endif


LDFLAGS=-ldl -lcrypt -lm


OBJADD=   
#uncomment for mysql , linux ONLY!
ifeq "$(SQL)" "mysql"
  CFLAGS+= -DUSE_MYSQL -I/usr/include/mysql 
#-I../../MySQL/include 
  OBJADD= -lmysqlclient
# ../../MySQL/lib/libmysqlclient.a -lz
endif

SHLIBEXT=so
SHLIBCFLAGS=-fPIC
SHLIBLDFLAGS=-shared

DO_CC=$(CC) -w $(CFLAGS) $(SHLIBCFLAGS) $(INCLUDEDIRS) -o $@ -c $<
DO_CXX=$(CXX) -w $(CFLAGS) $(SHLIBCFLAGS) $(INCLUDEDIRS) -o $@ -c $<

#############################################################################
# SETUP AND BUILD
# GAME
#############################################################################

$(OBJDIR)/%.o: %.cpp
	$(DO_CC)

$(MMOBJDIR)/%.o: %.cpp
	$(DO_CC)

$(OBJDIR)/%.o: %.cc
	$(DO_CC)

$(MMOBJDIR)/%.o: %.cc
	$(DO_CC)

SRC = \
dll.cpp h_export.cpp util.cpp bfixed.cpp \
admin_commands.cpp users.cpp cbase.cpp timer.cpp \
CLinkList.cpp CPlugin.cpp admin_mod.cpp sutils.cpp statics.cpp

ifneq "$(MM)" "mm"
	SRC+= linkfunc.cpp
endif


OBJ=$(addprefix $(OBJDIR)/,$(subst .cpp,.o,$(SRC)))
MMOBJ=$(addprefix $(MMOBJDIR)/,$(subst .cpp,.o,$(SRC)))

default: metamod

all: metamod non_metamod

non_metamod: 
	$(MAKE) i_standard

metamod:
	$(MAKE) i_metamod MM=mm


i_standard: $(MODNAME)_$(ARCH).$(SHLIBEXT)

i_metamod:  $(MODNAME)_MM_$(ARCH).$(SHLIBEXT)


$(MODNAME)_$(ARCH).$(SHLIBEXT) : neat $(OBJ)
	$(CC) $(CFLAGS) $(SHLIBLDFLAGS) $(LDFLAGS) -o $@ $(OBJ) $(OBJADD) 

$(MODNAME)_MM_$(ARCH).$(SHLIBEXT) : neat $(MMOBJ)
	$(CC) $(CFLAGS) -DUSE_METAMOD $(SHLIBLDFLAGS) $(LDFLAGS) -o $@ $(MMOBJ) $(OBJADD) 

neat:
	- if [ ! -d $(OBJDIR) ]; then mkdir $(OBJDIR); fi
	- if [ ! -d $(MMOBJDIR) ]; then mkdir $(MMOBJDIR); fi
clean:
	-rm -f $(OBJ)
	-rm -f $(MMOBJ)
	-rm -f *.$(SHLIBEXT)

spotless: clean
	-rm -f *~
	-rmdir $(OBJDIR)
	-rmdir $(MMOBJDIR)

depend:
	$(CC) -MM $(INCLUDEDIRS) $(SRC)

test: $(TESTDIR)/$(MODNAME)_MM_$(ARCH).$(SHLIBEXT)


$(TESTDIR)/$(MODNAME)_MM_$(ARCH).$(SHLIBEXT): $(MODNAME)_MM_$(ARCH).$(SHLIBEXT)
	install $< $@

dlls: ../dlls/$(MODNAME)_MM_$(ARCH).$(SHLIBEXT)

../dlls/$(MODNAME)_MM_$(ARCH).$(SHLIBEXT): $(MODNAME)_$(ARCH).$(SHLIBEXT)
	install $< $@

win32 snap cleanall:
