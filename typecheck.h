#ifndef TYPECHECK_H
#define TYPECHECK_H

#include "functioncontainer.h"
#include "statement.h"

typedef struct TypeCheckContext TypeCheckContext;
struct TypeCheckContext {
  Type *returnType;
  FunctionContainer *functionContainer; // FunctionDeclaration vector
};

void tag_type_to_statement(StatementUnion *statementUnion,
                           TypeCheckContext *context);

#endif
