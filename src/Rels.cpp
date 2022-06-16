#include "Rels.h"

/// Constructor 1 : Default
Rels::Rels() : hash_table(NULL), mask(0), num_slots(0), num_nodes(0), num_chains(0) { }

/// Constructor 2: overloaded
Rels::Rels(uint32_t size) :
		hash_table(new Rel * [size]),
		mask(size - 1),
		num_slots(size),
		num_nodes(0),
		num_chains(0) {

			for (uint32_t i = 0; i < size; i++) {
				this->hash_table[i] = NULL;
			}
}

/// Destructor
Rels::~Rels() {
	class Rel * q;
	if (this->hash_table) {
		for (uint32_t i = 0; i < this->num_slots; i++) {
			while (this->hash_table[i] != NULL) {
				q = this->hash_table[i]->get_next();
				delete this->hash_table[i];
				this->hash_table[i] = q;
			}
		}
		delete [] this->hash_table;
	}
}


/// Insert an element into the hash table
void Rels::insert(char * n, uint32_t j) {
	/// Find the hash value of the input term
	uint32_t HashValue = this->djb2(n) & this->mask;

	/// Now search in the hash table to check whether this term exists or not
	if (this->hash_table[HashValue] != NULL) {
		class Rel * q;

		/// Traverse the linked list that represents the chain.
		for (q = this->hash_table[HashValue]; q != NULL; q = q->get_next()) {
			if (strcmp(q->get_code(), n) == 0) {
				return; /// Return and exit
			}
		}
	} else {
		this->num_chains++;
	}

	this->num_nodes++;

	/// Create a new record and re-assign the linked list's head
	class Rel * record = new Rel(n, j);

	/// Reassign the chain's head
	record->set_next(this->hash_table[HashValue]);
	this->hash_table[HashValue] = record;
}


/// Search for an element in the hash table
bool Rels::search(char * n, uint32_t * r) {
	/// Find the hash value of the input term
	uint32_t HashValue = this->djb2(n) & this->mask;

	/// Now search in the hash table to check whether this term exists or not.
	if (this->hash_table[HashValue] != NULL) {
		class Rel * q;

		/// Traverse the linked list that represents the chain.
		for (q = this->hash_table[HashValue]; q != NULL; q = q->get_next()) {
			if (strcmp(q->get_code(), n) == 0) {
				*r = q->get_judgment();
				return true; /// Return and exit
			}
		}
	}
	*r = 0;
	return false;
}


/// Accessors
inline uint32_t Rels::get_num_nodes() { return this->num_nodes; }

void Rels::display() {
	class Rel * q;
	for (uint32_t i = 0; i < this->num_slots; i++) {
		if (this->hash_table[i] != NULL) {
			for (q = this->hash_table[i]; q != NULL; q = q->get_next()) {
				q->display();
//				getchar();
			}
		}
	}
}

/// The DJB2 Hash Function (Dan Bernstein)
uint32_t Rels::djb2(char * str) {
	unsigned long hash = 5381;
	int c;

	while ((c = *str++))
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

	return hash;
}
