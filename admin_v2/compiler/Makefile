

CC      =gcc
CFLAGS	=-I../amxdll -DLINUX -Wall -march=i586
LINK    =gcc
LINKFLAGS =
TARGET	=sc

# debug flags
# CFLAGS += -g

# optimization flags
CFLAGS += -O -DNDEBUG
 
#
#
#
OBJS=sc1.o sc2.o sc3.o sc4.o sc5.o sc6.o sc7.o scvars.o sclist.o scexpand.o

all :	$(TARGET)

clean :
	rm -f $(OBJS) $(TARGET) *~

#
# Inference rules
#

%.o : %.c sc.h
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<

sc :	$(OBJS)
	$(LINK) $(LINKFLAGS) $(OBJS) -o $@

