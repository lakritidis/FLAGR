/// The method of Condorcet Winners: Assign Scores to the items of MergedList w.r.t to Condorcet criterion.

void MergedList::CondorcetWinners(class InputList ** inlists,  class SimpleScoreStats * s, class InputParams * prms) {

	class MergedItem *p, *q;
	bool verbose = false;
	rank_t p_rank = 0, q_rank = 0;
	score_t wins = 0.0, losses = 0.0, ties = 0.0, voter_weight = 1.0;

	uint32_t weights_norm = prms->get_weights_normalization();

	for (rank_t i = 0; i < this->num_nodes; i++) {
		p = this->item_list[i];

		for (rank_t j = i + 1; j < this->num_nodes; j++) {
			q = this->item_list[j];

			wins = 0; losses = 0; ties = 0;

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

				p_rank = p->get_ranking(k)->get_rank();
				q_rank = q->get_ranking(k)->get_rank();

//				printf("%d: %5.3f-%5.3f - (%d,%d)\n",
//					k, p->get_ranking(k)->get_input_list()->get_voter()->get_weight(),
//					q->get_ranking(k)->get_input_list()->get_voter()->get_weight(), p_rank, q_rank);

				if (p_rank < q_rank) {
					wins += voter_weight;
					if (verbose) { printf("WIN %5.3f: ", wins); }
				} else if (p_rank == q_rank) {
					ties += voter_weight;
					if (verbose) { printf("TIE %5.3f: ", ties); }
				} else {
					losses += voter_weight;
					if (verbose) { printf("LOSS %5.3f: ", losses); }
				}

				if (verbose) {
					printf("\nLIST %d: Item %d (%s) rank %d (VS) item %d (%s) rank %d\n",
						k, i, p->get_code(), p_rank, j, q->get_code(), q_rank);
				}
			}

			if (wins > losses) {
				if (verbose) {
					printf("Item %d (%s) WINS item %d (%s) -- %5.3f-%5.3f-%5.3f\n", i, p->get_code(),
						j, q->get_code(), wins, ties, losses); getchar();
				}

				p->set_final_score(p->get_final_score() + 1);

			} else if (wins < losses) {
				if (verbose) {
					printf("Item %d (%s) LOSSES FROM item %d (%s) -- %5.3f-%5.3f-%5.3f\n",
						i, p->get_code(), j, q->get_code(), wins, ties, losses);
					getchar();
				}
				q->set_final_score(q->get_final_score() + 1);
			}
		}
	}

	qsort(this->item_list, this->num_nodes, sizeof(class MergedItem *), &MergedList::cmp_score_desc);
}

