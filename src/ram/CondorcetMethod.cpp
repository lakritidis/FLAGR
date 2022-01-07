/// 2. CONDORCET METHOD: Assign Scores to the items of the MergedList according to the Condorcet method.
void MergedList::CondorcetMethod(double min_w, double max_w, double mean_w, double sd_w) {

	class MergedItem *p, *q;
	bool verbose = false;
	double p_rank = 0.0, q_rank = 0.0, wins = 0, losses = 0, ties = 0, weight = 0;

	for (rank_t i = 0; i < this->num_nodes; i++) {
		p = this->item_list[i];

		for (rank_t j = i + 1; j < this->num_nodes; j++) {
			q = this->item_list[j];

			wins = 0; losses = 0; ties = 0;

			for (uint32_t k = 0; k < this->num_input_lists; k++) {
				#if WEIGHTS_NORMALIZATION == 1
					weight = p->get_ranking(k)->get_input_list()->get_voter()->get_weight();
				#elif WEIGHTS_NORMALIZATION == 2
					weight = (p->get_ranking(k)->get_input_list()->get_voter()->get_weight() - min_w) / (max_w - min_w);
				#elif WEIGHTS_NORMALIZATION == 3
					weight = p->get_ranking(k)->get_input_list()->get_voter()->get_weight() * sd_w * sd_w / mean_w;
				#endif

				p_rank = p->get_ranking(k)->get_rank();
				q_rank = q->get_ranking(k)->get_rank();

//				printf("%d: %5.3f-%5.3f - (%5.3f,%5.3f)\n",
//					k, p->get_ranking(k)->get_input_list()->get_voter()->get_weight(),
//					q->get_ranking(k)->get_input_list()->get_voter()->get_weight(), p_rank, q_rank);

				if (p_rank < q_rank) {
					wins += weight;
					if (verbose) { printf("WIN %5.3f: ", wins); }
				} else if (p_rank == q_rank) {
					ties += weight;
					if (verbose) { printf("TIE %5.3f: ", ties); }
				} else {
					losses += weight;
					if (verbose) { printf("LOSS %5.3f: ", losses); }
				}

				if (verbose) {
					printf("LIST %d: Item %d (%s) rank %5.3f (VS) item %d (%s) rank %5.3f\n",
						k, i, p->get_code(), p_rank, j, q->get_code(), q_rank);
				}
			}

			if (wins > losses) {
				if (verbose) {
					printf("Item %d (%s) WINS item %d (%s) -- %5.3f-%5.3f-%5.3f\n", i, p->get_code(),
						j, q->get_code(), wins, ties, losses); getchar();
				}

				p->set_score(p->get_score() + 1);

			} else if (wins < losses) {
				if (verbose) {
					printf("Item %d (%s) LOSSES FROM item %d (%s) -- %5.3f-%5.3f-%5.3f\n",
						i, p->get_code(), j, q->get_code(), wins, ties, losses);
					getchar();
				}
				q->set_score(q->get_score() + 1);
			}
		}
	}

	qsort(this->item_list, this->num_nodes, sizeof(class MergedItem *), &MergedList::cmp_score_desc);
}

/*
void MergedList::CondorcetMethodSlow(double min_w, double max_w, double mean_w, double sd_w) {
	class MergedItem *p, *q;
	bool verbose = false;
	double p_rank = 0.0, q_rank = 0.0, wins = 0, losses = 0, ties = 0;

	for (rank_t i = 0; i < this->num_nodes; i++) {
		p = this->item_list[i];

		for (rank_t j = 0; j < this->num_nodes; j++) {
			q = this->item_list[j];

			wins = 0; losses = 0; ties = 0;

			if (p != q) {
				for (uint32_t k = 0; k < this->num_input_lists; k++) {
					p_rank = p->get_ranking(k)->get_rank();
					q_rank = q->get_ranking(k)->get_rank();

//						printf("%d: %5.3f-%5.3f - (%5.3f,%5.3f)\n",
//							k, p->get_ranking(k)->List->get_voter()->get_weight(),
//							q->get_ranking(k)->List->get_voter()->get_weight(), p_rank, q_rank);

					if (p_rank < q_rank) {
						wins += p->get_ranking(k)->get_input_list()->get_voter()->get_weight();
						if (verbose) { printf("WIN %5.3f: ", wins); }
					} else if (p_rank == q_rank) {
						ties += p->get_ranking(k)->get_input_list()->get_voter()->get_weight();
						if (verbose) { printf("TIE %5.3f: ", ties); }
					} else {
						losses += p->get_ranking(k)->get_input_list()->get_voter()->get_weight();
						if (verbose) { printf("LOSS %5.3f: ", losses); }
					}

					if (verbose) {
						printf("LIST %d: Item %d (%s) rank %5.3f (VS) item %d (%s) rank %5.3f\n",
							k, i, p->get_code(), p_rank, j, q->get_code(), q_rank);
					}
				}
			}

			if (wins > losses) {
				if (verbose) {
					printf("Item %d (%s) WINS item %d (%s) -- %5.3f-%5.3f-%5.3f\n",
						i, p->get_code(), j, q->get_code(), wins, ties, losses);
					getchar();
				}

				p->set_score(p->get_score() + 1);

			} else {
				if (verbose) {
					printf("Item %d (%s) LOSSES FROM item %d (%s) -- %5.3f-%5.3f-%5.3f\n",
						i, p->get_code(), j, q->get_code(), wins, ties, losses);
					getchar();
				}
			}
		}
	}

	qsort(this->item_list, this->num_nodes, sizeof(class MergedItem *), &MergedList::cmp_score_desc);
}
*/
