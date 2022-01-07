#ifndef EVALUATOR_H
#define EVALUATOR_H

class Evaluator {
	private:
		class Rels * relevs;

		uint32_t true_positives;

		double average_precision;
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
		Evaluator(class Rels *);
		~Evaluator();

		void insert_relev(uint32_t, char *, uint32_t);
		void clear();
		void evaluate(uint32_t, class MergedList *, FILE *);
		double evaluate_input(uint32_t, class InputList *);

		/// Accessors
		uint32_t get_num_nodes();
		uint32_t get_true_positives();

		double get_precision(uint32_t);
		double get_recall(uint32_t);
		double get_F1(uint32_t);
		double get_dcg(uint32_t);
		double get_ndcg(uint32_t);
		double get_average_precision();
		double get_average_ndcg();
};

#endif // EVALUATOR_H
