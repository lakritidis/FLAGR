#include "Rel.h"

/// Constructor 1 :default
Rel::Rel() : code(NULL), judgment(0), next(NULL) { }

/// Constructor 2
Rel::Rel(char * c, uint32_t j) : code(NULL), judgment(j), next(NULL) {
	this->copy_code(c);
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

void Rel::display() {
	printf("\t%s Relevance: %d\n", this->code, this->judgment);
}

/// Mutators
inline void Rel::set_judgment(uint32_t v) { this->judgment = v; }
inline void Rel::set_code(char * v) { this->copy_code(v); }
inline void Rel::set_next(class Rel * v) { this->next = v; }

/// Accessors
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

float str_to_float(char *arr){
    int i,j,flag;
    float val;
    char c;
    i=0;
    j=0;
    val=0;
    flag=0;
    while ((c = *(arr+i))!='\0'){
//      if ((c<'0')||(c>'9')) return 0;
        if (c!='.'){
            val =(val*10)+(c-'0');
            if (flag == 1){
                --j;
            }
        }
        if (c=='.'){ if (flag == 1) return 0; flag=1;}
        ++i;
    }
    val = val*pow(10,j);
    return val;
}
