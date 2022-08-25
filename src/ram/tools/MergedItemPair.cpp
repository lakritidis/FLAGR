#include "MergedItemPair.h"

/// Used in the Preference Relations method of [3]
/// Default Constructor
MergedItemPair::MergedItemPair() : item1(NULL), item2(NULL), score(0.0) { }

/// Constructor 1
MergedItemPair::MergedItemPair(class MergedItem * i1, class MergedItem * i2) :
	item1(i1), item2(i2), score(0.0) {
}

/// Destructor
MergedItemPair::~MergedItemPair() { }


/// Compute the a-majority opinion
void MergedItemPair::compute_a_majority_opinion(score_t a, score_t b, uint32_t N) {
	uint32_t r = 0, r1 = 0, r2 = 0;
	uint32_t n0 = 0, n1 = 0;
	score_t DisagreementScore = 0.00;
	class InputList * inlist;

//	printf("\n\nComparing\n"); this->item1->display(); printf("\twith\n\t"); this->item2->display();

	/// Find the number of lists that agree (=n0) or disagree(=n1) with the ranking (item1, item2).
	for (r = 0; r < this->item1->get_num_alloc_rankings(); r++) {
		if (this->item1->get_ranking(r)->get_rank() < this->item2->get_ranking(r)->get_rank()) {
			n0++;
		} else if (this->item1->get_ranking(r)->get_rank() > this->item2->get_ranking(r)->get_rank()) {
			n1++;
		}
	}

	/// Update the disagreement scores of each list according to Eq. 4 of [3].
	for (r = 0; r < this->item1->get_num_alloc_rankings(); r++) {

		r1 = this->item1->get_ranking(r)->get_rank();
		r2 = this->item2->get_ranking(r)->get_rank();
		DisagreementScore = 0.0;

		if (r1 == NOT_RANKED_ITEM_RANK && r2 == NOT_RANKED_ITEM_RANK) {
			DisagreementScore = 0.5;

		} else {
			if (n0 + n1 >= ceil(b * N)) {

				/// Eq. 2 is satisfied for x = 0
				if (n0 < a * (n0 + n1)) {
					if (r1 < r2) {
						DisagreementScore = 1.0;
					}

				} else if (n1 < a * (n0 + n1)) {
					if (r1 > r2) {
						DisagreementScore = 1.0;
					}
				}
			}
		}

		if (DisagreementScore > 0) {
			inlist = this->item1->get_ranking(r)->get_input_list();
			inlist->set_voter_weight( inlist->get_voter()->get_weight() + DisagreementScore );
		}
	}
//	getchar();
}

/// Compute a-majority opinion (debug mode)
void MergedItemPair::compute_a_majority_opinion_debug(score_t a, score_t b, uint32_t N) {
	uint32_t r = 0, r1 = 0, r2 = 0;
	uint32_t n0 = 0, n1 = 0;
	score_t DisagreementScore = 0.00;
	class InputList * inlist;

	printf("\n\nComparing\n"); this->item1->display(); printf("\twith\n\t"); this->item2->display();

	/// Find the number of lists that agree (=n0) or disagree(=n1) with the ranking (item1, item2).
	for (r = 0; r < this->item1->get_num_alloc_rankings(); r++) {
		if (this->item1->get_ranking(r)->get_rank() < this->item2->get_ranking(r)->get_rank()) {
			n0++;
		} else if (this->item1->get_ranking(r)->get_rank() > this->item2->get_ranking(r)->get_rank()) {
			n1++;
		}
	}

	/// Update the disagreement scores of each list according to Eq. 4 of [3].
	for (r = 0; r < this->item1->get_num_alloc_rankings(); r++) {
		printf("\tChecking list %d:\n", r);

		r1 = this->item1->get_ranking(r)->get_rank();
		r2 = this->item2->get_ranking(r)->get_rank();
		DisagreementScore = 0.0;

		if (r1 == NOT_RANKED_ITEM_RANK && r2 == NOT_RANKED_ITEM_RANK) {
			printf("\t\tBoth items are not ranked.\n");
			DisagreementScore = 0.5;
		} else {
			if (n0 + n1 >= ceil(b * N)) {
				printf("\t\t(1): %d + %d >= %2.1f is satisfied.\n", n0, n1, ceil(b * N));

				/// Eq. 2 is satisfied for x = 0
				if (n0 < a * (n0 + n1)) {
					printf("\t\t(2): %d < %2.1f * (%d + %d) is satisfied for n0.\n", n0, a, n0, n1);

					if (r1 > r2) {
						printf("\t\tList %d agrees with a-majority\n", r);
					} else if (r1 < r2) {
						printf("\t\tList %d disagrees with a-majority\n", r);
						DisagreementScore = 1.0;
					}

				} else if (n1 < a * (n0 + n1)) {

					printf("\t\t(2): %d < %2.1f * (%d + %d) is satisfied for n1.\n", n1, a, n0, n1);
					if (r1 < r2) {
						printf("\t\tList %d agrees with a-majority\n", r);
					} else if (r1 > r2) {
						printf("\t\tList %d disagrees with a-majority\n", r);
						DisagreementScore = 1.0;
					}

				} else {
					printf("\t\t(2): None of %d < %2.1f * (%d + %d) and %d < %2.1f * (%d + %d) is satisfied.\n",
							n0, a, n0, n1, n1, a, n0, n1);
				}
			} else {
				printf("\t\t(1): %d + %d >= %2.1f NOT satisfied.\n", n0, n1, ceil(b * N));
			}
		}

		printf("\t\tList %d disagreement score: %2.1f\n", r, DisagreementScore);
		if (DisagreementScore > 0) {
			inlist = this->item1->get_ranking(r)->get_input_list();
			inlist->set_voter_weight( inlist->get_voter()->get_weight() + DisagreementScore );
		}
	}
//	getchar();
}

/// The MergedItemPair is treated as an edge in the aggregate graph. This function computes the
/// weight of the edge.
void MergedItemPair::compute_weight() {
	uint32_t r = 0, r1 = 0, r2 = 0;

	for (r = 0; r < this->item1->get_num_alloc_rankings(); r++) {
		r1 = this->item1->get_ranking(r)->get_rank();
		r2 = this->item2->get_ranking(r)->get_rank();

		/// The weight of this directed edge represents the total weighted votes in favor of the
		/// preference relation item2 is better than item1 (i.e. r2 < r1).
		if (r2 < r1) {
			this->score += this->item2->get_ranking(r)->get_input_list()->get_voter()->get_weight();
		}
	}

//	this->display(0);
}

/// Display the ItemPair
void MergedItemPair::display(uint32_t t) {
	if (t == 0) {
		printf("Edge (%s, %s) Score = %12.10f\n", this->item1->get_code(), this->item2->get_code(), this->score);
	} else if (t == 1) {
		printf("Edge Score: %12.10f\n", this->score);
		printf("Left Node:\n");
		this->item1->display();
		printf("\nRight Node:\n");
		this->item2->display();
	}
}

/// Accessors
class MergedItem * MergedItemPair::get_item1() { return this->item1; }
class MergedItem * MergedItemPair::get_item2() { return this->item2; }
score_t MergedItemPair::get_score() { return this->score; }

/// Mutators
void MergedItemPair::set_item1(class MergedItem * v) { this->item1 = v; }
void MergedItemPair::set_item2(class MergedItem * v) { this->item2 = v; }
