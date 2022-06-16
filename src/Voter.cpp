#include "Voter.h"

/// Constructor 1 : default
Voter::Voter() : name(NULL), weight(0.0) { }

/// Constructor 2
Voter::Voter(char * n, score_t w) : name(NULL), weight(w) {
	this->set_name(n);
}

/// Destructor
Voter::~Voter() {
	if (this->name) {
		delete [] this->name;
	}
}

/// Display the Voter object
void Voter::display() {
	printf("Voter Name: %s - Weight: %5.3f\n", this->name, this->weight);
}

/// Mutators
void Voter::set_weight(score_t v) { this->weight = v; }
void Voter::set_name(char * v) {
	this->name = new char[strlen(v) + 1];
	strcpy(this->name, v);
}

/// Accessors
char * Voter::get_name() { return this->name; }
score_t Voter::get_weight() { return this->weight; }
