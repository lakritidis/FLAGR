#include "Ranking.h"

/// Constructor
Ranking::Ranking(class InputList * l, rank_t r, rank_t i) {
	this->input_list = l;
	this->rank = r;
	this->index = i;
}

/// Destructor: nothing to destroy
Ranking::~Ranking() { }

/// Mutators
void Ranking::set_input_list(class InputList * v) { this->input_list = v; }
void Ranking::set_rank(rank_t v) { this->rank = v; }
void Ranking::set_index(rank_t v) { this->index = v; }

/// Accessors
class InputList * Ranking::get_input_list() { return this->input_list; }
rank_t Ranking::get_rank() { return this->rank; }
rank_t Ranking::get_index() { return this->index; }
