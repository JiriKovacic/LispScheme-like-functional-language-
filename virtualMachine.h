#ifndef VIRTUAL_MACHINE_H_INCLUDED
#define VIRTUAL_MACHINE_H_INCLUDED

#include "memory.h"
#include "globalStack.h"
#include "frame.h"

using namespace std;

class VirtualMachine {
private:
	Memory *memory;
	GlobalStack *gStack;
	Frame **frameArray;
	int arrayIndex;

public:
	VirtualMachine();
	void startInterpret();
	void pushAddressToStack(int addr);
	int popAddressFromStack();
	int stackSize();
	void addFrame(Frame *frame);
	void removeFrame();
	string getResult();
	Memory* getMemory() { return memory; }
	void setMemory(unsigned int memoryIndex);
	unsigned char getByteValue(int addr);
	int frameArraySize();
	Frame* getFrame(int arrayIndex);
	Frame* getActualFrame();
	int getActualEnvironmentAddress();
	int getSaveLoadEnvironmentAddress();
	int getClosoureEnvironmentAddress();
	void setActualEnvironmentAddress(int addr);
	void setSaveLoadEnvironmentAddress(int addr);
};

#endif // VIRTUAL_MACHINE_H_INCLUDED