#ifndef RANKING_H
#define RANKING_H

/// This structure describes the  ranking of an element  in an input  list; input_list is a pointer
/// reference to the original input list object. The variables rank and index of the element may be
/// different in incomplete lists. In TREC an example list has elements with rankings 90,97,102,...
/// This is an incomplete list and the indices are 0, 1, 2,... respectively.  Notice that the index
/// refers to an input list that is sorted by rank.
class Ranking {
	private:
		class InputList * input_list;
		rank_t rank;  /// The ranking of an item in input_list

	public:
		Ranking(class InputList *, rank_t, rank_t);
		~Ranking();

		void set_input_list(class InputList *);
		void set_rank(rank_t);

		class InputList * get_input_list();
		rank_t get_rank();
};

#endif // RANKING_H
