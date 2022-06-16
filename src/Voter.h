#ifndef VOTER_H
#define VOTER_H


class Voter {
	private:
		char * name;
		score_t weight;

	public:
		Voter();
		Voter(char *, score_t);
		~Voter();

		void display();

		/// Mutators
		void set_name(char *);
		void set_weight(score_t);

		/// Accessors
		char * get_name();
		score_t get_weight();
};

#endif // VOTER_H
