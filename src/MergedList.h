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

		static int cmp_code_asc(const void *a, const void *b) {
			class MergedItem *x = *(class MergedItem **)a;
			class MergedItem *y = *(class MergedItem **)b;

			return strcmp(x->get_code(), y->get_code());
		}

		static int cmp_score_asc(const void *a, const void *b) {
			class MergedItem *x = *(class MergedItem **)a;
			class MergedItem *y = *(class MergedItem **)b;

			if (x->get_final_score() == y->get_final_score()) {
				return y->get_num_rankings() - x->get_num_rankings();
			}
			return x->get_final_score() - y->get_final_score();
		}

		static int cmp_score_desc(const void *a, const void *b) {
			class MergedItem *x = *(class MergedItem **)a;
			class MergedItem *y = *(class MergedItem **)b;

			if (x->get_final_score() == y->get_final_score()) {
				return y->get_num_rankings() - x->get_num_rankings();
			} else if (y->get_final_score() > x->get_final_score()) {
				return 1;
			} else {
				return -1;
			}
		}

		static int cmp_voter(const void *a, const void *b) {
			class Voter * x = *(class Voter **)a;
			class Voter * y = *(class Voter **)b;

			if (x->get_weight() > y->get_weight()) { return 1; }
			return -1;
		}

		static int cmp_edges(const void *a, const void *b) {
			class MergedItemPair * x = * (class MergedItemPair **)a;
			class MergedItemPair * y = * (class MergedItemPair **)b;

			return strcmp (x->get_item2()->get_code(), y->get_item2()->get_code());
		}

		score_t * compute_state_matrix(class SimpleScoreStats *, class InputParams * );
		void matrixvec_multiply(score_t *, score_t *, score_t **);

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
		void show_weights(class InputList *);
		void update_weight(char *, score_t);
		void reset_scores();
		void reset_weights();
		void rebuild(class InputList **);
		void clear_contents();
		void merge_with(class MergedList *, class InputParams *);
		rank_t get_item_rank(char *);

		/// Rank Aggregation Methods
		void CombSUM(class InputList **, class SimpleScoreStats *, class InputParams *);
		void CombMNZ(class InputList **, class SimpleScoreStats *, class InputParams *);
		void CondorcetMethod(class InputList **, class SimpleScoreStats *, class InputParams *);
		void Outranking(class InputList **, class SimpleScoreStats *, class InputParams *);
		void MC(class InputList **,  class SimpleScoreStats *, class InputParams *);
		void MC4(class InputList **,  class SimpleScoreStats *, class InputParams *);
		class Voter ** DIBRA(class InputList **, class SimpleScoreStats *, class InputParams *);
		void PrefRel(class InputList **, class SimpleScoreStats *, class InputParams *);
		class MergedList * Agglomerative(class InputList **,  class SimpleScoreStats *, class InputParams *);

		/// Rank Correlation Methods
		double SpearmanRho(class InputList *);
		double SpearmanRho(class MergedList *);
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
