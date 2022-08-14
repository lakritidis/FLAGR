# (Py)FLAGR

**Fuse, Learn, AGgregate, Rerank

FLAGR is a high performing, modular library for rank aggregation. To ensure the highest possible performance, the core FLAGR library is written in C++ and implements a wide collection of unsupervised rank aggregation methods. Its modular design allows third-party programmers to implement their own algorithms and easily rebuild the entire library. FLAGR can be built as a standard application, or as a shared library (`so` or `dll`). In the second case, it can be linked from other C/C++ programs, or even from programs written in other languages (e.g. Python, PHP, etc.).

In this context, PyFLAGR is a Python library that links to FLAGR and allows a developer to exploit the efficient FLAGR implementations from a standard Python program.

## Installing PyFLAGR

PyFLAGR can be installed directly by using `pip`:

`pip install pyflagr`

Alternatively, PyFLAGR can be installed from the sources by navigating to the directory where `setup.py` resides:

`pip install .`

## Importing and using PyFLAGR

PyFLAGR groups its supported rank aggregation methods in four modules:

1. `Comb`: In this module the `CombSUM` and `CombMNZ` methods are implemented. Each method comes in four variants according to the rank/score normalization method. Future releases of FLAGR will also include CombAVG, CombMAX and CombMIN.
2. `Majoritarian`: Includes `CondorcetWinners`, `CopelandWinners` and `OutrankingApproach`.
3. `MarkovChains`: The fourth and most popular method (termed `MC4`) based on Markov Chains is implemented. Future releases of FLAGR will include the other three implementations.
4. `Weighted`: This module implements several self-weighting rank aggregation methods. These methods automatically identify the expert voters and include:
 1. The Preference Relations Graph method of Desarkar et.al, 2016.
 2. The Agglomerative method of Chatterjee et.al, 2018.
 3. The Iterative, Distance-Based method of Akritidis et.al, 2022.

The following statements demonstrate the imports of all PyFLAGR rank aggregation methods in a typical jupyter notebook.
```
	import pyflagr.Comb as SCORE_BASED
	import pyflagr.Majoritarian as ORDER_BASED
	import pyflagr.MarkovChains as MARKOV_CHAINS
	import pyflagr.Weighted as WGT
```

All PyFLAGR rank aggregation methods include:
* a standard class constructor: several hyper-parameters of the corresponding algorithm  and other execution arguments can be passed through the constructor. All the constructor inputs have default values, therefore, they are considered optional. This means that all constructors can be called *any* argument at all.
* an `aggregate` method that runs the algorithm on the selected input and (optionally) evaluates the generated aggregate list. In all algorithms, `aggregate` method accepts the following arguments:

| Parameter    | Type                                         | Default Value  | Values  |
| :----------- | :--------------------------------------------| :--------------| :------ |
| `input_file` | String - Required, unless `input_df` is set. | Empty String   | A CSV file that contains the input lists to be aggregated. |
| `input_df` | Pandas DataFrame - Required, unless `input_file` is set. | `None` | A Pandas DataFrame that contains the input lists to be aggregated. **Note:** If both `input_file` and `input_df` are set, only the former is used; the latter is ignored. |
| `rels_file`  | String, Optional. | Empty String | A CSV file that contains the relevance judgements of the involved list elements. If such a file is passed, FLAGR will evaluate the generated aggregate list/s by computing several retrieval effectiveness evaluation measures. The results of the evaluation will be stored in the `eval_df` DataFrame. Otherwise, no evaluation will take place and `eval_df` will be empty. Read more on the evaluation of rank aggregation quality. |
| `rels_df`    | Pandas DataFrame, Optional. | `None` | A Pandas DataFrame that contains the relevance judgements of the involved list elements. If such a dataframe is passed, FLAGR will evaluate the generated aggregate list/s by computing several retrieval effectiveness evaluation measures. The results of the evaluation will be stored in the `eval_df` DataFrame. Otherwise, no evaluation will take place and `eval_df` will be empty. Read more on the evaluation of rank aggregation quality. **Note:** If both `rels_file` and `rels_df` are set, only the former is used; the latter is ignored. |
| `output_dir` | String, Optional. | Temporary directory (OS-specific) | The directory where the output files (aggregate lists and evaluation) will be stored. If it is not set, the default location will be used. |


## Input data

The core library, FLAGR, accepts data (namely, the input lists to be aggregated) in a single, specially formatted CSV file. The columns in the CSV file are organized according to the following manner:

`Query/Topic,Voter,Item,Score,Algorithm/Dataset`

where:
* `Query/Topic`: the query string or the topic for which the list is submitted.
* `Voter`: the name of the voter, or the ranker who submits the list.
* `Item`: a unique name that identifies a particular element in the list. A voter cannot submit the same element for the same query/topic two or more times. This means that each element appears exactly once in each list. However, the same element may appear in lists submitted by other voters.
* `Score`: the score assigned to an `Item` by a specific `Voter`. In may cases (e.g. search engine rankings), the individual scores are unknown. In such cases the scores can be replaced by the (reverse) ranking of an `Item` in such a manner that the top rankings receive higher scores than the ones that have been assigned lower rankings.

PyFLAGR has two mechanisms for passing data to FLAGR, namely:
* either by forwarding the name and the location of the aforementioned input CSV file (this is the `input_file` argument of the `aggregate` method),
* or by accepting a Pandas Dataframe from the user (this is the `input_df` argument of the `aggregate` method). In this case, PyFLAGR internally dumps the `input_df` contents into a temporary CSV file and passes the name and the location of that temporary file to FLAGR.

Optionally, the user may specify a second CSV file (called as `rels_file`), or a Dataframe (called as `rels_df`) that contain judgments about the relevance of the included elements w.r.t a query. The columns in `rels_file` are organized as follows:

`Query/Topic,0,Item,Relevance`

where:
* `Query/Topic`: the query string or the topic for which the corresponding `Item` is evaluated.
* `0`: A hypothetical hyper-voter (also called voter `0`) who has flawless knowledge of the `Query/Topic` and determines whether an `Item` is relevant to it, or not. The value of this column must be always `0`.
* `Item`: a unique name that identifies a particular element of which the relevance to the `Query/Topic` is evaluated.
* `Relevance`: the relevance score assigned to an `Item` by `Voter 0`.

## Output data format

PyFLAGR returns a Pandas Dataframe that contains the final aggregate list.

Optionally, FLAGR may also create a second output file to write the results of the evaluation of the effectiveness of an algorithm. This happens when a `rels_file` is provided to the algorithm. The `aggregate` method of all algorithms *always* returns two Pandas Dataframes according to the provided input.


## References:
\[1\] Renda E., Straccia U., "Web metasearch: rank vs. score based rank aggregation methods", In Proceedings of the 2003 ACM symposium on Applied computing, pp. 841-846, 2003.

\[2\] Farah, M., Vanderpooten, D., "An outranking approach for rank aggregation in information retrieval", In Proceedings of the 30th ACM Conference on Research and Development in Information Retrieval, pp. 591-598, 2007.

\[3\] Desarkar, M. S., Sarkar, S., Mitra, P., "Preference relations based unsupervised rank aggregation for metasearch", Expert Systems with Applications, vol. 49, pp. 86-98, 2016.

\[4\] Chatterjee, S., Mukhopadhyay, A., Bhattacharyya, M., "A weighted rank aggregation approach towards crowd opinion analysis", Knowledge-Based Systems, vol. 149, pp. 47-60, 2018.

\[5\] Akritidis L., Fevgas A., Bozanis P., Manolopoulos Y., "An Unsupervised Distance-Based Model for Weighted Rank Aggregation with List Pruning", Expert Systems with Applications, vol. 202, pp. 117435, 2022.

\[6\] Dwork C., Kumar R., Naor M., Sivakumar D., "Rank Aggregation Methods for the Web", In Proceedings of the 10th International Conference on World Wide Web, pp. 613-622, 2001.
