# Code example: Using FLAGR from third-party C/C++ applications

This example project demonstrates how a standard C/C++ application can link to the FLAGR shared library (either `flagr.so`, or `flagr.dll`) and invoke the implementations of the underlying rank aggregation algorithms. The code file `main.cpp` must include the attached `flagr.h` header file that contains the descriptors of the FLAGR exposed functions.

As a demonstration, this sample program calls 10 rank aggregation methods from FLAGR. The algorithms read the provided input files and write their results (namely, the generated aggregate lists and optionally, the performance evaluation measures) in the output directory that was specified above.

## Compilation & Building

`main.cpp` must be built by:
* adding its current path into the build path, and 
* linking to the provided FLAGR shared library (either `flagr.so` on Linux, or `flagr.dll` on Windows).

The package contains two scripts that automatically compile and build the demonstration program. More specifically, these scripts include:

* a `makefile` for Linux-based systems, and
* the batch file `makefile.bat` for Windows-based systems.

Both scripts require a GCC compiler.

## Input arguments

This demonstration program accepts 4 optional arguments in the following fashion:

`main [cutoff] [input_file] [output_path] [qrels_file]`

The input arguments are:
1. `cutoff` represents the evaluation cut-off point. That is, how many elements of the aggregate list will be included in the evaluation process. If nothing is passed, the default value 10 is used.
2. `input_file`: This where the aggregation algorithm/s read data from (that is, the input lists to be aggregated). If nothing is passed, the default value is `../../examples/testdata/testdata.csv`.
3. `output_path`: This is where the program writes the generated aggregate lists and the results of the evaluation process. If nothing is passed, then the default value `output` is used.
4. `qrels_file`: This file stores the relevance judgments of the list elements and it is used by FLAGR to evaluate the employed rank aggregation algorithm/s. If nothing is passed, then the default value `../../examples/testdata/testdata_qrels.csv` is used.

## Default input data
If the `input_file` parameter is left blank, then the program uses the sample data that is stored in `../../examples/testdata/testdata.csv`. In addition, it reads some indicative relevance judgments from `../../examples/testdata/testdata_qrels.csv` to evaluate the performance of the algorithms. Both files comprise a synthetic dataset that was created by employing [RASDaGen](https://github.com/lakritidis/RASDaGen), a synthetic dataset generator for rank aggregation applications.

`testdata.csv` contains the preference lists that were submitted by 50 voters for 20 queries. Each input list contains 30 elements. Therefore, the number of rows in this file is equal to $50 \times 20 \times 30=30000$. The columns of this CSV file must be organized in the following manner:

``Query, Voter Name, Item Code, Item Score, Algorithm/Dataset``

where
* `Query` represents the topic for which the preference list is submitted,
* `Voter` is the name of the ranker who submitted a preference list for a particular `Query`,
* `Item Code` is a unique name that identifies each element of the preference lists,
* `Item Score` is the preference score assigned to an item by a `Voter`, and
* `Algorithm/Dataset` is a user-defined string that represents the origin of a particular preference list.

On the other hand, `testdata_qrels.csv` contains relevance judgments for the preference list elements of the primary input file for each query. It is organized in the following fashion:

``Query, 0, Item Code, Relevance``

where:
* `Query` represents the topic for which the preference list is submitted,
* `0`: unused. This value must be always 0.
* `Item Code` is a unique name that identifies each element of the preference lists,
* `Relevance` is nn integer value that represents the relevance of the item with respect to the mentioned `Query`. Typically, zero values represent irrelevant and incorrect elements; negative values represent spam elements; and positive values represent relevant, correct and informative elements.
