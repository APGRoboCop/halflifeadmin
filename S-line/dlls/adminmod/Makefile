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

ifdef COMSPEC
    OS=windows
else
	OS=linux
endif

#################################################################
#
#  CONFIGURATION
#
#################################################################


# The default target to build
DEFTARGET=Ometamod

# The version number 
include Version.mk

# The source pathes to the SDK and metamod
# These are the settings for a standard installation

# The top directory of the HL SDK.
SDKSRC= ../../../../hlsdk-2.3-p4/multiplayer

# The directory with the metamod include files.
# For Linux we assume a link called 'metamod' in the adminmod directory
METADIR= ../../../../metamod-p-37
METASRC= $(METADIR)/metamod

# The standard C++ library for static linking
#STDCPPLIB=/usr/lib/libstdc++-libc6.2-2.so.3 # Old dir [APG]RoboCop[CL]
STDCPPLIB=/usr/lib/i386-linux-gnu/libstdc++.so.6

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
	SDKTOP= $(HOME)/sdk2.1/SDK
	SDKSRC= $(SDKTOP)/SourceCode
	METADIR= $(SDKSRC)/metamod/metamod
	TESTDIR= /hdb3/halflife/cstrike/dlls
	MYSQLINCDIR= /hdb1/home/alfred/hl_code/mysql/include
	MYSQLLIBDIR= /hdb1/home/alfred/hl_code/mysql/lib 
	STDCPPLIB=/usr/lib/gcc-lib/i586-mandrake-linux-gnu/2.96/libstdc++.a
endif

ifeq "$(USER)" "flori"
	TESTDIR=/data/hlds_l/Admin
endif


# Your timezone string
TZONE=

ifeq "$(USER)" "flori"
	TZONE=$(shell date +%z)
endif

#make sure this is the correct compiler for your system
CC=gcc-4.8
CXX=g++-4.8
#CC=clang
#CXX=clang++

# !! If you only want to compile and go you shouldn't need to edit anything below this line !!
###############################################################################################


######################################################
# Bogus targets
######################################################

OPT=dbg
SQL=no
MM=mm
REL=beta5

default: $(DEFTARGET)

all: release mysqlrelease pgsqlrelease


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
	$(MAKE) am_metamod MM=mm SQL=$(SQL) OPT=opt REL=APG

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
ARCH= i686
SHLIBEXT=so

#ARCH=i586
ARCHD=
ifeq "$(OS)" "linux"
	INSTALL=install -m 644
	#LD_WINDLL= PATH=$(PATH_WIN) $(PATH_WIN)/dllwrap
	#TARGET=linux
	#LIBFILE=$(LIBFILE_LINUX)
	#TARGET_FILE=$(TARGET_LINUX)
	ARCHD = $(shell uname -m)
	ifeq "$(ARCHD)" "x86_64"
		ARCH=x86_64
		ARCHD=.amd64
		#MODARCH=amd64
		#MODARCH=i386
		#STDCPPLIB = /usr/lib/gcc-lib/x86_64-pc-linux-gnu/3.3.3/libstdc++_pic.a
		STDCPPLIB = -lstdc++
	else
		ARCHD=
		#MODARCH=i386
	endif
else    ## windows
	INSTALL=cp
	#LD_WINDLL= $(PATH_WIN)/dllwrap
	TARGET=win32
	#LIBFILE=$(LIBFILE_WIN)
	#TARGET_FILE=$(TARGET_WIN)
endif

# set the beta flag for beta versions
ifeq "$(REL)" "release"
	BETA = 
else
	BETA = -DBETA=\"$(REL)\"
endif

BASE_CFLAGS=-include apsymbols.h -Dstricmp=strcasecmp -D_strnicmp=strncasecmp \
	-Dstrnicmp=strncasecmp -Dstrcmpi=strcasecmp -DLINUX -DVERSION=\"$(VERSION)\" $(BETA)
	
XTFLAGS= -DTZONE=\"$(TZONE)\"

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

GCCMAJ = $(shell gcc -dumpversion | sed -e 's/\.[0-9][0-9]*\.[0-9]//')

CFWARN = -Wall
#CFWARN = -w

ifeq "$(GCCMAJ)" "2"
	CFWARN += -Wbad-function-cast -Wmissing-declarations -Wnested-externs
endif


ifeq "$(ARCH)" "x86_64"
    # K8 targets only available from gcc 3.4 on.
	CFOPT = -march=i686 
else
	CFOPT = -march=i686 
endif

# debug build (WONT WORK WITH omit-frame-pointer !!!!!)

CFDBG= -g -ggdb
CFDBG+= -fno-defer-pop -fno-omit-frame-pointer

#full optimization // -O1 works best? [APG]RoboCop[CL]
CFOPT = -O2 -mtune=generic -m32 -mfpmath=sse -msse -msse2 \
	-fomit-frame-pointer -falign-loops=2 -falign-jumps=2 -falign-functions=2

CFLAGS = $(BASE_CFLAGS) $(CFWARN) 

ifeq "$(ARCH)" "x86_64"
	CFLAGS += -DTARGET_64BIT
endif

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

OBJDIR= obj$(ARCHD)

INCLUDEDIRS=-I../amxdll -I../../tools/amxconvert -I../../commonlib/include -I$(METASRC) \
	-I$(SDKSRC)/common -I$(SDKSRC)/engine -I$(SDKSRC)/pm_shared -I$(SDKSRC)/dlls 

ifeq "$(MM)" "mm"
	XTFLAGS+= -DUSE_METAMOD
	INCLUDEDIRS+= -I$(METADIR)
	OBJDIR := $(OBJDIR)MM
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
# mutil.cpp required? [APG]RoboCop[CL]
CPPSRC = \
dll.cpp h_export.cpp util.cpp \
admin_commands.cpp users.cpp cbase.cpp timer.cpp \
CLinkList.cpp CPlugin.cpp admin_mod.cpp version.cpp \
sutils.cpp statics.cpp authid.cpp amutil.cpp AmFSNode.cpp

CSRC = amlibc.c 

ifneq "$(MM)" "mm"
	CPPSRC+= linkfunc.cpp
endif

#
# Object files
#
OBJ=$(addprefix $(OBJDIR)/,$(subst .cpp,.o,$(CPPSRC)))
OBJ+=$(addprefix $(OBJDIR)/,$(subst .c,.o,$(CSRC)))

# Objects from the amx converter
OBJADD = $(OBJDIR)/amxconvert_l.o

#
# Dependencies files
#
DEP=$(addprefix $(OBJDIR)/,$(subst .cpp,.dep,$(CPPSRC)))
DEP+=$(addprefix $(OBJDIR)/,$(subst .c,.dep,$(CSRC)))

#
# Libraries
#
LDFLAGS=-ldl -lcrypt -lm
LIBADD=   

ifeq "$(ARCH)" "x86_64"
	COMMONLIB = amcommon64
else
	COMMONLIB = amcommon
endif

COMMONLIB_DIR = ../../commonlib
COMMONLIB_TARGET = $(COMMONLIB_DIR)/lib$(COMMONLIB).a

LDFLAGS += -L$(COMMONLIB_DIR) -l$(COMMONLIB)


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

# Implicit rules

# Objects
$(OBJDIR)/%.o: %.cpp
	$(DO_CXX)

$(OBJDIR)/%.o: %.cc
	$(DO_CXX)

$(OBJDIR)/%.o: %.c
	$(DO_CC)

# Dependencies
$(OBJDIR)/%.dep: %.c
	$(SHELL) -ec '$(CC) -MM $(INCLUDEDIRS) $(CFLAGS) $< \
                      | sed '\''s/\($*\)\.o[ :]*/$(OBJDIR)\/\1.o $(OBJDIR)\/$*.dep : /g'\'' > $@; \
                      [ -s $@ ] || rm -f $@'

$(OBJDIR)/%.dep: %.cpp
	$(SHELL) -ec '$(CXX) -MM $(INCLUDEDIRS) $(CFLAGS) $< \
                      | sed '\''s/\($*\)\.o[ :]*/$(OBJDIR)\/\1.o $(OBJDIR)\/$*.dep : /g'\'' > $@; \
                      [ -s $@ ] || rm -f $@'

$(OBJDIR)/%.dep: %.cc
	$(SHELL) -ec '$(CXX) -MM $(INCLUDEDIRS) $(CFLAGS) $< \
                      | sed '\''s/\($*\)\.o[ :]*/$(OBJDIR)\/\1.o $(OBJDIR)\/$*.dep : /g'\'' > $@; \
                      [ -s $@ ] || rm -f $@'

#explicit rules

# Objects from amxconverter
$(OBJDIR)/amxconvert_l.o: ../../tools/amxconvert/amxconv_l.cpp
	$(DO_CXX)

VERSIONOBJ = $(OBJDIR)/version.o

# make sure to recompile version.c for each link
$(VERSIONOBJ): $(SRC)  Version.mk

version:
	-rm -f $(VERSIONOBJ)

SA_TARGET= $(MODNAME)$(MODARCH)$(MYSQLEXT)$(PGSQLEXT).$(SHLIBEXT)
MM_TARGET= $(MODNAME)_mm$(MODARCH)$(MYSQLEXT)$(PGSQLEXT).$(SHLIBEXT)


am_standalone: resource.h $(SA_TARGET) adminmod.rc 

am_metamod:  resource.h $(MM_TARGET) adminmod.rc 

resource.h: Version.mk
	./setrc.sh $@ "$(VERSION)" "$(MSFVERSION)" 

$(COMMONLIB_TARGET):
	-make -C $(COMMONLIB_DIR)

$(SA_TARGET) : neat version $(OBJ) $(OBJADD) $(COMMONLIB_TARGET)
	-rm -f $(MODNAME)$(MODARCH)$(MYSQLEXT)$(PGSQLEXT).$(SHLIBEXT)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(OBJADD) $(LIBADD) $(STDCPPLIB) $(SHLIBLDFLAGS) $(LDFLAGS)

$(MM_TARGET) : neat version $(OBJ) $(OBJADD) $(COMMONLIB_TARGET)
	-rm -f $(MODNAME)_mm$(MODARCH)$(MYSQLEXT)$(PGSQLEXT).$(SHLIBEXT)
	$(CXX) $(CFLAGS) -o $@ $(OBJ) $(OBJADD) $(LIBADD) $(STDCPPLIB) $(LDFLAGS) $(SHLIBLDFLAGS) 

neat:
	- if [ ! -d $(OBJDIR) ]; then mkdir $(OBJDIR); fi

clean:
	-rm -f $(OBJDIR)/*.o

tidy: clean
	-rm -f *~
	-rm -rf $(OBJDIR)

spotless: tidy
	-rm -f *.$(SHLIBEXT)

# explicit depend
depend: neat
	$(CC) -M $(INCLUDEDIRS) $(CSRC) | sed -e 's/^\(.*\.o\)[ :]*/$(OBJDIR)\/\1 : /' > $(OBJDIR)/depend
	$(CXX) -M $(INCLUDEDIRS) $(CPPSRC) | sed -e 's/^\(.*\.o\)[ :]*/$(OBJDIR)\/\1 : /' >> $(OBJDIR)/depend

# include auto depend
include $(DEP)
-include neat

tags:
	find . -name "*.[ch]*" -print | etags --members -


test: default
	cp -f $(MODNAME)_mm$(MODARCH)$(MYSQLEXT)$(PGSQLEXT).$(SHLIBEXT) $(TESTDIR)/dlls/$(MODNAME)_mm$(MODARCH).$(SHLIBEXT)

# $(TESTDIR)/$(MODNAME)_MM_$(ARCH).$(SHLIBEXT): $(MODNAME)_MM_$(ARCH).$(SHLIBEXT)
# 	install $< $@

# dlls: ../dlls/$(MODNAME)_MM_$(ARCH).$(SHLIBEXT)

# ../dlls/$(MODNAME)_MM_$(ARCH).$(SHLIBEXT): $(MODNAME)_$(ARCH).$(SHLIBEXT)
# 	install $< $@

win32 snap cleanall:
