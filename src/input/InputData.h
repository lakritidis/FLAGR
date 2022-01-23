#ifndef INPUTDATA_H
#define INPUTDATA_H

class InputData {
	private:
		class InputParams * params;

		uint32_t num_queries;
		class Query ** queries;

		uint32_t LetorNumVoters;
		uint32_t LetorMaxLength;

		/// Evaluation Metrics
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
		char * decompress_TREC_file(FILE *, char *);
		char * read_TREC_file(FILE *, char *);
		void process_TREC_lists(char *, uint32_t, char *);
		void read_TREC_qrels();

		void preprocess_LETOR_lists(char *, uint32_t);
		void process_LETOR_lists(char *, uint32_t);
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
