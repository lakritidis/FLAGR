#include "Evaluator.h"

/// Constructor 1 : default
Evaluator::Evaluator() {
	this->relevs = new Rels(DATASET_NUM_QUERIES);
	this->true_positives = 0;
	this->average_precision = 0.0;
	this->average_ndcg = 0.0;
	this->precision = NULL;
	this->recall = NULL;
	this->dcg = NULL;
	this->ndcg = NULL;
}

/// Destructor
Evaluator::~Evaluator() {
	if (this->relevs) {
		delete this->relevs;
		this->relevs = NULL;
	}

	this->clear();
}

/// Insert a relevance judgement into the relevs lexicon
void Evaluator::insert_relev(uint32_t i, char * r, uint32_t j) {
	this->relevs->insert(i, r, j);
}


void Evaluator::clear() {
	if (this->precision) {
		delete [] this->precision;
		this->precision = NULL;
	}

	if (this->recall) {
		delete [] this->recall;
		this->recall = NULL;
	}

	if (this->dcg) {
		delete [] this->dcg;
		this->dcg = NULL;
	}

	if (this->ndcg) {
		delete [] this->ndcg;
		this->ndcg = NULL;
	}
}

/// Evaluate a MergedList
void Evaluator::evaluate(uint32_t tid, class MergedList * lst, FILE * eval_file) {
    rank_t num_items = lst->get_num_items();
    rank_t num_relevant_items = this->relevs->get_num_nodes();
	uint32_t relevance = 0;
	double qty = 0.0;

	this->true_positives = 0;
	this->average_precision = 0.0;
	this->average_ndcg = 0.0;
	this->precision = new double[num_items];
	this->recall = new double[num_items];
	this->dcg = new double[num_items];
	this->ndcg = new double[num_items];

	/// Create the ideal ranking for the calculation of nDCG
	double * ideal_dcg = new double[num_items];

    for (rank_t i = 0; i < num_items; i++) {
		class MergedItem * p = lst->get_item(i);

//		printf("Checking %d - %s: ", i + 1, p->get_code()); fflush(NULL);
		if (this->relevs->search(tid, p->get_code(), &relevance)) {
			if (p->get_score() > 0) {
				this->true_positives++;
				this->average_precision += this->true_positives / (i + 1.0);
			}
//			printf(" * (%d) == METRICS : ", relevance);
		}

		/// Precision@k
		this->precision[i] = this->true_positives / (i + 1.0);

		/// Recall@k
		this->recall[i] = (double) this->true_positives / (double) num_relevant_items;

		/// DCG@k
		if (i == 0) {
			this->dcg[i] = (pow(2.0, relevance) - 1.0) / log2(i + 2.0);
		} else {
			this->dcg[i] = this->dcg[i - 1] + (pow(2.0, relevance) - 1.0) / log2(i + 2.0);
		}

		/// IdealDCG temp: needs to be sorted
		ideal_dcg[i] = (double)relevance;

		if (relevance > 0 && p->get_score() > 0) {
			this->average_ndcg += this->dcg[i] / (i + 1.0);
		}
//		printf("P@%d=%5.3f - R@%d=%5.3f - DCG@%d=%5.3f\n",
//			i+1, this->precision[i], i+1, this->recall[i], i+1, this->dcg[i]);
    }

	/// Sort ideal_dcg in decreasing relevance order and obtain IdealDCG@k
	qsort(ideal_dcg, num_items, sizeof(double), &Evaluator::cmp_dbl);

	/// Compute nDCG@k = DCG@k / IdealDCG@k
    for (rank_t i = 0; i < num_items; i++) {
		qty += (pow(2.00, ideal_dcg[i]) - 1.0) / (log2(i + 2.0));
		if (qty > 0) {
			this->ndcg[i] = this->dcg[i] / qty;
		} else {
			this->ndcg[i] = 0.0;
		}
    }

	/// Compute Average Precision and Average DCG
	if (this->true_positives > 0) {
		this->average_precision = this->average_precision / (double)num_relevant_items;
		this->average_ndcg = this->average_ndcg / (double)num_relevant_items;
	} else {
		this->average_precision = 0.0;
		this->average_ndcg = 0.0;
	}

	delete [] ideal_dcg;

	char str[100];
	sprintf(str, "Topic %d", tid);

	if (RESULTS_TYPE == 1) {
		printf("| %s (%d/%d)\t| %5.4f | %5.4f | %5.4f | %5.4f | %5.4f | %5.4f | %5.4f | %5.4f | %5.4f | %5.4f | %5.4f  | %5.4f  |\n",
			PadStr(str, 10, ' '), this->true_positives, num_relevant_items, this->average_precision,
			num_items > 4 ? this->precision[4] : 0,
			num_items > 9 ? this->precision[9] : 0,
			num_items > 14 ? this->precision[14] : 0,
			num_items > 19 ? this->precision[19] : 0,
			num_items > 29 ? this->precision[29] : 0,
			num_items > 99 ? this->precision[99] : 0,
			num_items > 199 ? this->precision[199] : 0,
			num_items > 499 ? this->precision[499] : 0,
			num_items > 999 ? this->precision[999] : 0,
			num_items > 9 ? this->ndcg[9] : 0,
			num_items > 19 ? this->ndcg[19] : 0);

		if (eval_file) {
			fprintf(eval_file, "| %s (%d/%d)\t| %5.4f | %5.4f | %5.4f | %5.4f | %5.4f | %5.4f | %5.4f | %5.4f | %5.4f | %5.4f | %5.4f  | %5.4f  |\n",
				PadStr(str, 10, ' '), this->true_positives, num_relevant_items, this->average_precision,
				num_items > 4 ? this->precision[4] : 0,
				num_items > 9 ? this->precision[9] : 0,
				num_items > 14 ? this->precision[14] : 0,
				num_items > 19 ? this->precision[19] : 0,
				num_items > 29 ? this->precision[29] : 0,
				num_items > 99 ? this->precision[99] : 0,
				num_items > 199 ? this->precision[199] : 0,
				num_items > 499 ? this->precision[499] : 0,
				num_items > 999 ? this->precision[999] : 0,
				num_items > 9 ? this->ndcg[9] : 0,
				num_items > 19 ? this->ndcg[19] : 0);
		}
	} else if (RESULTS_TYPE == 2) {
		printf("| %s (%d/%d)\t| %4.3f | %4.3f | %4.3f | %4.3f | %4.3f | %4.3f | %4.3f | %4.3f | %4.3f | %4.3f | %4.3f | %4.3f | %4.3f | %4.3f |\n",
			PadStr(str, 10, ' '), this->true_positives, num_relevant_items, this->average_precision,
			num_items > 4 ? this->precision[4] : 0,
			num_items > 9 ? this->precision[9] : 0,
			num_items > 19 ? this->precision[19] : 0,
			num_items > 29 ? this->precision[29] : 0,
			num_items > 49 ? this->precision[49] : 0,
			num_items > 99 ? this->precision[99] : 0,
			this->average_ndcg,
			num_items > 4 ? this->ndcg[4] : 0,
			num_items > 9 ? this->ndcg[9] : 0,
			num_items > 19 ? this->ndcg[19] : 0,
			num_items > 29 ? this->ndcg[29] : 0,
			num_items > 49 ? this->ndcg[49] : 0,
			num_items > 99 ? this->ndcg[99] : 0);

		if (eval_file) {
			fprintf(eval_file, "| %s (%d/%d)\t| %4.3f | %4.3f | %4.3f | %4.3f | %4.3f | %4.3f | %4.3f | %4.3f | %4.3f | %4.3f | %4.3f | %4.3f | %4.3f | %4.3f |\n",
				PadStr(str, 10, ' '), this->true_positives, num_relevant_items, this->average_precision,
				num_items > 4 ? this->precision[4] : 0,
				num_items > 9 ? this->precision[9] : 0,
				num_items > 19 ? this->precision[19] : 0,
				num_items > 29 ? this->precision[29] : 0,
				num_items > 49 ? this->precision[49] : 0,
				num_items > 99 ? this->precision[99] : 0,
				this->average_ndcg,
				num_items > 4 ? this->ndcg[4] : 0,
				num_items > 9 ? this->ndcg[9] : 0,
				num_items > 19 ? this->ndcg[19] : 0,
				num_items > 29 ? this->ndcg[29] : 0,
				num_items > 49 ? this->ndcg[49] : 0,
				num_items > 99 ? this->ndcg[99] : 0);
		}
	}
}

/// Evaluate an InputList
double Evaluator::evaluate_input(uint32_t tid, class InputList * lst) {
    rank_t num_items = lst->get_num_items();
    rank_t num_relevant_items = this->relevs->get_num_nodes();
	uint32_t relevance = 0;
	double qty = 0.0;

	this->true_positives = 0;
	this->average_precision = 0.0;
	this->average_ndcg = 0.0;
	this->precision = new double[num_items];
	this->recall = new double[num_items];
	this->dcg = new double[num_items];
	this->ndcg = new double[num_items];

	/// Create the ideal ranking for the calculation of nDCG
	double * ideal_dcg = new double[num_items];

    for (rank_t i = 0; i < num_items; i++) {
		class InputItem * p = lst->get_item(i);

//		if (strcmp(lst->get_voter()->get_name(), "input.srchvrs11b.gz") == 0 && tid == 145) {
//			printf("Checking %d - %s: ", i + 1, p->get_code()); fflush(NULL);
//			getchar();
//		}
		if (this->relevs->search(tid, p->get_code(), &relevance)) {
			this->true_positives++;
			this->average_precision += this->true_positives / (i + 1.0);
//			printf(" * (%d) == METRICS : ", relevance);
		}

		/// Precision@k
		this->precision[i] = this->true_positives / (i + 1.0);

		/// Recall@k
		this->recall[i] = (double) this->true_positives / (double) num_relevant_items;

		/// DCG@k
		if (i == 0) {
			this->dcg[i] = (pow(2.0, relevance) - 1.0) / log2(i + 2.0);
		} else {
			this->dcg[i] = this->dcg[i - 1] + (pow(2.0, relevance) - 1.0) / log2(i + 2.0);
		}

		/// IdealDCG temp: needs to be sorted
		ideal_dcg[i] = (double)relevance;

		if (relevance > 0) {
			this->average_ndcg += this->dcg[i] / (i + 1.0);
		}
//		printf("P@%d=%5.3f - R@%d=%5.3f - DCG@%d=%5.3f\n",
//			i+1, this->precision[i], i+1, this->recall[i], i+1, this->dcg[i]);
    }

	/// Sort ideal_dcg in decreasing relevance order and obtain IdealDCG@k
	qsort(ideal_dcg, num_items, sizeof(double), &Evaluator::cmp_dbl);

	/// Compute nDCG@k = DCG@k / IdealDCG@k
    for (rank_t i = 0; i < num_items; i++) {
		qty += (pow(2.00, ideal_dcg[i]) - 1.0) / (log2(i + 2.0));
		if (qty > 0) {
			this->ndcg[i] = this->dcg[i] / qty;
		} else {
			this->ndcg[i] = 0.0;
		}
    }

	/// Compute Average Precision and Average DCG
	if (this->true_positives > 0) {
		this->average_precision = this->average_precision / (double)num_relevant_items;
		this->average_ndcg = this->average_ndcg / (double)num_relevant_items;
	} else {
		this->average_precision = 0.0;
		this->average_ndcg = 0.0;
	}

	delete [] ideal_dcg;

	char str[100];
	sprintf(str, "%s - Topic %d", lst->get_voter()->get_name(), tid);
/*
	if (RESULTS_TYPE == 1) {
		printf("| %s (%d/%d)\t| %5.4f | %5.4f | %5.4f | %5.4f | %5.4f | %5.4f | %5.4f | %5.4f | %5.4f | %5.4f | %5.4f  | %5.4f  |\n",
			PadStr(str, 10, ' '), this->true_positives, num_relevant_items, this->average_precision,
			num_items > 4 ? this->precision[4] : 0,
			num_items > 9 ? this->precision[9] : 0,
			num_items > 14 ? this->precision[14] : 0,
			num_items > 19 ? this->precision[19] : 0,
			num_items > 29 ? this->precision[29] : 0,
			num_items > 99 ? this->precision[99] : 0,
			num_items > 199 ? this->precision[199] : 0,
			num_items > 499 ? this->precision[499] : 0,
			num_items > 999 ? this->precision[999] : 0,
			num_items > 9 ? this->ndcg[9] : 0,
			num_items > 19 ? this->ndcg[19] : 0);

	} else if (RESULTS_TYPE == 2) {
		printf("| %s (%d/%d)\t| %4.3f | %4.3f | %4.3f | %4.3f | %4.3f | %4.3f | %4.3f | %4.3f | %4.3f | %4.3f | %4.3f | %4.3f | %4.3f | %4.3f |\n",
			PadStr(str, 10, ' '), this->true_positives, num_relevant_items, this->average_precision,
			num_items > 4 ? this->precision[4] : 0,
			num_items > 9 ? this->precision[9] : 0,
			num_items > 19 ? this->precision[19] : 0,
			num_items > 29 ? this->precision[29] : 0,
			num_items > 49 ? this->precision[49] : 0,
			num_items > 99 ? this->precision[99] : 0,
			this->average_ndcg,
			num_items > 4 ? this->ndcg[4] : 0,
			num_items > 9 ? this->ndcg[9] : 0,
			num_items > 19 ? this->ndcg[19] : 0,
			num_items > 29 ? this->ndcg[29] : 0,
			num_items > 49 ? this->ndcg[49] : 0,
			num_items > 99 ? this->ndcg[99] : 0);
	}
*/
	delete [] this->precision;
	this->precision = NULL;
	delete [] this->recall;
	this->recall = NULL;
	delete [] this->dcg;
	this->dcg = NULL;
	delete [] this->ndcg;
	this->ndcg = NULL;

	return this->average_ndcg;
}

/// Accessors
uint32_t Evaluator::get_num_nodes() { return this->relevs->get_num_nodes(); }
uint32_t Evaluator::get_true_positives() { return this->true_positives; }
double Evaluator::get_average_precision() { return this->average_precision; }
double Evaluator::get_average_ndcg() { return this->average_ndcg; }

double Evaluator::get_precision(uint32_t i) {
	if (i < 1 || !this->precision[i - 1]) {
		return 0.0;
	}
	return this->precision[i - 1];
}

double Evaluator::get_recall(uint32_t i) {
	if (i < 1 || !this->recall[i - 1]) {
		return 0.0;
	}
	return this->recall[i - 1];
}

double Evaluator::get_F1(uint32_t i) {
	if (i < 1 || !this->recall[i - 1] || !this->precision[i - 1]) {
		return 0.0;
	}
	return 2 * this->precision[i-1] * this->recall[i-1] / (this->precision[i-1] + this->recall[i-1]);
}

double Evaluator::get_dcg(uint32_t i) {
	if (i < 1 || !this->dcg[i - 1]) {
		return 0.0;
	}
	return this->dcg[i - 1];
}

inline double Evaluator::get_ndcg(uint32_t i) {
	if (i < 1 || !this->ndcg[i - 1]) {
		return 0.0;
	}
	return this->ndcg[i - 1];
}
