#ifndef NODE_H
#define NODE_H

#include "container.h"
#include "functioncall.h"
#include "type.h"

typedef enum {
  NODE_ADD,    // +
  NODE_SUB,    // -
  NODE_MUL,    // *
  NODE_DIV,    // /
  NODE_EQ,     // ==
  NODE_NE,     // !=
  NODE_LT,     // <
  NODE_LE,     // <=
  NODE_REF,    // &
  NODE_DEREF,  // *
  NODE_IF,     // if文
  NODE_ASSIGN, // 代入
  NODE_LVAR,   // ローカル変数
  NODE_FUNC,   // 関数
  NODE_ARG,    // 関数の引数
  NODE_NUM     // 整数
} NodeKind;

typedef struct Node Node;
struct Node {
  NodeKind kind; //ノードの型
  Node *lhs;
  Node *rhs;
  int val;                    // kindがNODE_NUMのときのみ使う
  int offset;                 // kindがNODE_LVARのときのみ使う
  Type *type;                 // kindがNODE_LVARのときのみ使う
  FunctionCall *functionCall; // kindがNODE_FUNCのときのみ使う
};

#endif
