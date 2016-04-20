#include "reader.h"

using namespace std;

Reader::Reader(string input){
	this->input = input;
	actualPosition = 0;
	actualChar = nextChar();
	skipWhiteSpaces();
}

Token Reader::getNextToken(){
	char nameChars[256];
	string nameString;
	int counter = 0;
	while(true){
		if(isWhiteSpace() == true){
			skipWhiteSpaces();
			// END TOKEN
			if(counter > 0){
				nameChars[counter] = 0;
				nameString = string(nameChars);
				return createToken(nameString);
			}
			else{
				Token token;
				token.type = T_NULL;
				return token;
			}
		}
		else if(actualChar == EOF){
			// END TOKEN
			if(counter > 0){
				nameChars[counter] = 0;
				nameString = string(nameChars);
				return createToken(nameString);
			}
			else{
				Token token;
				token.type = T_EOF;
				return token;
			}
		}
		else if(actualChar == '('){
			// END TOKEN
			if(counter > 0){
				nameChars[counter] = 0;
				nameString = string(nameChars);
				return createToken(nameString);
			}
			else{
				actualChar = nextChar();
				skipWhiteSpaces();
				Token token;
				token.type = T_LEFT_BRACKET;
				return token;
			}
		}
		else if(actualChar == ')'){
			// END TOKEN
			if(counter > 0){
				nameChars[counter] = 0;
				nameString = string(nameChars);
				return createToken(nameString);
			}
			else{
				actualChar = nextChar();
				skipWhiteSpaces();
				Token token;
				token.type = T_RIGHT_BRACKET;
				return token;
			}
		}
		else{
			nameChars[counter] = actualChar;
			counter++;
			actualChar = nextChar();
		}
	}
}

bool Reader::isWhiteSpace(){
	if(actualChar == ' ' || actualChar == '\n' || actualChar == '\t') return true;
	return false;
}

void Reader::skipWhiteSpaces(){
	while(isWhiteSpace() == true && actualChar != EOF){
		actualChar = nextChar();
	}
}

Token Reader::createToken(string name){
	Token token;
	token.name = name;
	if(isFunction(name) == true){
		token.type = T_FUNCTION;
	}
	else if(isKeyword(name) == true){
		token.type = T_KEYWORD;
	}
	else if(isNumber(name) == true){
		token.type = T_NUMBER;
	}
	else{
		token.type = T_VARIABLE;
	}
	return token;
}

bool Reader::isNumber(string name){
	for(int i = 0; i < name.length(); i++){
		if(i == 0 && name[0] == '-') continue;
		if(name[i] < '0' || name[i] > '9') return false;
	}
	return true;
}

bool Reader::isKeyword(string name){
	for(int i = 0; i < NUMER_OF_KEYWORDS; i++){
		if(keywordsNames[i] == name) return true;
	}
	return false;
}

bool Reader::isFunction(string name){
	int idx = globalEnvTable->findFunction(name);
	if (idx < 0) return false;
	return globalEnvTable->isFunction(idx);
}

int Reader::nextChar() {
	if (actualPosition >= input.size()) {
		return EOF;
	}
	return input[actualPosition++];
}

string tokensNames[NUMER_OF_TOKENS] = {"FUNCTION", "VARIABLE", "KEYWORD", "NUMBER", "LEFT_BRACKET", "RIGHT_BRACKET", "EOF", "NULL" };