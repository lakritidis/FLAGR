#ifndef WEIGHTEDITEMSELECTOR_H
#define WEIGHTEDITEMSELECTOR_H


class WeightedItemSelector {
	private:
		class InputParams * params;
		class InputList ** inlists;
		uint32_t num_inlists;
		class SimpleScoreStats * stats;

	private:
		static int cmp_weight(const void *, const void *);

	public:
		WeightedItemSelector(InputParams *, class InputList **, uint32_t, class SimpleScoreStats *);
		~WeightedItemSelector();

	protected:

	private:
};

#endif // WEIGHTEDITEMSELECTOR_H
