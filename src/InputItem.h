#ifndef INPUTITEM_H
#define INPUTITEM_H


class InputItem {
	protected:
		char * code;
		rank_t rank;
		rank_t index;

	private:
		void copy_code(char *);

	public:
		InputItem();
		InputItem(char *, rank_t, rank_t);

		~InputItem();

		void display();

		/// Mutators
		void set_code(char *);
		void set_rank(rank_t);
		void set_index(rank_t);

		/// Accessors
		char * get_code();
		rank_t get_rank();
		rank_t get_index();
};

#endif // INPUTITEM_H
