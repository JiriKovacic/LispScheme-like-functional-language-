#ifndef MEMORY_ITEM_H_INCLUDED
#define MEMORY_ITEM_H_INCLUDED

#include <stdio.h>

using namespace std;

class NumberItem 
{
private:
	int value;
public:
	NumberItem() {} 
	NumberItem(int value);	
	NumberItem operator=(int value);
	int getValue() { return value; }
};

class ConsItem
{
private:
	int indexHead;
	int indexTail;
public:
	ConsItem() {}
	ConsItem(int indexHead, int indexTail);
	int getHead() { return indexHead; }
	int getTail() { return indexTail; }
};

class ClosoureItem
{
private:
	int indexFunction;
	int indexEnvironment;
public:
	ClosoureItem() {}
	ClosoureItem(int indexFunction, int indexEnvironment);
	int getFunctionAddr() { return indexFunction; }
	int getEnvironmentAddr() { return indexEnvironment; }
};

class FunctionItem
{
private:
	int argsCount;
	int bytecodeLength;
	unsigned char* bytecode;
	unsigned char* argsNames;
public:
	FunctionItem() {}
	FunctionItem(int argsCount, int bytecodeLength, unsigned char* argsNames, unsigned char* bytecode);
	int getArgsCount() { return argsCount; }
	int getBytecode(unsigned char** bytecodeArray);
	int getArgsNames(unsigned char** argsArray);
};

class FileItem
{
private:
	FILE *pointer;
public:
	FileItem() { pointer = NULL; }
	FileItem(FILE *pointer);
	FILE* getPointer() { return pointer; }
};

// Promenna nebo funkce
class EnvironmentRow
{
public:
	int nameIndex;
	int itemAddress;

	EnvironmentRow() { }
	EnvironmentRow(int nameIndex, int itemAddress);
};

// EnvironmentItem je pole envRows
class EnvironmentItem
{
private:
	int rowCount;
	EnvironmentRow *envRows;
	int parentEnvAddr;
public:
	EnvironmentItem() { }
	EnvironmentItem(int rowCount, EnvironmentRow *envRows, int parentEnvAddr);
	int getEnvironment(EnvironmentRow** environment);
	void setEnvironment(EnvironmentRow** environment);
	void changeRowCount(int rowCount);
	int findAddressInMemory(int idx);
	int getParentEnvAddr() { return parentEnvAddr; }
};

#endif // MEMORY_ITEM_H_INCLUDED