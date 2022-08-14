/// The CombSUM family of linear combination methods. Details about the 5 implemented variants are
/// published in the following paper:
/// Renda E., Straccia U., "Web metasearch: rank vs. score based rank aggregation methods", In
/// Proceedings of the 2003 ACM symposium on Applied computing, pp. 841-846, 2003.
/// ///////////////////////////////////////////////////////////////////////////////////////////////

void MergedList::CombSUM(class InputList ** inlists, class SimpleScoreStats * s, class InputParams * prms) {
	class MergedItem * q;
	class Ranking * r;
	class InputList * l;
	double score = 0.0, voter_weight = 1.0;

	uint32_t weights_norm = prms->get_weights_normalization();
	uint32_t ram = prms->get_aggregation_method();

	/// For each element in MergedList, compute the score w.r.t to the selected normalization method
	for (rank_t i = 0; i < this->num_nodes; i++) {
		q = this->item_list[i];

		for (uint32_t j = 0; j < q->get_num_alloc_rankings(); j++) {
			r = q->get_ranking(j);
			l = r->get_input_list();

			/// Voter weights normalization
			voter_weight = l->get_voter()->get_weight();

			/// Min-max normalization of voter weights
			if (weights_norm == 2) {
				voter_weight = (voter_weight - s->get_min_val()) / (s->get_max_val() - s->get_min_val());

			/// Z-normalization of voter weights
			} else if (weights_norm == 3) {
				voter_weight = voter_weight * s->get_std_val() * s->get_std_val() / s->get_max_val();

			/// Division of the voter weights by the maximum voter score
			} else if (weights_norm == 4) {
				voter_weight = voter_weight / s->get_max_val();
			}

			/// Compute the element scores. Case A: The element has been ranked in list l
			if(l && r->get_rank() != NOT_RANKED_ITEM_RANK) {
//				printf("Real Voter weight: %5.3f - Normalized: %5.3f\n",
//							l->get_voter()->get_weight(), voter_weight);

				/// Borda normalization: Eq: 4 (first branch) of [1]
				if(ram == 100 || ram == 5100) {
					score = (this->num_nodes - r->get_rank() + 1.0) / (score_t)this->num_nodes;

				/// Rank normalization: Eq: 3 of [1]
				} else if (ram == 101 || ram == 5101) {
					score = (l->get_num_items() - r->get_rank() + 1.0) / (score_t)l->get_num_items();

				/// Score normalization: Eq: 1 of [1]
				} else if (ram == 102 || ram == 5102) {
					score = (r->get_score() - l->get_min_score()) / (l->get_max_score() - l->get_min_score());

				/// Z-Score normalization: Eq: 2 of [1]
				} else if (ram == 103 || ram == 5103) {
					score = (r->get_score() - l->get_mean_score()) / l->get_std_score();

				/// Simple Borda normalization
				} else if (ram == 104 || ram == 5104) {
					score = (this->num_nodes - r->get_rank() + 1.0) / (score_t)this->num_nodes;
				}

			/// Case B: The element has NOT been ranked in list l
			} else {
				/// Borda normalization: Eq: 4 (second branch) of [1]
				if(ram == 100 || ram == 5100) {
					score = (this->num_nodes - l->get_num_items() + 1.0) / (2.0 * this->num_nodes);

				/// Rank/Score/Z-Score/SimpleBorda normalization: No scores are assigned to
				/// non-ranked elements [1]
				} else if(ram == 101 || ram == 102 || ram == 103 || ram == 104 ||
					ram == 5101 || ram == 5102 || ram == 5103 || ram == 5104) {
						score = 0.0;
				}
			}

			score *= voter_weight;

			q->set_final_score(q->get_final_score() + score);
		}
//		printf("%d : Item Score: %5.3f\n", i, q->get_score());
	}

	qsort(this->item_list, this->num_nodes, sizeof(class MergedItem *), &MergedList::cmp_score_desc);

//	this->display_list(); getchar();
}

