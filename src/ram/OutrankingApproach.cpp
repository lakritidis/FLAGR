/// The Outranking Approach of Farah and Vanderpooten, 2007. Published in:
/// Farah, M., Vanderpooten, D., "An outranking approach for rank aggregation in information
/// retrieval", In Proceedings of the 30th ACM Conference on Research and Development in
/// Information Retrieval, pp. 591-598, 2007.
/// ///////////////////////////////////////////////////////////////////////////////////////////////

void MergedList::Outranking(class InputList ** inlists, class SimpleScoreStats * s, class InputParams * prms) {
	score_t PREF_THRESHOLD = prms->get_pref_thr();
	score_t VETO_THRESHOLD = prms->get_veto_thr();
	score_t CONC_THRESHOLD = prms->get_conc_thr();
	score_t DISC_THRESHOLD = prms->get_disc_thr();

	uint32_t preference_threshold = (uint32_t)(PREF_THRESHOLD * this->num_nodes);
	uint32_t veto_threshold = (uint32_t)(VETO_THRESHOLD * this->num_nodes);
	uint32_t concordance_threshold = (uint32_t)(CONC_THRESHOLD * this->num_input_lists);
	uint32_t discordance_threshold = (uint32_t)(DISC_THRESHOLD * this->num_input_lists);

	class MergedItem *p, *q;
	score_t temp_score_1 = 0.0, temp_score_2 = 0.0, p_rank = 0.0, q_rank = 0.0;
	bool verbose = false;
	uint32_t scenario = 2;

	score_t voter_weight = 0.0;
	uint32_t weights_norm = prms->get_weights_normalization();

	for (rank_t i = 0; i < this->num_nodes; i++) {
		p = this->item_list[i];

		for (rank_t j = 0; j < this->num_nodes; j++) {
			q = this->item_list[j];

			temp_score_1 = 0.0;
			temp_score_2 = 0.0;

			if (p != q) {
				for (uint32_t k = 0; k < this->num_input_lists; k++) {
					voter_weight = p->get_ranking(k)->get_input_list()->get_voter()->get_weight();

					/// Min-max normalization of voter weights
					if (weights_norm == 2) {
						voter_weight = (voter_weight - s->get_min_val()) / (s->get_max_val() - s->get_min_val());

					/// Z normalization of voter weights
					} else if (weights_norm == 3) {
						voter_weight = voter_weight * s->get_std_val() * s->get_std_val() / s->get_max_val();

					/// Division of the voter weights by the maximum voter score
					} else if (weights_norm == 4) {
						voter_weight = voter_weight / s->get_max_val();
					}

					if (scenario == 1) {
						p_rank = p->get_ranking(k)->get_rank() * voter_weight;
						q_rank = q->get_ranking(k)->get_rank() * voter_weight;

					} else if (scenario == 2) {
						p_rank = p->get_ranking(k)->get_rank();
						q_rank = q->get_ranking(k)->get_rank();
					}
//					printf("%d: %5.3f - %5.3f - (%5.3f, %5.3f)\n", k,
//						p->get_ranking(k)->get_input_list()->get_voter()->get_weight(),
//						q->get_ranking(k)->get_input_list()->get_voter()->get_weight(), p_rank, q_rank);

					if (p_rank <= q_rank - preference_threshold) {
						if (scenario == 1) { temp_score_1++; }
						else if (scenario == 2) { temp_score_1 += voter_weight; }
					}

					if (p_rank >= q_rank + veto_threshold) {
						if (scenario == 1) { temp_score_2 ++; }
						else if (scenario == 2) { temp_score_2 += voter_weight; }
					}

					if (verbose) {
						printf("LIST %d: Item %d (%s) rank %5.3f (VS) item %d (%s) rank %5.3f\n",
							k, i, p->get_code(), p_rank, j, q->get_code(), q_rank);
					}
				}
			}

			if (verbose) {
				printf("Concordance = %5.2f. Discordance = %5.2f\n", temp_score_1, temp_score_2);
				getchar();
			}

			/// Compute the final concordance/discordance score
			if (temp_score_1 >= concordance_threshold && temp_score_2 <= discordance_threshold) {
				if (scenario == 1) {
					p->set_final_score(p->get_final_score() + 1.0);
				} else if (scenario == 2) {
					p->set_final_score(p->get_final_score() + temp_score_1);
				}
			}
		}
	}

	qsort(this->item_list, this->num_nodes, sizeof(class MergedItem *), &MergedList::cmp_score_desc);
}
