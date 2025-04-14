#ifndef RANKING_H
#define RANKING_H

/// This structure describes the ranking and the score of an element in an input list; The
/// input_list pointer points to the original input list object.

class Ranking {
	private:
		class InputList * input_list; /// A pointer to the input list
		uint32_t idx;
		rank_t rank;  	/// The ranking of an item in the input list
		score_t score;	/// The score of an item in the input list

	public:
		Ranking(class InputList *, uint32_t, rank_t, score_t);
		~Ranking();

		void display();

		/// Mutators
		void set_input_list(class InputList *);
		void set_idx(uint32_t);
		void set_rank(rank_t);
		void set_score(score_t);

		/// Accessors
		class InputList * get_input_list();
		uint32_t get_idx();
		rank_t get_rank();
		score_t get_score();
};

#endif // RANKING_H
