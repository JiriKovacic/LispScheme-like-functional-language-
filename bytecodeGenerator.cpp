#include "bytecodeGenerator.h"

BytecodeGenerator::BytecodeGenerator(SchemeObject *astRoot)
{
	this->astRoot = astRoot;
}

int BytecodeGenerator::generateBytecode(unsigned char **bytecodeArray) // ukazetel na pole charu
{
	unsigned char *localBytecodeArray;
	int bytecodeLength = this->astRoot->generateBytecode(&localBytecodeArray);
	*bytecodeArray = localBytecodeArray; // do pole charu priradim lokalni pole charu
	return bytecodeLength;
}





