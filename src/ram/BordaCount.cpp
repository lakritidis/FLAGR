/// 1. BORDA COUNT: Assign Scores to the items of the MergedList according to the Borda Count method
void MergedList::BordaCount(double min_w, double max_w, double mean_w, double sd_w) {
	class MergedItem * q;
	class Ranking * r;
	class InputList * l;
	double sc = 0.0, weight = 0.0;

//	printf("Weights Max: %5.3f, Min: %5.3f, Mean: %5.3f, SD: %5.3f\n", max_w, min_w, mean_w, sd_w); getchar();

	for (rank_t i = 0; i < this->num_nodes; i++) {
		q = this->item_list[i];

		for (uint32_t j = 0; j < q->get_num_alloc_rankings(); j++) {
			r = q->get_ranking(j);
			l = r->get_input_list();

			if(l && r->get_rank() != NOT_RANKED_ITEM_RANK) {

				#if WEIGHTS_NORMALIZATION == 1
					weight = l->get_voter()->get_weight();
				#elif WEIGHTS_NORMALIZATION == 2
					weight = (l->get_voter()->get_weight() - min_w) / (max_w - min_w);
				#elif WEIGHTS_NORMALIZATION == 3
					weight = l->get_voter()->get_weight() * sd_w * sd_w / mean_w;
				#elif WEIGHTS_NORMALIZATION == 4
					weight = l->get_voter()->get_weight() / max_w;
				#endif

//				printf("Voter weight: Real: %5.3f - Normalized: %5.3f\n", l->get_voter()->get_weight(), weight);

				sc = weight * (l->get_cutoff() - r->get_index());

				q->set_score(q->get_score() + sc);
			}
		}
//		printf("%d : Score: %5.3f\n",i, q->get_score());
	}

	qsort(this->item_list, this->num_nodes, sizeof(class MergedItem *), &MergedList::cmp_score_desc);

	for (rank_t i = 0; i < this->num_nodes; i++) {
		this->item_list[i]->set_final_ranking(i + 1);
	}
//	this->display_list();
//	getchar();
}

