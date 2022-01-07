#include "Aggregator.h"

/// Constructor
Aggregator::Aggregator() {
	this->num_lists = 0;
	this->num_alloc_lists = 4;
	this->input_lists = (class InputList **)malloc(this->num_alloc_lists * sizeof(class InputList *));
	this->output_list = NULL;
}

/// Destructor
Aggregator::~Aggregator() {
	if (this->input_lists) {
		for (uint16_t i = 0; i < this->num_lists; i++) {
			if (this->input_lists[i]) {
				delete this->input_lists[i];
			}
		}
		free(this->input_lists);
	}

	if (this->output_list) {
		delete this->output_list;
		this->output_list = NULL;
	}
}

/// Create a new input list for an aggregator
class InputList * Aggregator::create_list(char * v, double w) {
	this->input_lists[this->num_lists] = new InputList(this->num_lists, v, w);
	this->num_lists++;

	if (this->num_lists >= this->num_alloc_lists) {
		this->num_alloc_lists *= 2;
		this->input_lists = (class InputList **)realloc
				(this->input_lists, this->num_alloc_lists * sizeof(class InputList *));
	}

	return this->input_lists[this->num_lists - 1];
}

/// Merge the input lists into one MergedList
void Aggregator::merge_input_lists() {
	uint32_t l = 0, k = 0;

	this->output_list = new MergedList(1024, this->num_lists);

	for (l = 0; l < this->num_lists; l++) {
		for (k = 0; k < this->input_lists[l]->get_num_items(); k++) {
			this->output_list->insert(this->input_lists[l]->get_item(k), l, this->input_lists);
		}
	}

	this->output_list->convert_to_array();
//	this->output_list->display(); getchar();
}

/// Set the initial voter weights
void Aggregator::init_weights() {
	for (uint32_t l = 0; l < this->num_lists; l++) {
		this->input_lists[l]->get_voter()->set_weight(1.0);
	}
}

/// Destroy the output list
void Aggregator::destroy_output_list() {
	if (this->output_list) {
		delete this->output_list;
		this->output_list = NULL;
	}
}

/// Apply the rank aggregation method and construct the final output list
class Voter ** Aggregator::aggregate(uint32_t ram, uint32_t pp, score_t d1, score_t d2) {
	/// Merge the results of the input lists into one fused list with duplicates removed
	this->merge_input_lists();

	class Voter ** voters_list = NULL;

	/// Apply the aggregation method of the argument
#if ITERATIONS == 0
	if (ram == 1) { this->output_list->BordaCount(0.0, 1.0, 0.0, 1.0); } else
	if (ram == 2) { this->output_list->CondorcetMethod(0.0, 1.0, 0.0, 1.0); } else
	if (ram == 3) { this->output_list->Outranking(0.0, 1.0, 0.0, 1.0); } else
	if (ram == 4) { this->output_list->RankPosition(0.0, 1.0, 0.0, 1.0); }
#else
	voters_list = this->output_list->DIBRA(this->input_lists, pp, ram, d1, d2);
#endif

	return voters_list;
//	this->output_list->display(); getchar();
}

void Aggregator::display() {
	for (uint32_t i = 0; i < this->num_lists; i++) {
		if (this->input_lists[i]) {
			printf("\t=== Displaying Input List %d:\n", i);
			this->input_lists[i]->display();
			getchar();
		}
	}

	if (this->output_list) {
		this->output_list->display();
	}
}

/// Accessors
inline uint16_t Aggregator::get_num_lists() { return this->num_lists; }
inline rank_t Aggregator::get_num_items() { return this->output_list->get_num_items(); }
inline class InputList * Aggregator::get_input_list(uint32_t i) { return this->input_lists[i]; }
inline class MergedList * Aggregator::get_output_list() { return this->output_list; }
