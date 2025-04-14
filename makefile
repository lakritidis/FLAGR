CC=g++

UNAME_S := $(shell uname -s)
 
ifeq ($(UNAME_S), Linux)
	CFLAGS=-O3 -std=c++11 -Wall -Werror -Wno-unused-but-set-variable
	LIB_NAME = flagr.so
endif
ifeq ($(UNAME_S), Darwin)
	CFLAGS=-O3 -std=c++11 -Wall -Werror
	LIB_NAME = flagr.dylib
endif


INCDIR=-Isrc/
OBJDIR=obj/Release/
BINDIR=bin/Release/

LIBDIR=pyflagr/pyflagr/
LIBFLAGS=-shared -fPIC


FLAGR: flagr.o
	$(CC)  -o  $(BINDIR)FLAGR  $(OBJDIR)flagr.o  $(CFLAGS)  -s
	$(CC)  $(CFLAGS)  $(LIBFLAGS)  cflagr.cpp  -o  $(LIBDIR)$(LIB_NAME)
	cp $(LIBDIR)flagr.so $(BINDIR)$(LIB_NAME)

flagr.o: cflagr.cpp
	mkdir -p -m777 $(BINDIR)
	mkdir -p -m777 $(OBJDIR)
	mkdir -p -m777 $(BINDIR)output
	$(CC)  $(INCDIR)  $(CFLAGS)  -c  main.cpp  -o  $(OBJDIR)flagr.o

clean:
	rm -rf $(OBJDIR)
	rm -rf $(BINDIR)

