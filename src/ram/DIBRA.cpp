/// The Iterative Distance-Based Unsupervised Algorithm (DIBRA) of Akritidis et. al, 2022. Published
/// in:
/// Akritidis L., Fevgas A., Bozanis P., Manolopoulos Y., "An Unsupervised Distance-Based Model for
/// Weighted Rank Aggregation with List Pruning", Expert Systems with Applications, vol. 202,
/// pp. 117435, 2022.
/// ///////////////////////////////////////////////////////////////////////////////////////////////

class Voter ** MergedList::DIBRA(class InputList ** inlists, class SimpleScoreStats * s, class InputParams * prms) {

	double prev_weight = 0.0;
	score_t min_d = 1000.0, max_d = 0.0, sum_w = 0.0, mean_w = 0.0;
	score_t w[this->num_input_lists], dis[this->num_input_lists];

	uint32_t z = 0, max_cutoff = 0;
	uint32_t ram = prms->get_aggregation_method();
	uint32_t corel = prms->get_correlation_method();
	int32_t iteration = 0;
	class Voter ** voters_list = new Voter * [this->num_input_lists];

	s->reset();

	/// Reset the weights of all voters
	for (z = 0; z < this->num_input_lists; z++) {
		inlists[z]->set_voter_weight( 1.0 );
	}

	uint32_t conv = 1;

	/// Stop the execution when convergence is achieved
	while (1) {
		/// Set the scores of all elements to zero
		this->reset_item_scores();

		/// Execute the baseline method (by taking into consideration the current voter weights)
		if (ram >= 5100 && ram <= 5109) {
			this->CombSUM(inlists, s, prms);
		} else if (ram >= 5110 && ram <= 5119) {
			this->CombMNZ(inlists, s, prms);
		} else if (ram == 5200) {
			this->CondorcetWinners(inlists, s, prms);
		} else if (ram == 5201) {
			this->CopelandWinners(inlists, s, prms);
		} else if (ram == 5300) {
			this->Outranking(inlists, s, prms);
		}

		/// Initialize the statistics for the weights and the distances
		s->set_min_val(1000.0); s->set_max_val(0.0); s->set_mean_val(0.0); s->set_std_val(0.0);
		min_d = 1000.0; max_d = 0.0; sum_w = 0.0;

		/// Compute the similarity of each input list with the produced MergedList
		for (z = 0; z < this->num_input_lists; z++) {
			if (corel == 1) {
				dis[z] = this->SpearmanRho(inlists[z]);

			} else if (corel == 2) {
				dis[z] = this->ScaledFootruleDistance(z, inlists[z]);

			} else if (corel == 3) {
				dis[z] = this->CosineSimilarity(z, inlists[z]);

			} else if (corel == 4) {
				dis[z] = this->LocalScaledFootruleDistance(z, inlists[z]);

			} else if (corel == 5) {
				dis[z] = this->KendallsTau(z, inlists[z]);
			}

			/// Statistics for normalizing the distances - Generally unneeded, because normalization
			/// takes place within the distance function itself.
			if (dis[z] > max_d) {
				max_d = dis[z];
			}

			if (dis[z] < min_d) {
				min_d = dis[z];
			}

			if (inlists[z]->get_cutoff() > max_cutoff) {
				max_cutoff = inlists[z]->get_cutoff();
			}
		}

		conv = 1;

		/// Based on the computed distances of the previous loop, compute the new weights of voters
		for (z = 0; z < this->num_input_lists; z++) {

			prev_weight = inlists[z]->get_voter()->get_weight();

			/// Compute the new weight of the voter
			if (dis[z] > 0) {
//				if (z == 1) { w[z] = 1.0; } else { w[z] = 0.0; }
//				w[z] = prev_weight + 1.0 / ( 1.0 + exp( params->get_gamma() * (iteration + 1.00) * dis[z] )); /// Logistic
				w[z] = prev_weight + exp(- prms->get_gamma() * ( iteration + 1.00) * dis[z] ); /// Exponential
//				w[z] = prev_weight + (score_t)1.0 / (params->get_gamma() * (iteration + 1.0) * (iteration + 1.0) * tanh( dis[z] )); /// tanh
			} else {
				w[z] = prev_weight;
			}

//			printf("Iteration: %d. Voter %d (%s) distance: %5.3f - weight: %5.3f (prev: %5.3f - diff: %5.3f) , %5.3f\n",
//				iteration, z, inlists[z]->get_voter()->get_name(), dis[z], w[z],
//				prev_weight, w[z] - prev_weight, tanh( dis[z] ));

			/// Statistics for normalizing the weights
			if (w[z] > s->get_max_val()) {
				s->set_max_val(w[z]);
			}

			if (w[z] < s->get_min_val()) {
				s->set_min_val(w[z]);
			}

			sum_w += w[z];

			if (w[z] - prev_weight > prms->get_convergence_precision()) {
				conv = 0;
			}
		}

		mean_w = sum_w / (score_t) this->num_input_lists;
		s->set_mean_val(mean_w);
		sum_w = 0.0;
		for (z = 0; z < this->num_input_lists; z++) {
			sum_w += (w[z] - mean_w) * (w[z] - mean_w);
		}
		s->set_std_val( sqrt(sum_w / (double)this->num_input_lists) );

		/// Set the new voter weights
		for (z = 0; z < this->num_input_lists; z++) {
			// printf("Voter %d (%s) distance: %5.3f - weight: %5.3f\n", z, inlists[z]->get_voter()->get_name(), dis[z], w[z]); getchar();
			inlists[z]->set_voter_weight( w[z] );

			voters_list[z] = inlists[z]->get_voter();
		}
//		getchar();

		qsort(voters_list, this->num_input_lists, sizeof(Voter *), &MergedList::cmp_voter);
		iteration++;

		if (conv == 1 || iteration > prms->get_max_iterations()) {
			break;
		}
	}


	//printf("\n");
	//for (z = 0; z < this->num_input_lists; z++) {
	//	printf("%s|%5.3f\n", inlists[z]->get_voter()->get_name(), inlists[z]->get_voter()->get_weight());
	//}
	//getchar();


	/// ///////////////////////////////////////////////////////////////////////////////////////////
	/// Apply the weighted item selection post-processing step
	/// ///////////////////////////////////////////////////////////////////////////////////////////
	if (prms->get_item_selection() == 1) {
		this->perform_pruning(inlists, s, prms);
	} else if (prms->get_item_selection() == 2) {
		this->perform_item_selection(inlists, s, prms);
	}

//	printf(" Nodes: %d", this->num_nodes);
	return voters_list;
}
