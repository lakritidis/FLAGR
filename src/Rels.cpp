#include "Rels.h"

/// Constructor 1 : Default
Rels::Rels() {
	this->hash_table = NULL;
	this->num_nodes = 0;
	this->num_chains = 0;
	this->num_slots = 0;
	this->mask = 0;
}

/// Constructor 2: overloaded
Rels::Rels(uint32_t size) {
	this->hash_table = new Rel * [size];

	for (uint32_t i = 0; i < size; i++) {
		this->hash_table[i] = NULL;
	}

	this->num_nodes = 0;
	this->num_chains = 0;
	this->num_slots = size;
	this->mask = size - 1;
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
void Rels::insert(uint32_t i, char * n, uint32_t j) {
	/// Find the hash value of the input term
	uint32_t HashValue = KazLibHash(n) & this->mask;

	/// Now search in the hash table to check whether this term exists or not
	if (this->hash_table[HashValue] != NULL) {
		class Rel * q;

		/// Traverse the linked list that represents the chain.
		for (q = this->hash_table[HashValue]; q != NULL; q = q->get_next()) {
			if (strcmp(q->get_code(), n) == 0 && q->get_topic_id() == i) {
				return; /// Return and exit
			}
		}
	} else {
		this->num_chains++;
	}

	this->num_nodes++;

	/// Create a new record and re-assign the linked list's head
	class Rel * record = new Rel(i, n, j);

	/// Reassign the chain's head
	record->set_next(this->hash_table[HashValue]);
	this->hash_table[HashValue] = record;
}

/// Search for an element in the hash table
bool Rels::search(uint32_t i, char * n, uint32_t * r) {
	/// Find the hash value of the input term
	uint32_t HashValue = KazLibHash(n) & this->mask;

	/// Now search in the hash table to check whether this term exists or not
	if (this->hash_table[HashValue] != NULL) {
		class Rel * q;

		/// Traverse the linked list that represents the chain.
		for (q = this->hash_table[HashValue]; q != NULL; q = q->get_next()) {
			if (strcmp(q->get_code(), n) == 0 && q->get_topic_id() == i) {
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

void Rels::display(uint32_t topic_id) {
	class Rel * q;
	for (uint32_t i = 0; i < this->num_slots; i++) {
		if (this->hash_table[i] != NULL) {
			for (q = this->hash_table[i]; q != NULL; q = q->get_next()) {
				if (q->get_topic_id() == topic_id) {
					q->display();
				}
//				getchar();
			}
		}
	}
}

/// The Hash Function
uint32_t Rels::KazLibHash (char *key) {
   static unsigned long randbox[] = {
       0x49848f1bU, 0xe6255dbaU, 0x36da5bdcU, 0x47bf94e9U,
       0x8cbcce22U, 0x559fc06aU, 0xd268f536U, 0xe10af79aU,
       0xc1af4d69U, 0x1d2917b5U, 0xec4c304dU, 0x9ee5016cU,
       0x69232f74U, 0xfead7bb3U, 0xe9089ab6U, 0xf012f6aeU,
	};

	char *str = key;
	uint32_t acc = 0;

	while (*str) {
		acc ^= randbox[(*str + acc) & 0xf];
		acc = (acc << 1) | (acc >> 31);
		acc &= 0xffffffffU;
		acc ^= randbox[((*str++ >> 4) + acc) & 0xf];
		acc = (acc << 2) | (acc >> 30);
		acc &= 0xffffffffU;
	}
	return acc;
}
