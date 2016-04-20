#ifndef KEYWORDS_H_INCLUDED
#define KEYWORDS_H_INCLUDED

#include <string>

using namespace std;

#define NUMER_OF_KEYWORDS 28

enum keywordsEnum {
    KW_DEFINE,
	KW_DEFVAR,
    KW_IF,
    KW_LS,
    KW_LEQ, 
    KW_EQ,
	KW_GT,
    KW_GEQ,
    KW_NEQ,
	KW_PLUS,
	KW_MINUS,
	KW_TIMES,
	KW_DIVIDE,
    KW_CAR,
    KW_CDR,
    KW_CONS,
    KW_LIST,
    KW_TRUE,
    KW_FALSE, 
    KW_NIL,
	KW_AND,
	KW_OR,
	KW_BEGIN,
	KW_LAMBDA,
	KW_FILE_OPEN,
	KW_FILE_CLOSE,
	KW_FILE_LOAD,
	KW_FILE_WRITE
};

extern string keywordsNames[NUMER_OF_KEYWORDS];

#endif // KEYWORDS_H_INCLUDED