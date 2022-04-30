# FLAGR
**Fuse, Learn, AGgregate, Rerank**

This library implements a collection of unsupervised rank aggregation methods including:
* Borda Count
* Condorcet Method
* Outranking Approach [1]
* Preference Relation Method [2]
* Agglomerative Rank Aggregation [3]
* Distance-Based Iterative Aggregation [4]

The library reads the lists to be fused and aggregated from TREC-formatted input files, in both compressed (gz) and uncompressed form. Please refer to the [TREC guidelines](https://trec.nist.gov/) for more information. 

This code has been checked with Valgrind for memory leaks and runtime errors.

More methods are currently under development and will be included in future versions.

**References**
1. Farah, M., & Vanderpooten, D. (2007). An outranking approach for rank aggregation in information retrieval. In Proceedings of the 30th ACM Conference on Research and Development in Information Retrieval (pp. 591-598).
2. Desarkar, M. S., Sarkar, S., & Mitra, P. (2016). Preference relations based unsupervised rank aggregation for metasearch. Expert Systems with Applications, 49, 86-98.
3. Chatterjee, S., Mukhopadhyay, A., & Bhattacharyya, M. (2018). A weighted rank aggregation approach towards crowd opinion analysis. Knowledge-Based Systems, 149, 47-60.
4. L. Akritidis, A. Fevgas, P. Bozanis, Y. Manolopoulos, "An Unsupervised Distance-Based Model for Weighted Rank Aggregation with List Pruning", Expert Systems with Applications, to appear, 2022. 
