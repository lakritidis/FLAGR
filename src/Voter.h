#ifndef VOTER_H
#define VOTER_H


class Voter {
	private:
		char * name;
		double weight;

	public:
		Voter();
		Voter(char *, double);
		~Voter();

		void display();

		/// Mutators
		void set_name(char *);
		void set_weight(double);

		/// Accessors
		char * get_name();
		double get_weight();
};

#endif // VOTER_H
