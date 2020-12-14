#ifndef FUNCTIONCONTAINER
#define FUNCTIONCONTAINER

#include "container.h"
#include "parse.h"
#include "type.h"
#include "variable.h"

typedef struct FunctionContainer FunctionContainer;
FunctionContainer *new_function_container();
FunctionDefinition *function_container_get(FunctionContainer *container,
                                           const String *name);
bool function_container_push(FunctionContainer *container,
                             FunctionDefinition *declaration);

#endif
