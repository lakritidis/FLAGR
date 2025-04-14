#include "InputList.h"

/// Default Constructor
InputList::InputList() :
		id(0),
		voter(NULL),
		num_items(0),
		num_alloc_items(0),
		cutoff(0),
		stats(NULL),
		items(NULL) { }

/// Constructor 2
InputList::InputList(uint32_t i, char * v, score_t w) :
		id(i),
		voter(new Voter(v, w)),
		num_items(0),
		num_alloc_items(100),
		cutoff(0),
		stats(new SimpleScoreStats()),
		items((class InputItem **)malloc(this->num_alloc_items * sizeof(class InputItem *))) { }

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

	if (this->stats) {
		delete this->stats;
	}
}

/// Insert an item into the list
void InputList::insert_item(uint32_t i, char * code, rank_t r, score_t s) {
	if (this->num_items <= MAX_LIST_ITEMS) {
		this->items[this->num_items] = new InputItem(i, code, r, s, 0.0);
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
void InputList::replace_item(uint32_t i, char * code, rank_t r, score_t s) {
	delete this->items[r - 1];
	this->items[r - 1] = new InputItem(i, code, r, s, 0.0);
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

/// Sort the list in decreasing order of its element scores. Then, update the item rankings and
/// compute four score statistics: min, max, mean, and std.
void InputList::sort_by_score() {
	rank_t i = 0;
	score_t sum = 0.0, mean = 0.0;

	if (this->num_items > 0) {

		qsort(this->items, this->num_items, sizeof(class InputItem *), &InputList::cmp_score);

		// this->display(); getchar();

		/// Set the min/max score values
		this->stats->set_min_val( this->items[this->num_items - 1]->get_inscore() );
		this->stats->set_max_val( this->items[0]->get_inscore() );

		/// Compute and set the mean score value
		for (i = 0; i < this->num_items; i++) {
			this->items[i]->set_idx(i);
			sum += this->items[i]->get_inscore();
		}
		mean = sum / (score_t)this->num_items;
		this->stats->set_mean_val( mean );

		/// Compute and set the standard deviation of the element scores.
		sum = 0.0;
		for (i = 0; i < this->num_items; i++) {
			sum += (this->items[i]->get_inscore() - mean) * (this->items[i]->get_inscore() - mean);
		}

		this->stats->set_std_val( sqrt(sum / this->num_items) );
	}
}

/// Sort the list in decreasing order of its element scores. Then, update the item rankings and
/// compute four score statistics: min, max, mean, and std.
void InputList::sort_by_pscore() {
	if (this->num_items > 0) {
		qsort(this->items, this->num_items, sizeof(class InputItem *), &InputList::cmp_pscore);

		/// Reset the indices
		for (rank_t i = 0; i < this->num_items; i++) {
			this->items[i]->set_idx(i);
		}
	}
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

score_t InputList::get_min_score() { return this->stats->get_min_val(); }
score_t InputList::get_max_score() { return this->stats->get_max_val(); }
score_t InputList::get_mean_score() { return this->stats->get_mean_val(); }
score_t InputList::get_std_score() { return this->stats->get_std_val(); }
