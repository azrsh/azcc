#ifndef PARSE_H
#define PARSE_H

#include "container.h"
#include "statement.h"
#include "tokenize.h"
#include "type.h"

typedef struct FunctionDefinition FunctionDefinition;
struct FunctionDefinition {
  FunctionKind function;
  const String *name;      //常に非NULL
  Type *returnType;        //常に非NULL
  Vector *arguments;       // Variavble Nodes、常に非NULL
  CompoundStatement *body; //定義済みのとき常に非NULL
  size_t stackSize;        //定義済みのとき常に非NULL
};

typedef struct AbstractSyntaxTree AbstractSyntaxTree;
struct AbstractSyntaxTree {
  Vector *functionDefinitions; // Variable vector
  Vector *
      staticMemoryVariables; // Variable vector
                             // ここでいうstaticは静的なメモリ領域に確保されるという意味で、グローバル変数を含む
  Vector *stringLiterals;    // char* vector
};

AbstractSyntaxTree *parse(Token *head);

#endif
