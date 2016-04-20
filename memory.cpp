#include <cstdio>

#include "memory.h"
#include "memoryType.h"
#include "translationTable.h"
#include "garbageCollector.h"

extern TranslationTable *translationTable;

void initializeMemory(unsigned char **memory)
{
	for (unsigned int i = 0; i < MAXIMAL_MEMORY_SIZE; i++)
		(*memory)[i] = MEM_EMPTY;
}

Memory::Memory()
{
	memory = new unsigned char[MAXIMAL_MEMORY_SIZE](); // 4 MiB
	initializeMemory(&memory);
	memoryIndex = 0;
}

MemType Memory::getType(int index)
{
	switch (memory[index])
	{
	case MEM_NUMBER:
		return MEM_NUMBER;
	case MEM_CONS:
		return MEM_CONS;
	case MEM_CLOSOURE:
		return MEM_CLOSOURE;
	case MEM_FUNCTOIN:
		return MEM_FUNCTOIN;
	case MEM_ENVIRONMENT:
		return MEM_ENVIRONMENT;
	case MEM_TRUE:
		return MEM_TRUE;
	case MEM_FALSE:
		return MEM_FALSE;
	case MEM_NIL:
		return MEM_NIL;
	case MEM_EMPTY:
		return MEM_EMPTY;
	case MEM_FILE:
		return MEM_FILE;
	default:
		return MEM_EMPTY;
	}
	return MEM_EMPTY;
}

bool isNumber(int *index, unsigned char **memory)
{
	if ((*memory)[*index] == MEM_NUMBER)
		return true;
	else
		return false;
}

bool isFile(int *index, unsigned char **memory)
{
	if ((*memory)[*index] == MEM_FILE)
		return true;
	else
		return false;
}

bool isCons(int *index, unsigned char **memory)
{
	if ((*memory)[*index] == MEM_CONS)
		return true;
	else
		return false;
}

bool isClosoure(int *index, unsigned char **memory)
{
	if ((*memory)[*index] == MEM_CLOSOURE)
		return true;
	else
		return false;
}

bool isFunction(int *index, unsigned char **memory)
{
	if ((*memory)[*index] == MEM_FUNCTOIN)
		return true;
	else
		return false;
}

bool isEnvironment(int *index, unsigned char **memory)
{
	if ((*memory)[*index] == MEM_ENVIRONMENT)
		return true;
	else
		return false;
}

// Load from memory
// Cisla maji velikost 4B
NumberItem Memory::loadNumbers(int index)
{
	NumberItem numItem;
	int numberToRetreive = 0;
	if (isNumber(&index, &memory) == true)
	{
		for (int i = ++index; i < index + 4; i++)
		{
			numberToRetreive = (numberToRetreive << 8) + memory[i];
		}
		numItem = NumberItem(numberToRetreive);
	}
	else 
		return NULL;
	return numItem;
}

ConsItem Memory::loadCons(int index)
{
	ConsItem consItem;
	int numberToRetreiveHead = 0;
	int numberToRetreiveTail = 0;
	if (isCons(&index, &memory) == true)
	{
		for (int i = ++index; i < index + 4; i++)
		{
			numberToRetreiveHead = (numberToRetreiveHead << 8) + memory[i];
		}
		index += 3;
		for (int i = ++index; i < index + 4; i++)
		{
			numberToRetreiveTail = (numberToRetreiveTail << 8) + memory[i];
		}
		consItem = ConsItem(numberToRetreiveHead, numberToRetreiveTail);
	}
	else
		return ConsItem(-1,-1);
	return consItem;
}

ClosoureItem Memory::loadClosoure(int index) {
	ClosoureItem closoureItem;
	int numberToRetreiveFunc = 0;
	int numberToRetreiveEnv = 0;
	if (isClosoure(&index, &memory) == true)
	{
		for (int i = ++index; i < index + 4; i++)
		{
			numberToRetreiveFunc = (numberToRetreiveFunc << 8) + memory[i];
		}
		index += 3;
		for (int i = ++index; i < index + 4; i++)
		{
			numberToRetreiveEnv = (numberToRetreiveEnv << 8) + memory[i];
		}
		closoureItem = ClosoureItem(numberToRetreiveFunc, numberToRetreiveEnv);
	}
	else
		return ClosoureItem(-1, -1);
	return closoureItem;
}

FunctionItem Memory::loadFunction(int index) {
	FunctionItem fceItem;
	int argCount = 0;
	int bytecodeLength = 0;
	unsigned char *bytecode = new unsigned char[1024];
	unsigned char *argsNames = new unsigned char[1024];
	if (isFunction(&index, &memory) == true)
	{
		argCount = memory[++index];
		for (int i = 0; i < argCount; i++)
		{
			argsNames[i] = memory[++index];
		}
		bytecodeLength = memory[++index];
		for (int i = 0; i < bytecodeLength; i++)
		{
			bytecode[i] = memory[++index];
		}
		fceItem = FunctionItem(argCount, bytecodeLength, argsNames, bytecode);
	}
	else
		return FunctionItem(-1, -1, NULL, NULL);
	return fceItem;
}


EnvironmentItem Memory::loadEnvironment(int index) {
	EnvironmentItem envItem;
	int rowCnt = 0;
	EnvironmentRow *rows = new EnvironmentRow[256];
	if (isEnvironment(&index, &memory) == true)
	{
		int idx = 0;
		int addr = 0;
		int parent = 0;

		rowCnt = memory[++index];
		for (int i = ++index; i < index + 4; i++)
		{
			parent = (parent << 8) + memory[i];
		}
		index += 3;

		for (int j = 0; j < rowCnt; j++)
		{
			idx = memory[++index];
			for (int i = ++index; i < index + 4; i++)
			{
				addr = (addr << 8) + memory[i];
			}
			index += 3;
			
			rows[j].nameIndex = idx;
			rows[j].itemAddress = addr;
		}
		envItem = EnvironmentItem(rowCnt, rows, parent);
	}
	else
		return EnvironmentItem(-1, NULL, -1);
	return envItem;
}

FileItem Memory::loadFile(int index)
{
	FileItem fileItem;
	int fileToRetreive = 0;
	if (isFile(&index, &memory) == true)
	{
		for (int i = ++index; i < index + 4; i++)
		{
			fileToRetreive = (fileToRetreive << 8) + memory[i];
		}
		FILE *pointer = (FILE*)fileToRetreive;
		fileItem = FileItem(pointer);
	}
	else
		return NULL;
	return fileItem;
}

bool checkFreeSpace(unsigned int *memoryIndex, unsigned char **memory, int spaceSize)
{
	//  1/2 pameti je plna
	int half = ((*memoryIndex + 1) + spaceSize) / (MAXIMAL_MEMORY_SIZE / 2);
	// potrebujeme kopirovat 1/2 pameti do druhe poloviny pameti
	if (half == 1 && (*memoryIndex) < (MAXIMAL_MEMORY_SIZE / 2))
	{
		// call GC - kopirovani do druhe poloviny
		printf("call GC - kopirovani do DRUHE poloviny\n");
		GC *gc = new GC(MAXIMAL_MEMORY_SIZE / 2);
		gc->Run();
	}
	else if (half >= 1 && ((*memoryIndex + 1) + spaceSize > (MAXIMAL_MEMORY_SIZE))) // pretekli jsme konec pameti, tj. druha polovina je plna
	{
		// call GC - kopirovani do prvni poloviny
		printf("call GC - kopirovani do PRVNI poloviny\n");
		GC *gc = new GC(0);
		gc->Run();
	}

	return true;
}

unsigned int Memory::storeNumber(int numValue) // vraci index v pameti, kam se to vlozilo
{
	unsigned int indexToReturn = 0;
	// check, jestli ma dostatek volnych pametovych bunek, na cislo staci 5 bunek, tj. 1B na identifikaci a 4B na cislo
	// pokud se nikam cislo nevejde -> zavolat GC
	checkFreeSpace(&memoryIndex, &memory, 5);
	// samotny proces ulozeni hodnoty
	indexToReturn = ++memoryIndex;
	memory[indexToReturn] = MEM_NUMBER;	
	int mult = 3;
	for (unsigned int i = ++memoryIndex; i < memoryIndex + 4; i++)
	{
		memory[i] = (numValue >> mult-- * 8) & 0xFF;
	}
	memoryIndex += 3;
	// ulozim cislo do pameti a vratim prvni index nalezu, fragmentu cisla, vracim bunku identifikujici, ze se jedna o cislo
	return indexToReturn;
}

unsigned int Memory::storeCons(int headIdx, int tailIdx) // vraci index v pameti, kam se to vlozilo
{
	unsigned int indexToReturn = 0;
	// check, jestli ma dostatek volnych pametovych bunek, na cislo staci 5 bunek, tj. 1B na identifikaci a 4B na cislo
	// pokud se nikam cislo nevejde -> zavolat GC
	checkFreeSpace(&memoryIndex, &memory, 9);
	// samotny proces ulozeni hodnoty
	indexToReturn = ++memoryIndex;
	memory[indexToReturn] = MEM_CONS;
	int mult = 3;
	for (unsigned int i = ++memoryIndex; i < memoryIndex + 4; i++)
	{
		memory[i] = (headIdx >> mult-- * 8) & 0xFF;
	}
	memoryIndex += 3;
	mult = 3;
	for (unsigned int i = ++memoryIndex; i < memoryIndex + 4; i++)
	{
		memory[i] = (tailIdx >> mult-- * 8) & 0xFF;
	}
	memoryIndex += 3;
	// ulozim cislo do pameti a vratim prvni index nalezu, fragmentu cisla, vracim bunku identifikujici, ze se jedna o cislo
	return indexToReturn;
}

unsigned int Memory::storeClosoure(int functionIdx, int envIdx) {
	unsigned int indexToReturn = 0;
	// check, jestli ma dostatek volnych pametovych bunek, na cislo staci 5 bunek, tj. 1B na identifikaci a 4B na cislo
	// pokud se nikam cislo nevejde -> zavolat GC
	checkFreeSpace(&memoryIndex, &memory, 9);
	// samotny proces ulozeni hodnoty
	indexToReturn = ++memoryIndex;
	memory[indexToReturn] = MEM_CLOSOURE;
	int mult = 3;
	for (unsigned int i = ++memoryIndex; i < memoryIndex + 4; i++)
	{
		memory[i] = (functionIdx >> mult-- * 8) & 0xFF;
	}
	memoryIndex += 3;
	mult = 3;
	for (unsigned int i = ++memoryIndex; i < memoryIndex + 4; i++)
	{
		memory[i] = (envIdx >> mult-- * 8) & 0xFF;
	}
	memoryIndex += 3;
	// ulozim cislo do pameti a vratim prvni index nalezu, fragmentu cisla, vracim bunku identifikujici, ze se jedna o cislo
	return indexToReturn;
}

unsigned int Memory::storeFunction(int argCnt, int bcLen, unsigned char *argsNames, unsigned char *bytecode) {
	unsigned int indexToReturn = 0;
	// check, jestli ma dostatek volnych pametovych bunek, na cislo staci 5 bunek, tj. 1B na identifikaci a 4B na cislo
	// pokud se nikam cislo nevejde -> zavolat GC
	checkFreeSpace(&memoryIndex, &memory, bcLen + argCnt+ 3);
	
	indexToReturn = ++memoryIndex;
	memory[indexToReturn] = MEM_FUNCTOIN;
	memory[++memoryIndex] = (unsigned char)argCnt;
	for (int i = 0; i < argCnt; i++) {
		memory[++memoryIndex] = argsNames[i];
	}
	memory[++memoryIndex] = (unsigned char)bcLen;
	for (int i = 0; i < bcLen; i++) {
		memory[++memoryIndex] = bytecode[i];
	}
	return indexToReturn;
}

unsigned int Memory::storeFile(FILE *pointer) // vraci index v pameti, kam se to vlozilo
{
	unsigned int indexToReturn = 0;
	checkFreeSpace(&memoryIndex, &memory, 5);
	// samotny proces ulozeni hodnoty
	indexToReturn = ++memoryIndex;
	memory[indexToReturn] = MEM_FILE;
	int numValue = (int)pointer;
	int mult = 3;
	for (unsigned int i = ++memoryIndex; i < memoryIndex + 4; i++)
	{
		memory[i] = (numValue >> mult-- * 8) & 0xFF;
	}
	memoryIndex += 3;
	return indexToReturn;
}

unsigned int Memory::storeEnvironment(int rowCnt, EnvironmentRow *rows, int parentEnvAddr) {
	unsigned int indexToReturn = 0;
	// check, jestli ma dostatek volnych pametovych bunek, na cislo staci 5 bunek, tj. 1B na identifikaci a 4B na cislo
	// pokud se nikam cislo nevejde -> zavolat GC
	checkFreeSpace(&memoryIndex, &memory, 6 + 5*rowCnt);
	indexToReturn = ++memoryIndex;
	memory[indexToReturn] = MEM_ENVIRONMENT;
	memory[++memoryIndex] = (unsigned char)rowCnt;
	int mult = 3;
	for (unsigned int i = ++memoryIndex; i < memoryIndex + 4; i++)
	{
		memory[i] = (parentEnvAddr >> mult-- * 8) & 0xFF;
	}
	memoryIndex += 3;
	for (int j = 0; j < rowCnt; j++) {
		memory[++memoryIndex] = (unsigned char)(rows[j].nameIndex);
		mult = 3;
		for (unsigned int i = ++memoryIndex; i < memoryIndex + 4; i++)
		{
			memory[i] = (rows[j].itemAddress >> mult-- * 8) & 0xFF;
		}
		memoryIndex += 3;
	}
	return indexToReturn;
}

unsigned int Memory::storeTrue()
{
	unsigned int indexToReturn = 0;
	checkFreeSpace(&memoryIndex, &memory, 1);
	indexToReturn = ++memoryIndex;
	memory[indexToReturn] = MEM_TRUE;
	return indexToReturn;
}

unsigned int Memory::storeFalse()
{
	unsigned int indexToReturn = 0;
	checkFreeSpace(&memoryIndex, &memory, 1);
	indexToReturn = ++memoryIndex;
	memory[indexToReturn] = MEM_FALSE;
	return indexToReturn;
}

unsigned int Memory::storeNil()
{
	unsigned int indexToReturn = 0;
	checkFreeSpace(&memoryIndex, &memory, 1);
	indexToReturn = ++memoryIndex;
	memory[indexToReturn] = MEM_NIL;
	return indexToReturn;
}

string Memory::valueToString(int index) {
	int value, bcLen, rowCount;
	string strH, strT;
	NumberItem numItem;
	ConsItem conItem;
	ClosoureItem closItem;
	FunctionItem fceItem;
	FileItem fileItem;
	EnvironmentItem envItem;
	EnvironmentRow *envRow;
	unsigned char *bytecode;

	MemType type = getType(index);
	switch (type)
	{
	case MEM_NUMBER:
		numItem = loadNumbers(index);
		value = numItem.getValue();
		return to_string(value);
		break;
	case MEM_CONS:
		conItem = loadCons(index);
		strH = valueToString(conItem.getHead());
		strT = valueToString(conItem.getTail());
		return " ( " + strH + " . " + strT + " ) ";
		break;
	case MEM_CLOSOURE:
		closItem = loadClosoure(index);
		strH = valueToString(closItem.getFunctionAddr());
		strT = valueToString(closItem.getEnvironmentAddr());
		return "[ " + strH + " ; " + strT + " ]";
		break;
	case MEM_FUNCTOIN:
		fceItem = loadFunction(index);
		bcLen = fceItem.getBytecode(&bytecode);
		strH = "ARGS: " + to_string(fceItem.getArgsCount());
		strH += " BC_len: " + to_string(bcLen);
		strH += " { ";
		for (int i = 0; i < bcLen; i++) {
			strH += to_string((int)(bytecode[i])) + " ";
		}
		return strH + "}";
	case MEM_ENVIRONMENT:
		envItem = loadEnvironment(index);
		envRow = new EnvironmentRow[255];
		rowCount = envItem.getEnvironment(&envRow);
		strH = " {";
		for (int i = 0; i < rowCount; i++) {
			strH += "[" + translationTable->getName(envRow[i].nameIndex);
			strH += " ; " + valueToString(envRow[i].itemAddress);
			strH += "] ";
		}
		return strH + "} ";
	case MEM_FILE:
		fileItem = loadFile(index);
		value = (int)(fileItem.getPointer());
		return "FILE: " + to_string(value);
	case MEM_TRUE:
		return "TRUE";
		break;
	case MEM_FALSE:
		return "FALSE";
		break;
	case MEM_NIL:
		return "NIL";
		break;
	case MEM_EMPTY:
		return "EMPTY";
		break;
	default:
		return "NOT IMPLEMENTED YET!";
	}
	return "";
}

unsigned char Memory::getByteValue(int addr)
{
	return memory[addr];
}

void Memory::setMemory(/*unsigned char **newMemory, */unsigned int memoryIndex)
{
	/*delete[] memory;
	this->memory = *newMemory;*/
	this->memoryIndex = memoryIndex;
}

void Memory::GCwrite(unsigned int addr, unsigned char value)
{
	memory[addr] = value;
}