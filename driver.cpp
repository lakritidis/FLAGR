/**
	FLAGR - Fuse, Learn, Aggregate, Rerank
	A high performance library for rank aggregation

	References:
	[1]  E. Renda, U. Straccia, "Web metasearch: rank vs. score based rank aggregation methods", In
		 Proceedings of the 2003 ACM symposium on Applied computing, pp. 841-846, 2003.
	[2]  M. Farah, D. Vanderpooten, "An outranking approach for rank aggregation in information
		 retrieval", In Proceedings of the 30th ACM Conference on Research and Development in
		 Information Retrieval, pp. 591-598, 2007.
	[3]  M.S. Desarkar, S. Sarkar, P. Mitra, "Preference relations based unsupervised rank
		 aggregation for metasearch", Expert Systems with Applications, vol. 49, pp. 86-98, 2016.
	[4]  S. Chatterjee, A. Mukhopadhyay, M. Bhattacharyya, "A weighted rank aggregation approach
		 towards crowd opinion analysis", Knowledge-Based Systems, vol. 149, pp. 47-60, 2018.
	[5]  L. Akritidis, A. Fevgas, P. Bozanis, Y. Manolopoulos, "An Unsupervised Distance-Based Model
		 for Weighted Rank Aggregation with List Pruning", Expert Systems with Applications,
		 vol. 202, pp. 117435, 2022.
	[6]  C. Dwork, R. Kumar, M. Naor, D. Sivakumar, "Rank Aggregation Methods for the Web", In
		 Proceedings of the 10th International Conference on World Wide Web, pp. 613-622, 2001.
	[7]  R. Kolde, S. Laur, P. Adler, J. Vilo, "Robust rank aggregation for gene list integration
		 and meta-analysis", Bioinformatics, vol. 28, no. 4, pp. 573--580, 2012.
	[8]  K.L. Majumder, G.P. Bhattacharjee, "Algorithm AS 63: The incomplete Beta Integral", Applied
		 Statistics, vol. 22, no. 3, pp. 409-411, 1973.
	[9]	 R.P. DeConde, S. Hawley, S. Falcon, N. Clegg, B. Knudsen, R. Etzioni, "Combining results of
		 microarray experiments: a rank aggregation approach", Statistical Applications in Genetics
		 and Molecular Biology, vol. 5, no. 1, 2006.
	[10] L. Akritidis, P. Bozanis, "", IISA 2024
**/

#include "stdio.h"
#include "stdlib.h"
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <dirent.h>
#include <assert.h>
#include <string.h>
#include <vector>

typedef uint32_t rank_t;
typedef double score_t;

#define MAX_LIST_ITEMS 1000
#define NOT_RANKED_ITEM_RANK MAX_LIST_ITEMS * MAX_LIST_ITEMS

#include "src/SimpleScoreStats.cpp"
#include "src/Rel.cpp"
#include "src/Rels.cpp"
#include "src/InputParams.cpp"
#include "src/Voter.cpp"
#include "src/InputItem.cpp"
#include "src/InputList.cpp"
#include "src/Ranking.cpp"
#include "src/MergedItem.cpp"
#include "src/MergedList.cpp"
#include "src/Evaluator.cpp"
#include "src/Aggregator.cpp"
#include "src/Query.cpp"

#include "src/input/InputData.cpp"

/// This function drives the entire FLAGR execution - It passes the Python Parameters to FLAGR
void FLAGR_DRIVER (UserParams uParams) {
	class InputParams * PARAMS = new InputParams(uParams);
	// PARAMS->display(); fflush(NULL);

	class InputData * input_data = new InputData(PARAMS);

	input_data->aggregate();

	if (uParams.rels_file) {
		input_data->evaluate();
	}

	delete input_data;
	delete PARAMS;
}
