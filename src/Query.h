#ifndef QUERY_H
#define QUERY_H


class Query {
	private:
		uint32_t topic_id;
		char * topic;
        class Aggregator * agg;
        class Evaluator * eval;

        class Voter ** real_experts_list;
        class Voter ** computed_experts_list;

	private:
		static int cmp_voter(const void *a, const void *b) {
			class Voter * x = *(class Voter **)a;
			class Voter * y = *(class Voter **)b;

			if (x->get_weight() > y->get_weight()) { return -1; }
			return 1;
		}

	public:
		Query(uint32_t);
		~Query();

		class InputList * create_list(char *, double);
		void aggregate(uint32_t, uint32_t, score_t, score_t);

		void insert_relev(uint32_t, char *, uint32_t);
		void display();
		void evaluate(FILE *);
		void evaluate_input();
		void destroy_output_list();
		double evaluate_experts_list();
		void init_weights();

		/// Accessors
		uint32_t get_num_relevs();
		uint32_t get_num_items();
		uint32_t get_num_input_lists();

		uint32_t get_topic_id();
		double get_average_precision();
		double get_average_ndcg();
		double get_precision(uint32_t);
		double get_recall(uint32_t);
		double get_F1(uint32_t);
		double get_dcg(uint32_t);
		double get_ndcg(uint32_t);

		class InputList * get_input_list(uint32_t);

		/// Mutators
		void set_topic_id(uint32_t);
};

#endif // QUERY_H
