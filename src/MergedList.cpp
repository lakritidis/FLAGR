#include "MergedList.h"

#include "ram/CombSUM.cpp"
#include "ram/CombMNZ.cpp"
#include "ram/CondorcetMethod.cpp"
#include "ram/OutrankingApproach.cpp"
#include "ram/DIBRA.cpp"
#include "ram/PrefRel.cpp"
#include "ram/Agglomerative.cpp"
#include "ram/MC123.cpp"
#include "ram/MC4.cpp"


/// Constructor 1: default
MergedList::MergedList() :
		num_input_lists(0),
		hash_table(NULL),
		item_list(NULL),
		mask(0),
		num_slots(0),
		num_nodes(0),
		num_chains(0),
		weight(0.0),
		log10s{0.0} { }

/// Constructor 2: overloaded
MergedList::MergedList(uint32_t size, uint32_t n) :
		num_input_lists(n),
		hash_table(new MergedItem * [size]),
		item_list(NULL),
		mask(size - 1),
		num_slots(size),
		num_nodes(0),
		num_chains(0),
		weight(0.0),
		log10s{0.0} {

			for (uint32_t i = 0; i < size; i++) {
				this->hash_table[i] = NULL;
			}

			this->log10s[0] = 0;
			for (uint32_t i = 1; i < 100000; i++) {
				this->log10s[i] = log10( (double)i );
			}
}

/// Constructor 3: overloaded - Create an aggregate list from an input list.
/// Used in Agglomerative Aggregation algorithm of [4]
MergedList::MergedList(class InputList ** inlists, uint32_t nlists, uint32_t m) {
	uint32_t size = 1024;
	class InputItem * elem = NULL;
	rank_t nitems = inlists[m]->get_num_items();

	score_t score = 0;

	this->weight = 1.0;   /// How are the list weights initialized?
	this->num_input_lists = nlists;
	this->num_nodes = 0;
	this->num_chains = 0;
	this->num_slots = size;
	this->mask = size - 1;
	this->item_list = NULL;

	this->hash_table = new MergedItem * [size];

	for (uint32_t i = 0; i < size; i++) {
		this->hash_table[i] = NULL;
	}

//	printf("\ncreating list %d...\n", m);fflush((NULL));
	/// Create a tem
	for (rank_t i = 0; i < inlists[m]->get_num_items(); i++) {
		elem = inlists[m]->get_item(i);

		/// Chatterjee et al. 2018, Eq. 7
		score = 0.5 * (int32_t)(nitems * nitems - 2 * nitems * (elem->get_rank() - 1) - nitems);

//		printf("\tinserting item %d : %s (rank %d, score %+5.2f)...\n", i, elem->get_code(),
//				elem->get_rank(), score); fflush((NULL));

		this->insert(elem, m, inlists);
		this->update_weight(elem->get_code(), score);
	}

	this->log10s[0] = 0;
	for (uint32_t i = 1; i < 100000; i++) {
		this->log10s[i] = log10( (double)i );
	}

	this->convert_to_array();
	qsort(this->item_list, this->num_nodes, sizeof(class MergedItem *), &MergedList::cmp_score_desc);
}

/// Destructor
MergedList::~MergedList() {
	this->clear_contents();
}

/// Insert an element into the hash table
void MergedList::insert(class InputItem * n, uint32_t x, class InputList ** l) {
	/// Find the hash value of the input term
	uint32_t HashValue = this->djb2(n->get_code()) & this->mask;

	/// Now search in the hash table to check whether this term exists or not
	if (this->hash_table[HashValue] != NULL) {
		class MergedItem * q;

		/// Traverse the linked list that represents the chain.
		for (q = this->hash_table[HashValue]; q != NULL; q = q->get_next()) {
			if (strcmp(q->get_code(), n->get_code()) == 0) {

				q->insert_ranking( l[x], n->get_rank(), n->get_inscore() );

				return; /// Return and exit
			}
		}
	} else {
		this->num_chains++;
	}

	this->num_nodes++;

	/// Create a new record and re-assign the linked list's head
	class MergedItem * record = new MergedItem(n->get_code(), n->get_rank(), this->num_input_lists, l);
	record->insert_ranking( l[x], n->get_rank(), n->get_inscore() );

	/// Reassign the chain's head
	record->set_next(this->hash_table[HashValue]);
	this->hash_table[HashValue] = record;
}

/// Find an element into the hash table and update its weight
void MergedList::update_weight(char * code, score_t w) {
	/// Find the hash value of the input term
	uint32_t HashValue = this->djb2(code) & this->mask;

	/// Now search in the hash table to check whether this term exists or not
	if (this->hash_table[HashValue] != NULL) {
		class MergedItem * q;

		/// Traverse the linked list that represents the chain.
		for (q = this->hash_table[HashValue]; q != NULL; q = q->get_next()) {
			if (strcmp(q->get_code(), code) == 0) {

				q->set_final_score( q->get_final_score() + w );

				return; /// Return and exit
			}
		}
	}
}


/// Display the items of the MergedList object (hash_table)
void MergedList::display() {
	class MergedItem * q;
	for (uint32_t i = 0; i < this->num_slots; i++) {
		if (this->hash_table[i] != NULL) {
			for (q = this->hash_table[i]; q != NULL; q = q->get_next()) {
				q->display();
//				getchar();
			}
		}
	}
}


/// Display the items of the MergedList object (item_list)
void MergedList::display_list() {
	for (rank_t i = 0; i < this->num_nodes; i++) {
		this->item_list[i]->display();
//		getchar();
	}
}


/// Display the items of the MergedList object (item_list)
void MergedList::write_to_CSV(char * topic, class InputParams * params) {
	FILE * fp = fopen(params->get_output_file(), "a+");
	if (fp) {
		for (rank_t i = 0; i < this->num_nodes; i++) {
			fprintf(fp, "%s,PyFLAGR,%s,%10.6f\n", topic, this->item_list[i]->get_code(),
					this->item_list[i]->get_final_score());
		}
		fclose(fp);
	}
}


/// "Copy" the MergedList's hash table elementd to the internal item_list
void MergedList::convert_to_array() {
	rank_t x = 0;

	this->item_list = new MergedItem * [this->num_nodes];

	class MergedItem * q;
	for (uint32_t i = 0; i < this->num_slots; i++) {
		if (this->hash_table[i] != NULL) {
			for (q = this->hash_table[i]; q != NULL; q = q->get_next()) {
				this->item_list[x++] = q;
			}
		}
	}
}

/// Reset the scores (set to equal to 0) of the merged list elements
void MergedList::reset_scores() {
	for (rank_t i = 0; i < this->num_nodes; i++) {
		this->item_list[i]->set_final_score(0.0);
	}
}

/// Search for an item and return its rank
rank_t MergedList::get_item_rank(char *c) {
	uint32_t HashValue = this->djb2(c) & this->mask;

	if (this->hash_table[HashValue] != NULL) {
		class MergedItem * q;

		for (q = this->hash_table[HashValue]; q != NULL; q = q->get_next()) {
			if (strcmp(q->get_code(), c) == 0) {
				return q->get_final_ranking();
			}
		}
	}
	return NOT_RANKED_ITEM_RANK;
}

/// Clear all the contents of the Merged List and free all resources - Equivalent to destructor
void MergedList::clear_contents() {
	class MergedItem * q;

	if (this->hash_table) {
		for (uint32_t i = 0; i < this->num_slots; i++) {
			while (this->hash_table[i] != NULL) {
				q = this->hash_table[i]->get_next();
				delete this->hash_table[i];
				this->hash_table[i] = q;
			}
		}

		delete [] this->hash_table;
		this->hash_table = NULL;
	}

	if (this->item_list) {
		delete [] this->item_list;
		this->item_list = NULL;
	}

	this->num_nodes = 0;
	this->num_chains = 0;
}

/// Rebuild the Merged list from the input lists
void MergedList::rebuild(class InputList ** inlists) {
	uint32_t k = 0, l = 0;

	this->clear_contents();
	this->hash_table = new MergedItem * [this->num_slots];

	for (uint32_t i = 0; i < this->num_slots; i++) {
		this->hash_table[i] = NULL;
	}

	this->num_nodes = 0;
	this->num_chains = 0;
	this->item_list = NULL;

	for (l = 0; l < this->num_input_lists; l++) {
		for (k = 0; k < inlists[l]->get_cutoff(); k++) {
			this->insert(inlists[l]->get_item(k), l, inlists);
		}
	}

	this->convert_to_array();
	this->reset_scores();
}


/// ///////////////////////////////////////////////////////////////////////////////////////////////
/// RANK CORRELATION DISTANCE METHODS /////////////////////////////////////////////////////////////
/// Between the MergedList and an Input List///////////////////////////////////////////////////////
/// ///////////////////////////////////////////////////////////////////////////////////////////////
double MergedList::SpearmanRho(class InputList * in) {
	class MergedItem * q;
	double rho = 0.0, sum = 0.0;
	rank_t n = this->num_nodes;

	double denom = pow(n, 3) - n;

	for (rank_t i = 0; i < n; i++) {
		q = this->item_list[i];

		for (uint32_t j = 0; j < this->num_input_lists; j++) {
			if(q->get_ranking(j)->get_input_list() == in && q->get_ranking(j)->get_rank() != NOT_RANKED_ITEM_RANK) {
				sum += pow(q->get_ranking(j)->get_rank() - (i + 1.0), 2.0);
//				printf("Item %d of Merged List, Score: %5.3f was ranked in place %d in list %d - Sum=%5.3f\n",
//					i, q->get_score(), q->get_ranking(j)->Rank, j, sum);
			}
		}
	}

	rho = 1.0 - 6.0 * sum / denom;
//	printf("(%d-%d) - rho=%5.3f - sum:%5.3f\n", this->num_nodes, in->get_num_items(), rho, sum);
	return rho;
}

double MergedList::KendallsTau(uint32_t z, class InputList * in) {
	rank_t a = 0, b = 0, n = in->get_cutoff(), concordant = 0, discordant = 0;
	double tau = 0.0, denom = n * (n - 1.0) / 2.0;

	for (rank_t i = 0; i < n; i++) {
		a = this->get_item_rank( in->get_item(i)->get_code() );

		for (rank_t j = i + 1; j < n; j++) {
			b = this->get_item_rank( in->get_item(j)->get_code() );
			printf("Comparing pair (%d,%d) -> (%d,%d)", i, j, a, b);
			if (a < b) { concordant++; } else { discordant++; }
		}
	}

	tau = (double)concordant / denom;
//	printf("(%d-%d) - rho=%5.3f - sum:%5.3f\n", this->num_nodes, in->get_num_items(), rho, sum);
	return tau;
}

double MergedList::CosineSimilarity(uint32_t z, class InputList * in) {
	double l_score = 0.0, r_score = 0.0, c_score = 0.0, csim = 0.0;
	rank_t R = in->get_cutoff(), L = this->num_nodes, l = 0, r = 0;

	uint32_t scenario = 2;

	for (r = 0; r < R; r++) {
		if (scenario == 1) { r_score += 1.0 / ( (r + 1.0) * (r + 1.0) ); } else
		if (scenario == 2) { r_score += (R - r + 1.0) * (R - r + 1.0); } else /// 2
		if (scenario == 3) { r_score += (double)(R * R) / (double)( (r + R) * (r + R) ); } else
		if (scenario == 4) { r_score += (r + 1.0) * (r + 1.0); } else
		if (scenario == 5) { r_score += 1.0 / ( (r + 1.0) * (r + 1.0) ); }
		if (scenario == 6) { r_score += 1.0 / ( (r + 1.0) * (r + 1.0) ); }
	}

	for (l = 0; l < L; l++) {
		/// BEST
		if (scenario == 1 || scenario == 2 || scenario == 3 || scenario == 4) {
			l_score += (l + 1.0) * (l + 1.0);
		} else if (scenario == 5) {
			l_score += this->log10s[10 + l] * this->log10s[10 + l];
		} else if (scenario == 6) {
			r = this->item_list[l]->get_ranking(z)->get_rank();
			if(r < in->get_cutoff()) {
				l_score += this->log10s[10 + l] * this->log10s[10 + l];
			}
		}
	}

	for (l = 0; l < L; l++) {
		r = this->item_list[l]->get_ranking(z)->get_rank();

		if(r < in->get_cutoff()) {
			if (scenario == 1) { c_score += (l + 1.0) / (r + 1.0); } else
			if (scenario == 2) { c_score += (R - r + 1.0) * (l + 1.0); } else /// 2
			if (scenario == 3) { c_score += (double)(R) / ( (l + 1.0) * (r + R) ); } else
			if (scenario == 4) { c_score += (r + 1.0) * (l + 1.0); } else
			if (scenario == 5) { c_score += this->log10s[10 + l] / (r + 1.0); } else
			if (scenario == 6) { c_score += this->log10s[10 + l] / (r + 1.0); }
		}
	}

	/// Cosine Similarity
	csim = c_score / (sqrt(r_score) * sqrt(l_score));

	/// Jaccard Index
//	csim = c_score / (r_score + l_score - c_score);

	/// Sorensen-Dice coefficient
//	csim = 2.0 * c_score / (r_score + l_score);

//	csim = c_score / (r_score * l_score);

//	printf("Csim: %5.3f\n", csim);
	return 1.0 - csim;
}


/// Scaled Footrule Distance
double MergedList::ScaledFootruleDistance(uint32_t z, class InputList * in) {
	double d = 0.0, nd = 0.0;
	rank_t i = 0, r = 0, R = in->get_num_items(), L = this->num_nodes;

	for (i = 0; i < L; i++) {
		r = this->item_list[i]->get_ranking(z)->get_rank();

		if(r < in->get_cutoff()) {
			d += fabs( (double) i / L - (double) r / R );

//			printf("Item %d/%d of Merged List (Score: %5.3f) ranked in place %d/%d in list %d (%s) - Sum=%5.3f\n",
//				i, this->num_nodes, this->item_list[i]->get_score(), r, in->get_num_items(),
// 				z, in->get_voter()->get_name(), d); getchar();
		}
	}

	nd = 2.00 * d / R;

//	if (nd > 1) { printf("gt > 1: ==%d== d=%5.3f NormD=%5.3f", R, d, nd); getchar(); }
//	printf("SFD: %5.3f (Norm SFD: %5.3f) - Items: %d\n", d, nd, R); getchar();

	return nd;
}

/// Scaled Footrule Distance
double MergedList::LocalScaledFootruleDistance(uint32_t z, class InputList * in) {
	double d = 0.0, nd = 0.0, factor = 0.0;
//	double log11 = log10(11.0), log22 = log2(2.2);
	rank_t i = 0, r = 0, R = in->get_num_items(), L = this->num_nodes;

	for (i = 0; i < L; i++) {
		r = this->item_list[i]->get_ranking(z)->get_rank();

		if(r < in->get_cutoff()) {
//			factor = log10( 10.0 * (1.1 - (double) r / R ) ) / log11;  // good with GAMMA=4.5 (0.243 / 0.230)
//			factor = log10( 10.0 * (1.1 - (double) i / L ) ) / log11;
//			factor = 1.0;
			factor = (double)R / (r + 1.0); // BEST with 1.0 <= GAMMA <= 1.5 ( 0.247-0.160 / 0.232)
//			factor = (double)(R - r + 1.0) / (R + r + 1.0); // GOOD with 6.0 <= GAMMA <= 7.0

			d += factor * ( (double) (r) / (R) - (double) (i) / (L));

//			printf("Item %d/%d of Merged List (Score: %5.3f) was ranked in place %d/%d in list %d (%s) - Sum=%5.3f\n",
//				i, this->num_nodes, this->item_list[i]->get_score(), r, in->get_num_items(),
// 				z, in->get_voter()->get_name(), d); getchar();
		}
	}

	nd = 2.0 * fabs(d) / R;
//	nd = fabs(d);

//	printf("gt > 1: ==%d== d=%5.3f NormD=%5.3f", R, d, nd); getchar();
//	printf("SFD: %5.3f (Norm SFD: %5.3f) - Items: %d\n", d, nd, R); getchar();

	return nd;
}


/// Accessors
rank_t MergedList::get_num_items() { return this->num_nodes; }
class MergedItem * MergedList::get_item(uint32_t i) { return this->item_list[i]; }
class MergedItem ** MergedList::get_item_list() { return this->item_list; }
score_t MergedList::get_weight() { return this->weight; }

/// Mutators
void MergedList::set_weight(score_t v) { this->weight = v; }

/// The DJB2 Hash Function (Dan Bernstein)
uint32_t MergedList::djb2(char * str) {
	unsigned long hash = 5381;
	int c;

	while ((c = *str++))
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

	return hash;
}
