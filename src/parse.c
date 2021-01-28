/*
 * 翻訳単位をパースする。
 * parseのみを公開する。
 */

#include "parse.h"
#include "analyze.h"
#include "container.h"
#include "declaration.h"
#include "declarationparse.h"
#include "expressionparse.h"
#include "node.h"
#include "parseutil.h"
#include "statement.h"
#include "statementparse.h"
#include "type.h"
#include "typecheck.h"
#include "util.h"
#include "variable.h"
#include "variablecontainer.h"
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

// EBNF
// program = (function_definition | declaration)*
// function_definition = declaration_specifier declarator compound_statement

Program *program();
Variable *function_definition(Declaration *base, ParseContext *context);

Program *parse(Token *head) {
  token = head;
  return program();
}

//プログラムをパースする
Program *program() {
  Program *result = calloc(1, sizeof(Program));
  result->functionDefinitions = new_vector(16);
  result->staticMemoryVariables = new_vector(16);
  result->stringLiterals = new_vector(16);

  ParseContext *context = new_scope_context(NULL);
  context->translationUnit = calloc(1, sizeof(TranslationUnitContext));
  context->translationUnit->staticMemoryVariables =
      result->staticMemoryVariables;
  context->translationUnit->stringLiterals = result->stringLiterals;

  while (!at_eof()) {
    Declaration *base = declaration_specifier(context);
    if (!base)
      ERROR_AT(token->string->head, "不正な構文です");

    //関数定義
    {
      Token *head = token;

      Variable *functionDefinition = function_definition(base, context);
      if (functionDefinition) {
        vector_push_back(result->functionDefinitions, functionDefinition);
        continue;
      }

      token = head; //ロールバック
    }

    //宣言文
    {
      Token *head = token;
      Declaration *declarationResult = declaration(base, context);
      if (declarationResult) {
        analyze_global_declaration(declarationResult, context);
        continue;
      }
      token = head; //ロールバック
    }

    ERROR_AT(token->string->head, "認識できない構文です");
  }

  return result;
}

//関数の定義をパースする
Variable *function_definition(Declaration *base, ParseContext *context) {
  if (base->storage != STORAGE_NONE && base->storage != STORAGE_STATIC)
    return NULL;

  Variable *functionVariable = declarator(base->type, context);
  if (!functionVariable || functionVariable->type->kind != TYPE_FUNC)
    return NULL;

  //引数のパース
  //新しいスコープなので先頭に新しい変数テーブルを追加
  ParseContext *localContext = new_scope_context(context);
  localContext->function = calloc(1, sizeof(FunctionContext));
  // switch文のネスト情報の初期化
  localContext->function->switchStatementNest = new_list_node(NULL);

  //関数定義では引数の指定がないとき引数なしの関数に読み替える
  if (!functionVariable->type->arguments)
    functionVariable->type->arguments = new_vector(0);

  // Node Vector
  Vector *argumentNodes = new_vector(16);
  {
    Vector *argumentDeclarations = functionVariable->type->arguments;
    const int parameterCount = vector_length(argumentDeclarations);
    for (int i = 0; i < parameterCount; i++) {
      Declaration *declaration = vector_get(argumentDeclarations, i);
      Variable *variable =
          vector_get(declaration->declarators, 0); //要素数は必ず1

      Type *type = variable->type;

      // func(void)への対応とvoid型の値の排除
      if (type->kind == TYPE_VOID) {
        if (i == 0 && parameterCount == 1 && !variable->name) {
          functionVariable->type->arguments = new_vector(0);
          argumentNodes = new_vector(0);
          break;
        } else {
          ERROR_AT(token->string->head, "関数定義の引数の宣言が不正です");
        }
      }

      // abstract_declaratorであるとき
      // この確認はfunc(void)の確認後に行う必要がある
      if (!variable->name) {
        return NULL;
      }

      //関数定義の引数のうち、配列はポインタに読み替え
      if (variable->type->kind == TYPE_ARRAY) {
        variable->type->kind = TYPE_PTR;
        variable->type->length = 0;
      }

      Node *node = new_node_variable_definition(variable, localContext, token);
      node->type =
          node->variable->type; // tag_type_to_node(type)できないので手動型付け

      vector_push_back(argumentNodes, node);
    }
  }

  if (!consume("{"))
    return NULL;

  //-----関数宣言との対応づけ-------
  //再帰関数に対応するためにここでやる
  FunctionDefinition *definition = NULL;
  {
    Variable *variable = variable_container_get(
        localContext->scope->variableContainer, functionVariable->name);
    if (variable && variable->type->kind != TYPE_FUNC)
      ERROR_AT(functionVariable->name->head,
               "同名のシンボルが既に定義されています");

    if (variable) {
      if (!type_compare_deep(functionVariable->type->returnType,
                             variable->type->returnType))
        ERROR_AT(functionVariable->name->head,
                 "関数の定義と前方宣言の戻り値が一致しません");

      //宣言に引数がなければ引数チェックをスキップ
      if (variable->type->arguments) {
        if (!type_vector_compare(parameter_declaration_vector_to_type_vector(
                                     variable->type->arguments),
                                 node_vector_to_type_vector(argumentNodes)))
          ERROR_AT(functionVariable->name->head,
                   "関数の定義と前方宣言の引数が一致しません");
      }
      definition = functionVariable->function =
          new_function_declaration(functionVariable->type->returnType,
                                   functionVariable->name, argumentNodes);
      definition->arguments = argumentNodes; //一致が確認できたので上書き
    } else {
      definition =
          new_function_declaration(functionVariable->type->returnType,
                                   functionVariable->name, argumentNodes);
      functionVariable = variable_to_function(functionVariable, definition);
      variable_container_push(context->scope->variableContainer,
                              functionVariable);
    }

    functionVariable->storage = base->storage;
    definition->function = base->function;
  }
  //--------------------------------

  //事前定義識別子
  {
    //__func__
    Type *charPtr = new_type(TYPE_PTR);
    charPtr->base = new_type(TYPE_CHAR);
    Variable *func = new_variable(charPtr, char_to_string("__func__"));
    func->kind = VARIABLE_LOCAL;
    func->storage = STORAGE_STATIC;
    func->initialization =
        new_node_num(vector_length(context->translationUnit->stringLiterals));
    func->initialization->kind = NODE_STRING;
    vector_push_back(context->translationUnit->stringLiterals,
                     string_to_char(functionVariable->name));
    vector_push_back(context->translationUnit->staticMemoryVariables, func);
    variable_container_push(localContext->scope->variableContainer, func);
  }

  //関数には引数があるので複文オブジェクトの生成を特別扱いしている
  CompoundStatement *body = calloc(1, sizeof(CompoundStatement));
  {
    //新しいスコープの追加を外へ移動

    //ブロック内の文をパース
    body->blockItemList = new_vector(32);
    while (!consume("}")) {
      BlockItem *blockItem = calloc(1, sizeof(BlockItem));
      blockItem->declaration =
          declaration(declaration_specifier(localContext), localContext);
      if (blockItem->declaration) {
        blockItem->statement =
            analyze_blockItem_declaration(blockItem->declaration, localContext);
        blockItem->declaration = NULL;
        if (!blockItem->statement)
          continue;
      } else {
        blockItem->statement = statement(localContext);
      }
      vector_push_back(body->blockItemList, blockItem);
    }
  }

  //型検査
  {
    TypeCheckContext *context = calloc(1, sizeof(TypeCheckContext));
    context->returnType = functionVariable->type->returnType;
    context->variableContainer = localContext->scope->variableContainer;
    tag_type_to_statement(new_statement_union_compound(body), context);
  }

  definition->body = body;
  definition->stackSize = localContext->function->currentStackOffset;
  return functionVariable;
}
