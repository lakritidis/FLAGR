#include "InputItem.h"

/// Constructor 1: default
InputItem::InputItem() {
	this->code = NULL;
	this->rank = 0;
}

/// Constructor 2: overloaded
InputItem::InputItem(char * c, rank_t r) {
	this->copy_code(c);
	this->rank = r;
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
	printf("ITEM: %s, RANKING: %d\n", this->code, this->rank);
}

/// Mutators
void InputItem::set_code(char * v) { this->copy_code(v); }
void InputItem::set_rank(rank_t v) { this->rank = v; }

/// Accessors
char * InputItem::get_code() { return this->code; }
rank_t InputItem::get_rank() { return this->rank; }
