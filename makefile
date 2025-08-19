CC=g++

UNAME_S := $(shell uname -s)
 
ifeq ($(UNAME_S), Linux)
	CFLAGS=-O3 -std=c++11 -Wall -Werror -Wno-unused-but-set-variable
	LIB_NAME = flagr.so
endif
ifeq ($(UNAME_S), Darwin)
	UNAME_M := $(shell uname -m)
	ifeq ($(UNAME_M), arm64)
		CFLAGS=-O3 -std=c++11 -Wall -Werror -arch arm64 -Wno-vla-extension -Wno-unused-but-set-variable -Wno-deprecated-declarations
	else
		CFLAGS=-O3 -std=c++11 -Wall -Werror -Wno-vla-extension -Wno-unused-but-set-variable -Wno-deprecated-declarations
	endif
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
	cp $(LIBDIR)$(LIB_NAME) $(BINDIR)$(LIB_NAME)

flagr.o: cflagr.cpp
	mkdir -p $(BINDIR)
	mkdir -p $(OBJDIR)
	mkdir -p $(BINDIR)output
	$(CC)  $(INCDIR)  $(CFLAGS)  -c  main.cpp  -o  $(OBJDIR)flagr.o

clean:
	rm -rf $(OBJDIR)
	rm -rf $(BINDIR)

