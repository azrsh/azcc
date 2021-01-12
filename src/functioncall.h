#ifndef FUNCTIONCALL_H
#define FUNCTIONCALL_H

#include "container.h"
#include "type.h"
#include "variable.h"

typedef struct FunctionCall FunctionCall;
struct FunctionCall {
  Vector *arguments;  //引数のベクタ, Node
  Variable *returnStack;
};

#endif
