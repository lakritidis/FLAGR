#ifndef REL_H
#define REL_H

/// Rel
/// A Relevant Result given by the TREC qrels file

class Rel {
	private:
		char * code;
		uint32_t judgment;
		class Rel * next;

	private:
		void copy_code(char *);

	public:
		Rel();
		Rel(char *, uint32_t);
		~Rel();

		void display();

		/// Mutators
		void set_code(char *);
		void set_judgment(uint32_t);
		void set_next(class Rel *);

		/// Accessors
		char * get_code();
		uint32_t get_judgment();
		class Rel * get_next();
};

#endif // REL_H
