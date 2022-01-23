/// 4. RANK POSITION: Assign Scores to the items of the MergedList according to the Rank Position method.
void MergedList::RankPosition(double min_w, double max_w, double mean_w, double sd_w, class InputParams * params) {
	class MergedItem * q = NULL;
	class Ranking * r = NULL;
	class InputList * l = NULL;

	double sc = 0.0, weight = 1.0, temp_score = 0.0;

	for (rank_t i = 0; i < this->num_nodes; i++) {
		q = this->item_list[i];
		temp_score = 0.0;

		for (uint32_t j = 0; j < q->get_num_alloc_rankings(); j++) {
			r = q->get_ranking(j);
			l = r->get_input_list();

			if(l && r->get_rank() != NOT_RANKED_ITEM_RANK) {
				weight = l->get_voter()->get_weight();

				if (params->get_weights_normalization() == 2) {
					weight = (weight - min_w) / (max_w - min_w);   /// Min-max normalization

				} else if (params->get_weights_normalization() == 3) {
					weight = weight * sd_w * sd_w / mean_w;   /// Standardization = (w * std^2/mean)

				} else if (params->get_weights_normalization() == 4) {
					weight = weight / max_w;  /// Divide by max
				}

				if (weight > 0) {
					sc = weight * (l->get_max_rank() - r->get_rank());
					temp_score += 1.0 / sc;
				}
			}
		}

		q->set_score(q->get_score() + 1.0 / temp_score);
	}

	qsort(this->item_list, this->num_nodes, sizeof(class MergedItem *), &MergedList::cmp_score_asc);
}


