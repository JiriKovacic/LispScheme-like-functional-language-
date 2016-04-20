#include "astBuilder.h"

AstBuilder::AstBuilder(Reader *reader){
	this->reader = reader;
}

SchemeObject* AstBuilder::getNextAst(){
	int bracketCounter = 0;
	SchemeObject* actualParent = NULL;
	SchemeObject* actualObject = NULL;
	Token actualToken, lastToken;
	actualToken.type = T_NULL;
	while(true){
		lastToken = actualToken;
		actualToken = reader->getNextToken();
		switch(actualToken.type){
			case T_FUNCTION:
				if(lastToken.type == T_LEFT_BRACKET){
					actualObject = new SchemeFunction(actualToken.name, actualParent);
					if(actualParent != NULL){
						actualParent->addArgument(actualObject);
					} 
					actualParent = actualObject;
				}
				else{
					actualObject = new SchemeFunction(actualToken.name, actualParent);
					if(actualParent != NULL){
						actualParent->addArgument(actualObject);
					}
				}
				break;
			case T_VARIABLE:
				if(lastToken.type == T_LEFT_BRACKET){
					actualObject = new SchemeVariable(actualToken.name, actualParent);
					if(actualParent != NULL){
						actualParent->addArgument(actualObject);
					} 
					actualParent = actualObject;
				}
				else{
					actualObject = new SchemeVariable(actualToken.name, actualParent);
					if(actualParent != NULL){
						actualParent->addArgument(actualObject);
					} 
				}
				break;
			case T_KEYWORD:
				if(lastToken.type == T_LEFT_BRACKET){
					actualObject = new SchemeKeyword(actualToken.name, actualParent);
					if(actualParent != NULL){
						actualParent->addArgument(actualObject);
					}
					actualParent = actualObject;
				}
				else{
					actualObject = new SchemeKeyword(actualToken.name, actualParent);
					if(actualParent != NULL){
						actualParent->addArgument(actualObject);
					} 
				}
				break;
			case T_NUMBER:
				if(lastToken.type == T_LEFT_BRACKET){
					actualObject = new SchemeConstant(actualToken.name, actualParent);
					if(actualParent != NULL){
						actualParent->addArgument(actualObject);
					} 
					actualParent = actualObject;
				}
				else{
					actualObject = new SchemeConstant(actualToken.name, actualParent);
					if(actualParent != NULL){
						actualParent->addArgument(actualObject);
					} 
				}
				break;
			case T_LEFT_BRACKET:
				bracketCounter++;
				break;
			case T_RIGHT_BRACKET:
				bracketCounter--;
				if(bracketCounter == 0) return actualParent;
				actualParent = actualParent->parent;
				break;
			case T_EOF:
				return NULL;
				break;
		}
	}
}