#ifndef EVALUATOR_H
#define EVALUATOR_H

class Evaluator {
	private:
		class Rels * relevs;

		uint32_t true_positives;

		double average_precision;
		double average_recall;
		double average_dcg;
		double average_ndcg;

		double * precision;
		double * recall;
		double * dcg;
		double * ndcg;

	private:
		static int cmp_dbl(const void *a, const void *b) {
			double x = *(double *)a;
			double y = *(double *)b;
			if (y >= x) {
				return 1;
			}
			return -1;
		}

	public:
		Evaluator();
		~Evaluator();

		void insert_relev(char *, uint32_t);
		void clear();
		void evaluate(rank_t, char *, class MergedList *, FILE *);
		double evaluate_input(class InputList *);

		void display_relevs();

		/// Accessors
		uint32_t get_num_rel();
		uint32_t get_true_positives();

		double get_precision(uint32_t);
		double get_recall(uint32_t);
		double get_F1(uint32_t);
		double get_dcg(uint32_t);
		double get_ndcg(uint32_t);

		double get_average_precision();
		double get_average_recall();
		double get_average_dcg();
		double get_average_ndcg();
};

#endif // EVALUATOR_H
