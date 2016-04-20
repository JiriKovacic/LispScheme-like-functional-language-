#ifndef GLOBAL_ENV_H_INCLUDED
#define GLOBAL_ENV_H_INCLUDED

#include <string>
#include "instructionSet.h"

using namespace std;

class EnvItem {
	public:
		string itemName;
		bool isFunction;
		int argCount;
		unsigned char *bytecode;
		int bytecodeLength;
		int address;
};

class GlobalEnvTable {
    private:
        EnvItem functions[256]; 
		int funcCounter;
	public:
		GlobalEnvTable();
		int findFunction(string name);
		string getName(int id);
		int getBytecode(int index, unsigned char **bytecode);
		int getNumberOfArgs(int index);
		void reserveFunctionName(string name);
		void reserveVariableName(string name);
		void saveFunction(EnvItem item);
		void saveVariable(int index, int addr);
		bool isFunction(int index);
		int getAddress(int index);
		void setAddress(int index, int addr);
		int getSize();
};

#endif // GLOBAL_ENV_H_INCLUDED