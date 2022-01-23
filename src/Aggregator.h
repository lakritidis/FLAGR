#ifndef AGGREGATOR_H
#define AGGREGATOR_H


class Aggregator {
	private:
		uint16_t num_lists;
		uint16_t num_alloc_lists;
		class InputList ** input_lists;
		class MergedList * output_list;

	private:
		void merge_input_lists();

	public:
		Aggregator();
		~Aggregator();

		class InputList * create_list(char *, double);
		class Voter ** aggregate(class InputParams *);
		void init_weights();
		void destroy_output_list();
		void display();

		/// Accessors
		uint16_t get_num_lists();
		rank_t get_num_items();
		class InputList * get_input_list(uint32_t);
		class MergedList * get_output_list();
};

#endif // AGGREGATOR_H
