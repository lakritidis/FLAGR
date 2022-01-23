#ifndef INPUTLIST_H
#define INPUTLIST_H


class InputList {
	private:
		uint32_t id;
		class Voter * voter;

		rank_t num_items;
		rank_t num_alloc_items;
		rank_t cutoff;
		rank_t min_rank;
		rank_t max_rank;

		class InputItem ** items;

	private:
		static int cmp_rank(const void *a, const void *b) {
			class InputItem * x = *(class InputItem **)a;
			class InputItem * y = *(class InputItem **)b;

			return x->get_rank() - y->get_rank();
		}

	public:
		InputList();
		InputList(uint32_t, char *, double);
		~InputList();

		void insert_item(char *, rank_t);
		void replace_item(char *, rank_t);
		class InputItem * search_item(char *);
		void display();
		void sort_by_ranking();
		void reverse_ranking();

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
		rank_t get_min_rank();
		rank_t get_max_rank();
		class InputItem * get_item(rank_t);
};

#endif // INPUTLIST_H
