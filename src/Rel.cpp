#include "Rel.h"

/// Constructor 1 :default
Rel::Rel() {
	this->code = NULL;
}

/// Constructor 2
Rel::Rel(uint32_t i, char * c, uint32_t j) {
	this->topic_id = i;
	this->copy_code(c);
	this->judgment = j;
}

/// Interal copy code function
void Rel::copy_code(char * c) {
	if (c) {
		this->code = new char [strlen(c) + 1];
		strcpy(this->code, c);
	}
}

/// Destructor
Rel::~Rel() {
	if (this->code) {
		delete [] this->code;
	}
}


/// Mutators
inline void Rel::set_topic_id(uint32_t v) { this->topic_id = v; }
inline void Rel::set_judgment(uint32_t v) { this->judgment = v; }
inline void Rel::set_code(char * v) { this->copy_code(v); }
inline void Rel::set_next(class Rel * v) { this->next = v; }

/// Accessors
inline uint32_t Rel::get_topic_id() { return this->topic_id; }
inline char * Rel::get_code() { return this->code; }
inline uint32_t Rel::get_judgment() { return this->judgment; }
inline class Rel * Rel::get_next() { return this->next; }

/// itoa version for Linux Systems
/// to convert an integer number to a char variable.
void reverse(char s[]) {
	int32_t c = 0, i = 0, j = 0;

	for (i = 0, j = strlen(s)-1; i < j; i++, j--) {
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
}

/// Convert an Integer to an Alphanumeric buffer
void itoa_l(int32_t n, char s[]) {
	int32_t i = 0, sign = 0;

	if ((sign = n) < 0) {        /// record sign
		n = -n;                  /// make n positive
	}
	i = 0;
	do {                         /// generate digits in reverse order
		s[i++] = n % 10 + '0';   /// get next digit
	} while ((n /= 10) > 0);     /// delete it
	if (sign < 0) {
		s[i++] = '-';
	}
	s[i] = '\0';
	reverse(s);
}

/// Convert an Integer to an Alphanumeric buffer of fixed size of 6 characters
void itoa_lp (int32_t n, char s[]) {
	char t[10];
	itoa_l(n, t);

	int32_t i = 0, j = 0, len = strlen(t);

	for (i = 0; i < 6 - len; i++) {
		s[i] = 48;
	}

	for (j = i; j < 6; j++) {
		s[j] = t[j - i];
	}
	s[j] = 0;
}
