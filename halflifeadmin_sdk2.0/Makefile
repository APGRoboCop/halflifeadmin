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

#make sure this is the correct compiler for your system
CC=gcc
#CC=egcs

OBJDIR=obj

BASE_CFLAGS=-Dstricmp=strcasecmp -D_strnicmp=strncasecm -Dstrnicmp=strncasecmp -Dstrcmpi=strcasecmp 


#safe optimization
#CFLAGS=$(BASE_CFLAGS) -m486 -O6 -ffast-math -funroll-loops \
# -fexpensive-optimizations -malign-loops=2  -malign-jumps=2 \
# -malign-functions=2 -Wall

# full debug flags
#CFLAGS=$(BASE_CFLAGS) -g -ggdb
#full optimization (WONT WORK WITH omit-frame-pointer !!!!!)
CFLAGS=$(BASE_CFLAGS) -m486 -O6 -ffast-math -funroll-loops \
	-fomit-frame-pointer -fexpensive-optimizations -malign-loops=2 \
	-malign-jumps=2 -malign-functions=2

#use these when debugging 
#CFLAGS+= -g -ggdb

INCLUDEDIRS=-I../engine -I../common -I../pm_shared -I../dlls

LDFLAGS=-ldl -lcrypt


OBJADD=   
#uncomment for mysql , linux ONLY!
#CFLAGS+= -DUSE_MYSQL -I/hdb1/home/alfred/hl_code/mysql/include -L/hdb1/home/alfred/hl_code/mysql/lib
#OBJADD= /hdb1/home/alfred/hl_code/mysql/lib/libmysqlclient.a     


SHLIBEXT=so
SHLIBCFLAGS=-fPIC
SHLIBLDFLAGS=-shared

DO_CC=$(CC) -w $(CFLAGS) $(SHLIBCFLAGS) $(INCLUDEDIRS) -o $@ -c $<

#############################################################################
# SETUP AND BUILD
# GAME
#############################################################################

$(OBJDIR)/%.o: %.cpp
	$(DO_CC)

SRC = \
dll.cpp h_export.cpp linkfunc.cpp users.cpp util.cpp

OBJ=$(addprefix $(OBJDIR)/,$(subst .cpp,.o,$(SRC)))

$(MODNAME)_$(ARCH).$(SHLIBEXT) : neat $(OBJ)
	$(CC) $(CFLAGS) $(SHLIBLDFLAGS) $(LDFLAGS) -o $@ $(OBJ) $(OBJADD) 

neat:
	- if [ ! -d $(OBJDIR) ]; then mkdir $(OBJDIR); fi
clean:
	-rm -f $(OBJ)
	-rm -f $(MODNAME)_$(ARCH).$(SHLIBEXT)
spotless: clean
	-rmdir $(OBJDIR)
depend:
	$(CC) -MM $(INCLUDEDIRS) $(SRC)
