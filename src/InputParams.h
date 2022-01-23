#ifndef INPUTPARAMS_H
#define INPUTPARAMS_H

/// DATASET TYPES (uint32_t dataset_type;)
///  1:  TREC-style (WebAdhoc)
///  2:  LETOR-style (MQ2007-agg, MQ2008-agg)

/// RANK AGGREGATION METHODS (uint32_t aggregation_method)
///  1:  Borda Count
///  2:  Condorcet Method
///  3:  Outranking Approach (SIGIR 2007)
///  4:  Rank Position
///  5.  DIBRA @ Borda Count  (ESWA 2022)
///  6:  DIBRA @ Condorcet Method (ESWA 2022)
///  7:  DIBRA @ Outranking Approach (ESWA 2022)
///  8:  DIBRA @ Rank Position (ESWA 2022)
///  9:  Preference Relations (ESWA 2016)
/// 10:  Agglomerative (KBS 2018)

/// LIST CORRELATION MEASURES (uint32_t correlation_method)
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
		char * base_path;
		char * data_dir;
		uint32_t dataset_type;
		char * dataset_name;
		char * dataset_track;
		bool dataset_compressed;

		uint32_t aggregation_method;
		uint32_t correlation_method;
		uint32_t weights_normalization;
		int32_t max_iterations;
		int32_t iterations;
		uint32_t max_list_items;
		bool list_pruning;

		score_t convergence_precision;
		score_t alpha;
		score_t beta;
		score_t gamma;
		score_t delta1;
		score_t delta2;

		char * eval_file;
		char * dvar_file;

	public:
		InputParams();
		InputParams(char * bp, uint32_t dtype, char * dname, char * dtrack, bool dcomp,
			uint32_t amet, uint32_t cmet, uint32_t wnrm, int32_t mit, int32_t it, uint32_t mli,
			bool lp, score_t conv_p, score_t a, score_t b, score_t g, score_t d1, score_t d2);

		~InputParams();

		void build_filenames();

		/// Acessors
		char * get_base_path();
		char * get_data_dir();
		uint32_t get_dataset_type();
		char * get_dataset_name();
		char * get_dataset_track();
		bool get_dataset_compressed();

		uint32_t get_aggregation_method();
		uint32_t get_correlation_method();
		uint32_t get_weights_normalization();
		int32_t get_max_iterations();
		int32_t get_iterations();
		uint32_t get_max_list_items();
		bool get_list_pruning();

		char * get_eval_file();
		char * get_dvar_file();

		score_t get_convergence_precision();
		score_t get_alpha();
		score_t get_beta();
		score_t get_gamma();
		score_t get_delta1();
		score_t get_delta2();

		/// Mutators
		void set_base_path(char *);
		void set_data_dir(char *);
		void set_dataset_type(uint32_t);
		void set_dataset_name(char *);
		void set_dataset_track(char *);
		void set_dataset_compressed(bool);

		void set_aggregation_method(uint32_t);
		void set_correlation_method(uint32_t);
		void set_weights_normalization(uint32_t);
		void set_max_iterations(int32_t);
		void set_iterations(int32_t);
		void set_max_list_items(uint32_t);
		void set_list_pruning(bool);

		void set_eval_file(char *);
		void set_dvar_file(char *);

		void set_convergence_precision(score_t);
		void set_alpha(score_t);
		void set_beta(score_t);
		void set_gamma(score_t);
		void set_delta1(score_t);
		void set_delta2(score_t);
};

#endif // INPUTPARAMS_H
