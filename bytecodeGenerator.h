#ifndef BYTECODE_GENERATOR_H_INCLUDED
#define BYTECODE_GENERATOR_H_INCLUDED

#include "ast.h"

class BytecodeGenerator
{
private:
	SchemeObject *astRoot;
public:
	BytecodeGenerator(SchemeObject *astRoot);
	int generateBytecode(unsigned char **bytecodeArray); // return: delka bytecodu
};

#endif // BYTECODE_GENERATOR_H_INCLUDED
