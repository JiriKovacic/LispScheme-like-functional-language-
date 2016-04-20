#ifndef FRAME_H_INCLUDED
#define FRAME_H_INCLUDED

#include <vector>
#include "globalEnv.h"
#include "ast.h"
#include "reader.h"
#include "astBuilder.h"
#include "bytecodeGenerator.h"

using namespace std;

const unsigned int MAXIMAL_FRAME_ARRAY_SIZE(100);
extern GlobalEnvTable *globalEnvTable;

class Frame {
private:
	// Promenne
	unsigned char *bytecode;
	int bytecodeLength;
	int localVariablesCount;
	unsigned char *localVariablesNames;
	int actualIndex;
	vector<int> localVariables;
	int environmentAddress;

	// Metody
	int interpretNextByte(int index); // cela logika interpretace bytecodu, vraci index dalsi instrukce
	void insertLocalVariables(vector<int> *localVariables, int numArgs);
	void pushConstant(unsigned char cA, unsigned char cB, unsigned char cC, unsigned char cD);
	void pushArgument(unsigned char idx);
	void pushCons();
	void calculateMathOperation(instructionsEnum opType);
	int evaluateJump(instructionsEnum jumpType, unsigned char jumpSize);
	void evaluateEquality(instructionsEnum eqType);
	void evaluateAndOr(instructionsEnum type);
	void evaluateConsModifiers(instructionsEnum type);
	void callFunction(int index);
	void callClosoure(int index);
	void defineVariable(unsigned char index);
	int defineFunction(unsigned char * bytecode, int index);
	int defineLambda(unsigned char * bytecode, int index);
	void fileOpen(unsigned char index);
	void fileClose(unsigned char index);
	void fileWrite(unsigned char index);
	void fileLoad(unsigned char index);
public:
	Frame(int parentEnvAddr);
	void loadBytecodeGlobalEnvTable(int funcIndex);
	void loadBytecodeProgram(unsigned char * bytecode, int bytecodeLength);
	bool intrepretAll();
	vector<int> getLocalVariablesAdress();
	void setLocalVariableAddress(int index, int addr);
	int getEvnironmentAddress();
	void setEvnironmentAddress(int addr);
	int findAddressInMemory(int idx);
};

#endif // FRAME_H_INCLUDED
