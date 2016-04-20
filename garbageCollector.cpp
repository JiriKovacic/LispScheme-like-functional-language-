#include <cstdio>
#include <map>

#include "garbageCollector.h"


GC::GC(unsigned int allocPtrMemoryIdx)
{
	//gcTempMemory = new unsigned char[MAXIMAL_MEMORY_SIZE](); // 4 MiB
	//initializeTempMemory(&gcTempMemory);
	this->allocPtr = allocPtrMemoryIdx;
}

void GC::Run()
{
	//gcTempMemory = new unsigned char[MAXIMAL_MEMORY_SIZE](); // 4 MiB
	//initializeTempMemory(&gcTempMemory);
	//this->allocPtr = 0;
	int frameCnt = virtMachine->frameArraySize();
	for (int i = 0; i < frameCnt; i++)
	{
		int envAddr = virtMachine->getFrame(i)->getEvnironmentAddress();
		int newAddr = copyEnvironment(envAddr);
		virtMachine->getFrame(i)->setEvnironmentAddress(newAddr);
	}

	int gStackSize = virtMachine->stackSize();
	for (int i = 0; i < gStackSize; i++)
	{
		readRoot();  // tady se resi jenom globalni stack
	}
	pushToGlobalStack();

	copyLocalVars();

	// Nastaveni nove pameti na starou
	virtMachine->setMemory(allocPtr);
}

void GC::copyLocalVars()
{
	int localVarLength = -1;
	int newAddress = -1;
	for (int i = 0; i < virtMachine->frameArraySize(); i++)
	{
		localVarLength = virtMachine->getFrame(i)->getLocalVariablesAdress().size();
		for (int j = 0; j < localVarLength; j++)
		{
			int addr = virtMachine->getFrame(i)->getLocalVariablesAdress()[j];
			switch (virtMachine->getMemory()->getType(addr))
			{
			case MEM_NUMBER:
				// copy to new memory
				iter = mapp.find(addr);
				if (iter != mapp.end())
				{
					// key found
					newAddress = iter->second;
				}
				else
				{
					// key not found
					newAddress = copyNumber(addr); // vrati se adresa v nove memory, kde zacina mem number
					mapp.insert(pair<int, int>(addr, newAddress));
				}
				break;
			case MEM_CONS:
				iter = mapp.find(addr);
				if (iter != mapp.end())
					newAddress = iter->second;
				else
				{
					newAddress = copyCons(addr); // vrati se adresa v nove memory, kde zacina mem number		
					mapp.insert(pair<int, int>(addr, newAddress));
				}
				break;
			case MEM_FUNCTOIN:
				iter = mapp.find(addr);
				if (iter != mapp.end())
					newAddress = iter->second;
				else
				{
					newAddress = copyFunction(addr);
					mapp.insert(pair<int, int>(addr, newAddress));
				}
				break;
			case MEM_CLOSOURE:
				iter = mapp.find(addr);
				if (iter != mapp.end())
					newAddress = iter->second;
				else
				{
					newAddress = copyClosoure(addr);
					mapp.insert(pair<int, int>(addr, newAddress));
				}
				break;
			case MEM_ENVIRONMENT:
				iter = mapp.find(addr);
				if (iter != mapp.end())
					newAddress = iter->second;
				else
				{
					newAddress = copyEnvironment(addr);
					mapp.insert(pair<int, int>(addr, newAddress));
				}
				break;
			case MEM_FILE:
				iter = mapp.find(addr);
				if (iter != mapp.end())
					newAddress = iter->second;
				else
				{
					newAddress = copyFile(addr);
					mapp.insert(pair<int, int>(addr, newAddress));
				}
				break;
			case MEM_TRUE:
				iter = mapp.find(addr);
				if (iter != mapp.end())
					newAddress = iter->second;
				else
				{
					newAddress = copyTrue(addr);
					mapp.insert(pair<int, int>(addr, newAddress));
				}
				break;
			case MEM_FALSE:
				iter = mapp.find(addr);
				if (iter != mapp.end())
					newAddress = iter->second;
				else
				{
					newAddress = copyFalse(addr);
					mapp.insert(pair<int, int>(addr, newAddress));
				}
				break;
			case MEM_NIL:
				iter = mapp.find(addr);
				if (iter != mapp.end())
					newAddress = iter->second;
				else
				{
					newAddress = copyNil(addr);
					mapp.insert(pair<int, int>(addr, newAddress));
				}
				break;
			}
			virtMachine->getFrame(i)->setLocalVariableAddress(j, newAddress);
		}
	}
}

void GC::pushToGlobalStack()
{
	int addr = 0;
	for (int i = 0; i < tempStack.size(); i++)
	{
		addr = tempStack.top();
		tempStack.pop();
		virtMachine->pushAddressToStack(addr);
	}
}

void GC::readRoot()
{
	int newAddress = -1;
	int addr = virtMachine->popAddressFromStack();

	MemType m = virtMachine->getMemory()->getType(addr);
	switch (virtMachine->getMemory()->getType(addr))
	{
	case MEM_NUMBER:
		// copy to new memory
		iter = mapp.find(addr);
		if (iter != mapp.end())
		{
			// key found
			newAddress = iter->second;
		}
		else
		{
			// key not found
			newAddress = copyNumber(addr); // vrati se adresa v nove memory, kde zacina mem number
			mapp.insert(pair<int, int>(addr, newAddress));
		}
		tempStack.push(newAddress);
		break;
	case MEM_CONS:
		iter = mapp.find(addr);
		if (iter != mapp.end())
			newAddress = iter->second;
		else
		{
			newAddress = copyCons(addr); // vrati se adresa v nove memory, kde zacina mem number		
			mapp.insert(pair<int, int>(addr, newAddress));
		}
		tempStack.push(newAddress);
		break;
	case MEM_FUNCTOIN:
		iter = mapp.find(addr);
		if (iter != mapp.end())
			newAddress = iter->second;
		else
		{
			newAddress = copyFunction(addr);
			mapp.insert(pair<int, int>(addr, newAddress));
		}
		tempStack.push(newAddress);
		break;
	case MEM_CLOSOURE:
		iter = mapp.find(addr);
		if (iter != mapp.end())
			newAddress = iter->second;
		else
		{
			newAddress = copyClosoure(addr);
			mapp.insert(pair<int, int>(addr, newAddress));
		}
		tempStack.push(newAddress);
		break;
	case MEM_ENVIRONMENT:
		iter = mapp.find(addr);
		if (iter != mapp.end())
			newAddress = iter->second;
		else
		{
			newAddress = copyEnvironment(addr);
			mapp.insert(pair<int, int>(addr, newAddress));
		}
		tempStack.push(newAddress);
		break;
	case MEM_FILE:
		iter = mapp.find(addr);
		if (iter != mapp.end())
			newAddress = iter->second;
		else
		{
			newAddress = copyFile(addr);
			mapp.insert(pair<int, int>(addr, newAddress));
		}
		tempStack.push(newAddress);
		break;
	case MEM_TRUE:
		iter = mapp.find(addr);
		if (iter != mapp.end())
			newAddress = iter->second;
		else
		{
			newAddress = copyTrue(addr);
			mapp.insert(pair<int, int>(addr, newAddress));
		}
		tempStack.push(newAddress);
		break;
	case MEM_FALSE:
		iter = mapp.find(addr);
		if (iter != mapp.end())
			newAddress = iter->second;
		else
		{
			newAddress = copyFalse(addr);
			mapp.insert(pair<int, int>(addr, newAddress));
		}
		tempStack.push(newAddress);
		break;
	case MEM_NIL:
		iter = mapp.find(addr);
		if (iter != mapp.end())
			newAddress = iter->second;
		else
		{
			newAddress = copyNil(addr);
			mapp.insert(pair<int, int>(addr, newAddress));
		}
		tempStack.push(newAddress);
		break;
	case MEM_EMPTY:
		tempStack.push(0);
		break;
	}
}

bool GC::checkFreeSpace(unsigned int *memoryIndex, int spaceSize)
{
	int counter = 0;
	while (true)
	{
		// memoryIndex + 1 -> efektivnejsi reseni, snazime se ulozit cislo na prvni dobrou, aby se nemusel opakovat while cyklus
		for (unsigned int i = *memoryIndex + 1; i < (*memoryIndex + 1) + spaceSize; i++)
		{
			//if (gcTempMemory[i] == MEM_EMPTY)
			counter++;
		}
		if (counter >= spaceSize)
			return true;
		else
		{
			*memoryIndex += counter;
			counter = 0;
		}
	}
}

unsigned int GC::copyNumber(int addr) // vraci index v pameti, kam se to vlozilo
{
	unsigned int indexToReturn = 0;
	iter = mapp.find(addr);
	if (iter != mapp.end())
		indexToReturn = iter->second;
	else
	{
		indexToReturn = storeNumber(addr);

		mapp.insert(pair<int, int>(addr, indexToReturn));
	}

	return indexToReturn;
}

unsigned int GC::storeNumber(int addr)
{
	unsigned int indexToReturn = 0;
	// check, jestli ma dostatek volnych pametovych bunek, na cislo staci 5 bunek, tj. 1B na identifikaci a 4B na cislo
	checkFreeSpace(&allocPtr, 5);
	// samotny proces ulozeni hodnoty
	indexToReturn = ++allocPtr;
	virtMachine->getMemory()->GCwrite(indexToReturn, MEM_NUMBER);
	//gcTempMemory[indexToReturn] = MEM_NUMBER;
	addr++;
	for (unsigned int i = ++allocPtr; i < allocPtr + 4; i++, addr++)
	{
		//gcTempMemory[i] = virtMachine->getMemory()->getByteValue(addr);
		virtMachine->getMemory()->GCwrite(i, virtMachine->getMemory()->getByteValue(addr));
	}
	allocPtr += 3;
	// ulozim cislo do pameti a vratim prvni index nalezu, fragmentu cisla, vracim bunku identifikujici, ze se jedna o cislo

	return indexToReturn;
}

MemType GC::getType(int headAddr)
{
	switch (virtMachine->getMemory()->getType(headAddr))
	{
	case MEM_NUMBER:
		return MEM_NUMBER;
	case MEM_CONS:
		return MEM_CONS;
	case MEM_FUNCTOIN:
		return MEM_FUNCTOIN;
	case MEM_CLOSOURE:
		return MEM_CLOSOURE;
	case MEM_ENVIRONMENT:
		return MEM_ENVIRONMENT;
	case MEM_FILE:
		return MEM_FILE;
	case MEM_TRUE:
		return MEM_TRUE;
	case MEM_FALSE:
		return MEM_FALSE;
	case MEM_NIL:
		return MEM_NIL;
	default:
		return MEM_EMPTY;
	}
}

unsigned int GC::copyCons(int addr)
{
	unsigned int indexToReturn = 0;

	iter = mapp.find(addr);
	if (iter != mapp.end())
		indexToReturn = iter->second;
	else
	{

		ConsItem item = virtMachine->getMemory()->loadCons(addr);
		int headAddr = item.getHead();
		int tailAddr = item.getTail();


		// copy HEAD
		switch (getType(headAddr))
		{
		case MEM_NUMBER:
			headAddr = copyNumber(headAddr);
			break;
		case MEM_CONS:
			headAddr = copyCons(headAddr);
			break;
		case MEM_TRUE:
			headAddr = copyTrue(headAddr);
			break;
		case MEM_FALSE:
			headAddr = copyFalse(headAddr);
			break;
		case MEM_NIL:
			headAddr = copyNil(headAddr);
			break;
		}

		// copy TAIL
		switch (getType(tailAddr))
		{
		case MEM_NUMBER:
			tailAddr = copyNumber(tailAddr);
			break;
		case MEM_CONS:
			tailAddr = copyCons(tailAddr);
			break;
		case MEM_TRUE:
			tailAddr = copyTrue(tailAddr);
			break;
		case MEM_FALSE:
			tailAddr = copyFalse(tailAddr);
			break;
		case MEM_NIL:
			tailAddr = copyNil(tailAddr);
			break;
		}


		indexToReturn = storeCons(headAddr, tailAddr);
		mapp.insert(pair<int, int>(addr, indexToReturn));
	}

	return indexToReturn;
}

unsigned int GC::storeCons(int headAddr, int tailAddr)
{
	unsigned int indexToReturn = 0;
	// check, jestli ma dostatek volnych pametovych bunek, na cislo staci 5 bunek, tj. 1B na identifikaci a 4B na cislo
	checkFreeSpace(&allocPtr, 9);
	// samotny proces ulozeni hodnoty
	indexToReturn = ++allocPtr;
	//gcTempMemory[indexToReturn] = MEM_CONS;
	virtMachine->getMemory()->GCwrite(indexToReturn, MEM_CONS);

	// store Head
	int mult = 3;
	//headAddr++;
	for (unsigned int i = ++allocPtr; i < allocPtr + 4; i++)
	{
		//gcTempMemory[i] = (headAddr >> mult-- * 8) & 0xFF;
		virtMachine->getMemory()->GCwrite(i, (headAddr >> mult-- * 8) & 0xFF);
	}
	allocPtr += 3;

	// store Tail
	mult = 3;
	//tailAddr++;
	for (unsigned int i = ++allocPtr; i < allocPtr + 4; i++)
	{
		//gcTempMemory[i] = (tailAddr >> mult-- * 8) & 0xFF;
		virtMachine->getMemory()->GCwrite(i, (headAddr >> mult-- * 8) & 0xFF);
	}
	allocPtr += 3;
	// ulozim cislo do pameti a vratim prvni index nalezu, fragmentu cisla, vracim bunku identifikujici, ze se jedna o cislo

	return indexToReturn;
}

unsigned int GC::copyClosoure(int addr)
{
	unsigned int indexToReturn = 0;

	iter = mapp.find(addr);
	if (iter != mapp.end())
		indexToReturn = iter->second;
	else
	{
		ClosoureItem cItem = virtMachine->getMemory()->loadClosoure(addr);
		int envAddress = cItem.getEnvironmentAddr();
		int funcAddress = cItem.getFunctionAddr();

		int newEnvAddress = copyEnvironment(envAddress);

		// jeste na tu function, tady neviiiiim 
		int newFuncAddress = copyFunction(funcAddress);

		indexToReturn = storeClosoure(newFuncAddress, newEnvAddress);
		mapp.insert(pair<int, int>(addr, indexToReturn));
	}

	return indexToReturn;
}

unsigned int GC::storeClosoure(int functionIdx, int envIdx) {
	unsigned int indexToReturn = 0;
	// check, jestli ma dostatek volnych pametovych bunek, na cislo staci 5 bunek, tj. 1B na identifikaci a 4B na cislo
	// pokud se nikam cislo nevejde -> zavolat GC
	checkFreeSpace(&allocPtr, 9);
	// samotny proces ulozeni hodnoty
	indexToReturn = ++allocPtr;
	//memory[indexToReturn] = MEM_CLOSOURE;
	virtMachine->getMemory()->GCwrite(allocPtr++, MEM_CLOSOURE);
	int mult = 3;
	for (unsigned int i = ++allocPtr; i < allocPtr + 4; i++)
	{
		//memory[i] = (functionIdx >> mult-- * 8) & 0xFF;
		virtMachine->getMemory()->GCwrite(i, (functionIdx >> mult-- * 8) & 0xFF);
	}
	allocPtr += 3;
	mult = 3;
	for (unsigned int i = ++allocPtr; i < allocPtr + 4; i++)
	{
		//memory[i] = (envIdx >> mult-- * 8) & 0xFF;
		virtMachine->getMemory()->GCwrite(i, (envIdx >> mult-- * 8) & 0xFF);
	}
	allocPtr += 3;
	// ulozim cislo do pameti a vratim prvni index nalezu, fragmentu cisla, vracim bunku identifikujici, ze se jedna o cislo
	return indexToReturn;
}

unsigned int GC::copyFunction(int addr)
{
	unsigned int indexToReturn = 0;

	iter = mapp.find(addr);
	if (iter != mapp.end())
		indexToReturn = iter->second;
	else
	{

		FunctionItem fItem = virtMachine->getMemory()->loadFunction(addr);
		unsigned char * bytecode = new unsigned char[1024];
		int argCnt = fItem.getArgsCount();
		int bcLen = fItem.getBytecode(&bytecode);
		unsigned char *argsNames = new unsigned char[argCnt];

		indexToReturn = storeFunction(argCnt, bcLen, argsNames, bytecode);
		mapp.insert(pair<int, int>(addr, indexToReturn));
	}

	return indexToReturn;
}

unsigned int GC::storeFunction(int argCnt, int bcLen, unsigned char *argsNames, unsigned char *bytecode) {
	unsigned int indexToReturn = 0;

	// check, jestli ma dostatek volnych pametovych bunek, na cislo staci 5 bunek, tj. 1B na identifikaci a 4B na cislo
	// pokud se nikam cislo nevejde -> zavolat GC
	checkFreeSpace(&allocPtr, bcLen + argCnt + 3);

	indexToReturn = ++allocPtr;
	//memory[indexToReturn] = MEM_FUNCTOIN;
	virtMachine->getMemory()->GCwrite(allocPtr++, MEM_FUNCTOIN);
	//memory[++memoryIndex] = (unsigned char)argCnt;
	virtMachine->getMemory()->GCwrite(allocPtr, (unsigned char)argCnt);
	for (int i = 0; i < argCnt; i++) {
		//memory[++memoryIndex] = argsNames[i];
		virtMachine->getMemory()->GCwrite(i, argsNames[i]);
	}
	//memory[++memoryIndex] = (unsigned char)bcLen;
	virtMachine->getMemory()->GCwrite(++allocPtr, (unsigned char)bcLen);
	for (int i = 0; i < bcLen; i++) {
		//memory[++memoryIndex] = bytecode[i];
		virtMachine->getMemory()->GCwrite(++allocPtr, bytecode[i]);
	}
	return indexToReturn;
}

unsigned int GC::copyFile(int addr)
{
	unsigned int indexToReturn = 0;

	iter = mapp.find(addr);
	if (iter != mapp.end())
		indexToReturn = iter->second;
	else
	{
		indexToReturn = storeFile(addr);
		mapp.insert(pair<int, int>(addr, indexToReturn));
	}

	return indexToReturn;
}

unsigned int GC::storeFile(int addr)
{
	unsigned int indexToReturn = 0;

	FileItem fItem = virtMachine->getMemory()->loadFile(addr);

	checkFreeSpace(&allocPtr, 5);
	// samotny proces ulozeni hodnoty
	indexToReturn = ++allocPtr;
	//memory[indexToReturn] = MEM_FILE;
	virtMachine->getMemory()->GCwrite(allocPtr, MEM_FILE);
	int numValue = (int)fItem.getPointer();
	int mult = 3;
	for (unsigned int i = ++allocPtr; i < allocPtr + 4; i++)
	{
		//memory[i] = (numValue >> mult-- * 8) & 0xFF;
		virtMachine->getMemory()->GCwrite(i, (numValue >> mult-- * 8) & 0xFF);
	}
	allocPtr += 3;

	return indexToReturn;
}

unsigned int GC::copyEnvironment(int addr)
{
	unsigned int indexToReturn = 0;
	iter = mapp.find(addr);
	if (iter != mapp.end())
		indexToReturn = iter->second;
	else
	{

		EnvironmentItem env = virtMachine->getMemory()->loadEnvironment(addr);

		int newAddress = 0;
		int itemAddr = 0;
		MemType m;
		EnvironmentRow *envRow = new EnvironmentRow[255];
		int rowCount = env.getEnvironment(&envRow);
		for (int i = 0; i < rowCount; i++)
		{
			itemAddr = envRow[i].itemAddress;
			m = getType(itemAddr);

			switch (m)
			{
			case MEM_NUMBER:
				// copy to new memory
				newAddress = copyNumber(itemAddr); // vrati se adresa v nove memory, kde zacina mem number
				envRow[i].itemAddress = newAddress;
				break;
			case MEM_CONS:
				newAddress = copyCons(itemAddr);
				envRow[i].itemAddress = newAddress;
				break;
			case MEM_FUNCTOIN:
				newAddress = copyFunction(itemAddr);
				envRow[i].itemAddress = newAddress;
				break;
			case MEM_CLOSOURE:
				newAddress = copyClosoure(itemAddr);
				envRow[i].itemAddress = newAddress;
				break;
			case MEM_ENVIRONMENT:
				newAddress = copyEnvironment(itemAddr);
				envRow[i].itemAddress = newAddress;
				break;
			case MEM_FILE:
				newAddress = copyFile(itemAddr);
				envRow[i].itemAddress = newAddress;
				break;
			case MEM_TRUE:
				newAddress = copyTrue(itemAddr);
				envRow[i].itemAddress = newAddress;
				break;
			case MEM_FALSE:
				newAddress = copyFalse(itemAddr);
				envRow[i].itemAddress = newAddress;
				break;
			case MEM_NIL:
				newAddress = copyNil(itemAddr);
				envRow[i].itemAddress = newAddress;
				break;
			case MEM_EMPTY:
				envRow[i].itemAddress = 0;
				break;
			}
		}

		int parrentEnvAddr = env.getParentEnvAddr();
		int newParrentEnvAddr;
		if (virtMachine->getMemory()->getType(parrentEnvAddr) == MEM_ENVIRONMENT) {
			newParrentEnvAddr = copyEnvironment(parrentEnvAddr);
		}
		else {
			newParrentEnvAddr = copyNil(parrentEnvAddr);
		}

		indexToReturn = storeEnvironment(rowCount, envRow, newParrentEnvAddr);

		mapp.insert(pair<int, int>(addr, indexToReturn));
	}

	return indexToReturn;
}

unsigned GC::storeEnvironment(int rowCnt, EnvironmentRow *rows, int parentEnvAddr)
{
	unsigned int indexToReturn = 0;
	// check, jestli ma dostatek volnych pametovych bunek, na cislo staci 5 bunek, tj. 1B na identifikaci a 4B na cislo
	// pokud se nikam cislo nevejde -> zavolat GC
	checkFreeSpace(&allocPtr, 6 + 5 * rowCnt);
	indexToReturn = ++allocPtr;
	//memory[indexToReturn] = MEM_ENVIRONMENT;
	virtMachine->getMemory()->GCwrite(indexToReturn++, MEM_ENVIRONMENT);
	//memory[++memoryIndex] = (unsigned char)rowCnt;
	virtMachine->getMemory()->GCwrite(indexToReturn, (unsigned char)rowCnt);
	int mult = 3;
	for (unsigned int i = ++indexToReturn; i < indexToReturn + 4; i++)
	{
		//memory[i] = (parentEnvAddr >> mult-- * 8) & 0xFF;
		virtMachine->getMemory()->GCwrite(i, (parentEnvAddr >> mult-- * 8) & 0xFF);
	}
	indexToReturn += 3;
	for (int j = 0; j < rowCnt; j++) {
		//memory[++memoryIndex] = (unsigned char)(rows[j].nameIndex);
		virtMachine->getMemory()->GCwrite(++indexToReturn, (unsigned char)(rows[j].nameIndex));
		mult = 3;
		for (unsigned int i = ++indexToReturn; i < indexToReturn + 4; i++)
		{
			//memory[i] = (rows[j].itemAddress >> mult-- * 8) & 0xFF;
			virtMachine->getMemory()->GCwrite(i, (rows[j].itemAddress >> mult-- * 8) & 0xFF);
		}
		indexToReturn += 3;
	}
	return indexToReturn;
}

unsigned int GC::copyTrue(int addr)
{
	unsigned int indexToReturn = 0;
	iter = mapp.find(addr);
	if (iter != mapp.end())
		indexToReturn = iter->second;
	else
	{
		checkFreeSpace(&allocPtr, 1);
		indexToReturn = ++allocPtr;
		//gcTempMemory[indexToReturn] = MEM_TRUE;
		virtMachine->getMemory()->GCwrite(indexToReturn, MEM_TRUE);
		mapp.insert(pair<int, int>(addr, indexToReturn));
	}
	return indexToReturn;
}

unsigned int GC::copyFalse(int addr)
{
	unsigned int indexToReturn = 0;
	iter = mapp.find(addr);
	if (iter != mapp.end())
		indexToReturn = iter->second;
	else
	{
		checkFreeSpace(&allocPtr, 1);
		indexToReturn = ++allocPtr;
		//gcTempMemory[indexToReturn] = MEM_FALSE;
		virtMachine->getMemory()->GCwrite(indexToReturn, MEM_FALSE);
		mapp.insert(pair<int, int>(addr, indexToReturn));
	}
	return indexToReturn;
}

unsigned int GC::copyNil(int addr)
{
	unsigned int indexToReturn = 0;
	iter = mapp.find(addr);
	if (iter != mapp.end())
		indexToReturn = iter->second;
	else
	{
		checkFreeSpace(&allocPtr, 1);
		indexToReturn = ++allocPtr;
		//gcTempMemory[indexToReturn] = MEM_NIL;
		virtMachine->getMemory()->GCwrite(indexToReturn, MEM_NIL);
		mapp.insert(pair<int, int>(addr, indexToReturn));
	}
	return indexToReturn;
}