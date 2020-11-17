#include "typecheck.h"
#include "type.h"
#include <stdlib.h>

Type *new_type(TypeKind kind) {
  Type *type = calloc(1, sizeof(Type));
  type->kind = kind;
  return type;
}

bool type_compare_deep(Type *type1, Type *type2) {
  return (!type1 && !type2) || (type1 && type2 && type1->kind == type2->kind &&
                                type1->length == type2->length &&
                                type_compare_deep(type1->base, type2->base));
}

bool type_compare_deep_with_implicit_cast(Type *type1, Type *type2) {
  if (type_compare_deep(type1, type2))
    return true;
  if (!type1 || !type2)
    return false;

  if (type1->base && type2->base)
    return type_compare_deep_with_implicit_cast(type1->base, type2->base);

  return false;
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
    Type *lhsBase = node->lhs->type->base;
    if (!lhsBase)
      error("単項演算子*のオペランド型が不正です");
    node->type = lhsBase;
    return;
  case NODE_VAR:
    node->type = node->variable->type;
    return;
  case NODE_FUNC:
    node->type = node->functionCall->type;
    return;
  case NODE_ASSIGN:
    tag_type_to_node(node->lhs);
    tag_type_to_node(node->rhs);

    if (type_compare_deep_with_implicit_cast(node->lhs->type,
                                             node->rhs->type)) {
      node->type = node->lhs->type;
      return;
    }
    error("演算子=のオペランド型が不正です");
  }

  //二項演算子の型検査
  tag_type_to_node(node->lhs);
  tag_type_to_node(node->rhs);
  Type *lhs = node->lhs->type;
  Type *rhs = node->rhs->type;

  switch (node->kind) {
  case NODE_ADD:
    if (lhs->base != NULL && rhs->kind == INT) {
      node->type = lhs;
      return;
    }
    if (lhs->kind == INT && rhs->kind == INT) {
      node->type = lhs;
      return;
    }
    if (lhs->kind == INT && rhs->base != NULL) {
      error("未実装の演算です");
    }
    error("演算子+のオペランド型が不正です");
  case NODE_SUB:
    if (lhs->base != NULL && rhs->kind == INT) {
      node->type = lhs;
      return;
    }
    if (lhs->kind == INT && rhs->kind == INT) {
      node->type = lhs;
      return;
    }
    if (lhs->kind == PTR && rhs->base != NULL) {
      error("未実装の演算です");
    }
    error("演算子-のオペランド型が不正です");
  case NODE_MUL:
    if (lhs->kind == INT && rhs->kind == INT) {
      node->type = lhs;
      return;
    }
    error("演算子*のオペランド型が不正です");
  case NODE_DIV:
    if (lhs->kind == INT && rhs->kind == INT) {
      node->type = lhs;
      return;
    }
    error("演算子/のオペランド型が不正です");
  case NODE_EQ:
    if (lhs->kind == INT && rhs->kind == INT) {
      node->type = lhs;
      return;
    }
    error("演算子==のオペランド型が不正です");
  case NODE_NE:
    if (lhs->kind == INT && rhs->kind == INT) {
      node->type = lhs;
      return;
    }
    error("演算子!=のオペランド型が不正です");
  case NODE_LT:
    if (lhs->kind == INT && rhs->kind == INT) {
      node->type = lhs;
      return;
    }
    error("演算子<または>=のオペランド型が不正です");
  case NODE_LE:
    if (lhs->kind == INT && rhs->kind == INT) {
      node->type = lhs;
      return;
    }
    error("演算子<=または>のオペランド型が不正です");
  }

  error("予期しないノードが指定されました");
}
