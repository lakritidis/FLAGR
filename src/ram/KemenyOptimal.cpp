/// The Brute Force approach for Kemeny Optimal Aggregation. It constructs all possible permutations
/// and returns the one that minimizes (maximizes) the distance (correlation) from (with) all input
/// preference lists.
/// Note: Combinatorial complexity - Do not try that with aggregate lists with more than 10 items!

/* Function to swap values at two pointers */
void swap(class MergedItem **x, class MergedItem **y) {
    class MergedItem * temp;
    temp = *x;
    *x = *y;
    *y = temp;
}

void MergedList::permute(class MergedItem ** best, class InputList ** inlists, rank_t n_items, score_t * max_cor, int l, int r)  {
	uint32_t j = 0;
	rank_t rnk = 0;
	score_t cor = 0.0;

    /// Base case
	if (l == r) {
		for (rnk = 0; rnk < n_items; rnk++) {
			this->item_list[rnk]->set_final_ranking(rnk + 1);
		}

		for (j = 0; j < this->num_input_lists; j++) {
			cor += this->KendallsTau( 0, inlists[j] );
//			printf("Cumulative correlation of this permutation with list %d: %5.3f\n", i, cor);
		}
//		printf("Correlation of this permutation with all lists: %5.3f\n", cor);

		if (cor > *max_cor) {
			*max_cor = cor;
			for (rnk = 0; rnk < n_items; rnk++) {
				best[rnk] = this->item_list[rnk];
			}

//			for (rnk = 0; rnk < n_items; rnk++) {
//				printf("best list item %d: %s\n", rnk, best[rnk]->get_code());
//			}
		}
	} else {
		/// Permutations made
		for (int i = l; i <= r; i++) {

            /// Swapping done
            swap(&(this->item_list[l]), &(this->item_list[i]));

            /// Recursion called
            this->permute(best, inlists, n_items, max_cor, l + 1, r);

            /// backtrack
            swap(&(this->item_list[l]), &(this->item_list[i]));
        }
    }
}

void MergedList::KemenyOptimal(class InputList ** inlists, class SimpleScoreStats * s, class InputParams * prms) {
	rank_t i = 0;
	score_t max_cor = 0;

	class MergedItem ** best_list = new MergedItem * [this->num_nodes];
	for (i = 0; i < this->num_nodes; i++) {
		best_list[i] = this->item_list[i];
	}

	permute(best_list, inlists, this->num_nodes, &max_cor, 0, this->num_nodes - 1);

	for (rank_t rnk = 0; rnk < this->num_nodes; rnk++) {
		this->item_list[rnk] = best_list[rnk];
		this->item_list[rnk]->set_final_ranking(rnk + 1);
		this->item_list[rnk]->set_final_score(this->num_nodes - rnk + 1);
//		printf("best list item %d: %s\n", rnk, best_list[rnk]->get_code());
	}

	delete [] best_list;
}
