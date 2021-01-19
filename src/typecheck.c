#include "typecheck.h"
#include "container.h"
#include "declaration.h"
#include "functioncall.h"
#include "membercontainer.h"
#include "node.h"
#include "parse.h"
#include "statement.h"
#include "type.h"
#include "util.h"
#include "variable.h"
#include "variablecontainer.h"
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
    ERROR_AT(source->source, "void型の値のキャストは禁止されています");

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
    ERROR_AT(node->source, "void型を値として操作することは禁止されています");
}

//式への型付け
void tag_type_to_node_inner(Node *node, TypeCheckContext *context) {
  if (!node) {
    ERROR("指定されたノードが存在しません");
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
  case NODE_ARRAY: {
    //ここでいちおう型付けしているが、グローバル変数の初期化のときには強制的に宣言の型に上書きされる
    node->type = new_type(TYPE_ARRAY);
    Type *baseType = NULL;
    for (int i = 0; i < vector_length(node->elements); i++) {
      Node *element = vector_get(node->elements, i);
      tag_type_to_node(element, context);
      if (baseType)
        type_compare_deep_with_implicit_cast(baseType, element->type);
      else
        baseType = element->type;
    }
    node->type->base = baseType;
    return;
  }
  case NODE_LNOT:
    tag_type_to_node(node->lhs, context);
    node->type = new_type(TYPE_INT);
    return;
  case NODE_BNOT:
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
    if (lhsBase) {
      node->type = lhsBase;
      return;
    }

    if (node->lhs->type->kind == TYPE_FUNC) {
      node->type = node->lhs->type;
      return;
    }

    ERROR_AT(node->source, "単項演算子*のオペランド型が不正です");
    return;
  case NODE_VAR:
    node->type = node->variable->type;
    return;
  case NODE_FUNC: {
    //存在確認はパーサが行うので存在確認をスキップ
    tag_type_to_node(node->lhs, context);
    if (node->lhs->type->kind != TYPE_FUNC)
      ERROR("関数呼び出し演算子のオペランド型が不正です");

    Type *functionType = node->lhs->type;
    node->type = functionType->returnType;

    //引数の検証
    {
      Vector *callingArguments = node->functionCall->arguments;

      //型付けと同時にType型のVectorを生成
      //型付けも行っているので全てのreturn文の前に置く
      Vector *callingTypes = new_vector(16);
      for (int i = 0; i < vector_length(callingArguments); i++) {
        Node *arg = vector_get(callingArguments, i);
        tag_type_to_node(arg, context);
        vector_push_back(callingTypes, arg->type);
      }

      //引数がNULLならスキップ
      if (!functionType->arguments)
        return;

      // type_compare
      if (vector_length(functionType->arguments) !=
          vector_length(callingArguments))
        ERROR_AT(node->source,
                 "関数呼び出しと前方宣言の引数の数が一致しません");

      for (int i = 0; i < vector_length(functionType->arguments); i++) {
        Declaration *declaration = vector_get(functionType->arguments, i);
        Variable *variable = vector_get(declaration->declarators,
                                        0); //関数の引数のとき、必ず要素数は1

        Type *type1 = variable->type;
        Type *type2 = vector_get(callingTypes, i);
        if (type_compare_deep(type1, type2))
          continue;

        if (type_compare_deep_with_implicit_cast(type1, type2)) {
          Node *castNode =
              new_node_cast(type1, vector_get(callingArguments, i));
          vector_set(callingArguments, i, castNode);
          continue;
        }

        // 0をNULL扱いすべきか検証する
        {
          Node *argument = vector_get(callingArguments, i);
          bool isNull = type1->kind == TYPE_PTR && argument->kind == NODE_NUM &&
                        argument->val == 0;
          if (isNull) {
            vector_set(callingArguments, i, new_node_cast(type1, argument));
            continue;
          }
        }

        ERROR_AT(node->source,
                 "関数呼び出しと前方宣言の引数の型が一致しません");
      }
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

    ERROR_AT(node->source, "演算子=のオペランド型が不正です");
  }
  case NODE_DOT:
    tag_type_to_node(node->lhs, context);
    if (node->lhs->type->kind != TYPE_STRUCT)
      ERROR_AT(node->source, "ドット演算子のオペランド型が不正です");
    if (!node->lhs->type->members)
      ERROR_AT(node->source, "構造体の定義がみつかりません");
    if (node->rhs->kind != NODE_VAR)
      ERROR_AT(node->source, "ドット演算子のオペランド型が不正です");
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
  case NODE_BAND:
  case NODE_BXOR:
  case NODE_BOR:
  case NODE_LSHIFT:
  case NODE_RSHIFT:
  case NODE_COND:
  case NODE_COMMA:
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
      node->type = rhs;
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
    ERROR_AT(node->source, "演算子+のオペランド型が不正です");
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
    ERROR_AT(node->source, "演算子-のオペランド型が不正です");
  case NODE_MUL:
  case NODE_DIV:
  case NODE_MOD: {
    Type *result = check_arithmetic_binary_operator(lhs, rhs);
    if (result) {
      node->type = result;
      insert_implicit_cast_node(node->type, node);
      return;
    }
    ERROR_AT(node->source, "乗除算演算子のオペランド型が不正です");
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
    ERROR_AT(node->source, "比較演算子のオペランド型が不正です");
  }
  case NODE_LAND:
  case NODE_LOR:
    node->type = new_type(TYPE_INT);
    return;
  case NODE_BAND:
  case NODE_BXOR:
  case NODE_BOR:
  case NODE_LSHIFT:
  case NODE_RSHIFT: {
    Type *result = check_arithmetic_binary_operator(lhs, rhs);
    if (result) {
      node->type = result;
      insert_implicit_cast_node(node->type, node);
      return;
    }
    ERROR_AT(node->source, "ビット算演算子のオペランド型が不正です");
  }
  case NODE_COND: {
    tag_type_to_node(node->condition, context);

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

    if (type_compare_deep_with_implicit_cast(lhs, rhs)) {
      node->type = lhs;
      node->rhs = new_node_cast(lhs, node->rhs);
      return;
    }
    ERROR_AT(node->source, "条件演算子のオペランド型が不正です");
  }
  case NODE_COMMA:
    node->type = rhs;
    return;
  case NODE_LNOT:
  case NODE_BNOT:
  case NODE_REF:
  case NODE_DEREF:
  case NODE_ASSIGN:
  case NODE_VAR:
  case NODE_FUNC:
  case NODE_NUM:
  case NODE_CHAR:
  case NODE_STRING:
  case NODE_ARRAY:
  case NODE_CAST:
  case NODE_DOT:
    ERROR("コンパイラの内部エラー");
  }

  ERROR_AT(node->source, "予期しないノードが指定されました");
}

void tag_type_to_declaration(Declaration *declaration,
                             TypeCheckContext *context) {
  ERROR("invalid abstract syntax tree");
}

void tag_type_to_statement(StatementUnion *statementUnion,
                           TypeCheckContext *context) {
  if (!statementUnion) {
    ERROR("指定された文が存在しません");
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
      for (int i = 0; i < vector_length(compoundPattern->blockItemList); i++) {
        BlockItem *item = vector_get(compoundPattern->blockItemList, i);
        if (item->declaration)
          tag_type_to_declaration(item->declaration, context);
        else if (item->statement)
          tag_type_to_statement(item->statement, context);
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
        ERROR_AT(returnPattern->node->source,
                 "void型関数で戻り値が返されています");

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

  ERROR("予期しない文が指定されました");
}
