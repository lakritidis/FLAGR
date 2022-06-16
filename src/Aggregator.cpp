#include "Aggregator.h"

/// Default Constructor
Aggregator::Aggregator() :
		num_lists(0),
		num_alloc_lists(4),
		input_lists((class InputList **)malloc(this->num_alloc_lists * sizeof(class InputList *))),
		output_list(NULL) { }

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
class Voter ** Aggregator::aggregate(char * topic, class InputParams * params) {

	class Voter ** voters_list = NULL;
	uint32_t ram = params->get_aggregation_method();

	class SimpleScoreStats s;
	s.set_min_val(0.0);
	s.set_max_val(1.0);
	s.set_mean_val(0.0);
	s.set_std_val(1.0);

	/// Apply the aggregation method of the argument
	/// 10X. CombSUM [1]
	if (ram >= 100 && ram <= 109) {
		this->merge_input_lists();
		this->output_list->CombSUM(this->input_lists, &s, params);

	/// 11X. CombMNZ [1]
	} else if (ram >= 110 && ram <= 119) {
		this->merge_input_lists();
		this->output_list->CombMNZ(this->input_lists, &s, params);

	/// 200. Condorcet Winners Method
	} else if (ram == 200) {
		this->merge_input_lists();
		this->output_list->CondorcetMethod(this->input_lists, &s, params);

	/// 300. The outranking approach of [2]
	} else if (ram == 300) {
		this->merge_input_lists();
		this->output_list->Outranking(this->input_lists, &s, params);

	/// 5XXX. The DIBRA method of [5]
	} else if (ram >= 5100 && ram <= 5999) {
		this->merge_input_lists();
		voters_list = this->output_list->DIBRA(this->input_lists, &s, params);

	/// 600. The preference relations method of [3]
	} else if (ram == 600) {
		this->merge_input_lists();
		this->output_list->PrefRel(this->input_lists, &s, params);

	/// 700. The weighted agglomerative algorithm of [4]
	} else if (ram == 700) {
		class MergedList * temp = new MergedList(1024, this->num_lists);
		this->output_list = temp->Agglomerative(this->input_lists, &s, params);
		delete temp;

	/// 804. The fourth Markov Chains method of [6] (MC4)
	} else if (ram == 801 || ram == 802 || ram == 803) {
		this->merge_input_lists();
		this->output_list->MC(this->input_lists, &s, params);


	/// 804. The fourth Markov Chains method of [6] (MC4)
	} else if (ram == 804) {
		this->merge_input_lists();
		this->output_list->MC4(this->input_lists, &s, params);

	}

	this->output_list->write_to_CSV(topic, params);
//	this->output_list->display_list(); getchar();

	return voters_list;
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
