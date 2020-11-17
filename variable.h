#ifndef LOCALVARIABLE_H
#define LOCALVARIABLE_H

#include "container.h"
#include "type.h"

typedef enum {
  VARIABLE_LOCAL, // ローカル変数
  VARIABLE_GLOBAL // グローバル変数
} VariableKind;

typedef struct Variable Variable;
struct Variable {
  String name;       //名前
  Type *type;        //型
  VariableKind kind; //変数の種類
  int offset; // RBPからのオフセット、ローカル変数でのみ使用
};

#endif
