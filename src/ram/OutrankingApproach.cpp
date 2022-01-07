/// 3. OUTRANKING: Assign Scores to the items of the MergedList according to the OutRanking Approach
/// introduced in: M. Farah, D. Vanderpooten, "An outranking approach for rank aggregation in
/// information retrieval", SIGIR 2009, pp. 591-598.
void MergedList::Outranking(double min_w, double max_w, double mean_w, double sd_w) {
	double PREF_THRESHOLD = 0.0, VETO_THRESHOLD = 0.75, CONC_THRESHOLD = 0.50, DISC_THRESHOLD = 0.00;

	uint32_t preference_threshold = (uint32_t)(PREF_THRESHOLD * MAX_LIST_ITEMS);
	uint32_t veto_threshold = (uint32_t)(VETO_THRESHOLD * MAX_LIST_ITEMS);
	uint32_t concordance_threshold = (uint32_t)(CONC_THRESHOLD * this->num_input_lists);
	uint32_t discordance_threshold = (uint32_t)(DISC_THRESHOLD * this->num_input_lists);

	class MergedItem *p, *q;
	double temp_score_1 = 0.0, temp_score_2 = 0.0, p_rank = 0.0, q_rank = 0.0, weight = 0.0;
	bool verbose = false;
	uint32_t scenario = 1;

	for (rank_t i = 0; i < this->num_nodes; i++) {
		p = this->item_list[i];

		for (rank_t j = 0; j < this->num_nodes; j++) {
			q = this->item_list[j];

			temp_score_1 = 0.0;
			temp_score_2 = 0.0;

			if (p != q) {
				for (uint32_t k = 0; k < this->num_input_lists; k++) {
					#if WEIGHTS_NORMALIZATION == 1
						weight = p->get_ranking(k)->get_input_list()->get_voter()->get_weight();
					#elif WEIGHTS_NORMALIZATION == 2
						weight = (p->get_ranking(k)->get_input_list()->get_voter()->get_weight() - min_w) / (max_w - min_w);
					#elif WEIGHTS_NORMALIZATION == 3
						weight = p->get_ranking(k)->get_input_list()->get_voter()->get_weight() * sd_w * sd_w / mean_w;
					#endif

					if (scenario == 1) {
						p_rank = p->get_ranking(k)->get_rank() * weight;
						q_rank = q->get_ranking(k)->get_rank() * weight;
					} else if (scenario == 2) {
						p_rank = p->get_ranking(k)->get_rank();
						q_rank = q->get_ranking(k)->get_rank();
					}
//					printf("%d: %5.3f - %5.3f - (%5.3f, %5.3f)\n", k,
//						p->get_ranking(k)->get_input_list()->get_voter()->get_weight(),
//						q->get_ranking(k)->get_input_list()->get_voter()->get_weight(), p_rank, q_rank);

					if (p_rank <= q_rank - preference_threshold) {
						if (scenario == 1) { temp_score_1 ++; }
						else if (scenario == 2) { temp_score_1 += weight; }
					}

					if (p_rank >= q_rank + veto_threshold) {
						if (scenario == 1) { temp_score_2 ++; }
						else if (scenario == 2) { temp_score_2 += weight; }
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
					p->set_score(p->get_score() + 1);
				} else if (scenario == 2) {
					p->set_score(p->get_score() + temp_score_1);
				}
			}
		}
	}

	qsort(this->item_list, this->num_nodes, sizeof(class MergedItem *), &MergedList::cmp_score_desc);
//	printf("\nnormalization=%d and scenario no %d\n", WEIGHTS_NORMALIZATION, scenario); getchar();
}
