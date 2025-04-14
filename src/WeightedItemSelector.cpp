#include "WeightedItemSelector.h"

/// Object constructor
WeightedItemSelector::WeightedItemSelector(
	class InputParams * prms, class InputList ** in, uint32_t n, class SimpleScoreStats * s) :
		params(prms),
		inlists(in),
		num_inlists(n),
		stats(s)
		{ }

/// Destructor
WeightedItemSelector::~WeightedItemSelector() {
	//dtor
}



/// Imprementation of the Unsupervised Weighted Item Selector of [11].
class InputList ** WeightedItemSelector::filter_lists() {
	uint32_t i = 0, j = 0;
	uint32_t bucket = 0, num_buckets = (uint32_t)this->params->get_delta1();
	uint32_t pres_score = 0.0;

	score_t conf_score = 0.0, prot_score = 0.0;
	score_t d = this->params->get_delta2();

	rank_t z1 = 0, z2 = 0;

	class Voter * v = NULL;

	class InputList ** new_lists = new InputList * [this->num_inlists];

	/// delta_1 represents the number of buckets of Eq. of [10]
	/// Distribute the voters (i.e. input lists) to buckets and assign bucket-wise weights.
	qsort(this->inlists, this->num_inlists, sizeof(InputList *), &WeightedItemSelector::cmp_weight);

	/// Set the confidence scores for each voter
	for (i = 0; i < this->num_inlists; i++) {
		v = inlists[i]->get_voter();
		new_lists[i] = new InputList(i, v->get_name(), v->get_weight());

		bucket = (i * num_buckets) / this->num_inlists;

		conf_score = exp(-(score_t)bucket * num_buckets / (score_t)this->num_inlists);
		v->set_weight(conf_score);
	}




	/// Create new input preference lists from the original input lists. The new lists will not
	/// include elements with low preservation scores. The number of elements to be discarded is
	/// determined by the list's protection score.

/*
		pres_score = uint32_t(conf_score * inlists[i]->get_num_items());
		printf("List %d - Conf Score: %5.3f - Pres Score: %d\n", i, conf_score, pres_score);
*/
	return new_lists;
}

int WeightedItemSelector::cmp_weight(const void *a, const void *b) {
	class InputList * x = *(class InputList **)a;
	class InputList * y = *(class InputList **)b;

	if (x->get_voter()->get_weight() > y->get_voter()->get_weight()) { return -1; }
	return 1;
}
