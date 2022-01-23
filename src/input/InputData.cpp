#include "InputData.h"

#include "input/InputDataTREC.cpp"
#include "input/InputDataLETOR.cpp"

/// Default Constructor
InputData::InputData() {
	this->params = NULL;
	this->num_queries = 0;
	this->queries = NULL;
	this->mean_F1 = NULL;
	this->mean_precision = NULL;
	this->mean_recall = NULL;
	this->mean_dcg = NULL;
	this->MAP = 0.0;
	this->MNDCG = 0.0;
	this->LetorMaxLength = 0;
	this->LetorNumVoters = 0;
	this->avg_sprho = 0.0;
}

/// Constructor 2
/// t: Constructor type
/// t==1: TREC2009-TREC2013 Adhoc Task of Web Track
/// t==2: MQ2007-agg
/// t==3: MQ2008-agg
InputData::InputData(class InputParams * pr) {
	this->params = pr;
	uint32_t no_file = 0;

	this->MAP = 0.0;
	this->MNDCG = 0.0;
	this->mean_precision = new double[MAX_LIST_ITEMS];
	this->mean_recall = new double[MAX_LIST_ITEMS];
	this->mean_F1 = new double[MAX_LIST_ITEMS];
	this->mean_dcg = new double[MAX_LIST_ITEMS];
	this->mean_ndcg = new double[MAX_LIST_ITEMS];
	this->eval_file = fopen(pr->get_eval_file(), "w+");
	this->initialize_stats();

	/// Read a TREC-style file
	if (this->params->get_dataset_type() == 1) {
		DIR *dir;
		struct dirent *ent;

		if ((dir = opendir( this->params->get_data_dir() )) != NULL) {
			char filename[1024];

			this->num_queries = DATASET_NUM_QUERIES;
			this->queries = new Query * [this->num_queries];
			for (uint32_t i = 0; i < this->num_queries; i++) {
				this->queries[i] = new Query(1);
			}

			while ((ent = readdir(dir)) != NULL) {
				if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {

					sprintf(filename, "%s%s", this->params->get_data_dir(), ent->d_name);

					FILE * f = fopen(filename, "rb");
					if (f) {
						printf("Reading Run %d: %s (Items: ", ++no_file, filename); fflush(NULL);
						if (this->params->get_dataset_compressed()) {
							this->decompress_TREC_file(f, ent->d_name);
						} else {
							this->read_TREC_file(f, ent->d_name);
						}
						printf("%d)\n", this->compute_avg_list_length()); fflush(NULL);
						fclose(f);
					} else {
						printf("Error Opening Input File %s\n", filename);
					}
				}
			}
			closedir(dir);
		} else {
			printf("Error Opening Input Directory %s\n", this->params->get_data_dir());
			exit(-1);
		}

		for (uint32_t i = 0; i < this->num_queries; i++) {
			this->queries[i]->init_weights();
		}

		this->read_TREC_qrels();

	/// Read a LETOR-style file
	} else if (this->params->get_dataset_type() == 2) {
		this->num_queries = 0;

		uint32_t init_buf_len = 16777216, nread = 0, cur_buf_len = 0;
		class InputList * inlist = NULL;
		char voter[100], res[100];
		char * buf = (char *)malloc(init_buf_len * sizeof(char));

		printf("Preprocessing LETOR Input..."); fflush(NULL);

		FILE * f = fopen(params->get_data_dir(), "rb");
		if (f) {
			while(!feof(f)) {
				nread = fread(buf + cur_buf_len, sizeof(char), 4096, f);
				if (nread == 0) {
					break;
				}

				cur_buf_len += nread;
				if (cur_buf_len >= init_buf_len) {
					init_buf_len *= 2;
					buf = (char *)realloc(buf, init_buf_len * sizeof(char));
				}
			}
			this->preprocess_LETOR_lists(buf, cur_buf_len);

//			printf("%s\n", buf);
			fclose(f);
		} else {
			printf("Error Opening Input File %s\n", this->params->get_data_dir());
			exit(-1);
		}

		printf("\t[ OK, %d queries, %d voters, max list length: %d ]\n", this->num_queries,
				this->LetorNumVoters, this->LetorMaxLength);

		/// Initialize 1692 queries and 21 Input lists with 500 random elements each
		printf("Initializing Data..."); fflush(NULL);

		this->queries = new Query * [this->num_queries];

		for (uint32_t q = 0; q < this->num_queries; q++) {
//			printf("QUERY %d\n", q);
			this->queries[q] = new Query(1);
			this->queries[q]->set_topic_id(q + 1);

			for (uint32_t v = 0; v < this->LetorNumVoters; v++) {
				sprintf(voter, "voter-%d", v + 1);

				inlist = this->queries[q]->create_list(voter, 1.00);
/*
				for (uint32_t i = 0; i < this->LetorMaxLength; i++) {
					sprintf(res, "ELEMENT-%d-%d-%d", q, v + 1, i + 1);
//					printf("ELEMENT-%d-%d-%d\n", q, v, i);
                    inlist->insert_item(res, i + 1);
				}
*/
			}
		}

		printf("\t [ OK ]\nReading Input..."); fflush(NULL);

		this->process_LETOR_lists(buf, cur_buf_len);

		free(buf);
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
		fclose(eval_file);
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

void InputData::print_header() {
	char m1[1024], m2[1024], m3[1024], m4[1024], m5[1024];

	if (this->params->get_aggregation_method() == 1) { strcpy(m1, "BORDA COUNT (BASELINE)"); } else
	if (this->params->get_aggregation_method() == 2) { strcpy(m1, "CONDORCET METHOD (BASELINE)"); } else
	if (this->params->get_aggregation_method() == 3) { strcpy(m1, "OUTRANKING (BASELINE)"); } else
	if (this->params->get_aggregation_method() == 4) { strcpy(m1, "RANK POSITION (BASELINE)"); } else
	if (this->params->get_aggregation_method() == 5) { strcpy(m1, "DIBRA @ BORDA COUNT"); } else
	if (this->params->get_aggregation_method() == 6) { strcpy(m1, "DIBRA @ CONDORCET METHOD"); } else
	if (this->params->get_aggregation_method() == 7) { strcpy(m1, "DIBRA @ OUTRANKING"); } else
	if (this->params->get_aggregation_method() == 8) { strcpy(m1, "DIBRA @ RANK POSITION"); } else
	if (this->params->get_aggregation_method() == 9) { strcpy(m1, "PREFERENCE RELATIONS (ESWA2016)"); } else
	if (this->params->get_aggregation_method() == 10) { strcpy(m1, "AGGLOMERATIVE (KBS 2018)"); }

	printf("| %s / %s / %s", this->params->get_dataset_name(), this->params->get_dataset_track(), m1);

	if (this->params->get_aggregation_method() == 5 || this->params->get_aggregation_method() == 6 ||
		this->params->get_aggregation_method() == 7 || this->params->get_aggregation_method() == 8) {

		if (params->get_correlation_method() == 1) { strcpy(m3, "Spearman's Rho correlation"); } else
		if (params->get_correlation_method() == 2) { strcpy(m3, "Scaled Footrule Distance"); } else
		if (params->get_correlation_method() == 3) { strcpy(m3, "Weighted Cosine Distance"); } else
		if (params->get_correlation_method() == 4) { strcpy(m3, "Local Scaled Footrule Distance"); } else
		if (params->get_correlation_method() == 5) { strcpy(m3, "Kendall's Tau correlation"); }

		sprintf(m4, " - %s  - g = %3.1f)", m3, this->params->get_gamma());

	} else if (this->params->get_aggregation_method() == 9) {
		sprintf(m4, " - a = %3.1f, b = %3.1f)", this->params->get_alpha(), this->params->get_beta());
	} else {
		strcpy(m4, ")");
	}

	if (params->get_list_pruning() == 1) {
		strcpy(m5, "List Pruning Enabled.");
	} else {
		strcpy(m5, "List Pruning Disabled.");
	}
	printf("%s - %s\n", m4, m5); fflush(NULL);

	if (this->eval_file) {
		fprintf(eval_file, "| %s / %s / %s %s", this->params->get_dataset_name(), this->params->get_dataset_track(), m1, m2);
		fprintf(eval_file, "%s - %s\n", m4, m5); fflush(NULL);
	}
}

/// Evaluate the output lists of each query
void InputData::evaluate() {
	double precision_acc[MAX_LIST_ITEMS], recall_acc[MAX_LIST_ITEMS], F1_acc[MAX_LIST_ITEMS];
	double dcg_acc[MAX_LIST_ITEMS], ndcg_acc[MAX_LIST_ITEMS];
	double sum_avep = 0.0, sum_aven = 0.0;
	uint32_t m = 0;
	rank_t i = 0, cutoff = 0;
	char dash[151], lit[151]; dash[0] = 0; lit[0] = 0;

	/// Initialize the retrieval effectiveness metrics
	this->initialize_stats();

	for (i = 0; i < MAX_LIST_ITEMS; i++) {
		precision_acc[i] = 0.0;
		recall_acc[i] = 0.0;
		F1_acc[i] = 0.0;
		dcg_acc[i] = 0.0;
		ndcg_acc[i] = 0.0;
	}

	/// Print the column headers of the results (according to RESULTS_TYPE)
	if (RESULTS_TYPE == 1) {
		printf("%s\n", PadStr(dash, 137, '-'));
		if (eval_file) { fprintf(eval_file, "%s\n", PadStr(dash, 137, '-')); }

		this->print_header();
		strcpy(lit, "Topic");
		printf("%s\n", PadStr(dash, 137, '-'));
		printf("%s\n", PadStr(dash, 137, '-'));
		if (this->eval_file) { fprintf(eval_file, "%s\n", PadStr(dash, 137, '-')); }

		printf("| %s\t(Rel)\t|  AveP  |   P@5  |  P@10  |  P@15  |  P@20  |  P@30  |  P@100 |  P@200 |  P@500 | P@1000 | nDCG@10 | nDCG@20 |\n",
			PadStr(lit, 10, ' '));

		if (this->eval_file) {
			fprintf(eval_file, "| %s\t(Rel)\t|  AveP  |   P@5  |  P@10  |  P@15  |  P@20  |  P@30  |  P@100 |  P@200 |  P@500 | P@1000 | nDCG@10 | nDCG@20 |\n",
				PadStr(lit, 10, ' '));
			fprintf(eval_file, "%s\n", PadStr(dash, 137, '-')); fflush(NULL);
		}

		printf("%s\n", PadStr(dash, 137, '-')); fflush(NULL);
		if (this->eval_file) { fprintf(eval_file, "%s\n", PadStr(dash, 137, '-')); fflush(NULL); }

	} else if (RESULTS_TYPE == 2) {
		printf("%s\n", PadStr(dash, 137, '-'));
		if (eval_file) { fprintf(eval_file, "%s\n", PadStr(dash, 137, '-')); }

		this->print_header();
		strcpy(lit, "Topic");
		printf("%s\n", PadStr(dash, 137, '-'));
		printf("%s\n", PadStr(dash, 137, '-'));
		if (this->eval_file) { fprintf(eval_file, "%s\n", PadStr(dash, 137, '-')); }

		printf("| %s\t(Rel)\t| AvePr |  P@5  |  P@10 |  P@20 |  P@30 |  P@50 | P@100 | ANGCG |  N@5  |  N@10 |  N@20 |  N@30 |  N@50 | N@100 |\n",
			PadStr(lit, 10, ' '));
		printf("%s\n", PadStr(dash, 137, '-')); fflush(NULL);

		if (this->eval_file) {
			fprintf(eval_file, "| %s\t(Rel)\t| AvePr |  P@5  |  P@10 |  P@20 |  P@30 |  P@50 | P@100 | ANGCG |  N@5  |  N@10 |  N@20 |  N@30 |  N@50 | N@100 |\n",
				PadStr(lit, 10, ' '));
		}

		printf("%s\n", PadStr(dash, 137, '-')); fflush(NULL);
		if (this->eval_file) { fprintf(eval_file, "%s\n", PadStr(dash, 137, '-')); fflush(NULL); }

	} else if (RESULTS_TYPE == 3) {
		printf("%s\n", PadStr(dash, 153, '-'));
		if (eval_file) { fprintf(eval_file, "%s\n", PadStr(dash, 137, '-')); }

		this->print_header();
		strcpy(lit, "Topic");
		printf("%s\n", PadStr(dash, 153, '-'));
		printf("%s\n", PadStr(dash, 153, '-'));
		if (this->eval_file) { fprintf(eval_file, "%s\n", PadStr(dash, 137, '-')); }

		printf("| %s\t(Rel)\t| AvePr |  P@1  |  P@2  |  P@4  |  P@5  |  P@6  |  P@8  |  P@10 | ANGCG |  N@1  |  N@2  |  N@4  |  N@5  |  N@6  |  N@8  |  N@10 |\n",
			PadStr(lit, 10, ' '));
		printf("%s\n", PadStr(dash, 153, '-')); fflush(NULL);

		if (this->eval_file) {
			fprintf(eval_file, "| %s\t(Rel)\t| AvePr |  P@1  |  P@2 |  P@4 |  P@5 |  P@6 |  P@8  |  P@10  | ANGCG |  N@1  |  N@2  |  N@4  |  N@5  |  N@6  |  N@8  |  N@10  |\n",
				PadStr(lit, 10, ' '));
		}

		printf("%s\n", PadStr(dash, 153, '-')); fflush(NULL);
		if (this->eval_file) { fprintf(eval_file, "%s\n", PadStr(dash, 153, '-')); fflush(NULL); }
	}

	/// Start the evaluation
	for (m = 0; m < this->num_queries; m++) {
//		printf("Evaluating Query %d (%d items)\n", m + 1, this->queries[m]->get_num_items());
		this->queries[m]->evaluate(this->eval_file);

		if (this->queries[m]->get_num_items() > MAX_LIST_ITEMS) {
			cutoff = MAX_LIST_ITEMS;
		} else {
			cutoff = this->queries[m]->get_num_items();
		}

		for (i = 0; i < cutoff; i++) {
			precision_acc[i] += this->queries[m]->get_precision(i);
			recall_acc[i] += this->queries[m]->get_recall(i);
			F1_acc[i] += this->queries[m]->get_F1(i);
			dcg_acc[i] += this->queries[m]->get_dcg(i);
			ndcg_acc[i] += this->queries[m]->get_ndcg(i);
		}

		sum_avep += this->queries[m]->get_average_precision();
		sum_aven += this->queries[m]->get_average_ndcg();

		this->avg_sprho += this->queries[m]->evaluate_experts_list();
	}

	if (RESULTS_TYPE == 1) { printf("%s\n", PadStr(dash, 137, '-')); } else
	if (RESULTS_TYPE == 2) { printf("%s\n", PadStr(dash, 105, '-')); }

	for (i = 0; i < MAX_LIST_ITEMS; i++) {
		this->mean_precision[i] = precision_acc[i] / (double) this->num_queries;
		this->mean_recall[i] = recall_acc[i] / (double) this->num_queries;
		this->mean_F1[i] = F1_acc[i] / (double) this->num_queries;
		this->mean_dcg[i] = dcg_acc[i] / (double) this->num_queries;
		this->mean_ndcg[i] = ndcg_acc[i] / (double) this->num_queries;
	}
	this->MAP = sum_avep / (double) this->num_queries;
	this->MNDCG = sum_aven / (double) this->num_queries;
	this->avg_sprho = this->avg_sprho / (double) this->num_queries;
}

/// Evaluate the input lists of each query
void InputData::evaluate_input() {
	uint32_t m = 0;

	/// Initialize the retrieval effectiveness metrics
	this->initialize_stats();

//	char dash[151], lit[151]; dash[0] = 0; lit[0] = 0;
//	strcpy(lit, "Run - Topic");
//	printf("%s\n", PadStr(dash, 145, '-'));
//	printf("| %s |  AveP  |   P@5  |  P@10  |  P@15  |  P@20  |  P@30  |  P@100 |  P@200 |  P@500 | nDCG@10 | nDCG@20 |\n",
//		PadStr(lit, 40, ' '));
//	printf("%s\n", PadStr(dash, 145, '-'));

	for (m = 0; m < this->num_queries; m++) {
		this->queries[m]->evaluate_input();
	}
}

/// Apply the rank aggregation method and construct the output lists of each Aggregator
void InputData::aggregate() {
	printf("\nPerforming Aggregation...\nParameters ");
	this->print_header();
	printf("\n");

	for (uint32_t m = 0; m < this->num_queries; m++) {
		printf("Processing Query %d / %d... [ ", m + 1, this->num_queries); fflush(NULL);
		this->queries[m]->aggregate(this->params);
		printf(" OK ]\n"); fflush(NULL);
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
FILE * InputData::get_eval_file() { return this->eval_file; }
