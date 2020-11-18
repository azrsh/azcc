#include "typecheck.h"
#include "node.h"
#include "type.h"
#include "util.h"
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

Type *type_compare_deep_with_implicit_cast(Type *advantage,
                                           Type *disadvantage) {
  if (type_compare_deep(advantage, disadvantage))
    return advantage;
  if (!advantage || !disadvantage)
    return NULL;

  if (advantage->base && disadvantage->base)
    return type_compare_deep_with_implicit_cast(advantage->base,
                                                disadvantage->base);

  if (!advantage->base && !disadvantage->base)
    return advantage;

  return NULL;
}

Type *check_arithmetic_binary_operator(Type *lhs, Type *rhs) {
  if (lhs->base || rhs->base)
    return NULL;

  size_t lhsSize = type_to_size(lhs);
  size_t rhsSize = type_to_size(rhs);
  if (lhsSize >= rhsSize)
    return lhs;
  else
    return rhs;

  return NULL;
}

void add_implicit_cast_node(Node *node) {
  Node *lhs = node->lhs;
  if (!type_compare_deep(node->type, lhs->type)) {
    Node *castNode = calloc(1, sizeof(Node));
    castNode->kind = NODE_CAST;
    castNode->type = node->type;
    node->lhs = castNode;
    castNode->lhs = lhs;
  }

  Node *rhs = node->rhs;
  if (!type_compare_deep(node->type, rhs->type)) {
    Node *castNode = calloc(1, sizeof(Node));
    castNode->kind = NODE_CAST;
    castNode->type = node->type;
    node->rhs = castNode;
    castNode->lhs = rhs;
  }
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
  case NODE_ASSIGN: {
    tag_type_to_node(node->lhs);
    tag_type_to_node(node->rhs);

    //代入は代入先の型を最優先とする
    Type *result =
        type_compare_deep_with_implicit_cast(node->lhs->type, node->rhs->type);
    if (result) {
      node->type = result;
      if (!node->lhs->type->base && !node->rhs->type->base) {
        add_implicit_cast_node(node);
      }
      return;
    }

    error("演算子=のオペランド型が不正です");
  }
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
    if (lhs->kind == INT && rhs->base != NULL) {
      error("未実装の演算です");
    }
    {
      Type *result = check_arithmetic_binary_operator(lhs, rhs);
      if (result) {
        node->type = result;
        add_implicit_cast_node(node);
        return;
      }
    }
    error("演算子+のオペランド型が不正です");
  case NODE_SUB:
    if (lhs->base != NULL && rhs->kind == INT) {
      node->type = lhs;
      return;
    }
    if (lhs->kind == PTR && rhs->base != NULL) {
      error("未実装の演算です");
    }
    {
      Type *result = check_arithmetic_binary_operator(lhs, rhs);
      if (result) {
        node->type = result;
        add_implicit_cast_node(node);
        return;
      }
    }
    error("演算子-のオペランド型が不正です");
  case NODE_MUL: {
    Type *result = check_arithmetic_binary_operator(lhs, rhs);
    if (result) {
      node->type = result;
      add_implicit_cast_node(node);
      return;
    }
    error("演算子*のオペランド型が不正です");
  }
  case NODE_DIV: {
    Type *result = check_arithmetic_binary_operator(lhs, rhs);
    if (result) {
      node->type = result;
      add_implicit_cast_node(node);
      return;
    }
    error("演算子/のオペランド型が不正です");
  }
  case NODE_EQ: {
    Type *result = check_arithmetic_binary_operator(lhs, rhs);
    if (result) {
      node->type = result;
      add_implicit_cast_node(node);
      return;
    }
    error("演算子==のオペランド型が不正です");
  }
  case NODE_NE: {
    Type *result = check_arithmetic_binary_operator(lhs, rhs);
    if (result) {
      node->type = result;
      add_implicit_cast_node(node);
      return;
    }
    error("演算子!=のオペランド型が不正です");
  }
  case NODE_LT: {
    Type *result = check_arithmetic_binary_operator(lhs, rhs);
    if (result) {
      node->type = result;
      add_implicit_cast_node(node);
      return;
    }
    error("演算子<または>=のオペランド型が不正です");
  }
  case NODE_LE: {
    Type *result = check_arithmetic_binary_operator(lhs, rhs);
    if (result) {
      node->type = result;
      add_implicit_cast_node(node);
      return;
    }
    error("演算子<=または>のオペランド型が不正です");
  }
  }

  error("予期しないノードが指定されました");
}
