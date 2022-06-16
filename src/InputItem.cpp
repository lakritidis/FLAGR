#include "InputItem.h"

/// Default Constructor
InputItem::InputItem() : code(NULL), rank(0), inscore(0.0) { }

/// Constructor 2: overloaded
InputItem::InputItem(char * c, rank_t r, score_t s) : code(NULL), rank(r), inscore(s) {
	this->copy_code(c);
}

/// Destructor
InputItem::~InputItem() {
	if (this->code) {
		delete [] this->code;
	}
}

/// Copy the input code into the local buffer
void InputItem::copy_code(char *v) {
	this->code = new char[strlen(v) + 1];
	strcpy(this->code, v);
}

/// Display InputItem Object
void InputItem::display() {
	printf("ITEM: %s, RANKING: %d, SCORE: %5.3f\n", this->code, this->rank, this->inscore);
}

/// Mutators
void InputItem::set_code(char * v) { this->copy_code(v); }
void InputItem::set_rank(rank_t v) { this->rank = v; }
void InputItem::set_inscore(score_t v) { this->inscore = v; }

/// Accessors
char * InputItem::get_code() { return this->code; }
rank_t InputItem::get_rank() { return this->rank; }
score_t InputItem::get_inscore() { return this->inscore; }
