#ifndef INPUTLIST_H
#define INPUTLIST_H


class InputList {
	private:
		uint32_t id;
		class Voter * voter;

		rank_t num_items;
		rank_t num_alloc_items;
		rank_t cutoff;

		class SimpleScoreStats * stats;

		class InputItem ** items;

	private:
		static int cmp_score(const void *a, const void *b) {
			class InputItem * x = *(class InputItem **)a;
			class InputItem * y = *(class InputItem **)b;

			if (y->get_inscore() > x->get_inscore()) {
				return 1;
			} else if (y->get_inscore() < x->get_inscore()) {
				return -1;
			} else {
				if (x->get_rank() > y->get_rank()) {
					return 1;
				}
			}
			return -1;
		}

		static int cmp_pscore(const void *a, const void *b) {
			class InputItem * x = *(class InputItem **)a;
			class InputItem * y = *(class InputItem **)b;

			if (y->get_pscore() > x->get_pscore()) { return 1; }
			return -1;
		}

	public:
		static int cmp_weight(const void *a, const void *b) {
			class InputList * x = *(class InputList **)a;
			class InputList * y = *(class InputList **)b;

			if (x->get_voter()->get_weight() > y->get_voter()->get_weight()) { return -1; }
			return 1;
		}

		static int cmp_weight2(const void *a, const void *b) {
			score_t x = *(score_t *)a;
			score_t y = *(score_t * )b;

			if (x > y) { return -1; }
			return 1;
		}

	public:
		InputList();
		InputList(uint32_t, char *, score_t);
		~InputList();

		void insert_item(uint32_t, char *, rank_t, score_t);
		void replace_item(uint32_t, char *, rank_t, score_t);
		class InputItem * search_item(char *);
		void display();
		void sort_by_score();
		void sort_by_pscore();

		score_t SpearmanRho(class InputList *);

		/// Mutators
		void set_id(uint32_t);
		void set_voter_weight(double);
		void set_cutoff(rank_t);

		/// Accessors
		uint32_t get_id();
		class Voter * get_voter();
		rank_t get_num_items();
		rank_t get_cutoff();
		class InputItem * get_item(rank_t);

		score_t get_min_score();
		score_t get_max_score();
		score_t get_mean_score();
		score_t get_std_score();
};

#endif // INPUTLIST_H
