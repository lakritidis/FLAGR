/**
UPM
--------------------------------------------------------------------------------------------------
Unsupervised Rank Aggregation

Leonidas Akritidis, 13/01/2019
**/

#include "stdio.h"
#include "stdlib.h"
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <dirent.h>
#include <assert.h>
#include <string>
#include <vector>

using namespace std;

char * PadStr(char *str, uint32_t len, const char delim) {
	uint32_t i = 0;
	for (i = strlen(str); i < len; i++) {
		str[i] = delim;
	}
	str[i] = 0;
	return str;
}

double TestScaledFootrule() {
	uint32_t i = 0, j = 0;
	double rank_r = 0.0, rank_l = 0.0, dis = 0.0, ndis = 0.0;

	uint32_t nR1 = 5;
	char R1[5][2] = { "C", "D", "E", "A", "B" };

	uint32_t nL = 5;
	char L[5][2] = { "A", "B", "C", "D", "E" };

	for (i = 0; i < nR1; i++) {
		rank_r = (i + 1.0) / nR1;
		for (j = 0; j < nL; j++) {
			if (strcmp(R1[i], L[j]) == 0) {
				rank_l = (j + 1.0) / nL;
				dis += fabs(rank_l - rank_r);
//				printf("%5.3f - %5.3f\n", rank_r, rank_l);
				break;
			}
		}
	}
	ndis = 2 * dis / nR1;
	printf("Distance: %5.3f - Normalized: %5.3f\n", dis, ndis);

	return dis;
}

typedef uint32_t rank_t;
typedef double score_t;

uint32_t SUM_ITERATIONS = 0;

#define DATASET_TYPE 1
#define RESULTS_TYPE 2

#ifdef __linux__
	/// Linker Settings for Unix: -l z
	#include <zlib.h>

	const char BASE_PATH[] = "/media/leo/B65266EC5266B133/phd_Research/08 - Datasets/TREC/";

	#if DATASET_TYPE == 1
		const char TREC_NAME[] = "TREC2011";
		const char TREC_TRACK[] = "Web";
		const char TREC_TASK[] = "Adhoc";

		const uint32_t DATASET_NUM_QUERIES = 50;
	#elif DATASET_TYPE == 2
//		const char DATASET[] = "/media/leo/B65266EC5266B133/phd_Research/Datasets/LETOR/MQ2007-agg/agg.txt";
		const char DATASET[] = "/media/leo/B65266EC5266B133/phd_Research/Datasets/LETOR/MQ2007-agg/Fold1/test.txt";
	#endif
#elif _WIN32
	/// Linker Settings for Windows: -l zlib/libz.a
	#include "zlib/zlib.h"

	const char BASE_PATH[] = "C:/Users/Leon/Documents/datasets/TREC/";

	#if DATASET_TYPE == 1
		const char TREC_NAME[] = "TREC2014";
		const char TREC_TRACK[] = "Web";
		const char TREC_TASK[] = "Adhoc";

		const uint32_t DATASET_NUM_QUERIES = 50;
	#elif DATASET_TYPE == 2
		const char DATASET[] = "C:/Users/Leon/Documents/datasets/LETOR/MQ2007-agg/Fold1/test.txt";
//		const char DATASET[] = "C:/Users/Leon/Documents/datasets/LETOR/MQ2007-agg/agg.txt";
	#endif
#endif // _WIN32

/// Voter Weights Normalization Method
/// 1: No normalization (use raw values)
/// 2: Min-Max
/// 3: Z-score
/// 4: Division my max (same as 1)
#define WEIGHTS_NORMALIZATION 1

/// Aggregation Methods
/// 1: Borda Count
/// 2: Condorcet Method
/// 3: Outranking Approach
/// 4: Rank Position
#define AGGREGATION_METHOD 3

/// Rank Correlation Methods
/// 1: Spearman's Rho
/// 2: Scaled Footrule Distance
/// 3: Cosine Similarity
/// 4: Local Scaled Footrule Distance
/// 5: Kendall's Tau
#define CORRELATION_METHOD 3

/// ITERATIONS = 0 : execute the non-iterative (baseline) versions or rank aggregation methods.
/// ITERATIONS > 0 : execute the iterative versions or rank aggregation methods with a fixed number
///					 of iterations
/// ITERATIONS = -1: execute the iterative versions or rank aggregation methods and stop iterations
///                  when the voters weights converge
#define MAX_ITERATIONS 50
#define ITERATIONS -1

#if ITERATIONS == -1
	#define CONVERGENCE_PRECISION 0.001
#endif

#define MAX_LIST_ITEMS 1000
#define GAMMA 1.25 // try values 1.0-1.5
#define POST_PROCESSING 0

#define NOT_RANKED_ITEM_RANK MAX_LIST_ITEMS * MAX_LIST_ITEMS

#include "src/Rel.cpp"
#include "src/Rels.cpp"

#include "src/Voter.cpp"
#include "src/InputItem.cpp"
#include "src/InputList.cpp"
#include "src/Ranking.cpp"
#include "src/MergedItem.cpp"
#include "src/MergedList.cpp"
#include "src/Evaluator.cpp"
#include "src/Aggregator.cpp"
#include "src/Query.cpp"
#include "src/TRECInput.cpp"

int main() {
	// TestScaledFootrule(); exit(-1);
	score_t delta_1 = 0.0, delta_2 = 0.0;
	char buf[1024], buf2[1024], buf3[1024];
	char dash[151], lit[151]; dash[0] = 0; lit[0] = 0;

	#if DATASET_TYPE == 1
		sprintf(buf, "%s%s/%s_%s%s_Input/", BASE_PATH, TREC_NAME, TREC_NAME, TREC_TRACK, TREC_TASK);
	#elif DATASET_TYPE == 2
		sprintf(buf, "%s", DATASET); /// 2 LETOR 4.0 DATASETS
	#endif

	sprintf(buf2, "/home/leo/Desktop/rankagg/results/eval_%d_%d_%d_%s_%s_%s.txt",
		AGGREGATION_METHOD, CORRELATION_METHOD, ITERATIONS, TREC_NAME, TREC_TRACK, TREC_TASK);
	sprintf(buf3, "/home/leo/Desktop/rankagg/results/dvar_%d_%d_%d_%s_%s_%s.txt",
		AGGREGATION_METHOD, CORRELATION_METHOD, ITERATIONS, TREC_NAME, TREC_TRACK, TREC_TASK);

	FILE * d_file = fopen(buf3, "w+");

	class TRECInput * TREC_DATA = new TRECInput(DATASET_TYPE, buf, buf2);

	TREC_DATA->evaluate_input();

	delta_1 = 0.1; delta_2 = 0.1;
//	for (delta_1 = 0.1; delta_1 < 1.0; delta_1 += 0.1) {
//		for (delta_2 = 0.1; delta_2 < 1.0 - delta_1; delta_2 += 0.1) {
			fprintf(TREC_DATA->get_eval_file(),
				"RUN PARAMS:\nAggregation Method: %d\nCorrelation Method: %d\nIterations: %d\nList Pruning: %d\ndelta1=%2.1f\ndelta2=%2.1f\ng=%3.2f\n\n",
				AGGREGATION_METHOD, CORRELATION_METHOD, ITERATIONS, POST_PROCESSING, delta_1, delta_2, GAMMA); fflush(NULL);

			fprintf(TREC_DATA->get_eval_file(),
				"DATASET:\nTREC: %s\nTRACK: %s\nTASK:%s\n\n", TREC_NAME, TREC_TRACK, TREC_TASK);

			TREC_DATA->aggregate(AGGREGATION_METHOD, POST_PROCESSING, delta_1, delta_2);

			printf("Evaluating...\n"); fflush(NULL);
			TREC_DATA->evaluate();

			if (RESULTS_TYPE == 1) {
				printf("%s\n", PadStr(dash, 137, '-'));
				strcpy(lit, "Mean");
				printf("| %s\t\t| %5.4f | %5.4f | %5.4f | %5.4f | %5.4f | %5.4f | %5.4f | %5.4f | %5.4f | %5.4f | %5.4f  | %5.4f  |\n",
					PadStr(lit, 10, ' '), TREC_DATA->get_MAP(),
					MAX_LIST_ITEMS > 4 ? TREC_DATA->get_mean_precision(5) : 0,
					MAX_LIST_ITEMS > 9 ? TREC_DATA->get_mean_precision(10) : 0,
					MAX_LIST_ITEMS > 14 ? TREC_DATA->get_mean_precision(15) : 0,
					MAX_LIST_ITEMS > 19 ? TREC_DATA->get_mean_precision(20) : 0,
					MAX_LIST_ITEMS > 29 ? TREC_DATA->get_mean_precision(30) : 0,
					MAX_LIST_ITEMS > 99 ? TREC_DATA->get_mean_precision(100) : 0,
					MAX_LIST_ITEMS > 199 ? TREC_DATA->get_mean_precision(200) : 0,
					MAX_LIST_ITEMS > 499 ? TREC_DATA->get_mean_precision(500) : 0,
					MAX_LIST_ITEMS > 999 ? TREC_DATA->get_mean_precision(1000) : 0,
					MAX_LIST_ITEMS > 9 ? TREC_DATA->get_mean_ndcg(10) : 0,
					MAX_LIST_ITEMS > 19 ? TREC_DATA->get_mean_ndcg(20) : 0);
				printf("%s\n\n", PadStr(dash, 137, '-'));

				if (TREC_DATA->get_eval_file()) {
					fprintf(TREC_DATA->get_eval_file(), "%s\n", PadStr(dash, 137, '-'));
					strcpy(lit, "Mean");
					fprintf(TREC_DATA->get_eval_file(), "| %s\t\t| %5.4f | %5.4f | %5.4f | %5.4f | %5.4f | %5.4f | %5.4f | %5.4f | %5.4f | %5.4f | %5.4f  | %5.4f  |\n",
						PadStr(lit, 10, ' '), TREC_DATA->get_MAP(),
						MAX_LIST_ITEMS > 4 ? TREC_DATA->get_mean_precision(5) : 0,
						MAX_LIST_ITEMS > 9 ? TREC_DATA->get_mean_precision(10) : 0,
						MAX_LIST_ITEMS > 14 ? TREC_DATA->get_mean_precision(15) : 0,
						MAX_LIST_ITEMS > 19 ? TREC_DATA->get_mean_precision(20) : 0,
						MAX_LIST_ITEMS > 29 ? TREC_DATA->get_mean_precision(30) : 0,
						MAX_LIST_ITEMS > 99 ? TREC_DATA->get_mean_precision(100) : 0,
						MAX_LIST_ITEMS > 199 ? TREC_DATA->get_mean_precision(200) : 0,
						MAX_LIST_ITEMS > 499 ? TREC_DATA->get_mean_precision(500) : 0,
						MAX_LIST_ITEMS > 999 ? TREC_DATA->get_mean_precision(1000) : 0,
						MAX_LIST_ITEMS > 9 ? TREC_DATA->get_mean_ndcg(10) : 0,
						MAX_LIST_ITEMS > 19 ? TREC_DATA->get_mean_ndcg(20) : 0);
					fprintf(TREC_DATA->get_eval_file(), "%s\n\n", PadStr(dash, 137, '-'));
				}

			} else if (RESULTS_TYPE == 2) {
				printf("%s\n", PadStr(dash, 137, '-'));
				strcpy(lit, "Mean");
				printf("| %s\t\t| %4.3f | %4.3f | %4.3f | %4.3f | %4.3f | %4.3f | %4.3f | %4.3f | %4.3f | %4.3f | %4.3f | %4.3f | %4.3f | %4.3f |\n",
					PadStr(lit, 10, ' '), TREC_DATA->get_MAP(),
					MAX_LIST_ITEMS > 4 ? TREC_DATA->get_mean_precision(5) : 0,
					MAX_LIST_ITEMS > 9 ? TREC_DATA->get_mean_precision(10) : 0,
					MAX_LIST_ITEMS > 19 ? TREC_DATA->get_mean_precision(20) : 0,
					MAX_LIST_ITEMS > 29 ? TREC_DATA->get_mean_precision(30) : 0,
					MAX_LIST_ITEMS > 49 ? TREC_DATA->get_mean_precision(50) : 0,
					MAX_LIST_ITEMS > 99 ? TREC_DATA->get_mean_precision(100) : 0,
					TREC_DATA->get_MNDCG(),
					MAX_LIST_ITEMS > 4 ? TREC_DATA->get_mean_ndcg(5) : 0,
					MAX_LIST_ITEMS > 9 ? TREC_DATA->get_mean_ndcg(10) : 0,
					MAX_LIST_ITEMS > 19 ? TREC_DATA->get_mean_ndcg(20) : 0,
					MAX_LIST_ITEMS > 29 ? TREC_DATA->get_mean_ndcg(30) : 0,
					MAX_LIST_ITEMS > 49 ? TREC_DATA->get_mean_ndcg(50) : 0,
					MAX_LIST_ITEMS > 99 ? TREC_DATA->get_mean_ndcg(100) : 0);
				printf("%s\n\n", PadStr(dash, 137, '-'));

				if (TREC_DATA->get_eval_file()) {
					fprintf(TREC_DATA->get_eval_file(), "%s\n", PadStr(dash, 137, '-'));
					strcpy(lit, "Mean");
					fprintf(TREC_DATA->get_eval_file(), "| %s\t\t| %4.3f | %4.3f | %4.3f | %4.3f | %4.3f | %4.3f | %4.3f | %4.3f | %4.3f | %4.3f | %4.3f | %4.3f | %4.3f | %4.3f |\n",
						PadStr(lit, 10, ' '), TREC_DATA->get_MAP(),
						MAX_LIST_ITEMS > 4 ? TREC_DATA->get_mean_precision(5) : 0,
						MAX_LIST_ITEMS > 9 ? TREC_DATA->get_mean_precision(10) : 0,
						MAX_LIST_ITEMS > 19 ? TREC_DATA->get_mean_precision(20) : 0,
						MAX_LIST_ITEMS > 29 ? TREC_DATA->get_mean_precision(30) : 0,
						MAX_LIST_ITEMS > 49 ? TREC_DATA->get_mean_precision(50) : 0,
						MAX_LIST_ITEMS > 99 ? TREC_DATA->get_mean_precision(100) : 0,
						TREC_DATA->get_MNDCG(),
						MAX_LIST_ITEMS > 4 ? TREC_DATA->get_mean_ndcg(5) : 0,
						MAX_LIST_ITEMS > 9 ? TREC_DATA->get_mean_ndcg(10) : 0,
						MAX_LIST_ITEMS > 19 ? TREC_DATA->get_mean_ndcg(20) : 0,
						MAX_LIST_ITEMS > 29 ? TREC_DATA->get_mean_ndcg(30) : 0,
						MAX_LIST_ITEMS > 49 ? TREC_DATA->get_mean_ndcg(50) : 0,
						MAX_LIST_ITEMS > 99 ? TREC_DATA->get_mean_ndcg(100) : 0);
					fprintf(TREC_DATA->get_eval_file(), "%s\n\n", PadStr(dash, 137, '-'));
				}

				/// Results LaTeX tabular data
				printf("\n\n\n");
				printf("             & %4.3f & %4.3f & %4.3f & %4.3f & %4.3f & & %4.3f & %4.3f & %4.3f & %4.3f & %4.3f \\\\",
					TREC_DATA->get_MAP(),
					MAX_LIST_ITEMS > 4 ? TREC_DATA->get_mean_precision(5) : 0,
					MAX_LIST_ITEMS > 9 ? TREC_DATA->get_mean_precision(10) : 0,
					MAX_LIST_ITEMS > 19 ? TREC_DATA->get_mean_precision(20) : 0,
					MAX_LIST_ITEMS > 99 ? TREC_DATA->get_mean_precision(100) : 0,
					TREC_DATA->get_MNDCG(),
					MAX_LIST_ITEMS > 4 ? TREC_DATA->get_mean_ndcg(5) : 0,
					MAX_LIST_ITEMS > 9 ? TREC_DATA->get_mean_ndcg(10) : 0,
					MAX_LIST_ITEMS > 19 ? TREC_DATA->get_mean_ndcg(20) : 0,
					MAX_LIST_ITEMS > 99 ? TREC_DATA->get_mean_ndcg(100) : 0
				);
			}

			printf("\n=== Avg Iterations = %5.3f ===\n", (double)SUM_ITERATIONS / (double)DATASET_NUM_QUERIES);
			printf("\n=== Avg Spearman's Rho = %5.3f ===\n", TREC_DATA->get_avg_sprho());

			if (TREC_DATA->get_eval_file()) {
				fprintf(TREC_DATA->get_eval_file(), "\n=== Avg Iterations = %5.3f ===\n", (double)SUM_ITERATIONS / (double)DATASET_NUM_QUERIES);
				fprintf(TREC_DATA->get_eval_file(), "\n=== Avg Spearman's Rho = %5.3f ===\n", TREC_DATA->get_avg_sprho());
			}

			if (d_file) {
				fprintf(d_file, "%5.3f, %5.3f, %5.3f\n", delta_1, delta_2, TREC_DATA->get_MAP());
			} else {
				printf("Error openting heat map file\n");
				exit(-1);
			}

			TREC_DATA->destroy_output_lists();
//		}
//	}

	fclose(d_file);
	delete TREC_DATA;

	return 1;
}
