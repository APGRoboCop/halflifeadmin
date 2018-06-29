CC      =gcc-4.8
CFLAGS	=-DLINUX -Wall
LINK    =gcc-4.8
LINKFLAGS=
TARGET	=sc
OBJDIR= obj

AMXINCD = ../../dlls/amxdll
INCDIRS = -I $(AMXINCD)

# debug mode
#CFLAGS += -g -ggdb
CFLAGS += -O2 -msse2

ifeq "$(MARCH)" "x86_64"
	CFLAGS += -DTARGET_64BIT
	TARGET = sc64
	OBJDIR= obj64
endif

#
#
#
SRC   = sc1.o sc2.o sc3.o sc4.o sc5.o sc6.o sc7.o scvars.o sclist.o scexpand.o
OBJS  = $(addprefix $(OBJDIR)/,$(subst .c,.o,$(SRC)))

all :	$(TARGET) sc64

sc64:
	$(MAKE) MARCH=x86_64

neat:
	- if [ ! -d $(OBJDIR) ]; then mkdir $(OBJDIR); fi

clean :
	rm -f $(TARGET) sc64 *~
	rm -rf $(OBJDIR) $(OBJDIR)64 $(TARGET) *~

#
# Inference rules
#

$(OBJDIR)/%.o : %.c sc.h ;
	$(CC) -c $(CFLAGS) $(INCDIRS) $(CPPFLAGS) -o $@ $<

$(TARGET):	neat $(OBJS)
	$(LINK) $(LINKFLAGS) $(OBJS) -o $@

$(OBJDIR)/sc6.o : sc6.c sc.h $(AMXINCD)/amx.h
$(OBJDIR)/sc5.o : sc5.c sc5.scp
$(OBJDIR)/sc7.o : sc7.c sc7.scp

test:
	-install -m 0775 sc   /data3/Adminmod/scripting/compiler/
	-install -m 0775 sc64 /data3/Adminmod/scripting/compiler/
