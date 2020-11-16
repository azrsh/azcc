#ifndef LOCALVARIABLE_H
#define LOCALVARIABLE_H

#include "container.h"
#include "type.h"

typedef struct LocalVariable LocalVariable;
struct LocalVariable {
  String name; //名前
  Type *type;  //名前
  int offset;  // RBPからのオフセット
};

#endif
