#ifndef READER_H_INCLUDED
#define READER_H_INCLUDED

#include <cstdio>
#include "globalEnv.h"
#include "keywords.h"

using namespace std;

#define NUMER_OF_TOKENS 8

extern GlobalEnvTable *globalEnvTable;

enum tokensEnum { T_FUNCTION, T_VARIABLE, T_KEYWORD, T_NUMBER, T_LEFT_BRACKET, T_RIGHT_BRACKET, T_EOF, T_NULL };
extern string tokensNames[NUMER_OF_TOKENS];

class Token {
	public:
		string name;
		tokensEnum type;
};

class Reader {
	private:
		string input;
		int actualPosition;
		int actualChar;
		bool isWhiteSpace();
		void skipWhiteSpaces();
		Token createToken(string name);
		bool isNumber(string name);
		bool isKeyword(string name);
		bool isFunction(string name);
		int nextChar();
	public:
		Reader(string input);
		Token getNextToken();	
};

#endif // READER_H_INCLUDED