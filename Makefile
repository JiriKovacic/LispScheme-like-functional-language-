all:
	g++ -std=c++11 -o scheme ast.h ast.cpp astBuilder.h astBuilder.cpp bytecodeGenerator.h bytecodeGenerator.cpp frame.h frame.cpp garbageCollector.h garbageCollector.cpp globalEnv.h globalEnv.cpp globalStack.h globalStack.cpp instructionSet.h instructionSet.cpp keywords.h keywords.cpp main.cpp memory.h memory.cpp memoryItem.h memoryItem.cpp memoryType.h memoryType.cpp reader.h reader.cpp virtualMachine.h virtualMachine.cpp

run:
	./scheme

clean:
	rm -f scheme *~
