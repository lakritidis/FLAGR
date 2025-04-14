# (Py)FLAGR: A C++/Python library for Rank Aggregation

*Fuse, Learn, AGgregate, Rerank*

The fusion of multiple ranked lists of elements into a single aggregate list is a well-studied problem with numerous applications in Bioinformatics, recommendation systems, collaborative filtering, election systems and metasearch engines.

FLAGR is a high performance, modular, open source C++ library for rank aggregation problems. It implements baseline and state-of-the-art algorithms that accept multiple ranked preference lists and generate a single consensus list of elements. A portion of these methods apply exploratory analysis techniques and belong to the broad family of unsupervised learning techniques.

PyFLAGR is a Python library built on top of FLAGR library core. It uses the FLAGR algorithm implementations by linking to the associated shared library. It can be easily installed with pip and used in standard Python programs and Jupyter notebooks.

The current FLAGR version is 1.0.18.

Both libraries are licensed under the [http://www.apache.org/licenses/LICENSE-2.0](Apache License, version 2).

Detailed documentation on the FLAGR and PyFLAGR components are provided in the user manual that resides in the `docs/` directory.

Representative code examples in C++ and Python can be found in the `examples/` directory.


## Compiling from Sources

FLAGR can be compiled from its C++ sources by using the provided building scripts. These scripts require a working GCC compiler to be installed into the machine that performs the compilation.

There are two building scripts, one for Linux/MacOS and one for Windows. Specifically:

* In Linux/MacOS: type `make` in Terminal to build the binaries from the C++ sources. The FLAGR binaries include:
  * the executable program `FLAGR`, and
  * the shared `flagr.so`/`flagr.dylib` libraries. Third-party programs can link to these shared libraries to obtain access to the FLAGR core algorithm implementations.
* In Windows: type `makefile.bat` in Windows CLI or Windows Powershell. The batch file will build the binaries from the C++ sources by generating two files:
  * the executable program `FLAGR.exe`, and
  * the Dynamic Link Library `flagr.dll`. Third-party Windows programs can link to this shared library to obtain access to the FLAGR algorithm implementations.

All the binary outputs of the building scripts (i.e., executables and libraries) will be created by default into the `bin/Release/` directory of the package.

## Running FLAGR

The FLAGR binary is executed in an identical manner, regardless of the operating system. The application accepts 4 optional arguments in the following fashion:

`FLAGR [cutoff] [input_file] [output_path] [qrels_file]`

The input arguments are:

* `cutoff`: this is the evaluation cut-off point. That is, the number of items of the aggregate list that will be included in the evaluation process. If nothing is passed, then the value 10 is used.
* `input_file`: The full path to the input file that stores the input lists to be aggregated. This is where the aggregation algorithm/s read data from.
* `output_path`: This is where the program writes the generated aggregate lists and the results of the evaluation process. If nothing is passed, then the default value output is used.
* `qrels_file`: This file stores the relevance judgments of the list elements. It is used by FLAGR to evaluate the employed rank aggregation algorithm/s. If nothing is passed, then no evaluation takes place.


## Installing PyFLAGR

PyFLAGR can be installed directly by using `pip`:

`pip install pyflagr`

Alternatively, PyFLAGR can be installed from the sources by navigating to the directory where `setup.py` resides:

`pip install .`

