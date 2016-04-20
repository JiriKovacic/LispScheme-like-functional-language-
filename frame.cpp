#include <cstdio>

#include "frame.h"
#include "virtualMachine.h"
#include "garbageCollector.h"
#include "translationTable.h"

extern VirtualMachine *virtMachine;
extern TranslationTable *translationTable;
extern EnvironmentItem *environment;
extern bool calledFunction;

bool isActualClosoureCall = false;

void Frame::insertLocalVariables(vector<int> *localVariables, int numArgs)
{
	for (int i = numArgs - 1; i >= 0; i--)
	{
		(*localVariables)[i] = virtMachine->popAddressFromStack();
	}
}

Frame::Frame(int parentEnvAddr) {
	actualIndex = 0;
	environmentAddress = virtMachine->getMemory()->storeEnvironment(0, NULL, parentEnvAddr);
}

// Nacte bytecode funkce z globalEnvTable
void Frame::loadBytecodeGlobalEnvTable(int funcIndex)
{
	// zde je potreba nacist
	// bytecode envTable, lokalni promenne
	int numberOfArgs = globalEnvTable->getNumberOfArgs(funcIndex);
	localVariables.resize(numberOfArgs);

	this->bytecodeLength = globalEnvTable->getBytecode(funcIndex, &bytecode);
	insertLocalVariables(&localVariables, numberOfArgs);
}

// Nacte bytecode funkce 
void Frame::loadBytecodeProgram(unsigned char * bytecode, int bytecodeLength)
{
	this->bytecode = new unsigned char[bytecodeLength]();
	this->bytecode = bytecode;
	this->bytecodeLength = bytecodeLength;
}

void saveNumberToEnvRow(int idx)
{
	int envAddr = virtMachine->getActualEnvironmentAddress();
	EnvironmentRow *envRow = new EnvironmentRow[255];
	int rowCnt = -1;
	while (virtMachine->getMemory()->getType(envAddr) == MEM_ENVIRONMENT) {
		EnvironmentItem envItem = virtMachine->getMemory()->loadEnvironment(envAddr);
		rowCnt = envItem.getEnvironment(&envRow);
		envRow[rowCnt].itemAddress = virtMachine->popAddressFromStack();
		envRow[rowCnt].nameIndex = idx;
		rowCnt++;
		break;
	}
}

int Frame::interpretNextByte(int index)
{
	int address;
	int actualEnvAdress = -1;

	int skipBytes = -1;
	switch (this->bytecode[index])
	{
	case BC_PUSH_CONST:
		pushConstant(bytecode[index + 1], bytecode[index + 2], bytecode[index + 3], bytecode[index + 4]);
		skipBytes = 5;
		break;
	case BC_PUSH_ARG:
		pushArgument(bytecode[index + 1]);
		skipBytes = 2;
		break;
	case BC_PUSH_VAR:
		//address = globalEnvTable->getAddress(bytecode[index + 1]);
		address = findAddressInMemory(bytecode[index + 1]);
		if (address == -1) printf("ERROR: BC_PUSH_VAR in frame.cpp - index not found");
		virtMachine->pushAddressToStack(address);
		skipBytes = 2;
		break;
	case BC_PUSH_TRUE:
		address = virtMachine->getMemory()->storeTrue();
		virtMachine->pushAddressToStack(address);
		skipBytes = 1;
		break;
	case BC_PUSH_FALSE:
		address = virtMachine->getMemory()->storeFalse();
		virtMachine->pushAddressToStack(address);
		skipBytes = 1;
		break;
	case BC_PUSH_NIL:
		address = virtMachine->getMemory()->storeNil();
		virtMachine->pushAddressToStack(address);
		skipBytes = 1;
		break;
	case BC_POP:
		virtMachine->popAddressFromStack();
		skipBytes = 1;
		break;
	case BC_ADD:
		calculateMathOperation(BC_ADD);
		skipBytes = 1;
		break;
	case BC_SUB:
		calculateMathOperation(BC_SUB);
		skipBytes = 1;
		break;
	case BC_MUL:
		calculateMathOperation(BC_MUL);
		skipBytes = 1;
		break;
	case BC_DIV:
		calculateMathOperation(BC_DIV);
		skipBytes = 1;
		break;
	case BC_JUMP:
		skipBytes = evaluateJump(BC_JUMP, bytecode[index + 1]) + 2;
		break;
	case BC_JUMP_TRUE:
		skipBytes = evaluateJump(BC_JUMP_TRUE, bytecode[index + 1]) + 2;
		break;
	case BC_JUMP_FALSE:
		skipBytes = evaluateJump(BC_JUMP_FALSE, bytecode[index + 1]) + 2;
		break;
	case BC_CALL:
		calledFunction = true;
		address = findAddressInMemory(bytecode[index + 1]);

		if (address == -1) printf("ERROR: BC_CALL in frame.cpp - index not found");
		if (virtMachine->getMemory()->getType(address) == MEM_CLOSOURE) {
			callClosoure(address);
		}
		else{
			callFunction(address);
		}
		
		skipBytes = 2;
		break;
	case BC_EQ:
		evaluateEquality(BC_EQ);
		skipBytes = 1;
		break;
	case BC_NEQ:
		evaluateEquality(BC_NEQ);
		skipBytes = 1;
		break;
	case BC_GEQ:
		evaluateEquality(BC_GEQ);
		skipBytes = 1;
		break;
	case BC_LEQ:
		evaluateEquality(BC_LEQ);
		skipBytes = 1;
		break;
	case BC_GT:
		evaluateEquality(BC_GT);
		skipBytes = 1;
		break;
	case BC_LS:
		evaluateEquality(BC_LS);
		skipBytes = 1;
		break;
	case BC_AND:
		evaluateAndOr(BC_AND);
		skipBytes = 1;
		break;
	case BC_OR:
		evaluateAndOr(BC_OR);
		skipBytes = 1;
		break;
	case BC_HEAD:
		evaluateConsModifiers(BC_HEAD);
		skipBytes = 1;
		break;
	case BC_TAIL:
		evaluateConsModifiers(BC_TAIL);
		skipBytes = 1;
		break;
	case BC_CONS:
		pushCons();
		skipBytes = 1;
		break;
	case BC_DEFVAR:
		defineVariable(bytecode[index + 1]);
		skipBytes = 2;
		break;
	case BC_DEFUN:
		skipBytes = defineFunction(bytecode, index + 1) + 1;
		break;
	case BC_LAMBDA:
		skipBytes = defineLambda(bytecode, index + 1) + 1;
		break;
	case BC_FILE_OPEN:
		fileOpen(bytecode[index + 1]);
		skipBytes = 2;
		break;
	case BC_FILE_CLOSE:
		fileClose(bytecode[index + 1]);
		skipBytes = 2;
		break;
	case BC_FILE_WRITE:
		fileWrite(bytecode[index + 1]);
		skipBytes = 2;
		break;
	case BC_FILE_LOAD:
		fileLoad(bytecode[index + 1]);
		skipBytes = 2;
		break;
	default:
		printf("ERROR: Invalid bytecode instruction!");
		break;
	}
	return skipBytes;
}

void Frame::pushConstant(unsigned char cA, unsigned char cB, unsigned char cC, unsigned char cD) {
	int iA = (int)cA;
	int iB = ((int)cB) << 8;
	int iC = ((int)cC) << 16;
	int iD = ((int)cD) << 24;
	int number = iA | iB | iC | iD;
	int addr = virtMachine->getMemory()->storeNumber(number);
	virtMachine->pushAddressToStack(addr);
}

void Frame::pushArgument(unsigned char idx) {
	int index = (int)idx;
	int addr = localVariables[index];
	virtMachine->pushAddressToStack(addr);
}

void Frame::pushCons() {
	int idxTail = virtMachine->popAddressFromStack();
	int idxHead = virtMachine->popAddressFromStack();
	int addr = virtMachine->getMemory()->storeCons(idxHead, idxTail);
	virtMachine->pushAddressToStack(addr);
}

void Frame::calculateMathOperation(instructionsEnum opType) {
	int idxA = virtMachine->popAddressFromStack();
	int idxB = virtMachine->popAddressFromStack();

	// kdyz to nejsou cisla, vrati se NIL
	if (virtMachine->getMemory()->getType(idxA) != MEM_NUMBER || virtMachine->getMemory()->getType(idxB) != MEM_NUMBER) {
		int resAddr = virtMachine->getMemory()->storeNil();
		virtMachine->pushAddressToStack(resAddr);
		return;
	}

	int numberA = virtMachine->getMemory()->loadNumbers(idxA).getValue();
	int numberB = virtMachine->getMemory()->loadNumbers(idxB).getValue();
	int result;
	switch (opType)
	{
	case BC_ADD:
		result = numberA + numberB;
		break;
	case BC_SUB:
		result = numberA - numberB;
		break;
	case BC_MUL:
		result = numberA * numberB;
		break;
	case BC_DIV:
		result = numberA / numberB;
		break;
	}
	int addr = virtMachine->getMemory()->storeNumber(result);
	virtMachine->pushAddressToStack(addr);
}

int Frame::evaluateJump(instructionsEnum jumpType, unsigned char jumpSize) {
	int addr;
	int jumpValue = ((int)jumpSize) - JUMP_ADDR_OFFSET;
	if (jumpType == BC_JUMP_TRUE) {
		addr = virtMachine->popAddressFromStack();
		if (virtMachine->getMemory()->getType(addr) != MEM_TRUE) {
			return 0;
		}
	}
	if (jumpType == BC_JUMP_FALSE) {
		addr = virtMachine->popAddressFromStack();
		if (virtMachine->getMemory()->getType(addr) != MEM_FALSE) {
			return 0;
		}
	}
	return jumpValue;
}

void Frame::evaluateEquality(instructionsEnum eqType) {
	int numberA, numberB, resAddr;
	int addrB = virtMachine->popAddressFromStack();
	int addrA = virtMachine->popAddressFromStack();
	MemType typeA = virtMachine->getMemory()->getType(addrA);
	MemType typeB = virtMachine->getMemory()->getType(addrB);

	if (eqType == BC_EQ) {
		if ((typeA == MEM_TRUE && typeB == MEM_TRUE) || (typeA == MEM_FALSE && typeB == MEM_FALSE) || (typeA == MEM_NIL && typeB == MEM_NIL)) {
			resAddr = virtMachine->getMemory()->storeTrue();
			virtMachine->pushAddressToStack(resAddr);
			return;
		}
	}

	if (eqType == BC_NEQ) {
		if ((typeA == MEM_TRUE && typeB == MEM_FALSE) || (typeA == MEM_FALSE && typeB == MEM_TRUE)) {
			resAddr = virtMachine->getMemory()->storeTrue();
			virtMachine->pushAddressToStack(resAddr);
			return;
		}
	}

	// porovnavame pouze cisla, pokud jeden z argumentu neni cislo tak vratime FALSE
	if (typeA != MEM_NUMBER || typeB != MEM_NUMBER) {
		resAddr = virtMachine->getMemory()->storeFalse();
		virtMachine->pushAddressToStack(resAddr);
		return;
	}

	// zjisteni cisel a vyhodnoceni podminky
	numberA = virtMachine->getMemory()->loadNumbers(addrA).getValue();
	numberB = virtMachine->getMemory()->loadNumbers(addrB).getValue();
	switch (eqType) {
	case BC_EQ:
		if (numberA == numberB) {
			resAddr = virtMachine->getMemory()->storeTrue();
		}
		else {
			resAddr = virtMachine->getMemory()->storeFalse();
		}
		break;
	case BC_NEQ:
		if (numberA != numberB) {
			resAddr = virtMachine->getMemory()->storeTrue();
		}
		else {
			resAddr = virtMachine->getMemory()->storeFalse();
		}
		break;
	case BC_LEQ:
		if (numberA <= numberB) {
			resAddr = virtMachine->getMemory()->storeTrue();
		}
		else {
			resAddr = virtMachine->getMemory()->storeFalse();
		}
		break;
	case BC_GEQ:
		if (numberA >= numberB) {
			resAddr = virtMachine->getMemory()->storeTrue();
		}
		else {
			resAddr = virtMachine->getMemory()->storeFalse();
		}
		break;
	case BC_GT:
		if (numberA > numberB) {
			resAddr = virtMachine->getMemory()->storeTrue();
		}
		else {
			resAddr = virtMachine->getMemory()->storeFalse();
		}
		break;
	case BC_LS:
		if (numberA < numberB) {
			resAddr = virtMachine->getMemory()->storeTrue();
		}
		else {
			resAddr = virtMachine->getMemory()->storeFalse();
		}
		break;
	}

	virtMachine->pushAddressToStack(resAddr);
}

void Frame::evaluateAndOr(instructionsEnum type) {
	int idxA = virtMachine->popAddressFromStack();
	int idxB = virtMachine->popAddressFromStack();
	bool error = false;
	int addr;

	// kdyz to nejsou T/F, vrati false
	if (virtMachine->getMemory()->getType(idxA) != MEM_TRUE && virtMachine->getMemory()->getType(idxA) != MEM_FALSE) {
		error = true;
	}
	if (virtMachine->getMemory()->getType(idxB) != MEM_TRUE && virtMachine->getMemory()->getType(idxB) != MEM_FALSE) {
		error = true;
	}
	if (error == true) {
		addr = virtMachine->getMemory()->storeFalse();
		virtMachine->pushAddressToStack(addr);
		return;
	}

	// vyreseni AND nebo OR
	switch (type) {
	case BC_AND:
		if (virtMachine->getMemory()->getType(idxA) == MEM_TRUE && virtMachine->getMemory()->getType(idxB) == MEM_TRUE) {
			addr = virtMachine->getMemory()->storeTrue();
		}
		else {
			addr = virtMachine->getMemory()->storeFalse();
		}
		break;
	case BC_OR:
		if (virtMachine->getMemory()->getType(idxA) == MEM_TRUE || virtMachine->getMemory()->getType(idxB) == MEM_TRUE) {
			addr = virtMachine->getMemory()->storeTrue();
		}
		else {
			addr = virtMachine->getMemory()->storeFalse();
		}
		break;
	}

	virtMachine->pushAddressToStack(addr);
}

void Frame::evaluateConsModifiers(instructionsEnum type) {
	int addr = virtMachine->popAddressFromStack();

	// kdyz to neni cons bunka, tak se vrati NIL
	if (virtMachine->getMemory()->getType(addr) != MEM_CONS) {
		addr = virtMachine->getMemory()->storeNil();
		virtMachine->pushAddressToStack(addr);
		return;
	}

	ConsItem item = virtMachine->getMemory()->loadCons(addr);
	switch (type) {
	case BC_HEAD:
		virtMachine->pushAddressToStack(item.getHead());
		break;
	case BC_TAIL:
		virtMachine->pushAddressToStack(item.getTail());
		break;
	}
}

void Frame::callFunction(int index) {
	int idx = (int)index;

	Frame *frame = new Frame(virtMachine->getActualFrame()->getEvnironmentAddress());
	int numberOfArgs = virtMachine->getMemory()->loadFunction(idx).getArgsCount();
	frame->localVariables.resize(numberOfArgs);
	frame->bytecodeLength = virtMachine->getMemory()->loadFunction(idx).getBytecode(&frame->bytecode);
	frame->localVariablesCount = virtMachine->getMemory()->loadFunction(idx).getArgsNames(&frame->localVariablesNames);
	insertLocalVariables(&frame->localVariables, numberOfArgs);

	virtMachine->addFrame(frame);
	virtMachine->startInterpret();
	virtMachine->removeFrame();
}

void Frame::callClosoure(int index) {
	int idxClosoure = (int)index;
	ClosoureItem closItem = virtMachine->getMemory()->loadClosoure(idxClosoure);
	int idxFce = closItem.getFunctionAddr();
	int idxEnv = closItem.getEnvironmentAddr();

	Frame *frame = new Frame(idxEnv);
	int numberOfArgs = virtMachine->getMemory()->loadFunction(idxFce).getArgsCount();
	frame->localVariables.resize(numberOfArgs);
	frame->bytecodeLength = virtMachine->getMemory()->loadFunction(idxFce).getBytecode(&frame->bytecode);
	frame->localVariablesCount = virtMachine->getMemory()->loadFunction(idxFce).getArgsNames(&frame->localVariablesNames);
	insertLocalVariables(&frame->localVariables, numberOfArgs);

	isActualClosoureCall = true;
	virtMachine->addFrame(frame);
	virtMachine->startInterpret();
	virtMachine->removeFrame();
	isActualClosoureCall = false;
}

void Frame::defineVariable(unsigned char index) {
	int addr = virtMachine->popAddressFromStack();

	EnvironmentItem actualEnv = virtMachine->getMemory()->loadEnvironment(virtMachine->getActualEnvironmentAddress());
	EnvironmentRow *envRow = new EnvironmentRow[255];
	int rowCount = actualEnv.getEnvironment(&envRow);
	envRow[rowCount].itemAddress = addr;
	envRow[rowCount].nameIndex = index;
	rowCount++;
	int newEnvAddr = virtMachine->getMemory()->storeEnvironment(rowCount, envRow, actualEnv.getParentEnvAddr());
	virtMachine->setActualEnvironmentAddress(newEnvAddr);

	virtMachine->pushAddressToStack(addr);
}

int Frame::defineFunction(unsigned char * code, int index) {
	unsigned char *tmpBytecode = new unsigned char[1024];
	unsigned char *tmpArgsNames = new unsigned char[1024];

	int functionIdx = (int)(code[index++]);
	int argCount = (int)(code[index++]);

	for (int i = 0; i < argCount; i++)
	{
		tmpArgsNames[i] = code[index++];
	}

	int bytecodeLength = (int)(code[index++]);

	for (int i = 0; i < bytecodeLength; i++)
	{
		tmpBytecode[i] = code[index++];
	}

	int addr = virtMachine->getMemory()->storeFunction(argCount, bytecodeLength, tmpArgsNames, tmpBytecode);

	EnvironmentItem actualEnv = virtMachine->getMemory()->loadEnvironment(virtMachine->getActualEnvironmentAddress());
	EnvironmentRow *envRow = new EnvironmentRow[255];
	int rowCount = actualEnv.getEnvironment(&envRow);
	envRow[rowCount].itemAddress = addr;
	envRow[rowCount].nameIndex = functionIdx;
	rowCount++;
	int newEnvAddr = virtMachine->getMemory()->storeEnvironment(rowCount, envRow, actualEnv.getParentEnvAddr());
	virtMachine->setActualEnvironmentAddress(newEnvAddr);

	virtMachine->pushAddressToStack(addr);

	return 3 + bytecodeLength + argCount;
}

int Frame::defineLambda(unsigned char * code, int index) {
	unsigned char *tmpBytecode = new unsigned char[1024];
	unsigned char *tmpArgsNames = new unsigned char[1024];

	int argCount = (int)(code[index++]);
	for (int i = 0; i < argCount; i++)
	{
		tmpArgsNames[i] = code[index++];
	}

	int bytecodeLength = (int)(code[index++]);
	for (int i = 0; i < bytecodeLength; i++)
	{
		tmpBytecode[i] = code[index++];
	}

	// je to lamdba
	if (calledFunction == false) {
		int functionAddr = virtMachine->getMemory()->storeFunction(argCount, bytecodeLength, tmpArgsNames, tmpBytecode);
		virtMachine->pushAddressToStack(functionAddr);
	}
	// je to closoure
	else {
		/*printf("LocalVariablesCount: %d\n", localVariablesCount);
		for (int i = 0; i < localVariablesCount; i++) {
			printf("Var[%d]: %s\n", i, translationTable->getName(localVariablesNames[i]).c_str());
		}*/
		// lokalni promenne ulozit do env
		EnvironmentItem actualEnv = virtMachine->getMemory()->loadEnvironment(virtMachine->getActualEnvironmentAddress());
		EnvironmentRow *envRow = new EnvironmentRow[255];
		int rowCount = actualEnv.getEnvironment(&envRow);
		for (int i = 0; i < localVariables.size(); i++) {
			envRow[rowCount].itemAddress = localVariables[i];
			envRow[rowCount].nameIndex = localVariablesNames[i];
			rowCount++;
		}
		int newEnvAddr = virtMachine->getMemory()->storeEnvironment(rowCount, envRow, actualEnv.getParentEnvAddr());

		// Adresa pameti ulozene lambda funkce
		int functionAddr = virtMachine->getMemory()->storeFunction(argCount, bytecodeLength, tmpArgsNames, tmpBytecode);
		int closureAddr = virtMachine->getMemory()->storeClosoure(functionAddr, newEnvAddr);
		// Misto ulozeni lamda adresy v pameti tam ulozime adresu, kde je MEM_Closure
		//virtMachine->pushAddressToStack(addr);
		virtMachine->pushAddressToStack(closureAddr);
	}

	return 2 + bytecodeLength + argCount;
}

void Frame::fileOpen(unsigned char index) {
	string filePath = translationTable->getName(index);
	FILE *file = fopen(filePath.c_str(), "w");
	int addr = virtMachine->getMemory()->storeFile(file);
	virtMachine->pushAddressToStack(addr);
}

void Frame::fileClose(unsigned char index) {
	int addr = findAddressInMemory(index);
	if (virtMachine->getMemory()->getType(addr) == MEM_FILE) {
		FILE *file = virtMachine->getMemory()->loadFile(addr).getPointer();
		fclose(file);
	}
	else printf("ERROR: close file!");
	
	addr = virtMachine->getMemory()->storeNil();
	virtMachine->pushAddressToStack(addr);
}

void Frame::fileWrite(unsigned char index) {
	int addr = findAddressInMemory(index);
	int toWrite = virtMachine->popAddressFromStack();
	if (virtMachine->getMemory()->getType(addr) != MEM_FILE) {
		printf("ERROR: write file!");
		virtMachine->pushAddressToStack(0);
		return;
	}
	FILE *file = virtMachine->getMemory()->loadFile(addr).getPointer();

	fprintf(file, "%s\n", virtMachine->getMemory()->valueToString(toWrite).c_str());

	virtMachine->pushAddressToStack(addr);
}

void Frame::fileLoad(unsigned char index) {
	string filePath = translationTable->getName(index);
	FILE *inputFile = fopen(filePath.c_str(), "r");

	string fileStr = "";
	char buff[100];
	while (fgets(buff, 100, inputFile)) {
		string tmp(buff);
		fileStr += tmp;
	}

	Reader *reader = new Reader(fileStr);
	AstBuilder *builder = new AstBuilder(reader);
	SchemeObject *ast = NULL;
	BytecodeGenerator *generator;
	unsigned char *bytecodeArray = NULL;

	while (ast = builder->getNextAst(), ast != NULL) {
		generator = new BytecodeGenerator(ast);
		int bytecodeLength = generator->generateBytecode(&bytecodeArray); // posilam adresu toho pole

		// zde BUDE interpret - zacatek interpretu
		if (bytecodeLength > 0) {
			calledFunction = false;

			Frame *frame = new Frame(virtMachine->getActualFrame()->getEvnironmentAddress());
			frame->loadBytecodeProgram(bytecodeArray, bytecodeLength);
			virtMachine->addFrame(frame);

			virtMachine->startInterpret();
			virtMachine->removeFrame();

			string res = virtMachine->getResult();
			printf("RESULT: %s\n", res.c_str());
		}
		else {
			printf("ERROR: syntax!");
		}
		// konec interpretu
	}

	fclose(inputFile);

	int addr = virtMachine->getMemory()->storeNil();
	virtMachine->pushAddressToStack(addr);
}

bool Frame::intrepretAll()
{
	int moveIP;
	// dokud existuje instrukce
	// tato implementace umoznuje skoky
	while (actualIndex >= 0 && actualIndex < bytecodeLength) {
		moveIP = interpretNextByte(actualIndex);
		actualIndex += moveIP;
	}

	return true;
}

vector<int> Frame::getLocalVariablesAdress()
{
	return this->localVariables;
}

void Frame::setLocalVariableAddress(int index, int addr)
{
	this->localVariables[index] = addr;
}

int Frame::getEvnironmentAddress() {
	return environmentAddress;
}

void Frame::setEvnironmentAddress(int addr) {
	environmentAddress = addr;
}

int Frame::findAddressInMemory(int idx) {
	int address;
	int envAddr = virtMachine->getActualEnvironmentAddress();
	if (isActualClosoureCall == true) {
		envAddr = virtMachine->getClosoureEnvironmentAddress();
	}
	while (virtMachine->getMemory()->getType(envAddr) == MEM_ENVIRONMENT) {
		EnvironmentItem envItem = virtMachine->getMemory()->loadEnvironment(envAddr);
		address = envItem.findAddressInMemory(idx);
		if (address >= 0) return address;
		envAddr = envItem.getParentEnvAddr();
	}
	return -1;
}