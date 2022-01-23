#ifndef INPUTITEM_H
#define INPUTITEM_H


class InputItem {
	protected:
		char * code;
		rank_t rank;

	private:
		void copy_code(char *);

	public:
		InputItem();
		InputItem(char *, rank_t);

		~InputItem();

		void display();

		/// Mutators
		void set_code(char *);
		void set_rank(rank_t);

		/// Accessors
		char * get_code();
		rank_t get_rank();
};

#endif // INPUTITEM_H
