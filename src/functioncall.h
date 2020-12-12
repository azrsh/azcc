#ifndef FUNCTIONCALL_H
#define FUNCTIONCALL_H

#include "container.h"
#include "type.h"

typedef struct FunctionCall FunctionCall;
struct FunctionCall {
  const String *name; //名前
  Vector *arguments;  //引数の連結リスト, Node
};

#endif
