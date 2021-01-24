#ifndef NODE_H
#define NODE_H

#include "container.h"
#include "functioncall.h"
#include "variable.h"

typedef enum {
  NODE_ADD,    // +
  NODE_SUB,    // -
  NODE_MUL,    // *
  NODE_DIV,    // /
  NODE_MOD,    // %
  NODE_LSHIFT, // <<
  NODE_RSHIFT, // >>
  NODE_EQ,     // ==
  NODE_NE,     // !=
  NODE_LT,     // <
  NODE_LE,     // <=
  NODE_LAND,   // &&
  NODE_LOR,    // ||
  NODE_BAND,   // &
  NODE_BXOR,   // ^
  NODE_BOR,    // |
  NODE_LNOT,   // !
  NODE_BNOT,   // ~
  NODE_COND,   // cond ? res : res
  NODE_REF,    // &
  NODE_DEREF,  // *
  NODE_ASSIGN, // 代入
  NODE_COMMA,  // 代入
  NODE_VAR,    // 変数
  NODE_FUNC,   // 関数
  NODE_NUM,    // 整数
  NODE_CHAR,   // 文字
  NODE_STRING, // 文字列
  NODE_ARRAY,  // 配列
  NODE_CAST,   // キャスト
  NODE_DOT,    // ドット演算子
} NodeKind;

typedef struct Node Node;
struct Node {
  NodeKind kind;              //ノードの種類
  Type *type;                 //ノードの型
  Node *lhs;                  //左辺
  Node *rhs;                  //右辺
  Node *condition;            // kindがNODE_CONDのときのみ使う
  const char *source;         //エラー出力のための情報
  Variable *variable;         // kindがNODE_VARのときのみ使う
  FunctionCall *functionCall; // kindがNODE_FUNCのときのみ使う

  Vector *elements; // Node Vector、kindがNODE_ARRAYのときのみ使う
  int val;          // kindがNODE_NUMのときに数字
                    // NODE_CHARのとき文字
                    // NODE_STRINGのとき文字列リテラルのID
};

#endif
