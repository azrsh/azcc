#ifndef TYPECHECK_H
#define TYPECHECK_H

#include "statement.h"

void tag_type_to_statement(StatementUnion *statementUnion, Type *returnType);

#endif
