#ifndef DECLARATION_H
#define DECLARATION_H

#include "container.h"
#include "type.h"

typedef struct Declaration Declaration;
struct Declaration {
  Type *type;
  Vector *declarators;
};

#endif
