# (Py)FLAGR

**Fuse, Learn, AGgregate, Rerank

FLAGR is a high performing, modular library for rank aggregation. To ensure the highest possible performance, the core FLAGR library is written in C++ and implements a wide collection of unsupervised rank aggregation methods. Its modular design allows third-party programmers to implement their own algorithms and easily rebuild the entire library. FLAGR can be built as a standard application, or as a shared library (`so` or `dll`). In the second case, it can be linked from other C/C++ programs, or even from programs written in other languages (e.g. Python, PHP, etc.).

In this context, PyFLAGR is a Python library that links to FLAGR and allows a developer to exploit the efficient FLAGR implementations from a standard Python program.

The library is fully documented at [http://flagr.mywork.gr/](http://flagr.mywork.gr/)


## Installing PyFLAGR

PyFLAGR can be installed directly by using `pip`:

`pip install pyflagr`

Alternatively, PyFLAGR can be installed from the sources by navigating to the directory where `setup.py` resides:

`pip install .`
