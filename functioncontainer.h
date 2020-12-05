#ifndef FUNCTIONCONTAINER
#define FUNCTIONCONTAINER

#include "container.h"
#include "type.h"
#include "variable.h"

typedef struct FunctionDeclaration FunctionDeclaration;
struct FunctionDeclaration {
  const String *name;
  Type *returnType;
  Vector *arguments; // Type vector
};

typedef struct FunctionContainer FunctionContainer;
FunctionContainer *new_function_container();
FunctionDeclaration *function_container_get(FunctionContainer *container,
                                            const String *name);
bool function_container_push(FunctionContainer *container,
                             FunctionDeclaration *declaration);

#endif
