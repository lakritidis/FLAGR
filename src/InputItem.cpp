#include "InputItem.h"

/// Default Constructor
InputItem::InputItem() : idx(0), code(NULL), rank(0), inscore(0.0), pscore(0.0) { }

/// Constructor 2: overloaded
InputItem::InputItem(uint32_t i, char * c, rank_t r, score_t s, score_t p) :
	idx(i), code(NULL), rank(r), inscore(s), pscore(p) {
		this->set_code(c);
}

/// Destructor
InputItem::~InputItem() {
	if (this->code) {
		delete [] this->code;
	}
}

/// Display InputItem Object
void InputItem::display() {
	printf("Item idx: %d, Code: %s, Ranking: %d, In-Score: %5.3f, Preserv-Score: %5.3f\n",
		this->idx, this->code, this->rank, this->inscore, this->pscore);
}

/// Mutators
void InputItem::set_idx(uint32_t v) { this->idx = v; }
void InputItem::set_code(char * v) {
	this->code = new char[strlen(v) + 1];
	strcpy(this->code, v);
}
void InputItem::set_rank(rank_t v) { this->rank = v; }
void InputItem::set_inscore(score_t v) { this->inscore = v; }
void InputItem::set_pscore(score_t v) { this->pscore = v; }

/// Accessors
uint32_t InputItem::get_idx() { return this->idx; }
char * InputItem::get_code() { return this->code; }
rank_t InputItem::get_rank() { return this->rank; }
score_t InputItem::get_inscore() { return this->inscore; }
score_t InputItem::get_pscore() { return this->pscore; }
