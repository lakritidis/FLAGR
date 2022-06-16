#ifndef MERGEDITEM_H
#define MERGEDITEM_H

class MergedItem : public InputItem {
	private:
		score_t final_score;
		rank_t final_ranking;
		uint32_t num_rankings;
		uint32_t num_alloc_rankings;

		class Ranking ** rankings;

		class MergedItem * next;

	public:
		MergedItem();
		MergedItem(class MergedItem *);
		MergedItem(char *, rank_t, uint32_t, class InputList **);
		~MergedItem();

		void insert_ranking(class InputList *, rank_t, score_t);
		void display();

		/// Mutators
		void set_final_score(score_t);
		void set_final_ranking(rank_t);
		void set_next(class MergedItem *);

		/// Accessors
		score_t get_final_score();
		rank_t get_final_ranking();
		uint32_t get_num_rankings();
		uint32_t get_num_alloc_rankings();
		class MergedItem * get_next();
		class Ranking * get_ranking(uint32_t);
};

#endif // MERGEDITEM_H
