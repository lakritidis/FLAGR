#ifndef INPUTITEM_H
#define INPUTITEM_H


class InputItem {
	protected:
		uint32_t idx;
		char * code;		/// A unique identifier used to identify the common list elements
		rank_t rank;		/// The ranking of this item in its input list
		score_t inscore;	/// The score that was assigned to this item by its voter's input list
		score_t pscore;     /// Preservation Score from the Weighted Item Selection Algorithm

	private:
		void copy_code(char *);

	public:
		InputItem();
		InputItem(uint32_t, char *, rank_t, score_t, score_t);

		~InputItem();

		void display();

		/// Mutators
		void set_idx(uint32_t);
		void set_code(char *);
		void set_rank(rank_t);
		void set_inscore(score_t);
		void set_pscore(score_t);

		/// Accessors
		uint32_t get_idx();
		char * get_code();
		rank_t get_rank();
		score_t get_inscore();
		score_t get_pscore();
};

#endif // INPUTITEM_H
