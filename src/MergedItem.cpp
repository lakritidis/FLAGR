#include "MergedItem.h"
#include "ram/tools/BetaDistribution.cpp"

/// Default Constructor
MergedItem::MergedItem() :
	code(NULL),
	final_score (0.0),
	final_ranking(0),
	num_rankings(0),
	num_alloc_rankings(0),
	rankings(nullptr),
	next(nullptr) {
}

/// Constructor 2
MergedItem::MergedItem(class MergedItem * in) {
	class Ranking * r = NULL;

	this->code = new char[strlen(in->get_code()) + 1];
	strcpy(this->code, in->get_code());

	this->final_score = in->get_final_score();
	this->final_ranking = in->get_final_ranking();
	this->num_rankings = in->get_num_rankings();
	this->num_alloc_rankings = in->get_num_alloc_rankings();

	this->rankings = new Ranking * [this->num_alloc_rankings];
	for (uint32_t i = 0; i < this->num_alloc_rankings; i++) {
		r = in->get_ranking(i);
		this->rankings[i] = new Ranking(
			r->get_input_list(), r->get_idx(), r->get_rank(), r->get_score());
	}
	this->next = NULL;
}

/// Constructor 3
MergedItem::MergedItem(char * c, rank_t r, uint32_t nal, class InputList ** l) {
	this->set_code(c);
	this->final_score = 0.0;
	this->final_ranking = 0;
	this->num_rankings = 0;
	this->num_alloc_rankings = nal;

	this->rankings = new Ranking * [this->num_alloc_rankings];
	for (uint32_t i = 0; i < this->num_alloc_rankings; i++) {
		this->rankings[i] = new Ranking(l[i], NOT_RANKED_ITEM_RANK, NOT_RANKED_ITEM_RANK, 0.0);
	}
	this->next = NULL;
}

/// Destructor
MergedItem::~MergedItem() {
	if (this->code) {
		delete [] this->code;
	}

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
void MergedItem::insert_ranking(class InputList * l, uint32_t i, rank_t r, score_t s) {
	this->rankings[ l->get_id() ]->set_input_list (l);
	this->rankings[ l->get_id() ]->set_idx(i);
	this->rankings[ l->get_id() ]->set_rank(r);
	this->rankings[ l->get_id() ]->set_score(s);
	this->num_rankings++;
}


/// Display the MergedItem data
void MergedItem::display() {
	class Ranking * r;
	printf("Item: %s was found in %d input lists, Score: %E:\n",
		this->code, this->num_rankings, this->final_score);

	for (uint32_t i = 0; i < this->num_alloc_rankings; i++) {
		r = this->rankings[i];
		if (r->get_input_list()) {
			r->display();
		}
	}
	printf("\n");
}

/// Sort the individual item rankings in increasing score order.
void MergedItem::sort_rankings_by_score() {
	qsort(this->rankings, this->num_alloc_rankings, sizeof(class Ranking *), &MergedItem::cmp_score);
}

/// Callback function for QuickSort
int MergedItem::cmp_score(const void *a, const void *b) {
	class Ranking * x = * (class Ranking **)a;
	class Ranking * y = * (class Ranking **)b;

	if (x->get_score() > y->get_score()) {
		return 1;
	} else {
		return -1;
	}
}

/// Compute the beta values of the ranking scores
void MergedItem::compute_beta_values() {
	class Ranking * r;
	double p = 0.0;

	for (uint32_t i = 0; i < this->num_alloc_rankings; i++) {
		r = this->rankings[i];

		p = pbeta(r->get_score(), i + 1, this->num_alloc_rankings - i);

		r->set_score( p );
	}
}

/// Find the voters who included this element in their preference lists; sum up their weights and
/// return.
score_t MergedItem::get_sum_voter_weights() {
	class Ranking * r;
	score_t sum_weights = 0.0;

	for (uint32_t i = 0; i < this->num_alloc_rankings; i++) {
		r = this->rankings[i];
		if (r->get_input_list()) {
			if (r->get_rank() != NOT_RANKED_ITEM_RANK) {
				sum_weights += r->get_input_list()->get_voter()->get_weight();
			}
		}
	}

	return sum_weights;
}

/// Find the voters who included this element in their preference lists and set the given
/// preservation score to the corresponding input list element.
void MergedItem::set_individual_preservation_scores(score_t pscore) {
	class Ranking * r;
	class InputItem * elem;

//	printf("\nThis code: %s\n", this->code);
	for (uint32_t i = 0; i < this->num_alloc_rankings; i++) {
		r = this->rankings[i];
		if (r->get_input_list()) {
			if (r->get_rank() != NOT_RANKED_ITEM_RANK) {
				elem = r->get_input_list()->get_item(r->get_idx());
				elem->set_pscore(pscore);
//				printf("\t\tFound in list %s that code: %s\n",
//					r->get_input_list()->get_voter()->get_name(), elem->get_code());
			}
		}
	}
}

/// Mutators
void MergedItem::set_code(char *v) {
	this->code = new char[strlen(v) + 1];
	strcpy(this->code, v);
}
void MergedItem::set_final_score(score_t v) { this->final_score = v; }
void MergedItem::set_final_ranking(rank_t v) { this->final_ranking = v; }
void MergedItem::set_next(class MergedItem * v) { this->next = v; }

/// Accessors
char * MergedItem::get_code() { return this->code; }
score_t MergedItem::get_final_score() { return this->final_score; }
rank_t MergedItem::get_final_ranking() { return this->final_ranking; }
uint32_t MergedItem::get_num_rankings() { return this->num_rankings; }
uint32_t MergedItem::get_num_alloc_rankings() { return this->num_alloc_rankings; }
class MergedItem * MergedItem::get_next() { return this->next; }
class Ranking * MergedItem::get_ranking(uint32_t i) { return this->rankings[i]; }
