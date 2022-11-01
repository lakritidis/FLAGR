/// Custom methods implementation
/// Must be declared as public members of MergedList in MergedList.h
/// ///////////////////////////////////////////////////////////////////////////////////////////////

/// Custom rank aggregation method
void MergedList::CustomMethod1(class InputList ** inlists, class SimpleScoreStats * s, class InputParams * prms) {
	/// Your implementation here
	class MergedItem * q;
	class Ranking * r;

	/// Example iteration through list elements
	for (rank_t i = 0; i < this->num_nodes; i++) {
		q = this->item_list[i];

		/// Get the individual rankings and scores of q in the input lists
		for (uint32_t j = 0; j < q->get_num_alloc_rankings(); j++) {
			r = q->get_ranking(j);
			r->display();
		}
	}

	/// Sort the list elements in decreasing score order
	qsort(this->item_list, this->num_nodes, sizeof(class MergedItem *), &MergedList::cmp_score_desc);

	/// or you may want to sort the scores in increasing order
	/// qsort(this->item_list, this->num_nodes, sizeof(class MergedItem *), &MergedList::cmp_score_asc);
}


/// Another custom rank aggregation method
void MergedList::CustomMethod2(class InputList ** inlists, class SimpleScoreStats * s, class InputParams * prms) {
	/// Your implementation here

	/// Sort the list elements in decreasing score order
	qsort(this->item_list, this->num_nodes, sizeof(class MergedItem *), &MergedList::cmp_score_desc);

	/// or you may want to sort the scores in increasing order
	/// qsort(this->item_list, this->num_nodes, sizeof(class MergedItem *), &MergedList::cmp_score_asc);
}
