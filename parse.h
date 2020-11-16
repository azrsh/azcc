#ifndef PARSE_H
#define PARSE_H

#include "container.h"
#include "statement.h"
#include "tokenize.h"
#include "type.h"

typedef struct FunctionDefinition FunctionDefinition;
struct FunctionDefinition {
  String name;
  ListNode *variableContainer; // LocalVariable HashTable vector
  Vector *arguments;           // Local Variavble Nodes
  CompoundStatement *body;
  size_t stackSize;
};

ListNode *parse(Token *head);

#endif
