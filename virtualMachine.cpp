#include "virtualMachine.h"

// Inicializace interpretra - memory, stack, frameArray
VirtualMachine::VirtualMachine()
{
	memory = new Memory();
	gStack = new GlobalStack();
	frameArray = new Frame*[MAXIMAL_FRAME_ARRAY_SIZE];
	arrayIndex = 0;
}

void VirtualMachine::startInterpret()
{	
	frameArray[arrayIndex-1]->intrepretAll();
}

void VirtualMachine::pushAddressToStack(int addr) {
	gStack->pushInt(addr);
}

int VirtualMachine::popAddressFromStack() {
	return gStack->popInt();
}

void VirtualMachine::addFrame(Frame *frame) {
	frameArray[arrayIndex++] = frame;
}

void VirtualMachine::removeFrame() {
	arrayIndex--;
}

string VirtualMachine::getResult() {
	int addrResult = popAddressFromStack();
	return memory->valueToString(addrResult);
}

unsigned char VirtualMachine::getByteValue(int addr)
{
	return memory->getByteValue(addr);
}

int VirtualMachine::stackSize()
{
	return gStack->getSize();
}

void VirtualMachine::setMemory(unsigned int memoryIndex)
{
	this->getMemory()->setMemory(memoryIndex);
}

int VirtualMachine::frameArraySize()
{
	return this->arrayIndex;
}

Frame* VirtualMachine::getFrame(int arrayIndex)
{
	return frameArray[arrayIndex];
}

int VirtualMachine::getActualEnvironmentAddress() {
	return frameArray[arrayIndex - 2]->getEvnironmentAddress();
}

int VirtualMachine::getSaveLoadEnvironmentAddress() {
	return frameArray[arrayIndex - 3]->getEvnironmentAddress();
}

int VirtualMachine::getClosoureEnvironmentAddress() {
	return frameArray[arrayIndex - 1]->getEvnironmentAddress();
}

Frame* VirtualMachine::getActualFrame() {
	return frameArray[arrayIndex - 1];
}

void VirtualMachine::setActualEnvironmentAddress(int addr) {
	frameArray[arrayIndex - 2]->setEvnironmentAddress(addr);
}

void VirtualMachine::setSaveLoadEnvironmentAddress(int addr) {
	frameArray[arrayIndex - 3]->setEvnironmentAddress(addr);
}