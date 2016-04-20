#ifndef MEMORY_H_INCLUDED
#define MEMORY_H_INCLUDED

#include "memoryType.h"
#include "memoryItem.h"

using namespace std;

const unsigned int MAXIMAL_MEMORY_SIZE(1048576); // 1MB

class Memory {
private:
	unsigned char *memory;
	unsigned int memoryIndex; // posledne pouzity index pri zapisu do pameti
public:
	Memory();
	MemType getType(int index);
	NumberItem loadNumbers(int index);
	ConsItem loadCons(int index);
	ClosoureItem loadClosoure(int index);
	FunctionItem loadFunction(int index);
	FileItem loadFile(int index);
	//GlobalVariableItem loadGlobalVariable(int index);
	EnvironmentItem loadEnvironment(int index);
	unsigned int storeNumber(int numValue); // vraci index v pameti, kam se to vlozilo
	unsigned int storeCons(int headIdx, int tailIdx); // vraci index v pameti, kam se to vlozilo
	unsigned int storeClosoure(int functionIdx, int envIdx);  //	--//--
	unsigned int storeFunction(int argCnt, int bcLen, unsigned char *argsNames, unsigned char *bytecode);  //	--//--
	//unsigned int storeGlobalVariable(int argCnt, int bcLen, unsigned char *bytecode);  //	--//--
	unsigned int storeEnvironment(int rowCnt, EnvironmentRow *rows, int parentEnvAddr);  //	--//--
	unsigned int storeTrue();  //	--//--
	unsigned int storeFalse(); //	--//--
	unsigned int storeNil(); //		--//--
	unsigned int storeFile(FILE *pointer);
	string valueToString(int index);
	unsigned char getByteValue(int addr);
	void setMemory(/*unsigned char **newMemory, */unsigned int memoryIndex);
	void GCwrite(unsigned int addr, unsigned char value);
};

#endif // MEMORY_H_INCLUDED