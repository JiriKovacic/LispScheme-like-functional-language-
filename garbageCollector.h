#ifndef GARBAGECOLLECTOR_H_INCLUDED
#define GARBAGECOLLECTOR_H_INCLUDED

#include <stack>
#include <map>
#include <utility>

#include "globalEnv.h"
#include "virtualMachine.h"

using namespace std;

extern GlobalEnvTable *globalEnvTable;
extern VirtualMachine *virtMachine;

class GC{
private:
	map<int, int> mapp; // oldAddr, newAddr
	map<int, int>::iterator iter;

	unsigned int allocPtr; // celkem alokovane nove pameti
	unsigned int scanPtr;
	//unsigned char *gcTempMemory; 
	stack<int> tempStack; // zde budou docasne adresy hodnot na novou pamet, az bude GC u konce, tak se cele nakopiruje do globalStacku
	void readRoot();
	void pushToGlobalStack();
	bool checkFreeSpace(unsigned int *memoryIndex, int spaceSize);
	MemType getType(int headAddr);
	unsigned int copyNumber(int addr);
	unsigned int copyCons(int addr);
	unsigned int copyClosoure(int addr);
	unsigned int copyFunction(int addr);
	unsigned int copyFile(int addr);
	unsigned int copyEnvironment(int addr);
	unsigned int copyTrue(int addr);
	unsigned int copyFalse(int addr);
	unsigned int copyNil(int addr);

	unsigned int storeEnvironment(int rowCnt, EnvironmentRow *rows, int parentEnvAddr);
	unsigned int storeFunction(int argCnt, int bcLen, unsigned char *argsNames, unsigned char *bytecode);
	unsigned int storeClosoure(int functionIdx, int envIdx);
	unsigned int storeCons(int headIdx, int tailIdx);
	unsigned int storeNumber(int addr);
	unsigned int storeFile(int addr);
	//void copyGlobalVars();
	void copyLocalVars();
	
public:
	GC(unsigned int allocPtrMemoryIdx);
	void Run();
};

#endif // GARBAGECOLLECTOR_H_INCLUDED