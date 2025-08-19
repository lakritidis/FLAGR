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

echo Finding and copying compiler dependencies...
for %%i in (g++.exe) do set GXX_FULL_PATH=%%~$path:i
if defined GXX_FULL_PATH (
    for %%F in ("%GXX_FULL_PATH%") do set GXX_DIR=%%~dpF
    echo Compiler found at: %GXX_DIR%
    if exist "%GXX_DIR%libgcc_s_seh-1.dll" (
        echo Copying libgcc_s_seh-1.dll...
        copy "%GXX_DIR%libgcc_s_seh-1.dll" %LIBDIR%
    ) else (
        echo Warning: libgcc_s_seh-1.dll not found in compiler directory.
    )
    if exist "%GXX_DIR%libstdc++-6.dll" (
        echo Copying libstdc++-6.dll...
        copy "%GXX_DIR%libstdc++-6.dll" %LIBDIR%
    ) else (
        echo Warning: libstdc++-6.dll not found in compiler directory.
    )
) else (
    echo Warning: g++.exe not found in PATH. Cannot copy dependencies.
)
