#ifndef INPUTPARAMS_H
#define INPUTPARAMS_H

/// External user parameters are passed to FLAGR via this UserParams structure.
struct UserParams {
	char * input_file = NULL;
	char * rels_file = NULL;
	char * random_string = NULL;
	char * output_dir = NULL;

	int eval_points = 0;

	int rank_aggregation_method = 0;
	int weight_normalization = 0;
	int distance = 0;

	float tol = 0.0;
	int max_iter = 0;
	bool prune = false;
	bool exact = false;

	score_t pref_thr = 0.0;
	score_t veto_thr = 0.0;
	score_t conc_thr = 0.0;
	score_t disc_thr = 0.0;

	score_t alpha = 0.0;
	score_t beta = 0.0;
	score_t gamma = 0.0;
	score_t delta1 = 0.0;
	score_t delta2 = 0.0;
	score_t c1 = 0.0;
	score_t c2 = 0.0;
};


/// RANK AGGREGATION METHODS (uint32_t aggregation_method)
///  100:   CombSUM with Borda normalization                [1]
///  101:   CombSUM with Rank normalization                 [1]
///  102:   CombSUM with Score normalization                [1]
///  103:   CombSUM with Z-Score normalization              [1]
///  104:   CombSUM with SimpleBorda normalization
///  110:   CombMNZ with Borda normalization                [1]
///  111:   CombMNZ with Rank normalization	                [1]
///  112:   CombMNZ with Score normalization                [1]
///  113:   CombMNZ with Z-Score normalization              [1]
///  114:   CombMNZ with SimpleBorda normalization
///  200:   Condorcet Winners Method
///  201:   Copeland Winners Method
///  300:   Outranking Approach                             [2]
///  400:   Kemeny Optimal Aggregation (Brute Force)
///  401:   Robust Rank Aggregation (RRA)                   [7]
///  5100.  DIBRA @ CombSUM with Borda Normalization        [5]
///  5101.  DIBRA @ CombSUM with Rank Normalization         [5]
///  5102.  DIBRA @ CombSUM with Score Normalization        [5]
///  5103.  DIBRA @ CombSUM with Z-Score Normalization      [5]
///  5104.  DIBRA @ CombSUM with SimpleBorda Normalization  [5]
///  5110.  DIBRA @ CombMNZ with Borda Normalization        [5]
///  5111.  DIBRA @ CombMNZ with Rank Normalization         [5]
///  5112.  DIBRA @ CombMNZ with Score Normalization        [5]
///  5113.  DIBRA @ CombMNZ with Z-Score Normalization      [5]
///  5114.  DIBRA @ CombMNZ with SimpleBorda Normalization  [5]
///  5200.  DIBRA @ Condorcet Winners                       [5]
///  5201.  DIBRA @ Copeland Winners                        [5]
///  5300.  DIBRA @ Outranking Approach                     [5]
///  600.   Preference Relations Method                     [3]
///  700.   Agglomerative Aggregation                       [4]
///  801.   Markov Chains 1	(MC1)                           [6]
///  802.   Markov Chains 2 (MC2)                           [6]
///  803.   Markov Chains 3 (MC3)                           [6]
///  804.   Markov Chains 4	(MC4)                           [6]
///  805.   Markov Chains Thurstone (MCT)                   [9]


/// LIST CORRELATION/DISTANCE MEASURES (uint32_t correlation_method)
///  1:  Spearman's Rho
///  2:  Scaled Footrule Distance
///  3:  Cosine Similarity
///  4:  Local Scaled Footrule Distance
///  5:  Kendall's Tau

/// VOTER WEIGHTS NORMALIZATION METHOD (uint32_t weights_normalization)
/// 1: No normalization (use raw values)
/// 2: Min-Max
/// 3: Z-score
/// 4: Division my max (same as 1)

class InputParams {
	private:
		char * input_file;
		char * rels_file;
		char * output_file;
		char * eval_file;
		char * random_string;

		uint32_t aggregation_method;
		uint32_t correlation_method;
		uint32_t weights_normalization;
		int32_t max_iterations;
		uint32_t max_list_items;
		rank_t eval_points;
		bool list_pruning;
		bool exact;

		score_t convergence_precision;
		score_t alpha;
		score_t beta;
		score_t gamma;
		score_t delta1;
		score_t delta2;
		score_t c1;
		score_t c2;
		score_t pref_thr;
		score_t veto_thr;
		score_t conc_thr;
		score_t disc_thr;

	private:
		void generate_random_string(size_t);

	public:
		InputParams();
		InputParams(struct UserParams);
		~InputParams();

		void set_output_files(char *);
		void display();

		/// Accessors
		char * get_input_file();
		char * get_rels_file();
		char * get_output_file();
		char * get_eval_file();
		char * get_random_string();

		uint32_t get_aggregation_method();
		uint32_t get_correlation_method();
		uint32_t get_weights_normalization();
		int32_t get_max_iterations();
		int32_t get_iterations();
		uint32_t get_max_list_items();
		rank_t get_eval_points();
		bool get_list_pruning();
		bool get_exact();

		score_t get_convergence_precision();
		score_t get_alpha();
		score_t get_beta();
		score_t get_gamma();
		score_t get_delta1();
		score_t get_delta2();
		score_t get_c1();
		score_t get_c2();
		score_t get_pref_thr();
		score_t get_veto_thr();
		score_t get_conc_thr();
		score_t get_disc_thr();

		/// Mutators
		void set_input_file(char *);
		void set_rels_file(char *);
		void set_output_file(const char *);
		void set_eval_file(const char *);
		void set_random_string(const char *);

		void set_aggregation_method(uint32_t);
		void set_correlation_method(uint32_t);
		void set_weights_normalization(uint32_t);
		void set_max_iterations(int32_t);
		void set_iterations(int32_t);
		void set_max_list_items(uint32_t);
		void set_eval_points(rank_t);
		void set_list_pruning(bool);

		void set_convergence_precision(score_t);
		void set_alpha(score_t);
		void set_beta(score_t);
		void set_gamma(score_t);
		void set_delta1(score_t);
		void set_delta2(score_t);
		void set_c1(score_t);
		void set_c2(score_t);
		void set_pref_thr(score_t);
		void set_veto_thr(score_t);
		void set_conc_thr(score_t);
		void set_disc_thr(score_t);
};

#endif // INPUTPARAMS_H
