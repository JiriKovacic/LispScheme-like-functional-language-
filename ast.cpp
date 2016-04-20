#include "ast.h"
#include "instructionSet.h"
#include "virtualMachine.h"
#include "translationTable.h"

extern VirtualMachine *virtMachine;
extern TranslationTable *translationTable;
extern EnvironmentItem *environment;

SchemeObject::SchemeObject(SchemeObject *parent) {
	this->parent = parent;
	this->argCounter = 0;
}

void SchemeObject::addArgument(SchemeObject *argument) {
	args[argCounter] = argument;
	argCounter++;
}

SchemeFunction::SchemeFunction(string name, SchemeObject *parent) : SchemeObject(parent) {
	//this->globalEvnIndex = globalEnvTable->findFunction(name);
	this->translationTableIndex = translationTable->getIndex(name);
}

SchemeKeyword::SchemeKeyword(string name, SchemeObject *parent) : SchemeObject(parent) {
	for (int i = 0; i < NUMER_OF_KEYWORDS; i++) {
		if (name == keywordsNames[i]) this->name = (keywordsEnum)i;
	}
}

SchemeConstant::SchemeConstant(string valueStr, SchemeObject *parent) : SchemeObject(parent) {
	size_t lastChar;
	this->constValue = stoi(valueStr, &lastChar, 10);
}

SchemeVariable::SchemeVariable(string name, SchemeObject *parent) : SchemeObject(parent) {
	this->name = name;
}

void SchemeFunction::printGraphviz() {
	cout << "n_" << this << " [label=\"Function\\n(" << globalEnvTable->getName(globalEvnIndex) << ")\"]" << endl;
	cout << "n_" << this << " [[TTableIdx]label=\"Function\\n(" << translationTable->getName(translationTableIndex) << ")\"]" << endl;
	for (int i = 0; i < argCounter; i++) {
		args[i]->printGraphviz();
		cout << "n_" << this << " -> " << "n_" << args[i] << endl;
	}
}

void SchemeKeyword::printGraphviz() {
	cout << "n_" << this << " [label=\"Keyword\\n(" << keywordsNames[name] << ")\"]" << endl;
	for (int i = 0; i < argCounter; i++) {
		args[i]->printGraphviz();
		cout << "n_" << this << " -> " << "n_" << args[i] << endl;
	}
}

void SchemeConstant::printGraphviz() {
	cout << "n_" << this << " [label=\"Constant\\n(" << constValue << ")\"]" << endl;
	for (int i = 0; i < argCounter; i++) {
		args[i]->printGraphviz();
		cout << "n_" << this << " -> " << "n_" << args[i] << endl;
	}
}

void SchemeVariable::printGraphviz() {
	cout << "n_" << this << " [label=\"Variable\\n(" << name << ")\"]" << endl;
	for (int i = 0; i < argCounter; i++) {
		args[i]->printGraphviz();
		cout << "n_" << this << " -> " << "n_" << args[i] << endl;
	}
}

// AST strom prochazime POST ORDER
int SchemeConstant::generateBytecode(unsigned char **bytecodeArray)
{
	unsigned char *localBytecodeArray = new unsigned char[10];
	localBytecodeArray[0] = BC_PUSH_CONST;
	// int ktery ma 4B je ulozen do bytecodu jako little endian po jednotlivych bytech
	localBytecodeArray[1] = (unsigned char)(this->constValue);
	localBytecodeArray[2] = (unsigned char)(this->constValue >> 8);
	localBytecodeArray[3] = (unsigned char)(this->constValue >> 16);
	localBytecodeArray[4] = (unsigned char)(this->constValue >> 24);
	*bytecodeArray = localBytecodeArray;
	return 5;
}

int SchemeFunction::generateBytecode(unsigned char **bytecodeArray)
{
	unsigned char *tempBytecode = NULL;
	unsigned char *localBytecodeArray = new unsigned char[1024];
	int bytecodeLength = 0;
	int index = 0;
	// postorder - nejdrive musime projit vsechny potomky a pak sebe
	for (int i = 0; i < this->argCounter; i++)
	{
		bytecodeLength = args[i]->generateBytecode(&tempBytecode);
		for (int j = 0; j < bytecodeLength; j++, index++)
		{
			localBytecodeArray[index] = tempBytecode[j];
		}
		delete(tempBytecode);
	}
	// potomky uz mame, ted ziskame bytecode pro funkci
	localBytecodeArray[index++] = BC_CALL;
	localBytecodeArray[index++] = (unsigned char)translationTableIndex;//globalEvnIndex;
	*bytecodeArray = localBytecodeArray;
	return index;
}

int SchemeKeyword::generateBytecode(unsigned char **bytecodeArray)
{
	string functionName = "";
	string filePath;
	SchemeObject *functionObj, *obj1, *obj2, *condObj, *ifObj, *elseObj;
	EnvironmentRow *envRow = new EnvironmentRow[256];
	int bytecodeLength = 0;
	int translationTableIndex = 0;
	int functionAdressInMemory = 0;
	int rowCount = 0;
	int lambdaI = 0;
	unsigned char *localBytecode = new unsigned char[1024];

	switch (name)
	{
	case KW_DEFINE:
		functionObj = args[0];
		functionName = ((SchemeVariable*)args[0])->getName();
		defineFunctionArgCnt = functionObj->getArgsCnt();
		defineFunctionArgsArray = new string[defineFunctionArgCnt];

		// rezervace mista pro tuto funkce (kvuli rekurzi)		
		//globalEnvTable->reserveFunctionName(functionName);		
		translationTableIndex = translationTable->addRecord(functionName);
		if (translationTableIndex == -1) // nahrada za globalEnvTable->reserveFunctionName(functionName);
			printf("ERROR translationTable has NOT enough capacity");

		for (int i = 0; i < defineFunctionArgCnt; i++)
		{
			string argName = ((SchemeVariable*)functionObj->getArg(i))->getName();
			defineFunctionArgsArray[i] = argName;
			translationTable->addRecord(argName);
		}
		/*fce*/
		bytecodeLength = defineFunction(functionName, args[1], &localBytecode);
		break;
	case KW_DEFVAR:
		functionObj = args[0];
		functionName = ((SchemeVariable*)args[0])->getName();
		//globalEnvTable->reserveVariableName(functionName);
		translationTableIndex = translationTable->addRecord(functionName);
		bytecodeLength = defineVariable(functionName, functionObj->getArg(0), &localBytecode);
		break;
	case KW_LAMBDA:
		functionObj = args[0];
		defineFunctionArgCnt = functionObj->getArgsCnt() + 1;
		defineFunctionArgsArray = new string[defineFunctionArgCnt];
		
		defineFunctionArgsArray[0] = ((SchemeVariable*)args[0])->getName();
		translationTable->addRecord(defineFunctionArgsArray[0]);
		for (int i = 1; i < defineFunctionArgCnt; i++)
		{
			string argName = ((SchemeVariable*)functionObj->getArg(i-1))->getName();
			defineFunctionArgsArray[i] = argName;
			translationTable->addRecord(argName);
		}
		
		bytecodeLength = defineLambda(args[1], &localBytecode);
		break;
	case KW_IF:
		condObj = args[0]; // arg[i] i-ty podstrom
		ifObj = args[1];
		elseObj = NULL;
		if (argCounter > 2) elseObj = args[2];
		bytecodeLength = generateIfBc(condObj, ifObj, elseObj, &localBytecode);
		break;
	case KW_EQ:
		obj1 = args[0];
		obj2 = args[1];
		bytecodeLength = generateCondition(BC_EQ, obj1, obj2, &localBytecode);
		break;
	case KW_NEQ:
		obj1 = args[0];
		obj2 = args[1];
		bytecodeLength = generateCondition(BC_NEQ, obj1, obj2, &localBytecode);
		break;
	case KW_GEQ:
		obj1 = args[0];
		obj2 = args[1];
		bytecodeLength = generateCondition(BC_GEQ, obj1, obj2, &localBytecode);
		break;
	case KW_LEQ:
		obj1 = args[0];
		obj2 = args[1];
		bytecodeLength = generateCondition(BC_LEQ, obj1, obj2, &localBytecode);
		break;
	case KW_GT:
		obj1 = args[0];
		obj2 = args[1];
		bytecodeLength = generateCondition(BC_GT, obj1, obj2, &localBytecode);
		break;
	case KW_LS:
		obj1 = args[0];
		obj2 = args[1];
		bytecodeLength = generateCondition(BC_LS, obj1, obj2, &localBytecode);
		break;
	case KW_TRUE:
		localBytecode[0] = BC_PUSH_TRUE;
		bytecodeLength = 1;
		break;
	case KW_FALSE:
		localBytecode[0] = BC_PUSH_FALSE;
		bytecodeLength = 1;
		break;
	case KW_NIL:
		localBytecode[0] = BC_PUSH_NIL;
		bytecodeLength = 1;
		break;
	case KW_AND:
		bytecodeLength = generateCondCombinator(BC_AND, args, argCounter, &localBytecode);
		break;
	case KW_OR:
		bytecodeLength = generateCondCombinator(BC_OR, args, argCounter, &localBytecode);
		break;
	case KW_BEGIN:
		bytecodeLength = generateBegin(args, argCounter, &localBytecode);
		break;
	case KW_CAR:
		bytecodeLength = generateListModifier(BC_HEAD, args[0], &localBytecode);
		break;
	case KW_CDR:
		bytecodeLength = generateListModifier(BC_TAIL, args[0], &localBytecode);
		break;
	case KW_CONS:
		obj1 = args[0];
		obj2 = args[1];
		bytecodeLength = generateCons(obj1, obj2, &localBytecode);
		break;
	case KW_LIST:
		bytecodeLength = generateList(args, argCounter, &localBytecode);
		break;
	case KW_PLUS:
		bytecodeLength = generateMathOperation(BC_ADD, args, argCounter, &localBytecode);
		break;
	case KW_MINUS:
		bytecodeLength = generateMathOperation(BC_SUB, args, argCounter, &localBytecode);
		break;
	case KW_TIMES:
		bytecodeLength = generateMathOperation(BC_MUL, args, argCounter, &localBytecode);
		break;
	case KW_DIVIDE:
		bytecodeLength = generateMathOperation(BC_DIV, args, argCounter, &localBytecode);
		break;
	case KW_FILE_OPEN:
		filePath = ((SchemeVariable*)args[0])->getName();
		translationTableIndex = translationTable->addRecord(filePath);
		localBytecode[0] = BC_FILE_OPEN;
		localBytecode[1] = (unsigned char)translationTableIndex;
		bytecodeLength = 2;
		break;
	case KW_FILE_CLOSE:
		filePath = ((SchemeVariable*)args[0])->getName();
		translationTableIndex = translationTable->getIndex(filePath);
		localBytecode[0] = BC_FILE_CLOSE;
		localBytecode[1] = (unsigned char)translationTableIndex;
		bytecodeLength = 2;
		break;
	case KW_FILE_LOAD:
		filePath = ((SchemeVariable*)args[0])->getName();
		translationTableIndex = translationTable->addRecord(filePath);
		localBytecode[0] = BC_FILE_LOAD;
		localBytecode[1] = (unsigned char)translationTableIndex;
		bytecodeLength = 2;
		break;
	case KW_FILE_WRITE:
		filePath = ((SchemeVariable*)args[0])->getName();
		translationTableIndex = translationTable->getIndex(filePath);
		bytecodeLength = generateFileWrite(translationTableIndex, args[1], &localBytecode);
		break;
	default:
		break;
	}

	*bytecodeArray = localBytecode;
	return bytecodeLength;
}

int SchemeKeyword::defineFunction(string functionName, SchemeObject *astRoot, unsigned char **bytecodeArray)
{
	unsigned char *localBytecodeArray = new unsigned char[1024];
	unsigned char *tempBytecode = NULL;
	int index = 0;
	//int varIdx = globalEnvTable->findFunction(functionName);
	int varIdx = translationTable->getIndex(functionName); // nahrada za globalEnvTable->findFunction(functionName);
	if (varIdx == -1) // Nemelo by nastat
		printf("ERROR translationTable RECORD not FOUND -> in (ast.cpp - SchemeKeyword::defineFunction) ");

	localBytecodeArray[index++] = BC_DEFUN;
	localBytecodeArray[index++] = (unsigned char)varIdx;
	localBytecodeArray[index++] = (unsigned char)defineFunctionArgCnt;

	for (int i = 0; i < defineFunctionArgCnt; i++) {
		int argIdx = translationTable->getIndex(defineFunctionArgsArray[i]);
		localBytecodeArray[index++] = (unsigned char)argIdx;
	}

	isDefinedFunction = true;
	int bytecodeLength = astRoot->generateBytecode(&tempBytecode);
	isDefinedFunction = false;

	localBytecodeArray[index++] = (unsigned char)bytecodeLength;

	if (bytecodeLength > 255) printf("ERROR bytecode length overflow!");

	for (int i = 0; i < bytecodeLength; i++, index++)
	{
		localBytecodeArray[index] = tempBytecode[i];
	}
	delete(tempBytecode);

	*bytecodeArray = localBytecodeArray;
	return index;
}

int SchemeKeyword::defineVariable(string varName, SchemeObject *astRoot, unsigned char **bytecodeArray) {
	unsigned char *localBytecodeArray = new unsigned char[1024];
	unsigned char *tempBytecode = NULL;
	int index = 0;
	//int varIdx = globalEnvTable->findFunction(varName);
	int varIdx = translationTable->getIndex(varName); // nahrada za globalEnvTable->findFunction(varName);
	if (varIdx == -1) // Nemelo by nastat
		printf("ERROR translationTable RECORD not FOUND -> in (ast.cpp - SchemeKeyword::defineVariable) ");

	int bytecodeLength = astRoot->generateBytecode(&tempBytecode);
	for (int i = 0; i < bytecodeLength; i++, index++)
	{
		localBytecodeArray[index] = tempBytecode[i];
	}
	delete(tempBytecode);

	localBytecodeArray[index++] = BC_DEFVAR;
	localBytecodeArray[index++] = (unsigned char)varIdx;

	*bytecodeArray = localBytecodeArray;
	return index;
}

int SchemeKeyword::defineLambda(SchemeObject *astRoot, unsigned char **bytecodeArray) {
	unsigned char *localBytecodeArray = new unsigned char[1024];
	unsigned char *tempBytecode = NULL;
	int index = 0;

	localBytecodeArray[index++] = BC_LAMBDA;
	localBytecodeArray[index++] = (unsigned char)defineFunctionArgCnt;

	for (int i = 0; i < defineFunctionArgCnt; i++) {
		int argIdx = translationTable->getIndex(defineFunctionArgsArray[i]);
		localBytecodeArray[index++] = (unsigned char)argIdx;
	}

	isDefinedFunction = true;
	int bytecodeLength = astRoot->generateBytecode(&tempBytecode);
	isDefinedFunction = false;

	localBytecodeArray[index++] = (unsigned char)bytecodeLength;

	if (bytecodeLength > 255) printf("ERROR bytecode length overflow!");

	for (int i = 0; i < bytecodeLength; i++, index++)
	{
		localBytecodeArray[index] = tempBytecode[i];
	}
	delete(tempBytecode);

	*bytecodeArray = localBytecodeArray;
	return index;
}

int SchemeKeyword::generateIfBc(SchemeObject *cond, SchemeObject *ifPart, SchemeObject *elsePart, unsigned char **bytecodeArray) {
	unsigned char *localBytecodeArray = new unsigned char[1024];
	unsigned char *tempBytecode = NULL;
	int index = 0;
	int condJumpIndex = 0;
	int uncondJumpIndex = 0;
	int bytecodeLength = 0;

	// generovani bytecodu podminky
	bytecodeLength = cond->generateBytecode(&tempBytecode);
	for (int i = 0; i < bytecodeLength; i++, index++)
	{
		localBytecodeArray[index] = tempBytecode[i];
	}
	delete(tempBytecode);

	// podmineny skok za if cast kdyz podminka neplati
	// adresa bude doplnena az bude znama
	localBytecodeArray[index++] = BC_JUMP_FALSE;
	condJumpIndex = index;
	index++;

	// generovani bytecodu if casti
	bytecodeLength = ifPart->generateBytecode(&tempBytecode);
	for (int i = 0; i < bytecodeLength; i++, index++)
	{
		localBytecodeArray[index] = tempBytecode[i];
	}
	delete(tempBytecode);

	// doplneni adresy skoku
	if (elsePart != NULL) localBytecodeArray[condJumpIndex] = (unsigned char)(bytecodeLength + 2 + JUMP_ADDR_OFFSET);
	else localBytecodeArray[condJumpIndex] = (unsigned char)(bytecodeLength + JUMP_ADDR_OFFSET);

	if (elsePart != NULL) {
		// skok za else cast
		// adresa bude doplnena az bude znama
		localBytecodeArray[index++] = BC_JUMP;
		uncondJumpIndex = index;
		index++;

		// generovani bytecodu else casti
		bytecodeLength = elsePart->generateBytecode(&tempBytecode);
		for (int i = 0; i < bytecodeLength; i++, index++)
		{
			localBytecodeArray[index] = tempBytecode[i];
		}
		delete(tempBytecode);

		// doplneni adresy skoku
		localBytecodeArray[uncondJumpIndex] = (unsigned char)(bytecodeLength + JUMP_ADDR_OFFSET);
	}

	*bytecodeArray = localBytecodeArray;
	return index;
}

int SchemeKeyword::generateCondition(instructionsEnum instrType, SchemeObject *obj1, SchemeObject *obj2, unsigned char **bytecodeArray) {
	unsigned char *localBytecodeArray = new unsigned char[1024];
	unsigned char *tempBytecode = NULL;
	int index = 0;
	int bytecodeLength = 0;

	// generovani bytecodu prvniho podstromu
	bytecodeLength = obj1->generateBytecode(&tempBytecode);
	for (int i = 0; i < bytecodeLength; i++, index++)
	{
		localBytecodeArray[index] = tempBytecode[i];
	}
	delete(tempBytecode);

	// generovani bytecodu druheho podstromu
	bytecodeLength = obj2->generateBytecode(&tempBytecode);
	for (int i = 0; i < bytecodeLength; i++, index++)
	{
		localBytecodeArray[index] = tempBytecode[i];
	}
	delete(tempBytecode);

	// pridani podminky
	localBytecodeArray[index++] = instrType;

	*bytecodeArray = localBytecodeArray;
	return index;
}

int SchemeKeyword::generateCondCombinator(instructionsEnum instrType, SchemeObject *args[], int argCnt, unsigned char **bytecodeArray) {
	unsigned char *localBytecodeArray = new unsigned char[1024];
	unsigned char *tempBytecode = NULL;
	int index = 0;
	int bytecodeLength = 0;

	// vygenerovani bytecodu pro vsechny argumenty
	for (int i = 0; i < argCnt; i++) {
		bytecodeLength = args[i]->generateBytecode(&tempBytecode);
		for (int j = 0; j < bytecodeLength; j++, index++)
		{
			localBytecodeArray[index] = tempBytecode[j];
		}
		delete(tempBytecode);
	}

	// vygenerovani n-1 prikazu pro AND nebo OR
	for (int i = 0; i < argCnt - 1; i++) {
		localBytecodeArray[index++] = instrType;
	}

	*bytecodeArray = localBytecodeArray;
	return index;
}

int SchemeKeyword::generateBegin(SchemeObject *args[], int argsCnt, unsigned char **bytecodeArray) {
	unsigned char *localBytecodeArray = new unsigned char[1024];
	unsigned char *tempBytecode = NULL;
	int index = 0;
	int bytecodeLength = 0;

	// vygenerovani bytecodu pro vsechny argumenty
	for (int i = 0; i < argsCnt; i++) {
		bytecodeLength = args[i]->generateBytecode(&tempBytecode);
		for (int j = 0; j < bytecodeLength; j++, index++)
		{
			localBytecodeArray[index] = tempBytecode[j];
		}
		delete(tempBytecode);

		// smazat vysledek ze zasobniku kdyz to neni posledni prikaz
		if (i != argCounter - 1) {
			localBytecodeArray[index++] = BC_POP;
		}
	}

	*bytecodeArray = localBytecodeArray;
	return index;
}

int SchemeKeyword::generateCons(SchemeObject *obj1, SchemeObject *obj2, unsigned char **bytecodeArray) {
	unsigned char *localBytecodeArray = new unsigned char[1024];
	unsigned char *tempBytecode = NULL;
	int index = 0;
	int bytecodeLength = 0;

	// generovani bytecodu prvniho podstromu
	bytecodeLength = obj1->generateBytecode(&tempBytecode);
	for (int i = 0; i < bytecodeLength; i++, index++)
	{
		localBytecodeArray[index] = tempBytecode[i];
	}
	delete(tempBytecode);

	// generovani bytecodu druheho podstromu
	bytecodeLength = obj2->generateBytecode(&tempBytecode);
	for (int i = 0; i < bytecodeLength; i++, index++)
	{
		localBytecodeArray[index] = tempBytecode[i];
	}
	delete(tempBytecode);

	// generovani CONS
	localBytecodeArray[index++] = BC_CONS;

	*bytecodeArray = localBytecodeArray;
	return index;
}

int SchemeKeyword::generateList(SchemeObject *args[], int argCnt, unsigned char **bytecodeArray) {
	unsigned char *localBytecodeArray = new unsigned char[1024];
	unsigned char *tempBytecode = NULL;
	int index = 0;
	int bytecodeLength = 0;

	// vygenerovani bytecodu pro vsechny argumenty
	for (int i = 0; i < argCnt; i++) {
		bytecodeLength = args[i]->generateBytecode(&tempBytecode);
		for (int j = 0; j < bytecodeLength; j++, index++)
		{
			localBytecodeArray[index] = tempBytecode[j];
		}
		delete(tempBytecode);
	}

	// pridani NIL do posledni cons bunky
	localBytecodeArray[index++] = BC_PUSH_NIL;

	// vygenerovani n-1 prikazu pro CONS
	for (int i = 0; i < argCnt; i++) {
		localBytecodeArray[index++] = BC_CONS;
	}

	*bytecodeArray = localBytecodeArray;
	return index;
}

int SchemeKeyword::generateListModifier(instructionsEnum instrType, SchemeObject *obj, unsigned char **bytecodeArray) {
	unsigned char *localBytecodeArray = new unsigned char[1024];
	unsigned char *tempBytecode = NULL;
	int index = 0;
	int bytecodeLength = 0;

	bytecodeLength = obj->generateBytecode(&tempBytecode);
	for (int i = 0; i < bytecodeLength; i++, index++)
	{
		localBytecodeArray[index] = tempBytecode[i];
	}
	delete(tempBytecode);

	localBytecodeArray[index++] = instrType;

	*bytecodeArray = localBytecodeArray;
	return index;
}

int SchemeKeyword::generateMathOperation(instructionsEnum instrType, SchemeObject *args[], int argCnt, unsigned char **bytecodeArray) {
	unsigned char *localBytecodeArray = new unsigned char[1024];
	unsigned char *tempBytecode = NULL;
	int index = 0;
	int bytecodeLength = 0;

	// vygenerovani bytecodu pro vsechny argumenty
	// jednotlive podstromy se generuji v opacnem poradi aby se pak lepe vyhodnocovaly vyrazy
	for (int i = argCnt - 1; i >= 0; i--) {
		bytecodeLength = args[i]->generateBytecode(&tempBytecode);
		for (int j = 0; j < bytecodeLength; j++, index++)
		{
			localBytecodeArray[index] = tempBytecode[j];
		}
		delete(tempBytecode);
	}

	// vygenerovani n-1 prikazu pro AND nebo OR
	for (int i = 0; i < argCnt - 1; i++) {
		localBytecodeArray[index++] = instrType;
	}

	*bytecodeArray = localBytecodeArray;
	return index;
}

int SchemeKeyword::generateFileWrite(int fileIndex, SchemeObject *astRoot, unsigned char **bytecodeArray) {
	unsigned char *localBytecodeArray = new unsigned char[1024];
	unsigned char *tempBytecode = NULL;
	int index = 0;
	
	int bytecodeLength = astRoot->generateBytecode(&tempBytecode);
	for (int i = 0; i < bytecodeLength; i++, index++)
	{
		localBytecodeArray[index] = tempBytecode[i];
	}
	delete(tempBytecode);

	localBytecodeArray[index++] = BC_FILE_WRITE;
	localBytecodeArray[index++] = (unsigned char)fileIndex;

	*bytecodeArray = localBytecodeArray;
	return index;
}

int SchemeVariable::generateBytecode(unsigned char **bytecodeArray)
{
	unsigned char *localBytecodeArray = new unsigned char[1024];
	// je to promenna pri define
	if (isDefinedFunction == true)
	{
		if (defineLambdaFunctionArgCnt > defineFunctionArgCnt)
		{
			for (int i = 0; i < defineLambdaFunctionArgCnt; i++)
			{
				if (this->name == defineLambdaFunctionArgsArray[i])
				{
					localBytecodeArray[0] = BC_PUSH_ARG;
					localBytecodeArray[1] = (unsigned char)i;
					*bytecodeArray = localBytecodeArray;
					return 2;
				}
			}
		}
		else
		{
			for (int i = 0; i < defineFunctionArgCnt; i++)
			{
				if (this->name == defineFunctionArgsArray[i])
				{
					localBytecodeArray[0] = BC_PUSH_ARG;
					localBytecodeArray[1] = (unsigned char)i;
					*bytecodeArray = localBytecodeArray;
					return 2;
				}
			}
		}
	}

	//int idx = globalEnvTable->findFunction(name);
	int idx = translationTable->getIndex(name);
	if (idx >= 0) {
		// je to funkce (rekurzivni)
		// dat tenhle if
		if (this->argCounter > 0)
		{
			unsigned char *tempBytecode = NULL;
			int bytecodeLength = 0;
			int index = 0;
			int translationTableIdx = translationTable->getIndex(name);//globalEnvTable->findFunction(this->getName());
			// postorder - nejdrive musime projit vsechny potomky a pak sebe
			for (int i = 0; i < this->argCounter; i++)
			{
				bytecodeLength = args[i]->generateBytecode(&tempBytecode);
				for (int j = 0; j < bytecodeLength; j++, index++)
				{
					localBytecodeArray[index] = tempBytecode[j];
				}
				delete(tempBytecode);
			}
			// potomky uz mame, ted ziskame bytecode pro funkci
			localBytecodeArray[index++] = BC_CALL;
			localBytecodeArray[index++] = (unsigned char)translationTableIdx;
			*bytecodeArray = localBytecodeArray;
			return index;
		}
		else
		{
			localBytecodeArray[0] = BC_PUSH_VAR;
			localBytecodeArray[1] = (unsigned char)idx;
			*bytecodeArray = localBytecodeArray;
			return 2;
		}
			
	}

	printf("Variable ERROR!");
	return 0;
}

SchemeObject* SchemeObject::getArg(int position) {
	if (position < 0 || position >= argCounter) return NULL;
	return args[position];
}