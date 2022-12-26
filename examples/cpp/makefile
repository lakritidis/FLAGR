CC=g++
CFLAGS=-O3 -std=c++11 -Wall -Werror

LIBDIR=.


main: main.o
	$(CC)  -L$(LIBDIR) -o main  main.o  -s $(LIBDIR)/flagr.so

main.o: main.cpp
	$(CC)  $(CFLAGS) -I$(LIBDIR)  -c  main.cpp  -o  main.o

clean:
	rm -f main.o
	rm -f main

