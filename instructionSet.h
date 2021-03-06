#ifndef INSTRUCTION_SET_H_INCLUDED
#define INSTRUCTION_SET_H_INCLUDED

#include <string>

using namespace std;

#define NUMER_OF_INSTRUCTIONS 33

enum instructionsEnum {
    BC_PUSH_CONST,
    BC_PUSH_ARG,
	BC_PUSH_VAR,
	BC_PUSH_TRUE,
	BC_PUSH_FALSE,
	BC_PUSH_NIL,
    BC_POP,
    BC_ADD,
    BC_SUB, 
    BC_MUL,
    BC_DIV,
    BC_JUMP,
    BC_JUMP_TRUE,
    BC_JUMP_FALSE,
    BC_CALL,
    BC_EQ,
    BC_NEQ,
    BC_GEQ,
    BC_LEQ, 
    BC_GT,
    BC_LS,
	BC_AND,
	BC_OR,
	BC_HEAD,
	BC_TAIL,
	BC_CONS,
	BC_DEFVAR,
	BC_DEFUN,
	BC_LAMBDA,
	BC_FILE_OPEN,
	BC_FILE_CLOSE,
	BC_FILE_LOAD,
	BC_FILE_WRITE
};

extern string instructionsNames[NUMER_OF_INSTRUCTIONS];

#endif // INSTRUCTION_SET_H_INCLUDED