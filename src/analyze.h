#ifndef ANALYZE_H
#define ANALYZE_H

#include "declaration.h"
#include "node.h"
#include "parseutil.h"
#include "statement.h"
#include "type.h"

// 関数宣言の読み替え
void analyze_function_type(Type *type);
Node *analyze_local_declaration(Declaration *target, ParseContext *context);
void analyze_global_declaration(Declaration *target, Vector *globalVariables,
                                ParseContext *context);
StatementUnion *analyze_blockItem_declaration(Declaration *declaration,
                                              ParseContext *context);

#endif
