#include "memoryItem.h"
#include "translationTable.h"

extern TranslationTable *translationTable;

NumberItem::NumberItem(int value)
{
	this->value = value;
}

NumberItem NumberItem::operator=(int value)
{
	NumberItem item;
	item.value = value;
	return item;
}

ConsItem::ConsItem(int indexHead, int indexTail)
{
	this->indexHead = indexHead;
	this->indexTail = indexTail;
}

ClosoureItem::ClosoureItem(int indexFunction, int indexEnvironment) {
	this->indexFunction = indexFunction;
	this->indexEnvironment = indexEnvironment;
}

FunctionItem::FunctionItem(int argsCount, int bytecodeLength, unsigned char* argsNames, unsigned char* bytecode) {
	this->argsCount = argsCount;
	this->bytecodeLength = bytecodeLength;
	this->argsNames = argsNames;
	this->bytecode = bytecode;
}

FileItem::FileItem(FILE *pointer)
{
	this->pointer = pointer;
}

int FunctionItem::getBytecode(unsigned char** bytecodeArray) {
	unsigned char *localBytecodeArray = new unsigned char[1024];
	for (int i = 0; i < this->bytecodeLength; i++) {
		localBytecodeArray[i] = this->bytecode[i];
	}
	*bytecodeArray = localBytecodeArray;
	return this->bytecodeLength;
}

int FunctionItem::getArgsNames(unsigned char** argsArray) {
	unsigned char *localArgsArray = new unsigned char[1024];
	for (int i = 0; i < this->argsCount; i++) {
		localArgsArray[i] = this->argsNames[i];
	}
	*argsArray = localArgsArray;
	return this->argsCount;
}

EnvironmentItem::EnvironmentItem(int rowCount, EnvironmentRow *envRows, int parentEnvAddr) {
	this->rowCount = rowCount;
	this->envRows = envRows;
	this->parentEnvAddr = parentEnvAddr;
}

int EnvironmentItem::getEnvironment(EnvironmentRow** environment) {
	EnvironmentRow *localEnv = new EnvironmentRow[255];
	for (int i = 0; i < this->rowCount; i++) {
		localEnv[i] = this->envRows[i];
	}
	*environment = localEnv;
	return this->rowCount;
}

void EnvironmentItem::setEnvironment(EnvironmentRow** environment) {	
	for (int i = 0; i < this->rowCount; i++) 
	{
		envRows[i] = (*environment)[i];
	}
}

void EnvironmentItem::changeRowCount(int rowCount)
{
	this->rowCount = rowCount;
}

EnvironmentRow::EnvironmentRow(int nameIndex, int itemAddress) {
	this->nameIndex = nameIndex;
	this->itemAddress = itemAddress;
}

int EnvironmentItem::findAddressInMemory(int idx) {
	for (int i = 0; i < rowCount; i++) {
		if (envRows[i].nameIndex == translationTable->findIndex(idx)) {
			return envRows[i].itemAddress;
		}
	}
	return -1;
}