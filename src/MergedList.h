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

		double log10s[100000];
	private:
		uint32_t KazLibHash(char *);

		static int cmp_score_asc(const void *a, const void *b) {
			class MergedItem *x = *(class MergedItem **)a;
			class MergedItem *y = *(class MergedItem **)b;

			if (x->get_score() == y->get_score()) {
				return y->get_num_rankings() - x->get_num_rankings();
			}
			return x->get_score() - y->get_score();
		}

		static int cmp_score_desc(const void *a, const void *b) {
			class MergedItem *x = *(class MergedItem **)a;
			class MergedItem *y = *(class MergedItem **)b;

			if (x->get_score() == y->get_score()) {
				return y->get_num_rankings() - x->get_num_rankings();
			} else if (y->get_score() > x->get_score()) {
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

	public:
		MergedList();
		MergedList(uint32_t, uint32_t);
		~MergedList();

		void insert(class InputItem *, uint32_t, class InputList **);
		void convert_to_array();
		void display();
		void display_list();
		void show_weights(class InputList *);
		void update_weight(char *, score_t);
		void reset_scores();
		void reset_weights();
		void rebuild(class InputList **);
		void clear_contents();
		rank_t get_item_rank(char *);

		/// Rank Aggregation Methods
		void BordaCount(double, double, double, double, class InputParams *);
		void CondorcetMethod(double, double, double, double, class InputParams *);
		void Outranking(double, double, double, double, class InputParams *);
		void RankPosition(double, double, double, double, class InputParams *);
		class Voter ** DIBRA(class InputList **, class InputParams *);
		void PrefRel(class InputList **, class InputParams *);
		void Agglomerative(class InputList **, class InputParams *);

		/// Rank Correlation Methods
		double SpearmanRho(class InputList *);
		double ScaledFootruleDistance(uint32_t, class InputList *);
		double LocalScaledFootruleDistance(uint32_t, class InputList *);
		double CosineSimilarity(uint32_t, class InputList *);
		double KendallsTau(uint32_t, class InputList *);

		/// Accessors
		rank_t get_num_items();
		class MergedItem * get_item(uint32_t);
		class MergedItem ** get_item_list();
};

#endif // ITEMHASH_H
