#include "MergedItem.h"

/// Constructor 1 : Default
MergedItem::MergedItem() {
	this->score = 0.0;
	this->final_ranking = 0;
	this->num_rankings = 0;
	this->num_alloc_rankings = 0;
	this->rankings = NULL;
	this->next = NULL;
}

/// Constructor 2
MergedItem::MergedItem(char * c, rank_t r, uint32_t nal, class InputList ** l) : InputItem(c, r, 0) {
	this->score = 0.0;
	this->final_ranking = 0;
	this->num_rankings = 0;
	this->num_alloc_rankings = nal;
	this->rankings = new Ranking * [this->num_alloc_rankings];
	for (uint32_t i = 0; i < nal; i++) {
		this->rankings[i] = new Ranking(l[i], NOT_RANKED_ITEM_RANK, NOT_RANKED_ITEM_RANK);
	}
	this->next = NULL;
}

/// Destructor
MergedItem::~MergedItem() {
	if (this->rankings) {
		for (rank_t i = 0; i < this->num_alloc_rankings; i++) {
			if (this->rankings[i]) {
				delete this->rankings[i];
			}
		}
		delete [] this->rankings;
	}
}

/// Insert a ranking into the MergedItem
void MergedItem::insert_ranking(class InputList * l, rank_t r, rank_t i) {
	this->rankings[l->get_id()]->set_input_list(l);
	this->rankings[l->get_id()]->set_rank(r);
	this->rankings[l->get_id()]->set_index(i);
	this->num_rankings++;
}

/// Display the MergedItem data
void MergedItem::display() {
	class Ranking * r;
	printf("Item: %s was found in %d input lists, Score: %5.2f:\n",
		this->code, this->num_rankings, this->score);

	for (uint32_t i = 0; i < this->num_alloc_rankings; i++) {
		r = this->rankings[i];
		if (r->get_input_list()) {
			printf("\tList ID: %d - Ranking %d\n", r->get_input_list()->get_id(), r->get_rank());
		}
	}
	printf("\n");
}

/// Mutators
void MergedItem::set_score(score_t v) { this->score = v; }
void MergedItem::set_final_ranking(rank_t v) { this->final_ranking = v; }
void MergedItem::set_next(class MergedItem * v) { this->next = v; }

/// Accessors
score_t MergedItem::get_score() { return this->score; }
rank_t MergedItem::get_final_ranking() { return this->final_ranking; }
uint32_t MergedItem::get_num_rankings() { return this->num_rankings; }
uint32_t MergedItem::get_num_alloc_rankings() { return this->num_alloc_rankings; }
class MergedItem * MergedItem::get_next() { return this->next; }
class Ranking * MergedItem::get_ranking(uint32_t i) { return this->rankings[i]; }
