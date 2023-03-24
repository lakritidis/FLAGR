# (Py)FLAGR

*Fuse, Learn, AGgregate, Rerank*

FLAGR is a high performance, modular, open source C++ library for rank aggregation problems. It implements baseline and recent state-of-the-art algorithms that accept ranked preference lists and generate a single consensus list of elements.

PyFLAGR is a Python library built on top of FLAGR library core. It uses the FLAGR algorithm implementations by linking to the associated shared library. It can be easily installed with pip and used in standard Python programs and Jupyter notebooks.

The current FLAGR version is 1.0.8.

Both libraries are licensed under the [http://www.apache.org/licenses/LICENSE-2.0](Apache License, version 2).

Detailed documentation on the FLAGR and PyFLAGR components can be found in the `docs/` directory.

Code examples in C++ and Python exist in the `examples/` directory.


## Compiling from Sources

FLAGR can be compiled from its C++ sources by using the provided building scripts. These scripts require a working GCC compiler to be installed into the machine that performs the compilation.

There are two building scripts, one for Linux and one for Windows. Specifically:

* In Linux: type `make` in Terminal to build the binaries from the C++ sources. The FLAGR binaries include:
  * the executable program `FLAGR`, and
  * the shared `flagr.so` library. Third-party programs can link to this shared library to obtain access to the FLAGR algorithm implementations.
* In Windows: type `makefile.bat` in Windows CLI or Windows Powershell. The batch file will build the binaries from the C++ sources by generating two files:
  * the executable program `FLAGR.exe`, and
  * the Dynamic Link Library `flagr.dll`. Third-party Windows programs can link to this shared library to obtain access to the FLAGR algorithm implementations.

All the binary outputs of the building scripts (i.e., executables and libraries) will be created by default into the `bin/Release/` directory of the package.

## Running FLAGR

The application accepts 4 optional arguments in the following fashion:

`FLAGR [cutoff] [input_file] [output_path] [qrels_file]`

The input arguments are:

* `cutoff`: the evaluation cut-off point, that is, the number of items of the aggregate list that will be included in the evaluation process. If nothing is passed, then the value 10 is used.
* `input_file`: The full path to the input file that stores the input lists to be aggregated. This is where the aggregation algorithm/s read data from.
* `output_path`: This is where the output aggregate lists and the evaluation results are stored. If nothing is passed, then the default value output is used.
* `qrels_file`: This file stores the relevance judgments of the list elements. It is used to evaluate the employed rank aggregation algorithm/s. If nothing is passed, then no evaluation takes place.


## Installing PyFLAGR

PyFLAGR can be installed directly by using `pip`:

`pip install pyflagr`

Alternatively, PyFLAGR can be installed from the sources by navigating to the directory where `setup.py` resides:

`pip install .`

