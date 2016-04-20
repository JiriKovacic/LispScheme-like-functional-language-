#ifndef AST_H_INCLUDED
#define AST_H_INCLUDED

#define JUMP_ADDR_OFFSET 127

#include <cstddef>
#include <fstream>
#include <iostream>
#include "keywords.h"
#include "globalEnv.h"

using namespace std;

extern GlobalEnvTable *globalEnvTable;
extern string *defineFunctionArgsArray;
extern string *defineLambdaFunctionArgsArray;
extern int defineFunctionArgCnt;
extern int defineLambdaFunctionArgCnt;
extern bool isDefinedFunction;

class SchemeObject {
protected:
	SchemeObject(SchemeObject *parent);
	SchemeObject *args[32]; // staticke pole ukazatelu na schemeObjekty
	int argCounter;
public:
	SchemeObject *parent;
	void addArgument(SchemeObject *argument);
	virtual int generateBytecode(unsigned char **bytecodeArray) = 0;
	virtual void printGraphviz() = 0;
	//virtual bool isValid() = 0;
	int getArgsCnt() { return argCounter; }
	SchemeObject *getArg(int position);
};

class SchemeFunction : public SchemeObject {
private:
	int globalEvnIndex;
	int translationTableIndex;
public:
	SchemeFunction(string name, SchemeObject *parent);
	void printGraphviz();
	//bool isValid();
	int generateBytecode(unsigned char **bytecodeArray);
};

class SchemeKeyword : public SchemeObject {
private:
	keywordsEnum name;
	int defineFunction(string functionName, SchemeObject *astRoot, unsigned char **bytecodeArray);
	int defineVariable(string varName, SchemeObject *astRoot, unsigned char **bytecodeArray);
	int defineLambda(SchemeObject *astRoot, unsigned char **bytecodeArray);
	int generateIfBc(SchemeObject *cond, SchemeObject *ifPart, SchemeObject *elsePart, unsigned char **bytecodeArray);
	int generateCondition(instructionsEnum instrType, SchemeObject *obj1, SchemeObject *obj2, unsigned char **bytecodeArray);
	int generateCondCombinator(instructionsEnum instrType, SchemeObject *args[], int argsCount, unsigned char **bytecodeArray);
	int generateBegin(SchemeObject *args[], int argsCount, unsigned char **bytecodeArray);
	int generateCons(SchemeObject *obj1, SchemeObject *obj2, unsigned char **bytecodeArray);
	int generateList(SchemeObject *args[], int argsCount, unsigned char **bytecodeArray);
	int generateListModifier(instructionsEnum instrType, SchemeObject *obj, unsigned char **bytecodeArray);
	int generateMathOperation(instructionsEnum instrType, SchemeObject *args[], int argsCount, unsigned char **bytecodeArray);
	int generateFileWrite(int fileIndex, SchemeObject *astRoot, unsigned char **bytecodeArray);
public:
	SchemeKeyword(string name, SchemeObject *parent);
	void printGraphviz();
	//bool isValid();
	int generateBytecode(unsigned char **bytecodeArray);
};

class SchemeConstant : public SchemeObject {
private:
	int constValue;
public:
	SchemeConstant(string valueStr, SchemeObject *parent);
	void printGraphviz();
	//bool isValid();
	int generateBytecode(unsigned char **bytecodeArray);
};

class SchemeVariable : public SchemeObject {
private:
	string name;
public:
	SchemeVariable(string name, SchemeObject *parent);
	void printGraphviz();
	//bool isValid();
	int generateBytecode(unsigned char **bytecodeArray);
	string getName() { return name; }
};

#endif // AST_H_INCLUDED