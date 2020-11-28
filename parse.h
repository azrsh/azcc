#ifndef PARSE_H
#define PARSE_H

#include "container.h"
#include "statement.h"
#include "tokenize.h"
#include "type.h"

typedef struct FunctionDefinition FunctionDefinition;
struct FunctionDefinition {
  String name;
  Vector *arguments; // Variavble Nodes
  CompoundStatement *body;
  size_t stackSize;
};

typedef struct Typedef Typedef;
struct Typedef {
  String name;
  Type *type;
};

typedef struct Program Program;
struct Program {
  Vector *functions;       // FunctionDefinition vector
  Vector *globalVariables; // Variable vector
  Vector *stringLiterals;  // char* vector
};

Program *parse(Token *head);

#endif
