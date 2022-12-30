# Code example: Using PyFLAGR from third-party Python applications

This example project demonstrates the PyFLAGR's tools for testing and comparing rank aggregation methods.  We present the `Comparator` class and its member methods. This class allows the researchers to perform head-to-head comparisons of the performance of various rank aggregation algorithms.

As a demonstration, this sample program calls 20 rank aggregation methods from PyFLAGR. The algorithms read the provided input files and `Comparator` displays and plots the generated results.

## Installing PyFLAGR

PyFLAGR can be directly installed with pip:

`pip install pyflagr`

After the installation, PyFLAGR can be used in standard Python programs and Jupyter notebooks.


## Input data

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

