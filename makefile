CC=g++
CFLAGS=-O3 -std=c++11 -Wall -Werror
INCDIR=-Isrc/
OBJDIR=obj/
BINDIR=bin/
LIBFLAGS=-shared -fPIC
LIBDIR=pyflagr/pyflagr/

FLAGR: cflagr.o
	$(CC)  -o  $(BINDIR)FLAGR  $(OBJDIR)cflagr.o  $(CFLAGS)  -s

lib: cflagr.cpp
	$(CC)  $(CFLAGS)  $(LIBFLAGS)  cflagr.cpp  -o  $(LIBDIR)pyflagr.so

cflagr.o: cflagr.cpp
	$(CC)  $(INCDIR)  $(CFLAGS)  -c  cflagr.cpp  -o  $(OBJDIR)cflagr.o

clean:
	rm -f $(OBJDIR)/cflagr.o
	rm -f $(BINDIR)/FLAGR

