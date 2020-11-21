#include "variablecontainer.h"

typedef struct Scope Scope;
struct Scope {
  VariableContainer *variableContainer;
  String *beginLabel;
  String *endLabel;
};
