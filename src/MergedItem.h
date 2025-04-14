#ifndef MERGEDITEM_H
#define MERGEDITEM_H

class MergedItem {
	private:
		char * code;
		score_t final_score;
		rank_t final_ranking;
		uint32_t num_rankings;
		uint32_t num_alloc_rankings;

		class Ranking ** rankings;

		class MergedItem * next;

	private:
		/// Callback function for QuickSort
		static int cmp_score(const void *a, const void *b);

	public:
		MergedItem();
		MergedItem(class MergedItem *);
		MergedItem(char *, rank_t, uint32_t, class InputList **);
		~MergedItem();

		void insert_ranking(class InputList *, uint32_t, rank_t, score_t);
		void sort_rankings_by_score();
		void compute_beta_values();
		void display();

		void set_individual_preservation_scores(score_t);
		score_t get_sum_voter_weights();

		/// Mutators
		void set_code(char *);
		void set_final_score(score_t);
		void set_final_ranking(rank_t);
		void set_next(class MergedItem *);

		/// Accessors
		char * get_code();
		score_t get_final_score();
		rank_t get_final_ranking();
		uint32_t get_num_rankings();
		uint32_t get_num_alloc_rankings();
		class MergedItem * get_next();
		class Ranking * get_ranking(uint32_t);
};

#endif // MERGEDITEM_H
