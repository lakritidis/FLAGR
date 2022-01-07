#include "Voter.h"

/// Constructor 1 : default
Voter::Voter() {
	this->name = NULL;
	this->weight = 0.0;
}

/// Constructor 2
Voter::Voter(char * n, double w) {
	this->name = new char[strlen(n) + 1];
	strcpy(this->name, n);

	this->weight = w;
}

/// Destructor
Voter::~Voter() {
	if (this->name) {
		delete [] this->name;
	}
}

void Voter::display() {
	printf("Voter Name: %s - Weight: %5.3f\n", this->name, this->weight);
}

/// Mutators
inline void Voter::set_name(char * v) {
	this->name = new char[strlen(v) + 1];
	strcpy(this->name, v);
}
inline void Voter::set_weight(double v) { this->weight = v; }

/// Accessors
inline char * Voter::get_name() { return this->name; }
inline double Voter::get_weight() { return this->weight; }
