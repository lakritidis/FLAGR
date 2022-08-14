#ifndef MERGEDITEMPAIR_H
#define MERGEDITEMPAIR_H


class MergedItemPair {
	private:
		class MergedItem * item1;
		class MergedItem * item2;
		score_t score;

	public:
		MergedItemPair();
		MergedItemPair(class MergedItem *, class MergedItem *);
		~MergedItemPair();

		void compute_a_majority_opinion(score_t a, score_t b, uint32_t N);
		void compute_a_majority_opinion_debug(score_t a, score_t b, uint32_t N);
		void compute_weight();
		void display(uint32_t);

		/// Accessors
		class MergedItem * get_item1();
		class MergedItem * get_item2();
		score_t get_score();

		/// Mutators
		void set_item1(class MergedItem *);
		void set_item2(class MergedItem *);
		void set_score(score_t);
};

#endif // MERGEDITEMPAIR_H
