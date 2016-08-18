#
# ===========================================================================
#
# $Id: Makefile,v 1.1 2003/11/08 10:21:26 darope Exp $
#
#
# Copyright (c) 2003 Florian Zschocke, Alfred Reynolds
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
# ===========================================================================
#
# Comments:
#
# This the the Makefile for the XML markup library for Admin Mod and it's tools.
#



TARGET = ../libxmlmarkup

FLAGS = -DLINUX

SRCDIR = .
OBJDIR = obj
SRC = XMLNode.cpp XMLNodes.cpp XMLParser.cpp XMLStream.cpp
OBJS = $(addprefix $(OBJDIR)/,$(subst .cpp,.o,$(SRC)))

ifdef DEBUG
  CFLAGS= -g -ggdb
else
 CFLAGS = -O3
endif

#CFLAGS += -Iinclude

CXXFLAGS = $(CFLAGS)

default: $(TARGET).a

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -Wall $(CFLAGS) $(FLAGS) -o $@ -c $< 

$(OBJDIR)/%.o: $(SRCDIR)/%.cc
	$(CXX) -Wall $(CXXFLAGS) $(FLAGS) -o $@ -c $< 

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) -Wall $(CXXFLAGS) $(FLAGS) -o $@ -c $< 

$(TARGET).a: neat $(OBJS)
	-ar rcvsu $(TARGET).a $(OBJS)


neat:
	- if [ ! -d $(OBJDIR) ]; then mkdir $(OBJDIR); fi

clean:
	-rm -rf $(OBJDIR)
	-rm $(TARGET).a

spotless: clean
	rm -f  $(SRCDIR)/*~ *~

