set CC=g++.exe
set CFLAGS=-Wall -std=gnu++11 -O3 -std=c++11
set DLLPATH=.

%CC%  %CFLAGS%  -I%DLLPATH% -c  main.cpp  -o  main.o
%CC%  -L%DLLPATH% -o  main.exe  main.o  -s %DLLPATH%/flagr.dll