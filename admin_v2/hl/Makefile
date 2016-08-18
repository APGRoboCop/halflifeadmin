
 #
 # $Id: Makefile,v 1.15 2001/09/27 20:33:16 darope Exp $
 #
 #
 # Copyright (c) 1999-2001 Alfred Reynolds, Florian Zschocke, Magua
 #
 #   This file is part of Admin Mod.
 #
 #   Admin Mod is free software; you can redistribute it and/or modify
 #   it under the terms of the GNU General Public License as published by
 #   the Free Software Foundation; either version 2 of the License, or
 #   (at your option) any later version.
 #
 #   Admin Mod is distributed in the hope that it will be useful,
 #   but WITHOUT ANY WARRANTY; without even the implied warranty of
 #   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 #   GNU General Public License for more details.
 #
 #   You should have received a copy of the GNU General Public License
 #   along with Admin Mod; if not, write to the Free Software
 #   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 #
 #   In addition, as a special exception, the author gives permission to
 #   link the code of this program with the Half-Life Game Engine ("HL
 #   Engine") and Modified Game Libraries ("MODs") developed by VALVe,  
 #   L.L.C ("Valve") and Modified Game Libraries developed by Gearbox 
 #   Software ("Gearbox").  You must obey the GNU General Public License 
 #   in all respects for all of the code used other than the HL Engine and 
 #   MODs from Valve or Gearbox. If you modify this file, you may extend 
 #   this exception to your version of the file, but you are not obligated 
 #   to do so.  If you do not wish to do so, delete this exception statement
 #   from your version.


#
# AdminMod Makefile
# by Alfred Reynolds 2000, 2001
#
# rewritten by Florian Zschocke
# May 20, 2001

# Based on:
#
# Valve Half-Life SDK 2.1 Makefile
# Half-Life mp_i386.so Makefile for Linux
# August 1999 by Leon Hartwig (jehannum@planethalflife.com)
# Revised February 5, 2000
#
# Based on the Q2 Makefile by Zoid
#
# Shamelessly stealing from the MetaMOD Makefile
# by Will Day


###
#
# TARGETS:
# This makefile is used to create different configurations of Adminmod.
# You can build Adminmod as standalone or was a Metamod plugin. You can
# build it with or without MySQL support. The default is to compile in
# debug mode but you can also choose to build optimized objects. 
#
# That leaves you with eight different targets run make on:
#
# Debug builts:
# standalone     Standalone version
# standaloneSQL  Standalone with SQL support
# metamod        Metamod plugin
# metamodSQL     Metamod plugin with SQL support
#
# You have two possible ways to build with optimization. Either run make on one
# of the targets listed above with the variable OPT defined to opt, eg.:
# $ make standalone OPT=opt
# Or you use one of the four targets below which prefix the targets with an O:
#
# Ostandalone    Optimized standalone version
# OstandaloneSQL Optimized standalone with SQL support
# Ometamod       Optimized metamod plugin
# OmetamodSQL    Optimized metamod plugin with SQL support
#
# The default target is 'metamod'.
# Change it in the configuration section.
#
###


#################################################################
#
#  CONFIGURATION
#
#################################################################

# The source pathes to the SDK and metamod
# These are the settings for a standard installation

# The top directory of the HL SDK.
SDKTOP= ../../hlsdk
SDKSRC= $(SDKTOP)/SourceCode

# The directory with the metamod include files.
# For Linux we assume a link called 'metamod' in the adminmod directory
METATOP= ../../metamod
METADIR= $(METATOP)/metamod

# The directory of the MySQL include files
MYSQLINCDIR= 
#The directory of the MySQL library
MYSQLLIBDIR=





# Core team members:
# you can enter your personal directory setup here
# that will leave the standard definitions alone.
# To protect your makefile from changes by a cvs update
# check it out with the sticky tag.
ifeq "$(USER)" "alfred"
	SDKTOP= $(HOME)/sdk2.1/SDK
	SDKSRC= $(SDKTOP)/SourceCode
	METADIR= $(SDKSRC)/metamod/metamod
	TESTDIR= /hdb3/halflife/cstrike/dlls
	MYSQLINCDIR= /hdb1/home/alfred/hl_code/mysql/include
	MYSQLLIBDIR= /hdb1/home/alfred/hl_code/mysql/lib 
endif


# Your timezone string
TZONE= \"\"

ifeq "$(USER)" "flori"
	TZONE= \"CEST\"
endif

# The default target to build
DEFTARGET= metamod

# The version number 
VERSION=2.51

#make sure this is the correct compiler for your system
CC=gcc
CXX=g++

################################################################################################
# !! If you only want to compile and go you shouldn't need to edit anything below this line !! #
################################################################################################

######################################################
# Bogus targets
######################################################

OPT=dbg
SQL=no
MM=no


default: $(DEFTARGET)

all: default

standalone:
	$(MAKE) am_standalone SQL=$(SQL) OPT=$(OPT)

standaloneSQL:
	$(MAKE) am_standalone SQL=mysql OPT=$(OPT)

metamod:
	$(MAKE) am_metamod MM=mm SQL=$(SQL) OPT=$(OPT)

metamodSQL:
	$(MAKE) am_metamod MM=mm SQL=mysql OPT=$(OPT)

Ostandalone:
	$(MAKE) am_standalone SQL=$(SQL) OPT=opt

OstandaloneSQL:
	$(MAKE) am_standalone SQL=mysql OPT=opt

Ometamod:
	$(MAKE) am_metamod MM=mm SQL=$(SQL) OPT=opt

OmetamodSQL:
	$(MAKE) am_metamod MM=mm SQL=mysql OPT=opt



#####################################################
# GENERAL OPTIONS
#####################################################

MODNAME= admin
ARCH= i386
SHLIBEXT=so

SA_TARGET= $(MODNAME)_$(ARCH).$(SHLIBEXT)
MM_TARGET= $(MODNAME)_MM_$(ARCH).$(SHLIBEXT)

BASE_CFLAGS=-Dstricmp=strcasecmp -D_strnicmp=strncasecmp -Dstrnicmp=strncasecmp -Dstrcmpi=strcasecmp -DLINUX
XTFLAGS= -DTZONE=$(TZONE)

ifneq "$(VERSION)" ""
	ifeq "$(MM)" "mm"
		MM_VSTRING= \(MM\)
	endif

	ifeq "$(SQL)" "mysql"
		SQL_VSTRING= \(mysql\)
	endif

	XTFLAGS+= -DMOD_VERSION=\"$(VERSION)\ $(MM_VSTRING)$(SQL_VSTRING)\"
endif

ifeq "$(OPT)" "opt"
	XTFLAGS+= -DOPT_TYPE=\"optimized\"
else
	XTFLAGS+= -DOPT_TYPE=\"debugging\"
endif

#################################################################
# COMPILER FLAGS
#################################################################

CFLAGS=$(BASE_CFLAGS)


# debug build
CFDBG= -g -ggdb -Wall
CFDBG+= -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations
CFDBG+= -fno-thread-jumps -fno-defer-pop -fno-delayed-branch -fno-omit-frame-pointer -fkeep-static-consts
CFDBG+= -march=i586 

#full optimization (WONT WORK WITH omit-frame-pointer !!!!!)
CFOPT=  -march=i586 -O6 
CFOPT+= -ffast-math -funroll-loops \
	-fexpensive-optimizations -malign-loops=2 \
	-malign-jumps=2 -malign-functions=2 
CFOPT+= -Wall

# configuration dependand setup
ifeq "$(OPT)" "opt"
	CFLAGS+= $(CFOPT)
else
	CFLAGS+= $(CFDBG)
endif


SHLIBCFLAGS=-fPIC
SHLIBLDFLAGS=-shared




###############################################################
# DIRECTORY SETUP
###############################################################

OBJDIR= obj
MMOBJDIR= objMM
# SQLOBJDIR= objSQL
# MMSQLOBJDIR= objMMSQL


INCLUDEDIRS=-I$(SDKSRC)/engine -I$(SDKSRC)/common -I$(SDKSRC)/pm_shared -I$(SDKSRC)/dlls -I../amxdll -I.

ifeq "$(MM)" "mm"
	XTFLAGS+= -DUSE_METAMOD
	INCLUDEDIRS+= -I$(METADIR)
endif

ifeq "$(SQL)" "mysql"
	XTFLAGS+= -DUSE_MYSQL
	INCLUDEDIRS+= -I$(MYSQLINCDIR)
	OBJDIR := $(OBJDIR)SQL
	MMOBJDIR := $(MMOBJDIR)SQL
endif



##############################################################
# OBJECTS AND LIBRARIES
##############################################################

#
# Adminmod source files
#

SRC = \
dll.cpp h_export.cpp util.cpp bfixed.cpp \
admin_commands.cpp users.cpp cbase.cpp timer.cpp \
CLinkList.cpp CPlugin.cpp admin_mod.cpp sutils.cpp statics.cpp \
version.cpp amutil.cpp


ifneq "$(MM)" "mm"
	SRC+= linkfunc.cpp
endif

#
# Object files
#
OBJ=$(addprefix $(OBJDIR)/,$(subst .cpp,.o,$(SRC)))
MMOBJ=$(addprefix $(MMOBJDIR)/,$(subst .cpp,.o,$(SRC)))
OBJADD=   


#
# Libraries
#
LDFLAGS=-ldl -lcrypt -lm
LIBADD=   

ifeq "$(SQL)" "mysql"
	LIBADD+= $(MYSQLLIBDIR)/libmysqlclient.a
endif



##############################################################
# COMPILER SETUP
##############################################################


DO_CC=$(CC) -w $(CFLAGS) $(XTFLAGS) $(SHLIBCFLAGS) $(INCLUDEDIRS) -o $@ -c $<
DO_CXX=$(CXX) -w $(CFLAGS) $(XTFLAGS) $(SHLIBCFLAGS) $(INCLUDEDIRS) -o $@ -c $<




##############################################################
# SPECIAL SETUPS IF REQUIRED
##############################################################

#OBJADD := $(OBJADD) LeakTracer.o
#
#LeakTracer.o: LeakTracer.cc
#	$(CC) -fPIC -c -g -pipe -Wall -W LeakTracer.cc -o LeakTracer.o


##############################################################
# BUILD TARGETS AND RULES
##############################################################

$(OBJDIR)/%.o: %.cpp
	$(DO_CC)

$(OBJDIR)/%.o: %.cc
	$(DO_CC)

$(MMOBJDIR)/%.o: %.cpp
	$(DO_CC)

$(MMOBJDIR)/%.o: %.cc
	$(DO_CC)


# make sure to recompile vdate.c for each link
$(OBJDIR)/version.o: $(SRC) *.h Makefile
$(MMOBJDIR)/version.o: $(SRC) *.h Makefile


am_standalone: $(SA_TARGET)

am_metamod:  $(MM_TARGET)


$(SA_TARGET) : neat $(OBJ) $(OBJADD)
	$(CC) $(CFLAGS) $(SHLIBLDFLAGS) $(LDFLAGS) -o $@ $(OBJ) $(OBJADD) $(LIBADD) 

$(MM_TARGET) : neat $(MMOBJ) $(OBJADD)
	$(CC) $(CFLAGS) $(SHLIBLDFLAGS) $(LDFLAGS) -o $@ $(MMOBJ) $(OBJADD) $(LIBADD) 

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

# test: $(TESTDIR)/$(MODNAME)_MM_$(ARCH).$(SHLIBEXT)


# $(TESTDIR)/$(MODNAME)_MM_$(ARCH).$(SHLIBEXT): $(MODNAME)_MM_$(ARCH).$(SHLIBEXT)
# 	install $< $@

# dlls: ../dlls/$(MODNAME)_MM_$(ARCH).$(SHLIBEXT)

# ../dlls/$(MODNAME)_MM_$(ARCH).$(SHLIBEXT): $(MODNAME)_$(ARCH).$(SHLIBEXT)
# 	install $< $@

win32 snap cleanall:
