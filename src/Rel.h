#ifndef REL_H
#define REL_H

/// Rel
/// A Relevant Result given by the TREC qrels file

class Rel {
	private:
		uint32_t topic_id;
		char * code;
		uint32_t judgment;
		class Rel * next;

	private:
		void copy_code(char *);

	public:
		Rel();
		Rel(uint32_t, char *, uint32_t);
		~Rel();

		void display();

		/// Mutators
		void set_topic_id(uint32_t );
		void set_code(char *);
		void set_judgment(uint32_t);
		void set_next(class Rel *);

		/// Accessors
		uint32_t get_topic_id();
		char * get_code();
		uint32_t get_judgment();
		class Rel * get_next();
};

#endif // REL_H
