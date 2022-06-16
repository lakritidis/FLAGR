/// MC4: The fourth Markov Chains method of [6]

void MergedList::MC4(class InputList ** inlists, class SimpleScoreStats * s, class InputParams * prms) {

	rank_t i = 0;

	qsort(this->item_list, this->num_nodes, sizeof(class MergedItem *), &MergedList::cmp_code_asc);

	score_t * state_matrix = this->compute_state_matrix(s, prms);

	for (i = 0; i < this->num_nodes; i++) {
		this->item_list[i]->set_final_score( state_matrix[i] );
	}

	delete [] state_matrix;

	qsort(this->item_list, this->num_nodes, sizeof(class MergedItem *), &MergedList::cmp_score_desc);

//	this->display_list();
}


/// Compute the ergodic transition matrix of the ranking
score_t * MergedList::compute_state_matrix(class SimpleScoreStats * s, class InputParams * prms) {
	class MergedItem *p, *q;

	rank_t p_rank = 0, q_rank = 0, i = 0, j = 0, l = 0, num_items = this->num_nodes;
	score_t result = 0.0, val = 0.0, sum1 = 0.0, sum2 = 0.0, voter_weight = 1.0;
	score_t ergodic_number = prms->get_alpha();

	uint32_t weights_norm = prms->get_weights_normalization();
	uint32_t ram = prms->get_aggregation_method();
	uint32_t num_lists = this->num_input_lists;
	uint32_t k = 0;
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

//		printf("[ ");

		for (j = 0; j < num_items; j++) {
			q = this->item_list[j];

			result = 0.0;

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

//				printf("(%s -> %s) : %d - %d\n", p->get_code(), q->get_code(), p_rank, q_rank);
				if (p_rank < q_rank) {
					result++;
				}
			}

			if (result == 0.0 && i == j) {
				val = -1.0;
			} else if (result > ((score_t)num_lists / 2.0)) {
				val = 0.0;

			} else {
				if (ram == 804) {
					val = 1.0;
				} else {
					val = (num_lists - result) / (score_t)num_lists;
				}
			}
			transition_matrix[i][j] = val;
//			printf("%1.0f  ", transition_matrix[i][j]);
		}
//		printf("]\n");
	}


	/// STEP 3A: Normalize the partial transition matrix: Phase 1: all elements
//	printf("\n\n==== NORMALIZED TRANSITION MATRIX:\n");

	for (i = 0; i < num_items; i++) {
		for (j = 0; j < num_items; j++) {
			transition_matrix[i][j] /= (score_t)num_items;
		}
	}

	/// STEP 3B: Normalize the partial transition matrix: Phase 2: main diagonal elements
	for (i = 0; i < num_items; i++) {
//		printf("[ ");

		for (j = 0; j < num_items; j++) {

			if (i == j) {
				sum1 = 0.0;
				sum2 = 0.0;

				for (l = i + 1; l < num_items; l++) {
					sum1 += transition_matrix[i][l];
				}

				for (l = 0; l < j; l++) {
					sum2 += transition_matrix[i][l];
				}

//				printf("\ni: %d, j: %d, sum1: %4.3f, sum2: %4.3f\n", i, j, sum1, sum2);
				transition_matrix[i][j] = 1.0 - sum1 - sum2;
			}

//			printf("%4.3f  ", transition_matrix[i][j]);
		}
//		printf("]\n");
	}

	/// STEP 4: Compute the ergodic transition matrix from the normalized transition matrix
	for (i = 0; i < num_items; i++) {
//		printf("[ ");

		for (j = 0; j < num_items; j++) {
			transition_matrix[i][j] = transition_matrix[i][j] * (1 - ergodic_number) +
					ergodic_number / (score_t)num_items;

//			printf("%5.4f  ", transition_matrix[i][j]);
		}
//		printf("]\n");
	}


	/// STEP 5: Initialize the state matrix
	score_t * state_matrix = new score_t [ num_items ];
	score_t * next_state_matrix = new score_t [ num_items ];
	bool continue_loop = true;

	for (i = 0; i < num_items; i++) {
		state_matrix[i] = 1.0 / (score_t)num_items;
		next_state_matrix[i] = 0.0;
	}

	/// STEP 6: Compute the state matrix
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

