@echo off

set CC=g++.exe
set CFLAGS=-Wall -std=gnu++11 -O3 -std=c++11 -Wno-unused-but-set-variable
set INCDIR=-Isrc\
set OBJDIR=obj\Release\
set BINDIR=bin\Release\

set LIBDIR=pyflagr\pyflagr\
set LIBFLAGS=-shared
set DLLFLAGS=-Wl,--subsystem,windows

mkdir %BINDIR%
mkdir %OBJDIR%
mkdir %BINDIR%output

%CC%  %INCDIR%  %CFLAGS%  -c  main.cpp  -o  %OBJDIR%flagr.o
%CC%  -o  %BINDIR%FLAGR.exe  %OBJDIR%flagr.o  %CFLAGS%  -s

%CC%  %INCDIR%  %CFLAGS%  -c  dllflagr.cpp  -o  %OBJDIR%flagr.o
%CC%  %CFLAGS%  -o  %LIBDIR%flagr.dll -s %LIBFLAGS%  %OBJDIR%flagr.o %DLLFLAGS%

copy %LIBDIR%flagr.dll %BINDIR%flagr.dll
