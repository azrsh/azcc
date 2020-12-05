#ifndef LOCALVARIABLE_H
#define LOCALVARIABLE_H

#include "container.h"
#include "type.h"

typedef struct Node Node;

typedef enum {
  VARIABLE_LOCAL,      // ローカル変数
  VARIABLE_GLOBAL,     // グローバル変数
  VARIABLE_ENUMERATOR, // 列挙子
  VARIABLE_MEMBER      // メンバ変数
} VariableKind;

typedef struct Variable Variable;
struct Variable {
  const String *name; //名前
  Type *type;         //型
  VariableKind kind;  //変数の種類
  int offset; // RBPからのオフセット、ローカル変数でのみ使用
  Node *initialization; // 初期化式、グローバル変数でのみ使用
};

#endif
