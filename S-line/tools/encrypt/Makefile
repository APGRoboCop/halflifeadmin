FLAGS = -DLINUX

OBJS = encrypt_l.o encrypt.o

ifdef DEBUG
    CFLAGS = -g -ggdb
else
    CFLAGS = -O3
endif

CC = gcc
CXX = g++

CFLAGS += -I ../../commonlib/include
CXXFLAGS = $(CFLAGS)

LFLAGS = -L ../../commonlib -lamcommon

default: encrypt

.c.o:
	$(CC)  -Wall $(CFLAGS) $(FLAGS) -o $@ -c $<

.cc.o:
	$(CXX) -Wall $(CXXFLAGS) $(FLAGS) -o $@ -c $<

.cpp.o:
	$(CXX) -Wall $(CXXFLAGS) $(FLAGS) -o $@ -c $<

encrypt: $(OBJS)
	$(CC)  -Wall -o $@ $(OBJS) $(LFLAGS)

clean:
	rm -f *.o *~ encrypt
