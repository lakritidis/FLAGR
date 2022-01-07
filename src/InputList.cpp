#include "InputList.h"

/// Constructor 1: Default
InputList::InputList() {
	this->voter = NULL;
	this->num_items = 0;
	this->num_alloc_items = 0;
	this->cutoff = 0;
	this->items = NULL;
}

/// Constructor 2
InputList::InputList(uint32_t i, char * v, double w) {
	this->id = i;
	this->voter = new Voter(v, w);

	this->num_items = 0;
	this->cutoff = 0;
	this->num_alloc_items = 100;
	this->items = (class InputItem **)malloc(this->num_alloc_items * sizeof(class InputItem *));
}

/// Destructor
InputList::~InputList() {
	if (this->voter) {
		delete this->voter;
	}

	if (this->items) {
		for (rank_t i = 0; i < this->num_items; i++) {
			if (this->items[i]) {
				delete this->items[i];
			}
		}
		free(this->items);
	}
}

/// Insert an item into the list
void InputList::insert_item(char * code, rank_t r) {
	if (this->num_items <= MAX_LIST_ITEMS) {
		this->items[this->num_items] = new InputItem(code, r, this->num_items);
		this->num_items++;
		this->cutoff++;

		if (this->num_items >= this->num_alloc_items) {
			this->num_alloc_items *= 2;
			this->items = (class InputItem **)realloc
					(this->items, this->num_alloc_items * sizeof(class InputItem *));
		}
	}
}

/// Replace an item of the list
void InputList::replace_item(char * code, rank_t r) {
	delete this->items[r - 1];
	this->items[r - 1] = new InputItem(code, r, r - 1);
}

void InputList::sort_by_ranking() {
	qsort(this->items, this->num_items, sizeof(class InputItem *), &InputList::cmp_rank);
	for (uint32_t i = 0; i < this->num_items; i++) {
		this->items[i]->set_index(i);
//		this->items[i]->display();
	}
}

/// Display the Input List Data
void InputList::display() {
	printf("\t");
	this->voter->display();
	printf("\tNum Items: %d\n\tCutoff Point: %d\n", this->num_items, this->cutoff);
	for (uint32_t i = 0; i < this->num_items; i++) {
		printf("\t%d: ", i);
		this->items[i]->display();
	}
}

/// Mutators
void InputList::set_id(uint32_t v) { this->id = v; }
void InputList::set_voter_weight(double v) { this->voter->set_weight(v); }
void InputList::set_cutoff(rank_t v) { this->cutoff = v; }

/// Accessors
uint32_t InputList::get_id() { return this->id; }
class Voter * InputList::get_voter() { return this->voter; }
rank_t InputList::get_num_items() { return this->num_items; }
class InputItem * InputList::get_item(rank_t i) { return this->items[i]; }
rank_t InputList::get_cutoff() { return this->cutoff; }
rank_t InputList::get_max_rank() { return this->items[this->num_items-1]->get_rank(); }

