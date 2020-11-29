#include "typecheck.h"
#include "container.h"
#include "functioncall.h"
#include "functioncontainer.h"
#include "membercontainer.h"
#include "node.h"
#include "statement.h"
#include "type.h"
#include "util.h"
#include <stdlib.h>

Type *type_compare_deep_with_implicit_cast(Type *advantage,
                                           Type *disadvantage) {
  if (type_compare_deep(advantage, disadvantage))
    return advantage;
  if (!advantage || !disadvantage)
    return NULL;

  if (!advantage->base && !disadvantage->base)
    return advantage;

  if (type_compare_deep_with_implicit_cast(advantage->base, disadvantage->base))
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

Node *new_node_cast(Type *target, Node *source) {
  Node *castNode = calloc(1, sizeof(Node));
  castNode->kind = NODE_CAST;
  castNode->type = target;
  castNode->lhs = source;

  if (source->type->kind == TYPE_VOID)
    error("void型の値のキャストは禁止されています");

  return castNode;
}

void add_implicit_cast_node(Node *node) {
  Node *lhs = node->lhs;
  if (!type_compare_deep(node->type, lhs->type)) {
    Node *castNode = new_node_cast(node->type, lhs);
    node->lhs = castNode;
  }

  Node *rhs = node->rhs;
  if (!type_compare_deep(node->type, rhs->type)) {
    Node *castNode = new_node_cast(node->type, rhs);
    node->rhs = castNode;
  }
}

void tag_type_to_node_inner(Node *node, TypeCheckContext *context);

//式への型付け後にvoid型を弾く
void tag_type_to_node(Node *node, TypeCheckContext *context) {
  tag_type_to_node_inner(node, context);

  if (node->kind != NODE_FUNC && node->type->kind == TYPE_VOID)
    error("void型を値として操作することは禁止されています");
}

//式への型付け
void tag_type_to_node_inner(Node *node, TypeCheckContext *context) {
  if (!node) {
    error("指定されたノードが存在しません");
  }

  switch (node->kind) {
  case NODE_NUM:
    node->type = new_type(TYPE_INT);
    return;
  case NODE_STRING:
    node->type = new_type(TYPE_PTR);
    node->type->base = new_type(TYPE_CHAR);
    return;
  case NODE_LNOT:
    tag_type_to_node(node->lhs, context);
    node->type = node->lhs->type;
    return;
  case NODE_REF:
    tag_type_to_node(node->lhs, context);
    node->type = new_type(TYPE_PTR);
    node->type->base = node->lhs->type;
    return;
  case NODE_DEREF:
    tag_type_to_node(node->lhs, context);
    Type *lhsBase = node->lhs->type->base;
    if (!lhsBase)
      error("単項演算子*のオペランド型が不正です");
    node->type = lhsBase;
    return;
  case NODE_VAR:
    node->type = node->variable->type;
    return;
  case NODE_FUNC: {
    Type *returnType = node->functionCall->type;
    Vector *arguments = node->functionCall->arguments;
    node->type = returnType;

    Vector *argumentTypes = new_vector(16);
    for (int i = 0; i < vector_length(arguments); i++) {
      Node *arg = vector_get(arguments, i);
      tag_type_to_node(arg, context);
      vector_push_back(argumentTypes, arg->type);
    }
    //存在確認はパーサが行う
    FunctionDeclaration *declaration = function_container_get(
        context->functionContainer, node->functionCall->name);
    if (vector_length(declaration->arguments) == 0)
      return;
    if (vector_length(declaration->arguments) != vector_length(argumentTypes))
      error("関数の呼び出しと前方宣言の引数の数が一致しません");
    for (int i = 0; i < vector_length(declaration->arguments); i++) {
      Type *type1 = vector_get(declaration->arguments, i);
      Type *type2 = vector_get(argumentTypes, i);
      if (type_compare_deep(type1, type2))
        continue;

      if (type_is_primitive(type1) && type_is_primitive(type2)) {
        vector_set(arguments, i,
                   new_node_cast(type1, vector_get(arguments, i)));
        continue;
      }

      error("関数の呼び出しと前方宣言の引数の型が一致しません");
    }
    return;
  }
  case NODE_ASSIGN: {
    tag_type_to_node(node->lhs, context);
    tag_type_to_node(node->rhs, context);

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
  case NODE_DOT:
    tag_type_to_node(node->lhs, context);
    if (node->lhs->type->kind != TYPE_STRUCT)
      error("ドット演算子のオペランド型が不正です");
    if (node->rhs->kind != NODE_VAR)
      error("ドット演算子のオペランド型が不正です");
    node->rhs->variable = member_container_get(node->lhs->type->members,
                                               node->rhs->variable->name);
    tag_type_to_node(node->rhs, context);
    node->type = node->rhs->type;
    return;
  }

  //二項演算子の型検査
  tag_type_to_node(node->lhs, context);
  tag_type_to_node(node->rhs, context);
  Type *lhs = node->lhs->type;
  Type *rhs = node->rhs->type;

  switch (node->kind) {
  case NODE_ADD:
    if (lhs->base != NULL && rhs->kind == TYPE_INT) {
      node->type = lhs;
      return;
    }
    if (lhs->kind == TYPE_INT && rhs->base != NULL) {
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
    if (lhs->base != NULL && rhs->kind == TYPE_INT) {
      node->type = lhs;
      return;
    }
    if (lhs->kind == TYPE_PTR && rhs->base != NULL) {
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
  case NODE_LAND:
  case NODE_LOR:
    node->type = new_type(TYPE_INT);
    return;
  }

  error("予期しないノードが指定されました");
}

void tag_type_to_statement(StatementUnion *statementUnion,
                           TypeCheckContext *context) {
  if (!statementUnion) {
    error("指定された文が存在しません");
  }

  Type *returnType = context->returnType;

  // match if
  {
    IfStatement *ifPattern = statement_union_take_if(statementUnion);
    if (ifPattern) {
      tag_type_to_node(ifPattern->condition, context);
      tag_type_to_statement(ifPattern->thenStatement, context);
      if (ifPattern->elseStatement)
        tag_type_to_statement(ifPattern->elseStatement, context);
      return;
    }
  }

  // match switch
  {
    SwitchStatement *switchPattern =
        statement_union_take_switch(statementUnion);
    if (switchPattern) {
      tag_type_to_node(switchPattern->condition, context);
      tag_type_to_statement(switchPattern->statement, context);
      return;
    }
  }

  // match labeled
  {
    LabeledStatement *labeledPattern =
        statement_union_take_labeled(statementUnion);
    if (labeledPattern) {
      if (labeledPattern->constantExpression)
        tag_type_to_node(labeledPattern->constantExpression, context);
      tag_type_to_statement(labeledPattern->statement, context);
      return;
    }
  }

  // match while
  {
    WhileStatement *whilePattern = statement_union_take_while(statementUnion);
    if (whilePattern) {
      tag_type_to_node(whilePattern->condition, context);
      tag_type_to_statement(whilePattern->statement, context);
      return;
    }
  }

  // match do-while
  {
    DoWhileStatement *doWhilePattern =
        statement_union_take_do_while(statementUnion);
    if (doWhilePattern) {
      tag_type_to_node(doWhilePattern->condition, context);
      tag_type_to_statement(doWhilePattern->statement, context);
      return;
    }
  }

  // match for
  {
    ForStatement *forPattern = statement_union_take_for(statementUnion);
    if (forPattern) {
      tag_type_to_node(forPattern->initialization, context);
      tag_type_to_node(forPattern->condition, context);
      tag_type_to_node(forPattern->afterthought, context);
      tag_type_to_statement(forPattern->statement, context);
      return;
    }
  }

  // match compound
  {
    CompoundStatement *compoundPattern =
        statement_union_take_compound(statementUnion);
    if (compoundPattern) {
      ListNode *node = compoundPattern->statementHead;
      while (node) {
        tag_type_to_statement(node->body, context);
        node = node->next;
      }
      return;
    }
  }

  // match return
  {
    ReturnStatement *returnPattern =
        statement_union_take_return(statementUnion);
    if (returnPattern) {
      // voidのための処理
      if (returnType->kind == TYPE_VOID && !returnPattern->node)
        return;
      if (returnType->kind == TYPE_VOID)
        error("void型関数で戻り値が返されています");

      tag_type_to_node(returnPattern->node, context);
      if (!type_compare_deep(returnType, returnPattern->node->type) &&
          type_compare_deep_with_implicit_cast(returnType,
                                               returnPattern->node->type)) {
        returnPattern->node = new_node_cast(returnType, returnPattern->node);
      }
      return;
    }
  }

  // match break
  {
    BreakStatement *breakPattern = statement_union_take_break(statementUnion);
    if (breakPattern) {
      return;
    }
  }

  // match continue
  {
    ContinueStatement *continuePattern =
        statement_union_take_continue(statementUnion);
    if (continuePattern) {
      return;
    }
  }

  // match null
  {
    NullStatement *nullPattern = statement_union_take_null(statementUnion);
    if (nullPattern) {
      return;
    }
  }

  // match expression
  {
    ExpressionStatement *expressionPattern =
        statement_union_take_expression(statementUnion);
    if (expressionPattern) {
      tag_type_to_node(expressionPattern->node, context);
      return;
    }
  }

  error("予期しない文が指定されました");
}
