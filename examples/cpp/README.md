# Code example: Using FLAGR from third-party C/C++ applications

This example project demonstrates how a standard C/C++ application can link to the FLAGR shared library (either `flagr.so`, or `flagr.dll`) and invoke the implementations of the underlying rank aggregation algorithms. The code file `main.cpp` must include the attached `flagr.h` header file that contains the descriptors of the FLAGR exposed functions.

As a demonstration, this sample program calls 10 rank aggregation methods and writes their generated results in the output directory that was specified above.

## Compilation & Building

`main.cpp` must be built by:
* adding its current path into the build path, and 
* linking to the provided FLAGR shared library (either `flagr.so` on Windows, or `flagr.dll` on Linux).

This directory contains two scripts that automatically compile and build the demonstration program.

* a `makefile` for Linux-based systems.
* the batch file `makefile.bat` for Windows-based systems.

Both scripts require a GCC compiler.

## Arguments

This demonstration program accepts 4 optional arguments in the following fashion:

`main [cutoff] [input_file] [output_path] [qrels_file]`

The arguments are:
1. `cutoff` represents the evaluation cut-off point. That is, how many elements of the aggregate list will be included in the evaluation process. If nothing is passed, the default value 10 is used.
2. `input_file`: This where the aggregation algorithms read data from. If nothing is passed, the default value is `../../examples/testdata/testdata.csv`.
3. `output_path`: This is where the program writes the generated aggregate lists and the results of the evaluation process. If nothing is passed, then the default value `output` is used.
4. `qrels_file`: This file stores the relevance judgments of the list elements and it is used by FLAGR to evaluate the employed rank aggregation algorithm/s. If nothing is passed, then the default value `../../examples/testdata/testdata_qrels.csv` is used.
