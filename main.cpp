#include <iostream>
#include <fstream>
#include <cstdio>
#include "ast.h"
#include "globalEnv.h"
#include "virtualMachine.h"
#include "reader.h"
#include "astBuilder.h"
#include "bytecodeGenerator.h"
#include "memory.h"
#include "garbageCollector.h"
#include "memoryItem.h"
#include "translationTable.h"

using namespace std;

GlobalEnvTable *globalEnvTable;
VirtualMachine *virtMachine;
EnvironmentItem *environment;

TranslationTable *translationTable;


// Globalni promenne pro generovani bytecode v ast.cpp
string *defineFunctionArgsArray;
string *defineLambdaFunctionArgsArray;
int defineFunctionArgCnt;
int defineLambdaFunctionArgCnt;
bool isDefinedFunction;
bool calledFunction;

int main(int argc, char **argv) {
	globalEnvTable = new GlobalEnvTable();
	virtMachine = new VirtualMachine();
	translationTable = new TranslationTable();

	defineFunctionArgsArray = NULL;
	defineLambdaFunctionArgsArray = NULL;
	defineFunctionArgCnt = 0;
	defineLambdaFunctionArgCnt = 0;
	isDefinedFunction = false;
	
	int nilAddr = virtMachine->getMemory()->storeNil();
	Frame *frame = new Frame(nilAddr);
	virtMachine->addFrame(frame);

	while (true) {
		printf("> ");

		string inputStr = "";
		char buff[1000];
		while (fgets(buff, 1000, stdin)) {
			string tmp(buff);
			inputStr += tmp;
			bool exists = tmp.find('\n') != string::npos;
			if (exists) break;
		}

		if (inputStr.compare("EXIT\n") == 0) {
			break;
		}
		
		Reader *reader = new Reader(inputStr);
		AstBuilder *builder = new AstBuilder(reader);
		SchemeObject *ast = NULL;
		BytecodeGenerator *generator;
		unsigned char *bytecodeArray = NULL;
		int cnt = 0;
		
		while(ast = builder->getNextAst(), ast != NULL){
			generator = new BytecodeGenerator(ast);
			int bytecodeLength = generator->generateBytecode(&bytecodeArray); // posilam adresu toho pole

			/*for (int i = 0; i < bytecodeLength; i++) {
				printf("%d ", bytecodeArray[i]);
			}
			printf("\n");*/

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
		printf("\n");
	}

	virtMachine->removeFrame();

	printf("EOP ...\n");
}
