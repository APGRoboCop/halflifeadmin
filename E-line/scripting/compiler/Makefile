CC      =gcc
CFLAGS	=-DLINUX -Wall
LINK    =gcc
LINKFLAGS=
TARGET	=sc

AMXINCD = ../../dlls/amxdll
INCDIRS = -I$(AMXINCD)

# debug mode
#CFLAGS += -g -ggdb
CFLAGS += -O2

#
#
#
OBJDIR= obj
SRC   = sc1.o sc2.o sc3.o sc4.o sc5.o sc6.o sc7.o scvars.o sclist.o scexpand.o
OBJS  = $(addprefix $(OBJDIR)/,$(subst .c,.o,$(SRC)))

all :	$(TARGET)

neat:
	- if [ ! -d $(OBJDIR) ]; then mkdir $(OBJDIR); fi

clean :
	rm -f $(TARGET) *~
	rm -rf $(OBJDIR) $(TARGET) *~

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
