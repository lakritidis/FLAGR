cd C:\Program Files\CodeBlocks\MinGW\bin
g++ -O3 -c -o C:/Users/Leo/Documents/cpp_algorithms/FLAGR/shared/flagr.o C:/Users/Leo/Documents/cpp_algorithms/FLAGR/dllflagr.cpp
g++ -O3 -o C:/Users/Leo/Documents/cpp_algorithms/FLAGR/pyflagr/pyflagr/flagr.dll -s -shared C:/Users/Leo/Documents/cpp_algorithms/FLAGR/shared/flagr.o -Wl,--subsystem,windows
cd C:\Users\Leo\Documents\cpp_algorithms\FLAGR