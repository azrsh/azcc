#ifndef FUNCTIONCONTAINER
#define FUNCTIONCONTAINER

#include "container.h"
#include "type.h"
#include "variable.h"

typedef struct FunctionDeclaration FunctionDeclaration;
struct FunctionDeclaration {
  String name;
  Type *returnType;
};

typedef struct FunctionContainer FunctionContainer;
FunctionContainer *new_function_container();
FunctionDeclaration *function_container_get(FunctionContainer *container,
                                            String name);
bool function_container_push(FunctionContainer *container,
                             FunctionDeclaration *declaration);

#endif
