#include "Ranking.h"

/// Constructor 1
Ranking::Ranking(class InputList * l, uint32_t i, rank_t r, score_t s) :
	input_list(l),
	idx(i),
	rank(r),
	score(s) { }

/// Destructor: nothing to destroy
Ranking::~Ranking() {
}

/// Display Ranking contents
void Ranking::display() {
	if (this->idx != NOT_RANKED_ITEM_RANK) {
		printf("\tLIST: ID: %d, Voter: %s, Weight: %5.3f --- ITEM: idx: %d, Rank: %d, Score: %.5f, PScore: %.5f\n",
			this->input_list->get_id(), this->input_list->get_voter()->get_name(), this->input_list->get_voter()->get_weight(),
			this->idx, this->rank, this->score, this->input_list->get_item(this->idx)->get_pscore());
	} else {
		printf("\tLIST: ID: %d, Voter: %s, Weight: %5.3f --- ITEM: idx: %d, Rank: %d, Score: %.5f, PScore: Not set\n",
			this->input_list->get_id(), this->input_list->get_voter()->get_name(), this->input_list->get_voter()->get_weight(),
			this->idx, this->rank, this->score);
	}
}

/// Mutators
void Ranking::set_input_list(class InputList * v) { this->input_list = v; }
void Ranking::set_idx(uint32_t v) { this->idx = v; }
void Ranking::set_rank(rank_t v) { this->rank = v; }
void Ranking::set_score(score_t v) { this->score = v; }

/// Accessors
class InputList * Ranking::get_input_list() { return this->input_list; }
uint32_t Ranking::get_idx() { return this->idx; }
rank_t Ranking::get_rank() { return this->rank; }
score_t Ranking::get_score() { return this->score; }
