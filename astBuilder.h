#ifndef AST_BUILDER_H_INCLUDED
#define AST_BUILDER_H_INCLUDED

#include "ast.h"
#include "reader.h"

class AstBuilder {
	private:
		Reader *reader;
	public:
		AstBuilder(Reader *reader);
		SchemeObject* getNextAst();
};

#endif // AST_BUILDER_H_INCLUDED