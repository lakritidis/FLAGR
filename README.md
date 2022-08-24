# (Py)FLAGR

* Fuse, Learn, AGgregate, Rerank



The fusion of multiple ranked lists of elements into a single aggregate list is a well-studied research field with numerous applications in Bioinformatics, recommendation systems, collaborative filtering, election systems and metasearch engines.

FLAGR is a high performance, modular, open source C++ library for rank aggregation problems. It implements baseline and recent state-of-the-art aggregation algorithms that accept ranked preference lists and generate a single consensus list of elements. A portion of these methods apply exploratory analysis techniques and belong to the broad family of unsupervised learning techniques.

PyFLAGR is a Python library built on top of FLAGR library core. It can be easily installed with pip (see below) and used in standard Python programs and Jupyter notebooks. Representative code examples can be found on this page.

The current FLAGR version is 1.0.6.

Both libraries are licensed under the Apache License, version 2.

The library is fully documented at [http://flagr.mywork.gr/](https://flagr.site/)


## Installing PyFLAGR

PyFLAGR can be installed directly by using `pip`:

`pip install pyflagr`

Alternatively, PyFLAGR can be installed from the sources by navigating to the directory where `setup.py` resides:

`pip install .`
