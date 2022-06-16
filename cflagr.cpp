#include "flagr.cpp"

/// ////////////////////////////////////////////////////////////////////////////////////////////////
/// PYFLAGR legacy functions
/// C functions that i) wrap around the corresponding C++ functions and ii) are exposed to Python.
extern "C" {
	/// Wrapper for CombSUM/CombMNZ
	void Comb(const char inf[], const char relf[], const int evpts, const int ram, const char ranstr[], const char out[]) {
		struct PythonParams PyParams;
		srand(time(0));

		PyParams.input_file = (char *)malloc((strlen(inf) + 1) * sizeof(char));
		strcpy(PyParams.input_file, inf);

		PyParams.random_string = (char *)malloc((strlen(ranstr) + 1) * sizeof(char));
		strcpy(PyParams.random_string, ranstr);

		PyParams.output_dir = (char *)malloc((strlen(out) + 1) * sizeof(char));
		strcpy(PyParams.output_dir, out);

		if (strlen(relf) > 0) {
			PyParams.rels_file = (char *)malloc((strlen(relf) + 1) * sizeof(char));
			strcpy(PyParams.rels_file, relf);
		} else {
			PyParams.rels_file = NULL;
		}

		PyParams.eval_points = evpts;
		PyParams.rank_aggregation_method = ram;

		FLAGR_DRIVER(PyParams);

		if (PyParams.input_file) free(PyParams.input_file);
		if (PyParams.rels_file) free(PyParams.rels_file);
		if (PyParams.rels_file) free(PyParams.random_string);
		if (PyParams.output_dir) free(PyParams.output_dir);
	}

	/// Wrapper for the Condorcet Method
	void Condorcet(const char inf[], const char relf[], const int evpts, const char ranstr[], const char out[]) {

		struct PythonParams PyParams;
		srand(time(0));

		PyParams.input_file = (char *)malloc((strlen(inf) + 1) * sizeof(char));
		strcpy(PyParams.input_file, inf);

		PyParams.random_string = (char *)malloc((strlen(ranstr) + 1) * sizeof(char));
		strcpy(PyParams.random_string, ranstr);

		PyParams.output_dir = (char *)malloc((strlen(out) + 1) * sizeof(char));
		strcpy(PyParams.output_dir, out);

		if (strlen(relf) > 0) {
			PyParams.rels_file = (char *)malloc((strlen(relf) + 1) * sizeof(char));
			strcpy(PyParams.rels_file, relf);
		} else {
			PyParams.rels_file = NULL;
		}

		PyParams.eval_points = evpts;
		PyParams.rank_aggregation_method = 200;

		FLAGR_DRIVER(PyParams);

		if (PyParams.input_file) free(PyParams.input_file);
		if (PyParams.rels_file) free(PyParams.rels_file);
		if (PyParams.rels_file) free(PyParams.random_string);
		if (PyParams.output_dir) free(PyParams.output_dir);
	}


	/// Wrapper for the Outranking Approach [2]
	void OutrankingApproach
		(const char inf[], const char relf[], const int evpts, const char ranstr[], const char out[],
			const float pref_t, const float veto_t, const float conc_t, const float disc_t) {

				struct PythonParams PyParams;
				srand(time(0));

				PyParams.input_file = (char *)malloc((strlen(inf) + 1) * sizeof(char));
				strcpy(PyParams.input_file, inf);

				PyParams.random_string = (char *)malloc((strlen(ranstr) + 1) * sizeof(char));
				strcpy(PyParams.random_string, ranstr);

				PyParams.output_dir = (char *)malloc((strlen(out) + 1) * sizeof(char));
				strcpy(PyParams.output_dir, out);

				if (strlen(relf) > 0) {
					PyParams.rels_file = (char *)malloc((strlen(relf) + 1) * sizeof(char));
					strcpy(PyParams.rels_file, relf);
				} else {
					PyParams.rels_file = NULL;
				}

				PyParams.eval_points = evpts;
				PyParams.rank_aggregation_method = 300;
				PyParams.pref_thr = pref_t;
				PyParams.veto_thr = veto_t;
				PyParams.conc_thr = conc_t;
				PyParams.disc_thr = disc_t;

				FLAGR_DRIVER(PyParams);

				if (PyParams.input_file) free(PyParams.input_file);
				if (PyParams.rels_file) free(PyParams.rels_file);
				if (PyParams.rels_file) free(PyParams.random_string);
				if (PyParams.output_dir) free(PyParams.output_dir);
	}


	/// Wrapper the distance-based iterative rank aggregation method of [5] - DIBRA
	void DIBRA
		(const char inf[], const char relf[], const int evpts, const int agg, const char ranstr[],
			const char out[], const int wnorm, const int dist, const bool prune, const float gamma,
			const float d1, const float d2, const float tol, const int iter,
			const float pref_t, const float veto_t, const float conc_t, const float disc_t) {

				struct PythonParams PyParams;
				srand(time(0));

				PyParams.input_file = (char *)malloc((strlen(inf) + 1) * sizeof(char));
				strcpy(PyParams.input_file, inf);

				PyParams.random_string = (char *)malloc((strlen(ranstr) + 1) * sizeof(char));
				strcpy(PyParams.random_string, ranstr);

				PyParams.output_dir = (char *)malloc((strlen(out) + 1) * sizeof(char));
				strcpy(PyParams.output_dir, out);

				if (strlen(relf) > 0) {
					PyParams.rels_file = (char *)malloc((strlen(relf) + 1) * sizeof(char));
					strcpy(PyParams.rels_file, relf);
				} else {
					PyParams.rels_file = NULL;
				}

				PyParams.eval_points = evpts;
				PyParams.rank_aggregation_method = agg;
				PyParams.weight_normalization = wnorm;
				PyParams.distance = dist;
				PyParams.prune = prune;
				PyParams.gamma = gamma;
				PyParams.delta1 = d1;
				PyParams.delta2 = d2;
				PyParams.tol = tol;
				PyParams.max_iter = iter;
				PyParams.pref_thr = pref_t;
				PyParams.veto_thr = veto_t;
				PyParams.conc_thr = conc_t;
				PyParams.disc_thr = disc_t;

				FLAGR_DRIVER(PyParams);

				if (PyParams.input_file) free(PyParams.input_file);
				if (PyParams.rels_file) free(PyParams.rels_file);
				if (PyParams.rels_file) free(PyParams.random_string);
				if (PyParams.output_dir) free(PyParams.output_dir);
	}

	/// Wrapper the preference relation rank aggregation method of [3]
	void PrefRel(const char inf[], const char relf[], const int evpts, const char ranstr[],
				const char out[], const float alpha, const float beta) {

				struct PythonParams PyParams;
				srand(time(0));

				PyParams.input_file = (char *)malloc((strlen(inf) + 1) * sizeof(char));
				strcpy(PyParams.input_file, inf);

				PyParams.random_string = (char *)malloc((strlen(ranstr) + 1) * sizeof(char));
				strcpy(PyParams.random_string, ranstr);

				PyParams.output_dir = (char *)malloc((strlen(out) + 1) * sizeof(char));
				strcpy(PyParams.output_dir, out);

				if (strlen(relf) > 0) {
					PyParams.rels_file = (char *)malloc((strlen(relf) + 1) * sizeof(char));
					strcpy(PyParams.rels_file, relf);
				} else {
					PyParams.rels_file = NULL;
				}

				PyParams.eval_points = evpts;
				PyParams.rank_aggregation_method = 600;
				PyParams.alpha = alpha;
				PyParams.beta = beta;

				FLAGR_DRIVER(PyParams);

				if (PyParams.input_file) free(PyParams.input_file);
				if (PyParams.rels_file) free(PyParams.rels_file);
				if (PyParams.rels_file) free(PyParams.random_string);
				if (PyParams.output_dir) free(PyParams.output_dir);
	}

	/// Wrapper for Agglomerative rank aggregation method of [4]
	void Agglomerative(const char inf[], const char relf[], const int evpts, const char ranstr[],
				const char out[], const float c1, const float c2) {

				struct PythonParams PyParams;
				srand(time(0));

				PyParams.input_file = (char *)malloc((strlen(inf) + 1) * sizeof(char));
				strcpy(PyParams.input_file, inf);

				PyParams.random_string = (char *)malloc((strlen(ranstr) + 1) * sizeof(char));
				strcpy(PyParams.random_string, ranstr);

				PyParams.output_dir = (char *)malloc((strlen(out) + 1) * sizeof(char));
				strcpy(PyParams.output_dir, out);

				if (strlen(relf) > 0) {
					PyParams.rels_file = (char *)malloc((strlen(relf) + 1) * sizeof(char));
					strcpy(PyParams.rels_file, relf);
				} else {
					PyParams.rels_file = NULL;
				}

				PyParams.eval_points = evpts;
				PyParams.rank_aggregation_method = 700;
				PyParams.c1 = c1;
				PyParams.c2 = c2;

				FLAGR_DRIVER(PyParams);

				if (PyParams.input_file) free(PyParams.input_file);
				if (PyParams.rels_file) free(PyParams.rels_file);
				if (PyParams.rels_file) free(PyParams.random_string);
				if (PyParams.output_dir) free(PyParams.output_dir);
	}

	/// Wrapper for the Markov Chains methods of [6]
	void MC(const char inf[], const char relf[], const int evpts, const int ram, const char ranstr[],
			const char out[], const float ergodic_number, const float delta, const int iter) {

				struct PythonParams PyParams;
				srand(time(0));

				PyParams.input_file = (char *)malloc((strlen(inf) + 1) * sizeof(char));
				strcpy(PyParams.input_file, inf);

				PyParams.random_string = (char *)malloc((strlen(ranstr) + 1) * sizeof(char));
				strcpy(PyParams.random_string, ranstr);

				PyParams.output_dir = (char *)malloc((strlen(out) + 1) * sizeof(char));
				strcpy(PyParams.output_dir, out);

				if (strlen(relf) > 0) {
					PyParams.rels_file = (char *)malloc((strlen(relf) + 1) * sizeof(char));
					strcpy(PyParams.rels_file, relf);
				} else {
					PyParams.rels_file = NULL;
				}

				PyParams.eval_points = evpts;
				PyParams.rank_aggregation_method = ram;
				PyParams.alpha = ergodic_number;
				PyParams.delta1 = delta;
				PyParams.max_iter = iter;

				FLAGR_DRIVER(PyParams);

				if (PyParams.input_file) free(PyParams.input_file);
				if (PyParams.rels_file) free(PyParams.rels_file);
				if (PyParams.rels_file) free(PyParams.random_string);
				if (PyParams.output_dir) free(PyParams.output_dir);
	}
}


/// Main
int main(int argc, char *argv[]) {
#ifdef __linux__
	char input_file[] = "/media/leo/B65266EC5266B1331/phd_Research/08 - Datasets/TREC/Synthetic/MOSO.csv";
//	char input_file[] = "/media/leo/B65266EC5266B1331/phd_Research/08 - Datasets/TREC/Synthetic/RA1.csv";
	char qrels_file[] = "/media/leo/B65266EC5266B1331/phd_Research/08 - Datasets/TREC/Synthetic/MOSO_qrels.csv";
	char output_dir[] = "/tmp";
#elif _WIN32
	char input_file[] = "/media/leo/B65266EC5266B1331/phd_Research/08 - Datasets/TREC/Synthetic/MOSO.csv";
	char qrels_file[] = "/media/leo/B65266EC5266B1331/phd_Research/08 - Datasets/TREC/Synthetic/MOSO_qrels.csv";
	char output_dir[] = "/tmp";
#endif // _WIN32

	Comb(input_file, qrels_file, 20, 100, "Comb_out", output_dir);

	Condorcet(input_file, qrels_file, 20, "Condorcet_out", output_dir);

	OutrankingApproach(input_file, qrels_file, 20, "Outrank_out", output_dir, 0.0, 0.75, 0.0, 0.25);

	DIBRA(input_file, qrels_file, 20, 5100, "DIBRA_out", output_dir,
		2, 3, true, 1.5, 0.4, 0.1, 0.01, 50, 0.0, 0.75, 0.0, 0.25);

	PrefRel(input_file, qrels_file, 20, "PrefRel_out", output_dir, 0.1, 0.5);

	Agglomerative(input_file, qrels_file, 20, "Agglomerative_out", output_dir, 2.5, 1.5);

	MC(input_file, qrels_file, 20, 804, "MC_out", output_dir, 0.15, 0.00000001, 200);

	return 0;
}
