#include "globalEnv.h"

GlobalEnvTable::GlobalEnvTable(){
	funcCounter = 0;
}

int GlobalEnvTable::findFunction(string name){
	for(int i = 0; i < funcCounter; i++){
		if(functions[i].itemName == name) return i;
	}
	return -1;
}

string GlobalEnvTable::getName(int id){
	if(id >= funcCounter) return "";
	return functions[id].itemName;
}

int GlobalEnvTable::getBytecode(int index, unsigned char **bc){
	if (index < 0 || index >= funcCounter) {
		*bc = NULL;
		return 0;
	}
	*bc = functions[index].bytecode;
	return functions[index].bytecodeLength;
}

int GlobalEnvTable::getNumberOfArgs(int index){
	if(index < 0 || index >= funcCounter) return -1;
	return functions[index].argCount;
}

bool GlobalEnvTable::isFunction(int index) {
	return functions[index].isFunction;
}

void GlobalEnvTable::reserveFunctionName(string name) {
	int idx = findFunction(name);
	if (idx >= 0) {
		functions[idx].isFunction = true;
		return;
	}
	EnvItem item;
	item.isFunction = true;
	item.itemName = name;
	item.bytecode = NULL;
	item.bytecodeLength = 0;
	item.argCount = 0;
	functions[funcCounter] = item;
	funcCounter++;
}

void GlobalEnvTable::reserveVariableName(string name) {
	int idx = findFunction(name);
	if (idx >= 0) {
		functions[idx].isFunction = false;
		return;
	}
	EnvItem item;
	item.isFunction = false;
	item.itemName = name;
	item.address = -1;
	functions[funcCounter] = item;
	funcCounter++;
}

void GlobalEnvTable::saveFunction(EnvItem item) {
	int idx = findFunction(item.itemName);
	if (idx >= 0) {
		functions[idx] = item;
	}
	else {
		functions[funcCounter] = item;
		funcCounter++;
	}
}

void GlobalEnvTable::saveVariable(int index, int addr) {
	functions[index].isFunction = false;
	functions[index].address = addr;
}

int GlobalEnvTable::getAddress(int index) {
	return functions[index].address;
}

void GlobalEnvTable::setAddress(int index, int address)
{
	functions[index].address = address;
}

int GlobalEnvTable::getSize()
{
	return this->funcCounter;
}