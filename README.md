# (Py)FLAGR

**Fuse, Learn, AGgregate, Rerank

FLAGR is a high performing, modular library for rank aggregation. To ensure the highest possible performance, the core FLAGR library is written in C++ and implements a wide collection of unsupervised rank aggregation methods. Its modular design allows third-party programmers to implement their own algorithms and easily rebuild the entire library. FLAGR can be built as a standard application, or as a shared library (`so` or `dll`). In the second case, it can be linked from other C/C++ programs, or even from programs written in other languages (e.g. Python, PHP, etc.).

In this context, PyFLAGR is a Python library that links to FLAGR and allows a developer to exploit the efficient FLAGR implementations from a standard Python program.

## Compiling FLAGR as a shared library

The FLAGR shared library has been already pre-built and tested with the GCC compiler. The FLAGR Github repository includes the appropriate `.so` and `.dll` dynamic libraries in the `pyflagr` directory, so PyFLAGR can be immediately installed without compilation.

Nevertheless, in case a custom rank aggregation method has been implemented with FLAGR, or any modification in the C++ code has been made, FLAGR must be rebuilt and PyFLAGR must be reinstalled. For Linux-based systems with the GCC compiler, FLAGR can be built as a shared library by invoking the following system command:

`g++ -O3 -Wall -Werror -shared -std=c++11 -fPIC /path/to/cflagr/cflagr.cpp -o /path/to/so/flagr.so`

This command will generate the necessary `.so` library.

For Windows-based systems with the GCC compiler, FLAGR can be built as a Dynamic Link Library by invoking the following system commands:

* `g++ -O3 -c -o flagr.o /path/to/cflagr/cflagr.cpp`
* `g++ -O3 -o /path/to/dll/flagr.dll -s -shared flagr.o -Wl,--subsystem,windows`

This command will generate the necessary `.dll` library.

## Installing PyFLAGR

The installation of PyFLAGR can be performed by employing the `pip` Python package installer. Initially we navigate to the directory where the `setup.py` file resides.

After navigating to the directory where `setup.py` resides, install it by using `pip`:

`pip install .`

## Importing and using PyFLAGR

PyFLAGR groups its supported rank aggregation methods in four modules:

1. `Comb`: In this module the `CombSUM` and `CombMNZ` methods are implemented. Each method comes in four variants according to the rank/score normalization method. Future releases of FLAGR will also include CombAVG, CombMAX and CombMIN.
2. `Majoritarian`: Includes `CondorcetWinners` and `OutrankingApproach`.
3. `MarkovChains`: The fourth and most popular method (termed `MC4`) based on Markov Chains is implemented. Future releases of FLAGR will include the other three implementations.
4. `Weighted`: This module implements several self-weighting rank aggregation methods. These methods automatically identify the expert voters and include:
 1. The Preference Relations Graph method of Desarkar et.al, 2016.
 2. The Agglomerative method of Chatterjee et.al, 2018.
 3. The Iterative, Distance-Based method of Akritidis et.al, 2022.

The following statements demonstrate the imports of all PyFLAGR rank aggregation methods in a typical jupyter notebook.

`import pyflagr.Comb as SCORE_BASED
import pyflagr.Majoritarian as ORDER_BASED
import pyflagr.MarkovChains as MARKOV_CHAINS
import pyflagr.Weighted as WGT`

## 	References:
	[1] Renda E., Straccia U., "Web metasearch: rank vs. score based rank aggregation methods", In
		Proceedings of the 2003 ACM symposium on Applied computing, pp. 841-846, 2003.
	[2] Farah, M., Vanderpooten, D., "An outranking approach for rank aggregation in information
		retrieval", In Proceedings of the 30th ACM Conference on Research and Development in
		Information Retrieval, pp. 591-598, 2007.
	[3] Desarkar, M. S., Sarkar, S., Mitra, P., "Preference relations based unsupervised rank
		aggregation for metasearch", Expert Systems with Applications, vol. 49, pp. 86-98, 2016.
	[4] Chatterjee, S., Mukhopadhyay, A., Bhattacharyya, M., "A weighted rank aggregation approach
		towards crowd opinion analysis", Knowledge-Based Systems, vol. 149, pp. 47-60, 2018.
	[5] Akritidis L., Fevgas A., Bozanis P., Manolopoulos Y., "An Unsupervised Distance-Based Model
		for Weighted Rank Aggregation with List Pruning", Expert Systems with Applications,
		vol. 202, pp. 117435, 2022.
	[6] Dwork C., Kumar R., Naor M., Sivakumar D., "Rank Aggregation Methods for the Web", In
		Proceedings of the 10th International Conference on World Wide Web, pp. 613-622, 2001.
