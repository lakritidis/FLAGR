void MergedList::PrefRel(class InputList ** inlists, class InputParams * params) {
	uint32_t i = 0, j = 0, p = 0;
	uint32_t num_pairs = this->num_nodes * (this->num_nodes - 1) / 2;
	score_t DisagreementScore = 0.0, ListScore = 0.0, ItemScore = 0.0;

	class InputList * inlist = NULL;
	class MergedItemPair * ItemPair = new MergedItemPair();
	char * key = NULL, * prev_key = NULL;

	/// Form all the item pairs and compute a-majority opinions and list disagreement scores
	for (i = 0; i < this->num_nodes - 1; i++) {
		for (j = i + 1; j < this->num_nodes; j++) {
			ItemPair->set_item1( this->item_list[i] );
			ItemPair->set_item2( this->item_list[j] );
			ItemPair->compute_a_majority_opinion(params->get_alpha(), params->get_beta(), this->num_input_lists);
		}
	}

	delete ItemPair;

	/// Compute the list weights from their disagreement scores (eq. 5)
//	printf("\n\nNum Nodes: %d, Num Pairs: %d\n", this->num_nodes, num_pairs);

	for (i = 0; i < this->num_input_lists; i++) {
		inlist = this->item_list[0]->get_ranking(i)->get_input_list();
		DisagreementScore = inlist->get_voter()->get_weight();

		ListScore = 1.0 - DisagreementScore / num_pairs;
		inlist->set_voter_weight ( ListScore );

//		printf("List %d disagreement score = %2.1f, weight = %2.3f\n", i, DisagreementScore, ListScore);
	}

	/// Create the edges of the aggregate graph: 2 edges per MergedItem
	class MergedItemPair ** edges = new MergedItemPair * [2 * num_pairs];
	p = 0;
	for (i = 0; i < this->num_nodes - 1; i++) {
		for (j = i + 1; j < this->num_nodes; j++) {
			edges[p] = new MergedItemPair( this->item_list[i], this->item_list[j] );
			edges[p]->compute_weight();
			p++;

			edges[p] = new MergedItemPair( this->item_list[j], this->item_list[i] );
			edges[p]->compute_weight();
			p++;
		}
	}

	/// Sort the edges by their target (right) node
	qsort(edges, 2 * num_pairs, sizeof(class MergedItemPait *), &MergedList::cmp_edges);

	/// Find the in-degrees of the graph nodes by scanning the edges
	for (i = 0; i < 2 * num_pairs; i++) {
		key = edges[i]->get_item2()->get_code();
		if (i == 0) {
			prev_key = key;
		}

		ItemScore += edges[i]->get_score();

		if (key != prev_key) {
			this->update_weight(key, ItemScore);
			ItemScore = 0.0;
			prev_key = key;
		}

//		edges[i]->display(0);
	}

	this->update_weight(key, ItemScore);
	ItemScore = 0.0;
	prev_key = key;

	qsort(this->item_list, this->num_nodes, sizeof(class MergedItem *), &MergedList::cmp_score_desc);

	for (i = 0; i < 2 * num_pairs; i++) {
		delete edges[i];
	}
	delete [] edges;
}

