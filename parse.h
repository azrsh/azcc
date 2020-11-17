#ifndef PARSE_H
#define PARSE_H

#include "container.h"
#include "statement.h"
#include "tokenize.h"
#include "type.h"

typedef struct FunctionDefinition FunctionDefinition;
struct FunctionDefinition {
  String name;
  Vector *arguments;           // Variavble Nodes
  CompoundStatement *body;
  size_t stackSize;
};

typedef struct Program Program;
struct Program {
  Vector *functions;       // FunctionDefinition vector
  Vector *globalVariables; // Variable vector
};

Program *parse(Token *head);

#endif
