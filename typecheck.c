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
  castNode->source = source->source;

  if (source->type->kind == TYPE_VOID)
    error_at(source->source, "void型の値のキャストは禁止されています");

  return castNode;
}

void insert_implicit_cast_node(Type *target, Node *node) {
  Node *lhs = node->lhs;
  if (!type_compare_deep(target, lhs->type)) {
    Node *castNode = new_node_cast(target, lhs);
    node->lhs = castNode;
  }

  Node *rhs = node->rhs;
  if (!type_compare_deep(target, rhs->type)) {
    Node *castNode = new_node_cast(target, rhs);
    node->rhs = castNode;
  }
}

void tag_type_to_node_inner(Node *node, TypeCheckContext *context);

//式への型付け後にvoid型を弾く
void tag_type_to_node(Node *node, TypeCheckContext *context) {
  tag_type_to_node_inner(node, context);

  if (node->kind != NODE_FUNC && node->type->kind == TYPE_VOID)
    error_at(node->source, "void型を値として操作することは禁止されています");
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
  case NODE_CHAR:
    node->type = new_type(TYPE_CHAR);
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
      error_at(node->source, "単項演算子*のオペランド型が不正です");
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
      error_at(node->source,
               "関数の呼び出しと前方宣言の引数の数が一致しません");
    for (int i = 0; i < vector_length(declaration->arguments); i++) {
      Type *type1 = vector_get(declaration->arguments, i);
      Type *type2 = vector_get(argumentTypes, i);
      if (type_compare_deep(type1, type2))
        continue;

      if (type_compare_deep_with_implicit_cast(type1, type2)) {
        vector_set(arguments, i,
                   new_node_cast(type1, vector_get(arguments, i)));
        continue;
      }

      error_at(node->source,
               "関数の呼び出しと前方宣言の引数の型が一致しません");
    }
    return;
  }
  case NODE_ASSIGN: {
    tag_type_to_node(node->lhs, context);
    tag_type_to_node(node->rhs, context);

    bool isLhsNull = node->rhs->type->kind == TYPE_PTR &&
                     node->lhs->kind == NODE_NUM && node->lhs->val == 0;
    bool isRhsNull = node->lhs->type->kind == TYPE_PTR &&
                     node->rhs->kind == NODE_NUM && node->rhs->val == 0;
    if (isLhsNull) {
      node->lhs = new_node_cast(node->rhs->type, node->lhs);
    } else if (isRhsNull) {
      node->rhs = new_node_cast(node->lhs->type, node->rhs);
    }

    //代入は代入先の型を最優先とする
    if (type_compare_deep_with_implicit_cast(node->lhs->type,
                                             node->rhs->type)) {
      node->type = node->lhs->type;
      if (type_is_primitive(node->lhs->type) &&
          type_is_primitive(node->rhs->type)) {
        insert_implicit_cast_node(node->type, node);
      } else if (node->type->kind == TYPE_PTR &&
                 node->rhs->type->kind == TYPE_ARRAY) {
        node->rhs = new_node_cast(node->type, node->rhs);
      }
      return;
    }

    error_at(node->source, "演算子=のオペランド型が不正です");
  }
  case NODE_DOT:
    tag_type_to_node(node->lhs, context);
    if (node->lhs->type->kind != TYPE_STRUCT)
      error_at(node->source, "ドット演算子のオペランド型が不正です");
    if (node->rhs->kind != NODE_VAR)
      error_at(node->source, "ドット演算子のオペランド型が不正です");
    node->rhs->variable = member_container_get(node->lhs->type->members,
                                               node->rhs->variable->name);
    tag_type_to_node(node->rhs, context);
    node->type = node->rhs->type;
    return;
  case NODE_CAST:
    return; //キャストは型検査を行わない
  case NODE_ADD:
  case NODE_SUB:
  case NODE_MUL:
  case NODE_DIV:
  case NODE_MOD:
  case NODE_EQ:
  case NODE_NE:
  case NODE_LT:
  case NODE_LE:
  case NODE_LAND:
  case NODE_LOR:
    break; //次のswitch文で判定する
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
      error_at(node->source, "未実装の演算です");
    }
    {
      Type *result = check_arithmetic_binary_operator(lhs, rhs);
      if (result) {
        node->type = result;
        insert_implicit_cast_node(node->type, node);
        return;
      }
    }
    error_at(node->source, "演算子+のオペランド型が不正です");
  case NODE_SUB:
    if (lhs->base != NULL && rhs->kind == TYPE_INT) {
      node->type = lhs;
      return;
    }
    if (lhs->base != NULL && rhs->base != NULL &&
        type_compare_deep(lhs->base, rhs->base)) {
      node->type = new_type(TYPE_INT);
      return;
    }
    {
      Type *result = check_arithmetic_binary_operator(lhs, rhs);
      if (result) {
        node->type = result;
        insert_implicit_cast_node(node->type, node);
        return;
      }
    }
    error_at(node->source, "演算子-のオペランド型が不正です");
  case NODE_MUL:
  case NODE_DIV:
  case NODE_MOD: {
    Type *result = check_arithmetic_binary_operator(lhs, rhs);
    if (result) {
      node->type = result;
      insert_implicit_cast_node(node->type, node);
      return;
    }
    error_at(node->source, "乗除算演算子のオペランド型が不正です");
  }
  case NODE_EQ:
  case NODE_NE:
  case NODE_LT:
  case NODE_LE: {
    //ポインタ型と0の比較のときのみ0をNULLポインタと見なす
    bool isLhsNull = rhs->kind == TYPE_PTR && node->lhs->kind == NODE_NUM &&
                     node->lhs->val == 0;
    bool isRhsNull = lhs->kind == TYPE_PTR && node->rhs->kind == NODE_NUM &&
                     node->rhs->val == 0;
    if (isLhsNull) {
      node->lhs = new_node_cast(rhs, node->lhs);
      lhs = node->lhs->type;
    } else if (isRhsNull) {
      node->rhs = new_node_cast(lhs, node->rhs);
      rhs = node->rhs->type;
    }

    //比較対象の型の決定
    Type *target = check_arithmetic_binary_operator(lhs, rhs);
    if (!target && type_compare_deep_with_implicit_cast(lhs, rhs)) {
      target = lhs;
    }

    if (target) {
      node->type = new_type(TYPE_INT);
      insert_implicit_cast_node(target, node);
      return;
    }
    error_at(node->source, "比較演算子のオペランド型が不正です");
  }
  case NODE_LAND:
  case NODE_LOR:
    node->type = new_type(TYPE_INT);
    return;
  case NODE_LNOT:
  case NODE_REF:
  case NODE_DEREF:
  case NODE_ASSIGN:
  case NODE_VAR:
  case NODE_FUNC:
  case NODE_NUM:
  case NODE_CHAR:
  case NODE_STRING:
  case NODE_CAST:
  case NODE_DOT:
    error("コンパイラの内部エラー");
  }

  error_at(node->source, "予期しないノードが指定されました");
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
      if (forPattern->initialization)
        tag_type_to_node(forPattern->initialization, context);
      if (forPattern->condition)
        tag_type_to_node(forPattern->condition, context);
      if (forPattern->afterthought)
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
