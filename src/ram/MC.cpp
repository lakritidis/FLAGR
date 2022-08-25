/// The Markov Chains methods of Dwork et. al, 2001. Published in:
/// Dwork C., Kumar R., Naor M., Sivakumar D., "Rank Aggregation Methods for the Web", In
///	Proceedings of the 10th International Conference on World Wide Web, pp. 613-622, 2001.
/// ///////////////////////////////////////////////////////////////////////////////////////////////

void MergedList::MC(class InputList ** inlists, class SimpleScoreStats * s, class InputParams * prms) {

	rank_t i = 0;

	qsort(this->item_list, this->num_nodes, sizeof(class MergedItem *), &MergedList::cmp_code_asc);

	score_t * state_matrix = this->compute_state_matrix(s, prms);

	for (i = 0; i < this->num_nodes; i++) {
		this->item_list[i]->set_final_score( state_matrix[i] );
	}

	delete [] state_matrix;

	qsort(this->item_list, this->num_nodes, sizeof(class MergedItem *), &MergedList::cmp_score_desc);

	//this->display_list();
}

/// Compute the ergodic transition matrix of the ranking (used in MC4)
score_t * MergedList::compute_state_matrix(class SimpleScoreStats * s, class InputParams * prms) {
	class MergedItem *p, *q;

	rank_t p_rank = 0, q_rank = 0, i = 0, j = 0, num_items = this->num_nodes;
	score_t val = 0.0, sum = 0.0, voter_weight = 1.0;
	score_t ergodic_number = prms->get_alpha();

	uint32_t weights_norm = prms->get_weights_normalization();
	uint32_t ram = prms->get_aggregation_method();
	uint32_t num_lists = this->num_input_lists;
	uint32_t k = 0, wins = 0, lists_with_both = 0;
	int32_t iteration = 0;

	/// STEP 1: Initialize the transition matrix
	score_t ** transition_matrix = new score_t * [ num_items ];
	for (i = 0; i < num_items; i++) {
		transition_matrix[i] = new score_t[ num_items ];
		if (!transition_matrix[i]) {
			printf("Could not allocate memory to accommodate the transition matrix.\nInput data is too large");
			exit(-1);
		}
	}

	/// STEP 2: Compute the partial transition matrix
	for (i = 0; i < num_items; i++) {
		p = this->item_list[i];

		sum = 0.0;
		lists_with_both = 0;
//		printf("[ ");

		for (j = 0; j < num_items; j++) {
			q = this->item_list[j];

			wins = 0;

			for (k = 0; k < num_lists; k++) {
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

//				printf("%s vs %s in List %d : (%d - %d)\n", p->get_code(), q->get_code(), k, p_rank, q_rank);
				if (p_rank != NOT_RANKED_ITEM_RANK && q_rank != NOT_RANKED_ITEM_RANK) {
					lists_with_both++;

					if (p_rank < q_rank) {
						wins++;
					}
//					printf(" == Wins: %d\n", wins);
				}
			}

			/// MC1 Transition Matrix [6]
			if (ram == 801) {
				if (wins == num_lists) {
					val = 0.0;
				} else {
					val = 1.0 / (score_t)num_items;
				}
			}

			/// MC2 Transition Matrix [6]
			if (ram == 802) {
				if (wins > ((score_t)num_lists / 2.0)) {
					val = 0.0;
				} else {
					val = 1.0 / (score_t)num_items;
				}
			}

			/// MC3 Transition Matrix [6]
			if (ram == 803) {
				if (lists_with_both == 0) {
					val = 0.0;
				} else {
					val = (score_t)wins / (score_t)(lists_with_both * num_items);
				}
			}

			/// MC4 Transition Matrix [6]
			if (ram == 804) {
				if (lists_with_both == 0) {
					val = 0.5 / (score_t)num_items;
				} else {
					if (wins > ((score_t)num_lists / 2.0)) {
						val = 0.0;
					} else {
						val = 1.0 / (score_t)num_items;
					}
				}
			}

			/// MCT Transition Matrix [9]
			if (ram == 805) {
				if (lists_with_both == 0) {
					val = 0.5 / (score_t)num_items;
				} else {
					val = (lists_with_both - wins) / (score_t)(lists_with_both * num_items);
				}
			}

			/// Set the transition matrix element
			if (i != j) {
				transition_matrix[i][j] = val;
				sum += val;
			}
//			printf("%5.3f  ", transition_matrix[i][j]);
		}

		/// Set the transition matrix element (main diagonal -- see the sum of the probabilities)
		transition_matrix[i][i] = 1.0 - sum;

//		printf("%5.3f  ", transition_matrix[i][i]);
//		printf("]\n");
	}

//		getchar();
	/// STEP 3: Compute the ergodic stationary distribution matrix from the normalized transition
	/// matrix
	for (i = 0; i < num_items; i++) {
//		printf("[ ");

		for (j = 0; j < num_items; j++) {
			transition_matrix[i][j] = transition_matrix[i][j] * (1 - ergodic_number) +
					ergodic_number / (score_t)num_items;

//			printf("%5.4f  ", transition_matrix[i][j]);
		}
//		printf("]\n");
	}

	/// STEP 4: Initialize the state matrix
	score_t * state_matrix = new score_t [ num_items ];
	score_t * next_state_matrix = new score_t [ num_items ];
	bool continue_loop = true;

	for (i = 0; i < num_items; i++) {
		state_matrix[i] = 1.0 / (score_t)num_items;
		next_state_matrix[i] = 0.0;
	}

	/// STEP 5: Compute the state matrix
	iteration = 1;
	while(iteration < prms->get_max_iterations()) {

		this->matrixvec_multiply(next_state_matrix, state_matrix, transition_matrix);

		continue_loop = false;
		for (i = 0; i < num_items; i++) {
			if ( abs(next_state_matrix[i] - state_matrix[i]) > prms->get_convergence_precision() ) {
				continue_loop = true;
				break;
			}
		}

		if (!continue_loop) {
			break;
		}

		for (i = 0; i < num_items; i++) {
			state_matrix[i] = next_state_matrix[i];
			next_state_matrix[i] = 0;
		}

		iteration++;
	}

//	for (i = 0; i < num_items; i++) { printf("%7.6f\n", next_state_matrix[i]); }

	/// Deallocate the used memory
	for (i = 0; i < num_items; i++) {
		delete [] transition_matrix[i];
	}
	delete [] transition_matrix;
	delete [] next_state_matrix;

	return state_matrix;
}


/// Matrix-Vector multiplication : State Matrix DOT Transition Matrix
void MergedList::matrixvec_multiply(score_t * res_mat, score_t * state_mat, score_t ** tran_mat) {

	rank_t i = 0, j = 0, num_items = this->num_nodes;

	for (i = 0; i < num_items; i++) {
		for (j = 0; j < num_items; j++) {
			res_mat[i] += state_mat[j] * tran_mat[j][i];
		}
//		printf("S[%d]=%5.6f\n", i, (res_mat)[i]);
	}
}

