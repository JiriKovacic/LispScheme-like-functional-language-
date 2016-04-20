#include "globalStack.h"

GlobalStack::GlobalStack(){ }

void GlobalStack::pushInt(int number) {
	globalStack.push(number);
}

int GlobalStack::popInt() {
	int number = globalStack.top();
	globalStack.pop();
	return number;
}

int GlobalStack::getSize()
{
	return globalStack.size();
}