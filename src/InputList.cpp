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
		this->items[this->num_items] = new InputItem(code, r);
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
	this->items[r - 1] = new InputItem(code, r);
}

/// Search for an item in the list
class InputItem * InputList::search_item(char * code) {
	for (uint32_t i = 0; i < this->num_items; i++) {
		if ( strcmp(this->items[i]->get_code(), code) == 0 ) {
			return this->items[i];
		}
	}
	return NULL;
}

/// Compute the Spearman rho correlation of this list with another input list
score_t InputList::SpearmanRho(class InputList * in) {

	class InputItem * i1, * i2;
	int32_t diff = 0.0;
	rank_t sum = 0, i = 0, j = 0;
	score_t rho = 0.0;

	double denom = pow(this->num_items, 3) - this->num_items;

	for (i = 0; i < this->num_items; i++) {
		i1 = this->items[i];

		printf("Searching for %s... ", i1->get_code());

		for (j = 0; j < in->get_num_items(); j++) {
			i2 = in->get_item(j);

			if ( strcmp( i1->get_code(), i2->get_code() ) == 0) {
				diff = i1->get_rank() - i2->get_rank();
				sum += pow (diff, 2.0 );
//				printf("Found! (%d vs %d) ---> Sum = %d\n", i1->get_rank(), i2->get_rank(), sum);
				break;
			}
		}
	}

	rho = 1.0 - 6.0 * sum / denom;
	return rho;
}

/// Sort the list in increasing order of its element rankings
void InputList::sort_by_ranking() {
	if (this->num_items > 0) {
		qsort(this->items, this->num_items, sizeof(class InputItem *), &InputList::cmp_rank);
		this->min_rank = this->items[0]->get_rank();
		this->max_rank = this->items[this->num_items - 1]->get_rank();
	} else {
		this->min_rank = 0;
		this->max_rank = 0;
	}
}

void InputList::reverse_ranking() {
	this->sort_by_ranking();
	for (uint32_t i = 0; i < this->num_items; i++) {
//		printf("\tOld rank: %d, ", this->items[i]->get_rank());
		this->items[i]->set_rank( this->max_rank + this->min_rank - this->items[i]->get_rank());
//		this->items[i]->set_rank( this->num_items - i );
//		printf("New rank: %d\n", this->items[i]->get_rank());
//		this->items[i]->display();
	}
	this->sort_by_ranking();
}

/// Display the Input List Data
void InputList::display() {
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
rank_t InputList::get_max_rank() { return this->max_rank; }
