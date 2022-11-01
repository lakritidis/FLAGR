#ifndef INPUTDATA_H
#define INPUTDATA_H

class InputData {
	private:
		class InputParams * params;

		uint32_t num_queries;
		class Query ** queries;

		/// Evaluation Metrics
		rank_t num_ret;
		rank_t num_rel;
		rank_t num_rel_ret;

		double MAP;
		double MNDCG;
		double avg_sprho;
		double * mean_precision;
		double * mean_recall;
		double * mean_F1;
		double * mean_dcg;
		double * mean_ndcg;

		FILE * eval_file;

	private:
		char * read_file(FILE *, long *);
		void get_TSV_queries(char *, uint32_t);
		void process_TSV_lists(char *, uint32_t, char *);
		void read_TSV_qrels();
		void read_CSV_qrels();

		void preprocess_CSV(char *, uint32_t);
		void construct_CSV_lists(char *, uint32_t);

		void initialize_stats();

	public:
		InputData();
		InputData(class InputParams * PARAMS);
		~InputData();

		void aggregate();
		void evaluate();
		void evaluate_input();
		void destroy_output_lists();

		void print_header();

		/// Accessors
		uint32_t get_num_queries();
		class Query * get_query(uint32_t);

		double get_MAP();
		double get_MNDCG();
		rank_t get_num_ret();
		rank_t get_num_rel();
		rank_t get_num_rel_ret();
		double get_mean_precision(uint32_t);
		double get_mean_recall(uint32_t);
		double get_mean_F1(uint32_t);
		double get_mean_dcg(uint32_t);
		double get_mean_ndcg(uint32_t);
		double get_avg_sprho();
		FILE * get_eval_file();
		uint32_t compute_avg_list_length();
};

#endif // TRECINPUT_H
