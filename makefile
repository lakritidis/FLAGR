CC=g++
CFLAGS=-O3 -std=c++11 -Wall -Werror

INCDIR=-Isrc/
OBJDIR=obj/Release/
BINDIR=bin/Release/

LIBDIR=pyflagr/pyflagr/
LIBFLAGS=-shared -fPIC


FLAGR: flagr.o
	$(CC)  -o  $(BINDIR)FLAGR  $(OBJDIR)flagr.o  $(CFLAGS)  -s
	$(CC)  $(CFLAGS)  $(LIBFLAGS)  cflagr.cpp  -o  $(LIBDIR)flagr.so
	cp $(LIBDIR)flagr.so $(BINDIR)flagr.so

flagr.o: cflagr.cpp
	mkdir -p -m777 $(BINDIR)
	mkdir -p -m777 $(OBJDIR)
	mkdir -p -m777 $(BINDIR)output
	$(CC)  $(INCDIR)  $(CFLAGS)  -c  main.cpp  -o  $(OBJDIR)flagr.o

clean:
	rm -rf $(OBJDIR)
	rm -rf $(BINDIR)

