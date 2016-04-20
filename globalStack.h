#ifndef GLOBAL_STACK_H_INCLUDED
#define GLOBAL_STACK_H_INCLUDED

#include <stack>

using namespace std;

class GlobalStack {
private:
	stack<int> globalStack;
public:
	GlobalStack();
	void pushInt(int number);
	int popInt();
	int getSize();
};

#endif // GLOBAL_STACK_H_INCLUDED