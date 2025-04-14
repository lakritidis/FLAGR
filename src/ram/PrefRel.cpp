/// The Preference Relations method of Desarkar et. al 2016:
/// Desarkar, M. S., Sarkar, S., Mitra, P., "Preference relations based unsupervised rank
/// aggregation for metasearch", Expert Systems with Applications, vol. 49, pp. 86-98, 2016.
/// ///////////////////////////////////////////////////////////////////////////////////////////////

#include "tools/MergedItemPair.cpp"

void MergedList::PrefRel(class InputList ** inlists, class SimpleScoreStats * s, class InputParams * prms) {
	uint32_t i = 0, j = 0, p = 0;
	uint32_t num_pairs = this->num_nodes * (this->num_nodes - 1) / 2;
	score_t DisagreementScore = 0.0, ItemScore = 0.0, w[this->num_input_lists];

	class InputList * inlist = NULL;
	class MergedItemPair * ItemPair = new MergedItemPair();
	char * key = NULL, * prev_key = NULL;

	/// Form all the item pairs and compute a-majority opinions and list disagreement scores
	for (i = 0; i < this->num_nodes - 1; i++) {
		for (j = i + 1; j < this->num_nodes; j++) {
			ItemPair->set_item1( this->item_list[i] );
			ItemPair->set_item2( this->item_list[j] );
			ItemPair->compute_a_majority_opinion(prms->get_alpha(), prms->get_beta(), this->num_input_lists);
		}
	}

	delete ItemPair;

	/// Compute the list weights from their disagreement scores (eq. 5)
//	printf("\n\nNum Nodes: %d, Num Pairs: %d\n", this->num_nodes, num_pairs);
	for (i = 0; i < this->num_input_lists; i++) {
		inlist = this->item_list[0]->get_ranking(i)->get_input_list();
		DisagreementScore = inlist->get_voter()->get_weight();

		w[i] = 1.0 - DisagreementScore / num_pairs;
		inlist->set_voter_weight ( w[i] );

//		printf("List %d disagreement score = %2.1f, weight = %12.10f\n", i, DisagreementScore, w[i]);
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
	ItemScore = 0.0;
	for (i = 0; i < 2 * num_pairs; i++) {
		key = edges[i]->get_item2()->get_code();
		if (i == 0) {
			prev_key = key;
		}

//		edges[i]->display(0);

		if (key != prev_key) {
//			printf("\tItemScore for %s = %12.10f\n", prev_key, ItemScore);
			this->update_weight(prev_key, ItemScore);
			ItemScore = edges[i]->get_score();
			prev_key = key;
		} else {
			ItemScore += edges[i]->get_score();
		}
	}

//	printf("\tItemScore for %s = %12.10f\n", prev_key, ItemScore); getchar();

	this->update_weight(prev_key, ItemScore);

	qsort(this->item_list, this->num_nodes, sizeof(class MergedItem *), &MergedList::cmp_score_desc);

	for (i = 0; i < 2 * num_pairs; i++) {
		delete edges[i];
	}
	delete [] edges;

	/// ///////////////////////////////////////////////////////////////////////////////////////////
	/// Apply the weighted item selection post-processing step
	/// ///////////////////////////////////////////////////////////////////////////////////////////
	if (prms->get_item_selection() == 1) {
		this->perform_pruning(inlists, s, prms);
	}
}
