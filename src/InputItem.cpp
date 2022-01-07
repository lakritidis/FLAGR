#include "InputItem.h"

/// Constructor 1: default
InputItem::InputItem() {
	this->code = NULL;
	this->rank = 0;
	this->index = 0;
}

/// Constructor 2: overloaded
InputItem::InputItem(char * c, rank_t r, rank_t i) {
	this->copy_code(c);
	this->rank = r;
	this->index = i;
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
	printf("ITEM %d: %s, RANKING: %d\n", this->index, this->code, this->rank);
}

/// Mutators
void InputItem::set_code(char * v) { this->copy_code(v); }
void InputItem::set_rank(rank_t v) { this->rank = v; }
void InputItem::set_index(rank_t v) { this->index = v; }

/// Accessors
char * InputItem::get_code() { return this->code; }
rank_t InputItem::get_rank() { return this->rank; }
rank_t InputItem::get_index() { return this->index; }
