#ifndef RELS_H
#define RELS_H

/// Rels
/// A collection of Relevant Results (given by the TREC qrels file) for a specific query

class Rels {
	private:
		class Rel ** hash_table;

		uint32_t mask;
		uint32_t num_slots;  /// The number of slots of the hash table
		uint32_t num_nodes;	 /// The number of elements stored in the hash table.
		uint32_t num_chains; /// The number of non-empty chains.

	private:
		uint32_t KazLibHash(char *);

	public:
		Rels();
		Rels(uint32_t);
		~Rels();

		void display(uint32_t);
		void insert(uint32_t, char *, uint32_t);
		bool search(uint32_t, char *, uint32_t *);

		uint32_t get_num_nodes();
};

#endif // RELS_H
