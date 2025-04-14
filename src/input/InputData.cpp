#include "InputData.h"

// #include "InputDataTSV.cpp"
#include "InputDataCSV.cpp"

/// Default Constructor
InputData::InputData() :
		params(NULL),
		num_queries(0),
		queries(NULL),
		num_ret(0),
		num_rel(0),
		num_rel_ret(0),
		MAP(0.0),
		MNDCG(0.0),
		avg_sprho(0.0),
		mean_precision(NULL),
		mean_recall(NULL),
		mean_F1(NULL),
		mean_dcg(NULL),
		mean_ndcg(NULL),
		eval_file(NULL) { }

/// Constructor 2 receives the input parameters as argument
InputData::InputData(class InputParams * pr) :
		params(pr),
		num_queries(0),
		queries(NULL),
		num_ret(0),
		num_rel(0),
		num_rel_ret(0),
		MAP(0.0),
		MNDCG(0.0),
		avg_sprho(0.0),
		mean_precision(new double[MAX_LIST_ITEMS]),
		mean_recall(new double[MAX_LIST_ITEMS]),
		mean_F1(new double[MAX_LIST_ITEMS]),
		mean_dcg(new double[MAX_LIST_ITEMS]),
		mean_ndcg(new double[MAX_LIST_ITEMS]),
		eval_file(NULL) {

			char * out;
			long file_size = 0;

			if (pr->get_rels_file()) {
				this->eval_file = fopen(pr->get_eval_file(), "w+");
				if (!this->eval_file) {
					printf("Error creating evaluation file: %s\n\n", pr->get_eval_file());
					exit(-1);
				}
				this->initialize_stats();
			} else {
				this->eval_file = NULL;
			}

			/// Read a single CSV file (all voters and all queries in the same file)
			FILE * datafile = fopen(this->params->get_input_file(), "r");
			if (datafile) {
				out = this->read_file(datafile, &file_size);
				this->preprocess_CSV(out, file_size);
				this->construct_CSV_lists(out, file_size);

				free(out);
				fclose(datafile);

				for (uint32_t i = 0; i < this->num_queries; i++) {
					this->queries[i]->init_weights();
				}

				if (this->params->get_rels_file()) {
					this->read_CSV_qrels();
				}
			} else {
				printf("Error Opening Input File %s\n", this->params->get_input_file());
				exit(0);
			}
}

/// Destructor
InputData::~InputData() {
	if (this->num_queries > 0) {
		if (this->queries) {
			for (uint32_t i = 0; i < this->num_queries; i++) {
				if (this->queries[i]) {
					delete this->queries[i];
				}
			}
			delete [] this->queries;
			if (this->mean_precision) { delete [] this->mean_precision; }
			if (this->mean_recall) { delete [] this->mean_recall; }
			if (this->mean_F1) { delete [] this->mean_F1; }
			if (this->mean_dcg) { delete [] this->mean_dcg; }
			if (this->mean_ndcg) { delete [] this->mean_ndcg; }
		}
	}

	if (this->eval_file) {
		fclose(this->eval_file);
	}
}

/// Initialize statistics
void InputData::initialize_stats() {
	this->avg_sprho = 0.0;
	this->MAP = 0.0;
	this->MNDCG = 0.0;

	for (rank_t i = 0; i < MAX_LIST_ITEMS; i++) {
		this->mean_precision[i] = 0.0;
		this->mean_recall[i] = 0.0;
		this->mean_F1[i] = 0.0;
		this->mean_dcg[i] = 0.0;
		this->mean_ndcg[i] = 0.0;
	}
}

/// Dstroy the output lists for all quesries
void InputData::destroy_output_lists() {
	if (this->num_queries > 0) {
		if (this->queries) {
			for (uint32_t i = 0; i < this->num_queries; i++) {
				if (this->queries[i]) {
					this->queries[i]->destroy_output_list();
				}
			}
		}
	}
}

/// Read the contents of a file into a char buffer
char * InputData::read_file(FILE * source, long * file_size) {
	fseek(source, 0L, SEEK_END);
	*file_size = ftell(source);
	rewind(source);

	char * out = (char *)malloc((*file_size + 1) * sizeof(char));

	int nread = 0, c;
	while ((c = fgetc(source)) != EOF) {
		out[nread] = c;
		nread++;
	}
    out[nread - 1] = 0;
    *file_size = nread;

	// printf(out);
	return out;
}

/// Print execution information in stdout
void InputData::print_header() {
	char m1[1000], m3[1000], m4[1024], m5[1000];
	uint32_t ram = this->params->get_aggregation_method();

	if (ram == 100) { strcpy(m1, "CombSUM with Borda normalization"); } else
	if (ram == 101) { strcpy(m1, "CombSUM with Rank normalization"); } else
	if (ram == 102) { strcpy(m1, "CombSUM with Score normalization"); } else
	if (ram == 103) { strcpy(m1, "CombSUM with Z-Score normalization"); } else
	if (ram == 104) { strcpy(m1, "CombSUM with simple Borda normalization"); } else
	if (ram == 110) { strcpy(m1, "CombMNZ with Borda normalization"); } else
	if (ram == 111) { strcpy(m1, "CombMNZ with Rank normalization"); } else
	if (ram == 112) { strcpy(m1, "CombMNZ with Score normalization"); } else
	if (ram == 113) { strcpy(m1, "CombMNZ with Z-Score normalization"); } else
	if (ram == 114) { strcpy(m1, "CombMNZ with simple Borda normalization"); } else
	if (ram == 200) { strcpy(m1, "Condorcet Winners Method"); } else
	if (ram == 201) { strcpy(m1, "Copeland Winners Method"); } else
	if (ram == 300) { strcpy(m1, "Outranking Approach"); } else
	if (ram == 400) { strcpy(m1, "Kemeny Optimal Aggregation"); } else
	if (ram == 401) { strcpy(m1, "Robust Rank Aggregation (RRA)"); } else
	if (ram == 5100) { strcpy(m1, "DIBRA @ CombSUM with Borda normalization"); } else
	if (ram == 5101) { strcpy(m1, "DIBRA @ CombSUM with Rank normalization"); } else
	if (ram == 5102) { strcpy(m1, "DIBRA @ CombSUM with Score normalization"); } else
	if (ram == 5103) { strcpy(m1, "DIBRA @ CombSUM with Z-Score normalization"); } else
	if (ram == 5104) { strcpy(m1, "DIBRA @ CombSUM with simple Borda normalization"); } else
	if (ram == 5110) { strcpy(m1, "DIBRA @ CombMNZ with Borda normalization"); } else
	if (ram == 5111) { strcpy(m1, "DIBRA @ CombMNZ with Rank normalization"); } else
	if (ram == 5112) { strcpy(m1, "DIBRA @ CombMNZ with Score normalization"); } else
	if (ram == 5113) { strcpy(m1, "DIBRA @ CombMNZ with Z-Score normalization"); } else
	if (ram == 5114) { strcpy(m1, "DIBRA @ CombMNZ with simple Borda normalization"); } else
	if (ram == 5200) { strcpy(m1, "DIBRA @ Condorcet Winners Method"); } else
	if (ram == 5201) { strcpy(m1, "DIBRA @ Copeland Winners Method"); } else
	if (ram == 5300) { strcpy(m1, "DIBRA @ Outranking Approach"); } else
	if (ram == 600) { strcpy(m1, "Preference Relations Method"); } else
	if (ram == 700) { strcpy(m1, "Agglomerative Aggregation"); } else
	if (ram == 801) { strcpy(m1, "Markov Chains 1"); } else
	if (ram == 802) { strcpy(m1, "Markov Chains 2"); } else
	if (ram == 803) { strcpy(m1, "Markov Chains 3"); } else
	if (ram == 804) { strcpy(m1, "Markov Chains 4"); } else
	if (ram == 805) { strcpy(m1, "MCT"); }

	printf("| %s / %s", m1, this->params->get_input_file());

	if (this->params->get_aggregation_method() == 5 || this->params->get_aggregation_method() == 6 ||
		this->params->get_aggregation_method() == 7 || this->params->get_aggregation_method() == 8) {

		if (params->get_correlation_method() == 1) {
			strcpy(m3, "Spearman's Rho correlation");
		} else if (params->get_correlation_method() == 2) {
			strcpy(m3, "Scaled Footrule Distance");
		} else if (params->get_correlation_method() == 3) {
			strcpy(m3, "Weighted Cosine Distance");
		} else if (params->get_correlation_method() == 4) {
			strcpy(m3, "Local Scaled Footrule Distance");
		} else if (params->get_correlation_method() == 5) {
			strcpy(m3, "Kendall's Tau correlation");
		}

		sprintf(m4, " - %s  - g = %3.1f)", m3, this->params->get_gamma());

	} else if (this->params->get_aggregation_method() == 9) {
		sprintf(m4, " - a = %3.1f, b = %3.1f)", this->params->get_alpha(), this->params->get_beta());
	} else {
		strcpy(m4, ")");
	}

	if (params->get_item_selection() == 0) {
		strcpy(m5, "Post processing Weight-Based Item Selection Disabled.");
	} else {
		strcpy(m5, "Post processing Weight-Based Item Selection Enabled");
	}
	printf("%s - %s\n", m4, m5); fflush(NULL);

	if (this->eval_file) {
		fprintf(this->eval_file, "| %s / %s", m1, this->params->get_input_file());
		fprintf(this->eval_file, "%s - %s\n", m4, m5); fflush(NULL);
	}
}


/// Evaluate the aggregate lists that have been constructed for each query. This one must be called
/// the aggregate function of each query.
void InputData::evaluate() {
	rank_t max_pts = this->params->get_eval_points();
	double precision_acc[max_pts], recall_acc[max_pts], F1_acc[max_pts], dcg_acc[max_pts], ndcg_acc[max_pts];
	double sum_avep = 0.0, sum_aver = 0.0, sum_aved = 0.0, sum_aven = 0.0;
	uint32_t m = 0;
	rank_t i = 0, cutoff = 0;

	/// Initialize the retrieval effectiveness metrics
	this->initialize_stats();

	for (i = 0; i < max_pts; i++) {
		precision_acc[i] = 0.0;
		recall_acc[i] = 0.0;
		F1_acc[i] = 0.0;
		dcg_acc[i] = 0.0;
		ndcg_acc[i] = 0.0;
	}

	/// Write the header row in the CSV evaluation file
	fprintf(eval_file, "q,num_ret,num_rel,num_rel_ret,map,");
	for (i = 0; i < max_pts; i++) { fprintf(eval_file, "P_%d,", i + 1); }
	for (i = 0; i < max_pts; i++) { fprintf(eval_file, "recall_%d,", i + 1); }
	for (i = 0; i < max_pts; i++) { fprintf(eval_file, "dcg_cut_%d,", i + 1); }
	for (i = 0; i < max_pts; i++) {
		if (i < max_pts - 1) {
			fprintf(eval_file, "ndcg_cut_%d,", i + 1);
		} else {
			fprintf(eval_file, "ndcg_cut_%d", i + 1);
		}
	}
	fprintf(eval_file, "\n");

	/// Evaluate each query
	for (m = 0; m < this->num_queries; m++) {
//		printf("Evaluating Query %d: %s (%d items)\n", m + 1,
//				this->queries[m]->get_topic(), this->queries[m]->get_num_items());

		this->queries[m]->evaluate(max_pts, this->eval_file);

		if (this->queries[m]->get_num_items() > max_pts) {
			cutoff = max_pts;
		} else {
			cutoff = this->queries[m]->get_num_items();
		}

		this->num_ret += this->queries[m]->get_num_items();
		this->num_rel += this->queries[m]->get_num_rel();
		this->num_rel_ret += this->queries[m]->get_num_rel_ret();

		/// Update the accumulators so that we can compute the mean values in the end
		for (i = 0; i < cutoff; i++) {
			precision_acc[i] += this->queries[m]->get_precision(i);
			recall_acc[i] += this->queries[m]->get_recall(i);
			F1_acc[i] += this->queries[m]->get_F1(i);
			dcg_acc[i] += this->queries[m]->get_dcg(i);
			ndcg_acc[i] += this->queries[m]->get_ndcg(i);
		}

		sum_avep += this->queries[m]->get_average_precision();
		sum_aver += this->queries[m]->get_average_recall();
		sum_aved += this->queries[m]->get_average_dcg();
		sum_aven += this->queries[m]->get_average_ndcg();

		this->avg_sprho += this->queries[m]->evaluate_experts_list();
	}

	/// Compute the mean values
	for (i = 0; i < max_pts; i++) {
		this->mean_precision[i] = precision_acc[i] / (double) this->num_queries;
		this->mean_recall[i] = recall_acc[i] / (double) this->num_queries;
		this->mean_F1[i] = F1_acc[i] / (double) this->num_queries;
		this->mean_dcg[i] = dcg_acc[i] / (double) this->num_queries;
		this->mean_ndcg[i] = ndcg_acc[i] / (double) this->num_queries;
	}
	this->MAP = sum_avep / (double) this->num_queries;
	this->MNDCG = sum_aven / (double) this->num_queries;
	this->avg_sprho = this->avg_sprho / (double) this->num_queries;

	/// Create a last row in the CSV evaluation file with the mean values
	fprintf(eval_file, "all,%d,%d,%d,%7.6f,",
			this->num_ret, this->num_rel, this->num_rel_ret, this->MAP);

	for (i = 0; i < max_pts; i++) { fprintf(eval_file, "%7.6f,", this->mean_precision[i]); }
	for (i = 0; i < max_pts; i++) { fprintf(eval_file, "%7.6f,", this->mean_recall[i]); }
	for (i = 0; i < max_pts; i++) { fprintf(eval_file, "%7.6f,", this->mean_dcg[i]); }
	for (i = 0; i < max_pts; i++) {
		if (i < max_pts - 1) {
			fprintf(eval_file, "%7.6f,", this->mean_ndcg[i]);
		} else {
			fprintf(eval_file, "%7.6f", this->mean_ndcg[i]);
		}
	}
	fprintf(eval_file, "\n");
}


/// Evaluate the input lists of each query
void InputData::evaluate_input() {
	uint32_t m = 0;

	/// Initialize the retrieval effectiveness metrics
	this->initialize_stats();

	for (m = 0; m < this->num_queries; m++) {
		this->queries[m]->evaluate_input();
	}
}

/// Apply the selected rank aggregation method and construct the output lists of each Aggregator
void InputData::aggregate() {
/*
	printf("\nPerforming Aggregation...\nParameters ");
	this->print_header();
	printf("\n");
*/
	for (uint32_t m = 0; m < this->num_queries; m++) {
//		printf("Processing Query %d / %d... [ ", m + 1, this->num_queries); fflush(NULL);
		this->queries[m]->aggregate(this->params);
//		printf(" OK ]\n"); fflush(NULL);
	}
}

/// Compute the average list length per query
uint32_t InputData::compute_avg_list_length() {
	uint32_t sum = 0, sum2 = 0;
	for (uint32_t q = 0; q < this->num_queries; q++) {
		for (uint32_t l = 0; l < this->queries[q]->get_num_input_lists(); l++) {
			sum2++;
			sum += this->queries[q]->get_input_list(l)->get_num_items();
		}
	}
	return sum / sum2;
}

/// Accessors
uint32_t InputData::get_num_queries() { return this->num_queries; }
class Query * InputData::get_query(uint32_t i) { return this->queries[i]; }
double InputData::get_mean_precision(uint32_t i) { return this->mean_precision[i]; }
double InputData::get_mean_recall(uint32_t i) { return this->mean_recall[i]; }
double InputData::get_mean_F1(uint32_t i) { return this->mean_F1[i]; }
double InputData::get_mean_dcg(uint32_t i) { return this->mean_dcg[i]; }
double InputData::get_mean_ndcg(uint32_t i) { return this->mean_ndcg[i]; }
double InputData::get_MAP() { return this->MAP; }
double InputData::get_MNDCG() { return this->MNDCG; }
double InputData::get_avg_sprho() { return this->avg_sprho; }
rank_t InputData::get_num_ret() { return this->num_ret; }
rank_t InputData::get_num_rel() { return this->num_rel; }
rank_t InputData::get_num_rel_ret() { return this->num_rel_ret; }
