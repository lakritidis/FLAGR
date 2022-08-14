/// The Robust Rank Aggregation Algorithm of Kolde et. al 2012:
/// Kolde R., Laur S., Adler P., Vilo J., "Robust rank aggregation for gene list integration and
/// meta-analysis", Bioinformatics, vol. 28, no. 4, pp. 573--580, 2012.
/// ///////////////////////////////////////////////////////////////////////////////////////////////

/// This implementation imitates the one of the RobustRankAggreg package of R.
void MergedList::RobustRA(class InputList ** inlists,  class SimpleScoreStats * s, class InputParams * prms) {
	uint32_t i = 0;
	int error;
	rank_t rnk = 0;
	score_t min_p = 0.0;
	class MergedItem *p = NULL;
	class Ranking *r = NULL;

	uint32_t num_elements = this->num_nodes;
//	uint32_t num_elements = 6206; // for testing with RobustRankAggreg

	/// Equivalent to the rankMatrix function of RobustRankAggreg
	for (rnk = 0; rnk < this->num_nodes; rnk++) {
		p = this->item_list[rnk];

		for (i = 0; i < this->num_input_lists; i++) {
			r = p->get_ranking(i);

			if (r->get_rank() == NOT_RANKED_ITEM_RANK) {
				r->set_score(1.0);
			} else {
				r->set_score(r->get_rank() / (double)num_elements);
			}
		}
	}

	/// Find the p-values of the prevoiusly computed scores, assuming that the elements are
	/// distributed with beta distribution .
	for (rnk = 0; rnk < this->num_nodes; rnk++) {
		p = this->item_list[rnk];

		/// Sort the rankings of each element
		p->sort_rankings_by_score();

		/// Obtain the probabilites with the incomplete beta probability distribution function
		/// See Algorithm ASA063 of [8]
		p->compute_beta_values();
	}

	/// Compute the rho values: corrected p-values
	/// Exact correction: We apply the inverted incomplete beta distribution function on the obtained
	/// p-values and then, we apply the Stuart-Ares method
	if (prms->get_exact()) {
		double * rm = new double[this->num_input_lists];       /// Vector for the exact p-value correction
		double * buf = new double[this->num_input_lists + 2];  /// Helper buffer
		double * buf2 = new double[this->num_input_lists + 2]; /// Helper buffer2

		double * factorials170 = this->precompute_170_factorials();

		for (rnk = 0; rnk < this->num_nodes; rnk++) {
			p = this->item_list[rnk];

			min_p = 1.0;
			for (uint32_t j = 0; j < p->get_num_alloc_rankings(); j++) {
				r = p->get_ranking(j);
				if (r->get_score() < min_p) {
					min_p = r->get_score();
				}
			}

			for (uint32_t j = 0; j < this->num_input_lists; j++) {
				rm[this->num_input_lists - j - 1] = 1.0 - xinbta(this->num_input_lists - j, j + 1,
						log(betaFunction(this->num_input_lists-j, j+1)), min_p, &error);
			}

			double score = 1.0 - this->stuart(rm, buf, buf2, factorials170, p->get_code());

			p->set_final_score( score );
		}

		delete [] factorials170;
		delete [] rm;
		delete [] buf;
		delete [] buf2;

	/// Approximate correction: Just get the minimum p-value that was obtained from the incomplete
	/// beta distribution function
	} else {
		for (rnk = 0; rnk < this->num_nodes; rnk++) {
			p = this->item_list[rnk];

			min_p = 1.0;
			for (uint32_t j = 0; j < p->get_num_alloc_rankings(); j++) {
				r = p->get_ranking(j);
				if (r->get_score() < min_p) {
					min_p = r->get_score();
				}
			}

			if (min_p * p->get_num_alloc_rankings() < 1.0) {
				p->set_final_score( min_p * p->get_num_alloc_rankings() );
			} else {
				p->set_final_score(1.0);
			}
		}
	}

	/// Sort the list elements in increasing score order
	qsort(this->item_list, this->num_nodes, sizeof(class MergedItem *), &MergedList::cmp_score_asc);

//	this->display_list();
}


/// Stuart p-value correction
score_t MergedList::stuart(double * r, double * v, double * p, double * factorials170, char * code) {
	score_t score = 0.0;

	qsort(r, this->num_input_lists, sizeof(double), &MergedList::cmp_double);

	/// Initialize the auxiliary buffers
	for (uint32_t k = 0; k <= this->num_input_lists + 1; k++) {
		v[k] = 1.0;
		p[k] = 1.0;
	}

	for (uint32_t k = 1; k <= this->num_input_lists; k++) {
		v[k + 1] = this->sumStuart(v, r[this->num_input_lists - k], k, p, factorials170);
	}

	score = factorials170[this->num_input_lists] * v[this->num_input_lists + 1];

	return score;
}


score_t MergedList::sumStuart(double * v, double r, uint32_t k, double * p, double * factorials170) {
	for (uint32_t i = 1; i <= k; i++) {
		p[i] = v[k - i + 1] * pow(r, i) / factorials170[i];
//		printf("%E, ", p[i]);
	}

//	printf("\n===========\n");
	double sum = 0.0;
	for (uint32_t i = 1; i <= k; i++) {
		if (i % 2 == 0) {
			sum -= p[i];
		} else {
			sum += p[i];
		}
//		printf("%E, ", sum);
	}

	return sum;
}

