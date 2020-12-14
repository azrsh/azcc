#ifndef PARSE_H
#define PARSE_H

#include "container.h"
#include "statement.h"
#include "tokenize.h"
#include "type.h"

typedef struct FunctionDefinition FunctionDefinition;
struct FunctionDefinition {
  const String *name;      //常に非NULL
  Type *returnType;        //常に非NULL
  Vector *arguments;       // Variavble Nodes、常に非NULL
  CompoundStatement *body; //定義済みのとき常に非NULL
  size_t stackSize;        //定義済みのとき常に非NULL
};

typedef struct Typedef Typedef;
struct Typedef {
  const String *name;
  Type *type;
};

typedef struct Program Program;
struct Program {
  Vector *functionDefinitions; // FunctionDefinition vector
  Vector *globalVariables;     // Variable vector
  Vector *stringLiterals;      // char* vector
};

Program *parse(Token *head);

#endif
