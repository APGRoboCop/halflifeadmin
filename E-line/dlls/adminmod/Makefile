#
# AdminMod Makefile
# by Alfred Reynolds 2000, 2001
#
# rewritten by Florian Zschocke
# May 20, 2001
# Copyright 2001-2003

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
# build it with or without MySQL/PgSQL support. The default is to compile
# in debug mode but you can also choose to build optimized objects. 
#
# That leaves you with eight different targets run make on:
#
# Debug builds:
# standalone       Standalone version
# standaloneMySQL  Standalone with MySQL support
# standalonePgSQL  Standalone with PgSQL support
# metamod          Metamod plugin
# metamodMySQL     Metamod plugin with MySQL support
# metamodPgSQL     Metamod plugin with PgSQL support
#
# You have two options to build with optimization. Either run make on one
# of the targets listed above with the variable OPT defined to opt, eg.:
# $ make standalone OPT=opt
# Or you use one of the four targets below which prefix the targets with an O:
#
# Ostandalone      Optimized standalone version
# OstandaloneMySQL Optimized standalone with MySQL support
# OstandalonePgSQL Optimized standalone with PgSQL support
# Ometamod         Optimized metamod plugin
# OmetamodMySQL    Optimized metamod plugin with MySQL support
# OmetamodPgSQL    Optimized metamod plugin with PgSQL support
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


# The default target to build
DEFTARGET= metamod

# The version number 
include Version.mk

#make sure this is the correct compiler for your system
CC=gcc
CXX=g++

# The source pathes to the SDK and metamod
# These are the settings for a standard installation

# The top directory of the HL SDK.
SDKTOP= ../../../hlsdk
SDKSRC= $(SDKTOP)/multiplayer

# The directory with the metamod include files.
# For Linux we assume a link called 'metamod' in the adminmod directory
METATOP= ../../../metamod
METADIR= $(METATOP)/metamod

# The standard C++ library for static linking
STDCPPLIB=/usr/lib/libstdc++-libc6.2-2.a.3

# The directory of the MySQL include files
MYSQLINCDIR= /usr/include/mysql
# The directory of the MySQL library
MYSQLLIBDIR= /usr/lib/mysql

# The directory of the PostgreSQL include files
PGSQLINCDIR= /usr/include/pgsql
# The directory of the PostgreSQL library
PGSQLLIBDIR= /usr/lib/pgsql


# The directory to test your dll in
TESTDIR=""



# Core team members:
# you can enter your personal directory setup here
# that will leave the standard definitions alone.
# To protect your makefile from changes by a cvs update
# check it out with the sticky tag.
ifeq "$(USER)" "alfred"
	#SDKTOP= $(HOME)/sdk2.1/SDK
	#SDKSRC= $(SDKTOP)/SourceCode
	#METADIR= $(SDKSRC)/metamod/metamod
	#TESTDIR= /hdb3/halflife/cstrike/dlls
	#MYSQLINCDIR= /hdb1/home/alfred/hl_code/mysql/include
	#MYSQLLIBDIR= /hdb1/home/alfred/hl_code/mysql/lib 
	#STDCPPLIB=/usr/lib/gcc-lib/i586-mandrake-linux-gnu/2.96/libstdc++.a
	STDCPPLIB=/usr/lib/gcc-lib/i586-mandrake-linux-gnu/2.96/libstdc++.a
	CC=gcc-`gcc2-version`
	CXX=g++-`gcc2-version`
endif

ifeq "$(USER)" "flori"
	TESTDIR=/data/hlds_l/Admin
endif


# Your timezone string
TZONE= \"\"

ifeq "$(USER)" "flori"
	TZONE= \"CET\"
endif

ifeq "$(USER)" "alfred"
	TZONE= \"PST\"
endif


# !! If you only want to compile and go you shouldn't need to edit anything below this line !!
###############################################################################################

######################################################
# Bogus targets
######################################################

OPT=dbg
SQL=no
MM=mm
REL=beta

default: $(DEFTARGET)

all: default


sa: standalone

standalone:
	$(MAKE) am_standalone SQL=$(SQL) OPT=$(OPT)

sams: standaloneMySQL

standaloneMySQL:
	$(MAKE) am_standalone SQL=mysql OPT=$(OPT)

saps: standalonePgSQL

standalonePgSQL:
	$(MAKE) am_standalone SQL=pgsql OPT=$(OPT)

mm: metamod

metamod:
	$(MAKE) am_metamod MM=mm SQL=$(SQL) OPT=$(OPT)

mmms: metamodMySQL

metamodMySQL:
	$(MAKE) am_metamod MM=mm SQL=mysql OPT=$(OPT)

mmps: metamodPgSQL

metamodPgSQL:
	$(MAKE) am_metamod MM=mm SQL=pgsql OPT=$(OPT)

osa: Ostandalone

Ostandalone:
	$(MAKE) am_standalone SQL=$(SQL) OPT=opt

osams: OstandaloneMySQL

OstandaloneMySQL:
	$(MAKE) am_standalone SQL=mysql OPT=opt

osaps: OstandalonePgSQL

OstandalonePgSQL:
	$(MAKE) am_standalone SQL=pgsql OPT=opt

omm: Ometamod

Ometamod:
	$(MAKE) am_metamod MM=mm SQL=$(SQL) OPT=opt

ommms: OmetamodMySQL

OmetamodMySQL:
	$(MAKE) am_metamod MM=mm SQL=mysql OPT=opt

ommps: OmetamodPgSQL

OmetamodPgSQL:
	$(MAKE) am_metamod MM=mm SQL=pgsql OPT=opt

release:
	$(MAKE) clean MM=mm SQL=$(SQL) OPT=opt
	$(MAKE) am_metamod MM=mm SQL=$(SQL) OPT=opt REL=release

mysqlrelease:
	$(MAKE) clean MM=mm SQL=mysql OPT=opt
	$(MAKE) am_metamod MM=mm SQL=mysql OPT=opt REL=release

pgsqlrelease:
	$(MAKE) clean MM=mm SQL=pgsql OPT=opt
	$(MAKE) am_metamod MM=mm SQL=pgsql OPT=opt REL=release

mysql: $(DEFTARGET)MySQL

pgsql: $(DEFTARGET)PgSQL

opt: O$(DEFTARGET)

mysqlopt: O$(DEFTARGET)MySQL

pgsqlopt: O$(DEFTARGET)PgSQL


#################################################################
# GENERAL OPTIONS
#################################################################

MODNAME= admin
ARCH= i386
SHLIBEXT=so

# set the beta flag for beta versions
ifeq "$(REL)" "beta"
	BETA = -DBETA
else
	BETA =
endif

BASE_CFLAGS=-Dstricmp=strcasecmp -D_strnicmp=strncasecmp -Dstrnicmp=strncasecmp -Dstrcmpi=strcasecmp -DLINUX -DVERSION=\"$(VERSION)\" $(BETA)
XTFLAGS= -DTZONE=$(TZONE)

ifneq "$(VERSION)" ""
	ifeq "$(MM)" "mm"
		MM_VSTRING= \(MM\)
	endif

	ifeq "$(SQL)" "mysql"
		SQL_VSTRING= \(mysql\)
	endif

	ifeq "$(SQL)" "pgsql"
		SQL_VSTRING= \(pgsql\)
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
CFOPT=  -march=i586 -O3 
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
SHLIBLDFLAGS=-shared -nodefaultlibs -lc -lgcc




###############################################################
# DIRECTORY SETUP
###############################################################

OBJDIR= obj
SRCDIR= .
AMXDIR= ../amxdll
BINDIR= .
COMMONDIR = .

INCLUDEDIRS=-I$(SDKSRC)/engine -I$(SDKSRC)/common -I$(SDKSRC)/pm_shared -I$(SDKSRC)/dlls -I$(AMXDIR) -I../../lib/commonlib/include -I../../lib/xmlmarkup -I../../tools/amxconvert -I.

ifeq "$(MM)" "mm"
	XTFLAGS+= -DUSE_METAMOD
	INCLUDEDIRS+= -I$(METADIR)
	OBJDIR= objMM
endif

MYSQLEXT=""
ifeq "$(SQL)" "mysql"
	XTFLAGS+= -DUSE_MYSQL
	INCLUDEDIRS+= -I$(MYSQLINCDIR)
	OBJDIR := $(OBJDIR).mysql
	MYSQLEXT="_mysql"
endif

PGSQLEXT=""
ifeq "$(SQL)" "pgsql"
	XTFLAGS+= -DUSE_PGSQL
	INCLUDEDIRS+= -I$(PGSQLINCDIR)
	OBJDIR := $(OBJDIR).pgsql
	PGSQLEXT="_pgsql"
endif



##############################################################
# OBJECTS AND LIBRARIES
##############################################################

#
# Adminmod source files
#
CPPSRC = \
dll.cpp h_export.cpp util.cpp \
admin_commands.cpp users.cpp cbase.cpp timer.cpp \
CLinkList.cpp CPlugin.cpp admin_mod.cpp version.cpp \
sutils.cpp statics.cpp authid.cpp amutil.cpp AmFSNode.cpp \
events.cpp \
builtin_commands.cpp commandhandler.cpp \
fileio.cpp

CSRC = amlibc.c


#
# Object files
#
OBJ=$(addprefix $(OBJDIR)/,$(subst .cpp,.o,$(CPPSRC)))
OBJ+=$(addprefix $(OBJDIR)/,$(subst .c,.o,$(CSRC)))
# Objects from the amx converter
OBJADD = $(OBJDIR)/amxconvert_l.o

#
# Libraries
#
LDFLAGS=-ldl -lcrypt -lm -L../../lib -lamcommon -lxmlmarkup
LIBADD=   

ifeq "$(SQL)" "mysql"
#	LIBADD+= $(MYSQLLIBDIR)/libmysqlclient.a
	LIBADD+= -lmysqlclient
endif

ifeq "$(SQL)" "pgsql"
#	LIBADD+= $(PGSQLLIBDIR)/libpq.a
	LIBADD+= -lpq
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

LeakTracer.o: LeakTracer.cc
	$(CC) -fPIC -c -g -pipe -Wall -W LeakTracer.cc -o LeakTracer.o


##############################################################
# BUILD TARGETS AND RULES
##############################################################

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(DO_CXX)

$(OBJDIR)/%.o: $(SRCDIR)/%.cc
	$(DO_CXX)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(DO_CC)


# Objects from amxconverter
$(OBJDIR)/amxconvert_l.o: ../../tools/amxconvert/amxconv_l.cpp
	$(DO_CXX)

# make sure to recompile version.c for each link
$(OBJDIR)/version.o: $(SRC) $(SRCDIR)/*.h Version.mk


SA_TARGET= $(BINDIR)/$(MODNAME)_$(ARCH)$(MYSQLEXT)$(PGSQLEXT).$(SHLIBEXT)
MM_TARGET= $(BINDIR)/$(MODNAME)_MM_$(ARCH)$(MYSQLEXT)$(PGSQLEXT).$(SHLIBEXT)


am_standalone: $(SRCDIR)/resource.h $(SA_TARGET) $(COMMONDIR)/adminmod.rc 

am_metamod:   $(SRCDIR)/resource.h $(MM_TARGET) $(COMMONDIR)/adminmod.rc

$(SRCDIR)/resource.h: Version.mk
	./setrc.sh $@ "$(VERSION)" "$(MSFVERSION)" 

$(SA_TARGET) : neat $(OBJ) $(OBJADD)
	-rm -f $(SA_TARGET)
	$(CC)  $(CFLAGS) -o $@ $(OBJ) $(OBJADD) $(LIBADD) $(STDCPPLIB) $(SHLIBLDFLAGS) $(LDFLAGS)

$(MM_TARGET) : neat $(OBJ) $(OBJADD) 
	-rm -f  $(MM_TARGET)
	$(CXX) $(CFLAGS) -o $@ $(OBJ) $(OBJADD) $(LIBADD) $(STDCPPLIB) $(LDFLAGS) $(SHLIBLDFLAGS) 

neat:
	- if [ ! -d $(OBJDIR) ]; then mkdir $(OBJDIR); fi
clean:
	-rm -f $(OBJ)

tidy: clean
	-rm -f *~
	-rm -rf $(OBJDIR)

spotless: tidy
	-rm -f *.$(SHLIBEXT)


depend:
	$(CC) -MM $(INCLUDEDIRS) $(CSRC) | sed -e 's/^\(.*\.o\):/$(OBJDIR)\/\1/'
	$(CXX) -MM $(INCLUDEDIRS) $(CPPSRC) | sed -e 's/^\(.*\.o\):/$(OBJDIR)\/\1/'

tags:
	find . -name "*.[ch]*" -print | etags --members -


test: default
	cp -f $(MODNAME)_MM_$(ARCH)$(MYSQLEXT)$(PGSQLEXT).$(SHLIBEXT) $(TESTDIR)/dlls/$(MODNAME)_MM_$(ARCH).$(SHLIBEXT)

# $(TESTDIR)/$(MODNAME)_MM_$(ARCH).$(SHLIBEXT): $(MODNAME)_MM_$(ARCH).$(SHLIBEXT)
# 	install $< $@

# dlls: ../dlls/$(MODNAME)_MM_$(ARCH).$(SHLIBEXT)

# ../dlls/$(MODNAME)_MM_$(ARCH).$(SHLIBEXT): $(MODNAME)_$(ARCH).$(SHLIBEXT)
# 	install $< $@

win32 snap cleanall:
