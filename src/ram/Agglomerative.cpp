/// The Agglomerative Aggregation Algorithm of Chatterjee et. al, 2018. Published in:
/// Chatterjee, S., Mukhopadhyay, A., Bhattacharyya, M., "A weighted rank aggregation approach
/// towards crowd opinion analysis", Knowledge-Based Systems, vol. 149, pp. 47-60, 2018.
/// ///////////////////////////////////////////////////////////////////////////////////////////////

/// An assistant structure that stores a MergedList, and a similarity/correlation value.
/// During the Agglomerative Aggregation, we maintain one such record for each input list.
struct max_similarity {
	score_t sim;
	int32_t merge_with;
};

/// Insert/Copy an item from a MergedList to another MergedList: used in the Agglomerative
/// Aggregation algorithm of [4].
void MergedList::insert_merge(class MergedItem * item, score_t list_weight) {
	class MergedItem * q = NULL;
	class Ranking * rnk = NULL;
	score_t new_score = 0.0;

	uint32_t HashValue = this->djb2(item->get_code()) & this->mask;

	/// Search in the hash table to check whether this item exists or not
	if (this->hash_table[HashValue] != NULL) {

		/// Traverse the linked list that represents the chain
		for (q = this->hash_table[HashValue]; q != NULL; q = q->get_next()) {

			/// The item exists in this MergedList. Insert the new ranking and update its score
			if (strcmp(q->get_code(), item->get_code()) == 0) {
//				printf("\t\tThe item was found!\n");

				for (rank_t itemr = 0; itemr < item->get_num_alloc_rankings(); itemr++) {
					rnk = item->get_ranking(itemr);

					if (rnk->get_rank() != NOT_RANKED_ITEM_RANK) {
//						printf("\t\t\tThe item was ranked by list %d at rank %d\n", rnk->get_input_list()->get_id(), rnk->get_rank());
						q->insert_ranking(rnk->get_input_list(), rnk->get_rank(), rnk->get_score());
//						printf("\t\t\tNew list node is\n"); q->display();
					}
				}

				/// Chatterjee et al. 2018, Eq. 8
				new_score = ( this->weight * q->get_final_score() + list_weight * item->get_final_score() ) /
						( this->weight + list_weight );
/*
				printf("New Score: %5.3f --- (Old Score in List 1: %5.3f, List 1 weight: %5.3f,\
						Old Score in List 2: %5.3f, List 2 weight: %5.3f\n",
						new_score, q->get_final_score(), this->weight, item->get_final_score(), list_weight);
*/
				q->set_final_score( new_score );

				return;
			}
		}
	}

	/// The item was not found in this MergedList: create a new item record
//	printf("\t\tThe item was NOT found, creating a new node...\n");
	this->num_nodes++;

	class MergedItem * record = new MergedItem(item);

	/// Reassign the chain's head
	record->set_next(this->hash_table[HashValue]);
	this->hash_table[HashValue] = record;
}


/// Merge two temporary aggregate lists: used in the Agglomerative Aggregation algorithm [4]
void MergedList::merge_with(class MergedList * inlist, class InputParams * params) {
	rank_t r = 0;
	score_t c1 = params->get_c1(), c2 = params->get_c2();

	/// We delete the array of items of this MergedList (we only delete the array, NOT the hast table!)
	if (this->item_list) {
		delete [] this->item_list;
	}

	/// Traverse the incoming MergedList inlist: Insert each element of inlist into this MergedList
	for (r = 0; r < inlist->get_num_items(); r++) {
//		printf("\tSearching for the item %d (%s) of the second list in the first list...\n", r, in->get_item(r)->get_code());
		this->insert_merge( inlist->get_item(r), inlist->get_weight() );
//		getchar();
	}

//	printf("Old weight 1: %5.3f - Old weight 2: %5.3f", this->weight, inlist->get_weight());

	/// Compute the weight of the updated MergedList from its parents (Ref [4], Eq. 9).
	this->weight = (c1 * this->weight + c2 * inlist->get_weight()) / (c1 + c2);

//	printf("New weight: %5.3f\n", this->weight); getchar();

	/// Convert the hash table to an array, so that we can sort by score
	this->convert_to_array();
	qsort(this->item_list, this->num_nodes, sizeof(class MergedItem *), &MergedList::cmp_score_desc);
}


/// Compute the Spearman's rho correlation measure between this MergedList and another
double MergedList::SpearmanRho(class MergedList * inlist) {
	class MergedItem * q, * p;
	double rho = 0.0, sum = 0.0;
	rank_t n1 = this->num_nodes, n2 = inlist->get_num_items();

	double denom = pow(n1, 3) - n1;

	for (rank_t i = 0; i < n1; i++) {
		q = this->item_list[i];

		for (rank_t j = 0; j < n2; j++) {
			p = inlist->get_item(j);
//			printf("Comparing (%s, %d) with (%s, %d)\n", q->get_code(), i, p->get_code(), j);

			if (strcmp(q->get_code(), p->get_code()) == 0) {
				sum += (i - j) * (i - j);
				break;
			}
		}
	}

	rho = 1.0 - 6.0 * sum / denom;
//	printf("(%d-%d) - rho=%5.3f - sum:%5.3f\n", this->num_nodes, in->get_num_items(), rho, sum);

	return rho;
}


/// Determine the most similar MergedList of each MergedList. Instead on using a M x M matrix for
/// all similarities, it suffices to use a M x 1 array which stores only the max similarity values.
struct max_similarity * compute_similarities(struct max_similarity * max_similarities,
	class MergedList ** templist, uint32_t nlists) {

		uint32_t m = 0, n = 0;
		score_t sim = 0.0, max_sim = -2.0;

		/// Compute all the pairwise list similarities, but store only the largest one for each list
		for (m = 0; m < nlists; m++) {
			max_similarities[m].sim = -2.0;
			max_similarities[m].merge_with = -1;
		}

		for (m = 0; m < nlists; m++) {
			max_sim = -2.0;
			for (n = 0; n < nlists; n++) {
				if (m != n && templist[m] && templist[n]) {
					sim = templist[m]->SpearmanRho(templist[n]);

	//				printf("\nRho correlation between %d and %d = %5.3f\n", m, n, sim);
					if (sim > max_sim) {
						max_sim = sim;
						max_similarities[m].sim = max_sim;
						max_similarities[m].merge_with = n;
					}
				}
			}
		}
		return max_similarities;
}

/// Compute the initial weights based on the majority votes
void MergedList::compute_initial_weights(class MergedList ** list_array, class InputList ** input_lists) {
	rank_t k = 0, p_rank = 0, q_rank = 0, i = 0, j = 0;
	uint32_t nlists = this->num_input_lists, l = 0;
	score_t wins = 0, losses = 0;

	class Voter * v = NULL;
	class MergedItem *p = NULL, *q = NULL;
	class MergedList * temp_agglist = new MergedList(1024, nlists);

	for (l = 0; l < nlists; l++) {
		for (k = 0; k < input_lists[l]->get_num_items(); k++) {
			temp_agglist->insert(input_lists[l]->get_item(k), l, input_lists);
		}
	}
	temp_agglist->convert_to_array();

	for (i = 0; i < temp_agglist->get_num_items(); i++) {
		p = temp_agglist->get_item(i);

		for (j = i + 1; j < temp_agglist->get_num_items(); j++) {
			q = temp_agglist->get_item(j);

			wins = 0; losses = 0;

			for (k = 0; k < nlists; k++) {
				p_rank = p->get_ranking(k)->get_rank();
				q_rank = q->get_ranking(k)->get_rank();

				if (p_rank < q_rank) {
					wins++;
				} else if (p_rank > q_rank) {
					losses++;
				}

//				printf("\nLIST %d: Item %d (%s) rank %d (VS) item %d (%s) rank %d\n",
//					k, i, p->get_code(), p_rank, j, q->get_code(), q_rank);
			}

			/// If this item wins its opponent, then the voters who agree with this opinion increase
			/// their score by 1.
//			printf("Item %d (%s) -VS- item %d (%s) -- W: %5.3f - L: %5.3f\n", i, p->get_code(),
//				j, q->get_code(), wins, losses);

			for (k = 0; k < nlists; k++) {
				p_rank = p->get_ranking(k)->get_rank();
				q_rank = q->get_ranking(k)->get_rank();

				if (wins > losses && p_rank < q_rank) {
					v = p->get_ranking(k)->get_input_list()->get_voter();
					v->set_weight(v->get_weight() + 1);
//					printf("%s (LIST %d) Agrees: - New Weight: %5.3f\n", v->get_name(), k, v->get_weight());
				} else if (wins < losses && p_rank > q_rank) {
					v = q->get_ranking(k)->get_input_list()->get_voter();
					v->set_weight(v->get_weight() + 1);
//					printf("%s (LIST %d) Agrees: - New Weight: %5.3f\n", v->get_name(), k, v->get_weight());
				}
			}
//			getchar();
		}
	}

	delete temp_agglist;
}


class MergedList * MergedList::Agglomerative(class InputList ** inlists, class SimpleScoreStats * s, class InputParams * prms) {
	uint32_t m = 0, next = 0;
	score_t max_sim = -2.0;
	uint32_t nlists = this->num_input_lists, remaining_lists = this->num_input_lists, msl = 0;

	/// Which list is the most similar to another one? One record per each input list
	struct max_similarity * max_similarities = new max_similarity [nlists];

	/// Create one temporary aggregate per input list - this will help us in the progressive list merging
	class MergedList ** templist = new MergedList * [nlists];

	for (m = 0; m < nlists; m++) {
		templist[m] = new MergedList(inlists, nlists, m);
//		templist[m]->display(); getchar();
	}

	this->compute_initial_weights(templist, inlists);

	/// Compute the initial list similarities/correlations between the lists
	max_similarities = compute_similarities(max_similarities, templist, nlists);

	/// Start merging the lists
	while(remaining_lists > 1) {

		/// Find the pair of the most similar lists
		max_sim = -2.0;
		for (m = 0; m < nlists; m++) {
//			printf("Most similar list to %d is list %d: %5.3f\n", m, max_similarities[m].merge_with, max_similarities[m].sim);

			if (max_similarities[m].sim > max_sim) {
				max_sim = max_similarities[m].sim;
				next = m;
			}
		}

		/// Most similar list
		msl = max_similarities[next].merge_with;
//		printf("Next merge: list %d with list %d\n", next, msl );

		templist[next]->merge_with( templist[ msl ], prms );

		/// The list that was merged with is deleted (as it is no longer useful)
		delete templist[ msl ];
		templist[ msl ] = NULL;

		/// The list similarities are recomputed for the next iteration
		max_similarities = compute_similarities(max_similarities, templist, nlists);

		/// Update the max similarities list
		remaining_lists--;

//		printf("Merge completed, Remaining lists = %d\n\n", remaining_lists );
//		templist[next]->display();
	}

	/// Deallocate all the remaining resources
//	delete templist[next];
	class MergedList * ret = templist[next];
	delete [] templist;
	delete [] max_similarities;

	return ret;
}
