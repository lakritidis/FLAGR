#ifndef INPUTITEM_H
#define INPUTITEM_H


class InputItem {
	protected:
		char * code;		/// A unique identifier used to identify the common list elements
		rank_t rank;		/// The ranking of this item in its input list
		score_t inscore;	/// The score that was assigned to this item by its voter's input list

	private:
		void copy_code(char *);

	public:
		InputItem();
		InputItem(char *, rank_t, score_t);

		~InputItem();

		void display();

		/// Mutators
		void set_code(char *);
		void set_rank(rank_t);
		void set_inscore(score_t);

		/// Accessors
		char * get_code();
		rank_t get_rank();
		score_t get_inscore();
};

#endif // INPUTITEM_H
