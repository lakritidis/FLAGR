class Voter ** MergedList::DIBRA(class InputList ** inlists, class InputParams * params) {

	double prev_weight = 0.0, max_w = 1.0, min_w = 0.0, mean_w = 0.0, sd_w = 1.0, sum_w = 0.0;
//	double l1_norm_w = 0.0, l2_norm_w = 0.0;
	double max_d = 0.0, min_d = 0.0;
	double w[this->num_input_lists], dis[this->num_input_lists];

	uint32_t z = 0, cutoff = 0, max_cutoff = 0;
	int32_t iteration = 0;
	class Voter ** voters_list = new Voter * [this->num_input_lists];

	/// Reset the weights of all voters
	for (z = 0; z < this->num_input_lists; z++) {
		inlists[z]->set_voter_weight( 1.0 );
	}

	uint32_t conv = 1;

	/// Stop the execution when convergence is achieved
	while (1) {
		printf("%d ", iteration); fflush(NULL);

		/// Set the scores of all elements to zero
		this->reset_scores();

		/// Execute the baseline method (by taking into consideration the current voter weights)
		if (params->get_aggregation_method() == 5) {
			this->BordaCount(min_w, max_w, mean_w, sd_w, params);

		} else if (params->get_aggregation_method() == 6) {
			this->CondorcetMethod(min_w, max_w, mean_w, sd_w, params);

		} else if (params->get_aggregation_method() == 7) {
			this->Outranking(min_w, max_w, mean_w, sd_w, params);

		} else if (params->get_aggregation_method() == 8) {
			this->RankPosition(min_w, max_w, mean_w, sd_w, params);
		}

		min_w = 1000.0; max_w = 0.0; min_d = 1000.0; max_d = 0.0, mean_w = 0.0, sd_w = 0.0, sum_w = 0.0;

		/// Compute the similarity of each input list with the produced MergedList
		for (z = 0; z < this->num_input_lists; z++) {
			if (params->get_correlation_method() == 1) {
				dis[z] = this->SpearmanRho(inlists[z]);

			} else if (params->get_correlation_method() == 2) {
				dis[z] = this->ScaledFootruleDistance(z, inlists[z]);

			} else if (params->get_correlation_method() == 3) {
				dis[z] = this->CosineSimilarity(z, inlists[z]);

			} else if (params->get_correlation_method() == 4) {
				dis[z] = this->LocalScaledFootruleDistance(z, inlists[z]);

			} else if (params->get_correlation_method() == 5) {
				dis[z] = this->KendallsTau(z, inlists[z]);
			}

			/// Statistics for normalizing the distances - Generally uneeded - Normalization
			/// takes place within the distance function itself.
			if (dis[z] > max_d) { max_d = dis[z]; }
			if (dis[z] < min_d) { min_d = dis[z]; }
			if (inlists[z]->get_cutoff() > max_cutoff) { max_cutoff = inlists[z]->get_cutoff(); }
		}

		conv = 1;

		/// Based on the computed distances of the previous loop, compute the new weights of voters
		for (z = 0; z < this->num_input_lists; z++) {

			prev_weight = inlists[z]->get_voter()->get_weight();

			/// Compute the new weight of the voter
			if (dis[z] > 0) {
	//			if (z == 1) { w[z] = 1.0; } else { w[z] = 0.0; }
//				w[z] = prev_weight + 1.0 / ( 1.0 + exp( params->get_gamma() * (iteration + 1.00) * dis[z] )); /// Logistic
				w[z] = prev_weight + exp(- params->get_gamma() * (iteration + 1.00) * dis[z] ); /// Exponential
			} else {
				w[z] = prev_weight;
			}

//			printf("Voter %d (%s) distance: %5.3f - weight: %5.3f (prev: %5.3f - diff: %5.3f)\n",
//				z, inlists[z]->get_voter()->get_name(), dis[z], inlists[z]->get_voter()->get_weight(),
//				prev_weight, w[z] - prev_weight);

			/// Statistics for normalizing the weights
			if (w[z] > max_w) { max_w = w[z]; }
			if (w[z] < min_w) { min_w = w[z]; }
			mean_w += w[z];

			if (w[z] - prev_weight > params->get_convergence_precision()) {
				conv = 0;
			}
		}

		sum_w = 0.0;
		mean_w /= this->num_input_lists;
		for (z = 0; z < this->num_input_lists; z++) {
			sum_w += (w[z] - mean_w) * (w[z] - mean_w);
		}
		sd_w = sqrt(sum_w / (double)this->num_input_lists);

		/// Set the new voter weights
		for (z = 0; z < this->num_input_lists; z++) {
//			printf("Voter %d (%s) distance: %5.3f - weight: %5.3f\n", z, inlists[z]->get_voter()->get_name(), dis[z], w[z]);
			inlists[z]->set_voter_weight( w[z] );

			voters_list[z] = inlists[z]->get_voter();
		}
//		getchar();

		qsort(voters_list, this->num_input_lists, sizeof(Voter *), &MergedList::cmp_voter);
		iteration++;

		if (conv == 1 || iteration > params->get_max_iterations()) {
			SUM_ITERATIONS += iteration;
			break;
		}
	}

//	getchar();

	/// ///////////////////////////////////////////////////////////////////////////////////////////
	/// Apply the list pruning post-processing step
	/// ///////////////////////////////////////////////////////////////////////////////////////////
	if (params->get_list_pruning()) {
		for (z = 0; z < this->num_input_lists; z++) {

			double nw = (w[z] - min_w) / (max_w - min_w);

//			cutoff = (double)this->num_nodes / (double)inlists[z]->get_num_items() +
//				nw * this->num_input_lists * log10(10.0 + (double)inlists[z]->get_num_items());

			cutoff = (params->get_delta1() + params->get_delta2() * nw) * inlists[z]->get_num_items();

//			printf("Cutoff for voter %d (%5.3f - %s): %d\n", z, nw, inlists[z]->get_voter()->get_name(), cutoff); getchar();

			if (cutoff >= inlists[z]->get_num_items()) {
				inlists[z]->set_cutoff(inlists[z]->get_num_items());
			} else {
				inlists[z]->set_cutoff(cutoff);
			}
		}

		this->rebuild(inlists);

		if (params->get_aggregation_method() == 5) {
			this->BordaCount(min_w, max_w, mean_w, sd_w, params);

		} else if (params->get_aggregation_method() == 6) {
			this->CondorcetMethod(min_w, max_w, mean_w, sd_w, params);

		} else if (params->get_aggregation_method() == 7) {
			this->Outranking(min_w, max_w, mean_w, sd_w, params);

		} else if (params->get_aggregation_method() == 8) {
			this->RankPosition(min_w, max_w, mean_w, sd_w, params);
		}
	}

	printf(" Nodes: %d", this->num_nodes);
	return voters_list;
}
