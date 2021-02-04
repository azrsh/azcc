#include "container.h"
#include "declaration.h"
#include "node.h"
#include "parseutil.h"
#include "statement.h"
#include "type.h"
#include "util.h"
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

// 関数宣言の読み替え
void analyze_function_type(Type *type) {
  assert(type->kind == TYPE_FUNC);
  if (!type->arguments)
    return;

  // func(void)の読み替え
  if (vector_length(type->arguments)) {
    Vector *argumentTypes =
        parameter_declaration_vector_to_type_vector(type->arguments);
    Type *argumentType = vector_get(argumentTypes, 0);
    if (argumentType->kind == TYPE_VOID)
      type->arguments = new_vector(0);
  }

  //関数定義の引数のうち、配列はポインタに読み替え
  {
    Vector *argumentDeclarations = type->arguments;
    Vector *argumentTypes =
        parameter_declaration_vector_to_type_vector(argumentDeclarations);
    for (int i = 0; i < vector_length(argumentTypes); i++) {
      Type *type = vector_get(argumentTypes, i);
      if (type->kind == TYPE_ARRAY) {
        type->kind = TYPE_PTR;
        type->length = 0;
      }
    }
  }
}

Node *analyze_local_declaration(Declaration *target, ParseContext *context) {
  Node *node = NULL;

  for (int i = 0; i < vector_length(target->declarators); i++) {
    Variable *declarator = vector_get(target->declarators, i);

    //例外処理
    {
      StorageKind storage = target->storage;

      //未指定の場合、デフォルトの値を設定
      if (storage == STORAGE_NONE) {
        if (declarator->type->kind == TYPE_FUNC)
          storage = STORAGE_EXTERN;
        else
          storage = STORAGE_AUTO;
      }

      // 関数宣言の読み替え
      {
        Type *type = declarator->type;
        while (type) {
          if (type->kind == TYPE_FUNC)
            analyze_function_type(type);

          type = type->base;
        }
      }

      assert(declarator->type->kind != TYPE_FUNC || storage == STORAGE_EXTERN ||
             storage == STORAGE_TYPEDEF);

      declarator->storage = storage;
    }

    switch (declarator->storage) {
    case STORAGE_NONE:
      assert(0);
      break;
    case STORAGE_TYPEDEF: {
      Type *type = type_container_get(context->scope->typedefContainer,
                                      declarator->name);
      if (!type)
        type_container_push(context->scope->typedefContainer, declarator->name,
                            target->type);
      else if (!type_compare_deep(type, target->type))
        ERROR_AT(token->string->head, "typedefが衝突しています");
      break;
    }
    case STORAGE_EXTERN: {
      ERROR_AT(token->string->head,
               "ローカルでのextern指定子はサポートされていません");
      break;
    }
    case STORAGE_STATIC: {
      //静的変数の宣言
      Variable *staticVariable = variable_to_local_static(declarator);
      if (!variable_container_push(context->scope->variableContainer,
                                   staticVariable))
        ERROR_AT(token->string->head, "変数宣言が衝突しています");
      vector_push_back(context->translationUnit->staticMemoryVariables,
                       staticVariable);
      break;
    }
    case STORAGE_THREAD_LOCAL:
      ERROR_AT(token->string->head, "サポートされていない記憶クラス指定子です");
      break;
    case STORAGE_AUTO: {
      Node *declaratorNode =
          new_node_variable_definition(declarator, context, token);
      if (declarator->initialization) {
        Node *initDeclaratorNode =
            new_node(NODE_ASSIGN, declaratorNode, declarator->initialization);

        //宣言ならカンマ演算子に読み替え
        if (node)
          node = new_node(NODE_COMMA, node, initDeclaratorNode);
        else
          node = initDeclaratorNode;
      }
      break;
    }
    case STORAGE_REGISTER:
      ERROR_AT(token->string->head, "サポートされていない記憶クラス指定子です");
      break;
    }
  }
  return node;
}

void analyze_global_declaration(Declaration *target, ParseContext *context) {
  for (int i = 0; i < vector_length(target->declarators); i++) {
    Variable *declarator = vector_get(target->declarators, i);

    //例外処理
    {
      StorageKind storage = target->storage;

      //未指定の場合、デフォルトの値を設定
      if (storage == STORAGE_NONE) {
        if (declarator->type->kind == TYPE_FUNC)
          storage = STORAGE_EXTERN;
      }

      // 関数宣言の読み替え
      {
        Type *type = declarator->type;
        while (type) {
          if (type->kind == TYPE_FUNC)
            analyze_function_type(type);

          type = type->base;
        }
      }

      assert(storage != STORAGE_AUTO);

      declarator->storage = storage;
    }

    //宣言の読み替えのうち、構文木には伝播しないもの
    StorageKind storage = declarator->storage;
    {
      if (storage == STORAGE_STATIC) {
        if (declarator->type->kind == TYPE_FUNC)
          storage = STORAGE_EXTERN;
      }
    }

    switch (storage) {
    case STORAGE_STATIC:
    case STORAGE_NONE: {
      Vector *globalVariables = context->translationUnit->staticMemoryVariables;
      for (int i = 0; i < vector_length(globalVariables); i++) {
        Variable *variable = vector_get(globalVariables, i);
        if (string_compare(variable->name, declarator->name)) {
          ERROR_AT(token->string->head, "同名の変数が既に定義されています");
          break;
        }
      }

      //グローバル変数の宣言
      Variable *globalVariable = variable_to_global(declarator);
      Variable *declarated = variable_container_get(
          context->scope->variableContainer, globalVariable->name);
      if (declarated &&
          !type_compare_deep(declarated->type, declarator->type)) {
        ERROR_AT(token->string->head, "変数宣言が衝突しています");
      } else {
        variable_container_push(context->scope->variableContainer,
                                globalVariable);
      }

      vector_push_back(globalVariables, globalVariable);
      break;
    }
    case STORAGE_TYPEDEF: {
      Type *type = type_container_get(context->scope->typedefContainer,
                                      declarator->name);
      if (!type)
        type_container_push(context->scope->typedefContainer, declarator->name,
                            target->type);
      else if (!type_compare_deep(type, target->type))
        ERROR_AT(token->string->head, "typedefが衝突しています");
      break;
    }
    case STORAGE_EXTERN: {
      if (declarator->initialization) {
        ERROR_AT(token->string->head, "変数宣言は初期化できません");
      }

      //グローバル変数の宣言
      Variable *globalVariable = variable_to_global(declarator);
      Variable *declarated = variable_container_get(
          context->scope->variableContainer, globalVariable->name);
      if (declarated &&
          !type_compare_deep(declarated->type, declarator->type)) {
        ERROR_AT(token->string->head, "変数宣言が衝突しています");
      } else {
        variable_container_push(context->scope->variableContainer,
                                globalVariable);
      }
      break;
    }
    case STORAGE_THREAD_LOCAL:
      ERROR_AT(token->string->head, "サポートされていない記憶クラス指定子です");
      break;
    case STORAGE_AUTO:
      ERROR_AT(token->string->head,
               "auto記憶クラス指定子は大域変数に適用できません");
      break;
    case STORAGE_REGISTER:
      ERROR_AT(token->string->head, "サポートされていない記憶クラス指定子です");
      break;
    }
  }
}

StatementUnion *analyze_blockItem_declaration(Declaration *declaration,
                                              ParseContext *context) {
  Node *node = analyze_local_declaration(declaration, context);
  if (node) {
    ExpressionStatement *expressionStatement =
        calloc(1, sizeof(ExpressionStatement));
    expressionStatement->node = node;

    //宣言文から名前解決をした後文に置き換え
    return new_statement_union_expression(expressionStatement);
  }

  return NULL;
}
