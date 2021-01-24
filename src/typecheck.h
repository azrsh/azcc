#ifndef TYPECHECK_H
#define TYPECHECK_H

#include "statement.h"
#include "variablecontainer.h"

typedef struct TypeCheckContext TypeCheckContext;
struct TypeCheckContext {
  Type *returnType;
  VariableContainer *variableContainer;
};

void tag_type_to_statement(StatementUnion *statementUnion,
                           TypeCheckContext *context);

#endif
