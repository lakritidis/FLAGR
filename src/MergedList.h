#ifndef MERGEDLIST_H
#define MERGEDLIST_H


class MergedList {
	private:
		uint32_t num_input_lists;
		class MergedItem ** hash_table;
		class MergedItem ** item_list;
		uint32_t mask;
		uint32_t num_slots;  /// The number of slots of the hash table
		uint32_t num_nodes;	 /// The number of elements stored in the hash table.
		uint32_t num_chains; /// The number of non-empty chains.

		score_t weight;      /// Agglomerative Aggregation
		double log10s[100000];

	private:
		uint32_t djb2(char *);

		static int cmp_code_asc(const void *, const void *);
		static int cmp_score_asc(const void *, const void *);
		static int cmp_score_desc(const void *, const void *);
		static int cmp_voter(const void *, const void *);
		static int cmp_edges(const void *, const void *);
		static int cmp_double(const void *, const void *);
		static int cmp_voter_scores(const void *, const void *);

		score_t * compute_state_matrix(class SimpleScoreStats *, class InputParams * );
		void matrixvec_multiply(score_t *, score_t *, score_t **);

		void permute(class MergedItem **, class InputList **, rank_t, score_t *, int l, int r);

		score_t stuart(double *, double *, double *, double *, char *);
		score_t sumStuart(double *, double, uint32_t, double *, double *);
		uint64_t factorial(uint32_t);
		double factorial(double);
		double * precompute_170_factorials();
		void compute_initial_weights(class InputList **);

	public:
		MergedList();
		MergedList(class InputList **, uint32_t, uint32_t);
		MergedList(uint32_t, uint32_t);
		~MergedList();

		void insert(class InputItem *, uint32_t, class InputList **);
		void insert_merge(class MergedItem *, score_t);
		void convert_to_array();
		void display();
		void display_list();
		void write_to_CSV(char *, class InputParams *);
		void update_weight(char *, score_t);
		void reset_item_scores();
		void reset_weights();
		void rebuild(class InputList **);
		void clear_contents();
		void merge_with(class MergedList *, class InputParams *);
		rank_t get_item_rank(char *);

		/// Rank Aggregation Methods
		void CombSUM(class InputList **, class SimpleScoreStats *, class InputParams *);
		void CombMNZ(class InputList **, class SimpleScoreStats *, class InputParams *);
		void CondorcetWinners(class InputList **, class SimpleScoreStats *, class InputParams *);
		void CopelandWinners(class InputList **, class SimpleScoreStats *, class InputParams *);
		void Outranking(class InputList **, class SimpleScoreStats *, class InputParams *);
		void KemenyOptimal(class InputList **, class SimpleScoreStats *, class InputParams *);
		void RobustRA(class InputList **, class SimpleScoreStats *, class InputParams *);
		void MC(class InputList **,  class SimpleScoreStats *, class InputParams *);
		class Voter ** DIBRA(class InputList **, class SimpleScoreStats *, class InputParams *);
		void PrefRel(class InputList **, class SimpleScoreStats *, class InputParams *);
		class MergedList * Agglomerative(class InputList **,  class SimpleScoreStats *, class InputParams *);

		/// Custom Algorithm declarations
		void CustomMethod1(class InputList **,  class SimpleScoreStats *, class InputParams *);
		void CustomMethod2(class InputList **,  class SimpleScoreStats *, class InputParams *);

		/// Post-Processing methods (Weighted Aggregators only)
		void perform_pruning(class InputList **, class SimpleScoreStats *, class InputParams *);
		void perform_item_selection(class InputList **, class SimpleScoreStats *, class InputParams *);

		/// Rank Correlation Methods
		double SpearmanRho(class InputList *);
		double SpearmanRho(class MergedList *);
		double ScaledFootruleDistance(class MergedList *);
		double ScaledFootruleDistance(uint32_t, class InputList *);
		double LocalScaledFootruleDistance(uint32_t, class InputList *);
		double CosineSimilarity(uint32_t, class InputList *);
		double KendallsTau(uint32_t, class InputList *);

		/// Getters
		rank_t get_num_items();
		class MergedItem * get_item(uint32_t);
		class MergedItem ** get_item_list();
		score_t get_weight();

		/// Setters
		void set_weight(score_t);
};

#endif // ITEMHASH_H
