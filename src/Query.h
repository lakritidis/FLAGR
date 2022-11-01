#ifndef QUERY_H
#define QUERY_H


class Query {
	private:
		char * topic; 				/// Query String
        class Aggregator * agg;		/// Aggregator object: this one performs rank aggregation
        class Evaluator * eval;		/// Evaluator object: this one performs evaluation

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
		void aggregate(class InputParams * params);

		void insert_relev(char *, uint32_t);
		void display();
		void display_relevs();
		void evaluate(rank_t, FILE *);
		void evaluate_input();
		void destroy_output_list();
		double evaluate_experts_list();
		void init_weights();

		/// Accessors
		uint32_t get_num_items();
		rank_t get_num_rel();
		rank_t get_num_rel_ret();
		uint32_t get_num_input_lists();

		char * get_topic();
		double get_average_precision();
		double get_average_recall();
		double get_average_dcg();
		double get_average_ndcg();
		double get_precision(uint32_t);
		double get_recall(uint32_t);
		double get_F1(uint32_t);
		double get_dcg(uint32_t);
		double get_ndcg(uint32_t);

		class InputList * get_input_list(uint32_t);

		/// Mutators
		void set_topic(char *);
};

#endif // QUERY_H
