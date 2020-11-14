#include "9cc.h"
#include <stdlib.h>

Type *new_type(TypeKind kind) {
  Type *type = calloc(1, sizeof(Type));
  type->kind = kind;
  return type;
}

void tag_type_to_node(Node *node) {
  if (!node) {
    error("指定されたノードが存在しません");
  }

  switch (node->kind) {
  case NODE_NUM:
    node->type = new_type(INT);
    return;
  case NODE_REF:
    tag_type_to_node(node->lhs);
    node->type = new_type(PTR);
    node->type->base = node->lhs->type;
    return;
  case NODE_DEREF:
    tag_type_to_node(node->lhs);
    node->type = node->lhs->type->base;
    return;
  case NODE_LVAR:
    //検査済みのため通過
    return;
  case NODE_FUNC:
    node->type = node->functionCall->type;
    return;
  case NODE_ASSIGN:
    tag_type_to_node(node->lhs);
    tag_type_to_node(node->rhs);
    if (node->lhs->type->kind == node->rhs->type->kind) {
      node->type = node->lhs->type;
      return;
    }
    return;
  }

  //二項演算子の型検査
  tag_type_to_node(node->lhs);
  tag_type_to_node(node->rhs);
  Type *lhs = node->lhs->type;
  Type *rhs = node->rhs->type;

  switch (node->kind) {
  case NODE_ADD:
    if (lhs->kind == PTR && rhs->kind == INT) {
      node->type = lhs;
      return;
    }
    if (lhs->kind == INT && rhs->kind == INT) {
      node->type = lhs;
      return;
    }
    if (lhs->kind == INT && rhs->kind == PTR) {
      error("未実装の演算です");
    }
    error("加算演算子のオペランド型が不正です");
  case NODE_SUB:
    if (lhs->kind == PTR && rhs->kind == INT) {
      node->type = lhs;
      return;
    }
    if (lhs->kind == INT && rhs->kind == INT) {
      node->type = lhs;
      return;
    }
    if (lhs->kind == PTR && rhs->kind == PTR) {
      error("未実装の演算です");
    }
    error("加算演算子のオペランド型が不正です");
  case NODE_MUL:
    if (lhs->kind == INT && rhs->kind == INT) {
      node->type = lhs;
      return;
    }
    error("加算演算子のオペランド型が不正です");
    return;
  case NODE_DIV:
    if (lhs->kind == INT && rhs->kind == INT) {
      node->type = lhs;
      return;
    }
    return;
  case NODE_EQ:
    if (lhs->kind == INT && rhs->kind == INT) {
      node->type = lhs;
      return;
    }
    return;
  case NODE_NE:
    if (lhs->kind == INT && rhs->kind == INT) {
      node->type = lhs;
      return;
    }
    return;
  case NODE_LT:
    if (lhs->kind == INT && rhs->kind == INT) {
      node->type = lhs;
      return;
    }
    return;
  case NODE_LE:
    if (lhs->kind == INT && rhs->kind == INT) {
      node->type = lhs;
      return;
    }
    return;
  }

  error("予期しないノードが指定されました");
}
