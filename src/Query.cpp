#include "Query.h"

/// Constructor 1:
/// If sw == Initialize the evaluator by using the relevance judgments
Query::Query(uint32_t sw) :
		topic(NULL),
		agg(new Aggregator()),
		eval(NULL),
		real_experts_list(NULL),
		computed_experts_list(NULL) {

			if (sw == 1) {
				this->eval = new Evaluator();
			}
}

/// Destructor
Query::~Query() {
	if (this->real_experts_list) {
		for (uint32_t i = 0; i < this->agg->get_num_lists(); i++) {
			if (this->real_experts_list[i]) { delete this->real_experts_list[i]; }
		}
		delete [] this->real_experts_list;
		this->real_experts_list = NULL;
	}

	if (this->computed_experts_list) {
		delete [] this->computed_experts_list;
		this->computed_experts_list = NULL;
	}

	if (this->agg) {
		delete this->agg;
		this->agg = NULL;
	}

	if (this->eval) {
		delete this->eval;
		this->eval = NULL;
	}

	if (this->topic) {
		delete [] this->topic;
		this->topic = NULL;
	}
}

/// Create a new input list for an aggregator
class InputList * Query::create_list(char * v, double w) {
	return this->agg->create_list(v, w);
}

/// Apply the rank aggregation method and construct the final output list
void Query::aggregate(class InputParams * params) {
	this->computed_experts_list = this->agg->aggregate(this->topic, params);
}

/// Destroy the aggregate list
void Query::destroy_output_list() {
	if (this->computed_experts_list) {
		delete [] this->computed_experts_list;
	}

	this->agg->destroy_output_list();
	this->eval->clear();
}

/// Apply the rank aggregation method and construct the final output list
void Query::insert_relev(char * v, uint32_t j) {
	this->eval->insert_relev(v, j);
}

/// Evaluate the output list of the query by using the input relevance judgments
void Query::evaluate(rank_t ev_pts, FILE * e_file) {
	this->eval->evaluate(ev_pts, this->topic, this->agg->get_output_list(), e_file);
}

double Query::evaluate_experts_list() {
	int32_t dis = 0;
	uint32_t num_lists = this->agg->get_num_lists();
	rank_t i = 0, j = 0;
	double spearmansRho = 0.0;

	if (this->real_experts_list && this->computed_experts_list) {
		for (i = 0; i < num_lists; i++) {
			class Voter * v1 = this->real_experts_list[i];

//			printf("Voter %d: %s - Real Rank: %d (%5.3f), Computed Rank: ",i,v1->get_name(),i+1,v1->get_weight());
//			printf("%d ", i+1);

			for (j = 0; j < num_lists; j++) {
				class Voter * v2 = this->computed_experts_list[j];
				if (strcmp(v1->get_name(), v2->get_name()) == 0) {
//					printf("%d (%5.3f)\n", j+1, v2->get_weight());
//					printf("%d a\n", j + 1);
					dis += (i - j) * (i - j);
					break;
				}
			}
		}

		spearmansRho = 1.0 - (6.0 * dis) / (num_lists * (num_lists * num_lists - 1));
//		printf("Spearman's Rho value = %5.3f\n", spearmansRho);
	}
	return spearmansRho;
}

/// Evaluate all the input lists of the query by using the input relevance judgments
void Query::evaluate_input() {
	double voter_map = 0.0;
	uint32_t num_lists = this->agg->get_num_lists(), i = 0;
	class InputList * inlist;

	this->real_experts_list = new Voter * [this->agg->get_num_lists()];

	for (i = 0; i < num_lists; i++) {
		inlist = this->agg->get_input_list(i);

		voter_map = this->eval->evaluate_input(inlist);

		this->real_experts_list[i] = new Voter(inlist->get_voter()->get_name(), voter_map);
	}

	qsort(this->real_experts_list, num_lists, sizeof(Voter *), &Query::cmp_voter);
}

/// Set the initial weights of voters
void Query::init_weights() {
	this->agg->init_weights();
}

/// Display the query properties and input lists
void Query::display() {
	printf("Displaying Data for Query: %s\n", this->topic);
	this->agg->display();
}

/// Display the query properties and input lists
void Query::display_relevs() {
	printf("Displaying Rels for Query: %s\n", this->topic);
	this->eval->display_relevs();
}

/// Accessors
class InputList * Query::get_input_list(uint32_t i) { return this->agg->get_input_list(i); }
char * Query::get_topic() { return this->topic; }
uint32_t Query::get_num_items() { return this->agg->get_num_items(); }
rank_t Query::get_num_rel() { return this->eval->get_num_rel(); }
rank_t Query::get_num_rel_ret() { return this->eval->get_true_positives(); }
uint32_t Query::get_num_input_lists() { return this->agg->get_num_lists(); }
double Query::get_average_precision() { return this->eval->get_average_precision(); }
double Query::get_average_recall() { return this->eval->get_average_recall(); }
double Query::get_average_dcg() { return this->eval->get_average_dcg(); }
double Query::get_average_ndcg() { return this->eval->get_average_ndcg(); }
double Query::get_precision(uint32_t i) { return this->eval->get_precision(i); }
double Query::get_recall(uint32_t i) { return this->eval->get_recall(i); }
double Query::get_F1(uint32_t i) { return this->eval->get_F1(i); }
double Query::get_dcg(uint32_t i) { return this->eval->get_dcg(i); }
double Query::get_ndcg(uint32_t i) { return this->eval->get_ndcg(i); }

/// Mutators
void Query::set_topic(char * v) {
	this->topic = new char[strlen(v) + 1];
	strcpy(this->topic, v);
}
