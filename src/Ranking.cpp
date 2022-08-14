#include "Ranking.h"

/// Constructor 1
Ranking::Ranking(class InputList * l, rank_t r, score_t s) :
	input_list(l),
	rank(r),
	score(s) { }

/// Destructor: nothing to destroy
Ranking::~Ranking() {
}

/// Display Ranking contents
void Ranking::display() {
	printf("\tList ID: %d - Rank: %d, Score: %6.5f\n",
		this->input_list->get_id(), this->rank, this->score);
}

/// Mutators
void Ranking::set_input_list(class InputList * v) { this->input_list = v; }
void Ranking::set_rank(rank_t v) { this->rank = v; }
void Ranking::set_score(score_t v) { this->score = v; }

/// Accessors
class InputList * Ranking::get_input_list() { return this->input_list; }
rank_t Ranking::get_rank() { return this->rank; }
score_t Ranking::get_score() { return this->score; }
