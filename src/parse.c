#include "parse.h"
#include "container.h"
#include "declaration.h"
#include "declarationparse.h"
#include "expressionparse.h"
#include "node.h"
#include "parseutil.h"
#include "statement.h"
#include "tokenize.h"
#include "type.h"
#include "typecheck.h"
#include "typecontainer.h"
#include "util.h"
#include "variable.h"
#include "variablecontainer.h"
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
    StorageKind storage = target->storage;
    {
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

      assert(declarator->type->kind != TYPE_FUNC || storage == STORAGE_EXTERN);
    }

    switch (storage) {
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
    case STORAGE_STATIC:
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

void analyze_global_declaration(Declaration *target, Vector *globalVariables,
                                ParseContext *context) {
  for (int i = 0; i < vector_length(target->declarators); i++) {
    Variable *declarator = vector_get(target->declarators, i);

    //例外処理
    StorageKind storage = target->storage;
    {
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

      assert(declarator->type->kind != TYPE_FUNC || storage == STORAGE_EXTERN);
    }

    switch (storage) {
    case STORAGE_NONE: {
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
    case STORAGE_STATIC:
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

// EBNFパーサ
Program *program();
Variable *function_definition(Declaration *base, ParseContext *context);

StatementUnion *statement(ParseContext *context);
NullStatement *null_statement(ParseContext *context);
ExpressionStatement *expression_statement(ParseContext *context);
ReturnStatement *return_statement(ParseContext *context);
IfStatement *if_statement(ParseContext *context);
SwitchStatement *switch_statement(ParseContext *context);
LabeledStatement *labeled_statement(ParseContext *context);
WhileStatement *while_statement(ParseContext *context);
DoWhileStatement *do_while_statement(ParseContext *context);
ForStatement *for_statement(ParseContext *context);
CompoundStatement *compound_statement(ParseContext *context);
BreakStatement *break_statement(ParseContext *context);
ContinueStatement *continue_statement(ParseContext *context);

// program = (function_definition | global_variable_definition |
// function_declaration | type_specifier ";")*
// function_declaration = type_specifier identity "("
// function_declaration_argument? ")" ";"
// function_declaration_argument = type_specifier identifier? (","
// type_specifier identifier?)*
// function_definition = type_specifier identity "("
// function_definition_argument? ")" compound_statement
// function_definition_argument = type_specifier identity ("," type_specifier
// identity)*
// global_variable_definition = variable_definition ("=" initializer)? ";"
// struct_definition = "struct" identifier "{" (type_specifier identifier
// ";")*
// "}" ";"
// type_definition = "typedef" type_specifier identifier

// statement = null_statement | expression_statement | return_statement |
// if_statement | switch_statement | labeled_statement | while_statementa |
// do_while_statement | break_statement | continue_statement
// null_statement = ";"
// expression_statement = expression ";"
// return_statement = "return" expression ";"
// if_statement = "if" "(" expression ")" statement ("else" statement)?
// switch_statement = "switch" "(" expression ")" statement
// labeled_statement = (("case"? constant-expression) | "default") ":"
// statement while_statement = "while" "(" expression ")" statement
// do_while_statement = do statement "while" "(" expression ")" ";"
// for_statement = "for" "(" expression ";" expression ";" expression ")"
// statement
// compound_statement = "{" statement* "}"
// break_statement = "break" ";"
// continue_statement = "continue" ";"

//プログラムをパースする
Program *program() {
  Program *result = calloc(1, sizeof(Program));
  result->functionDefinitions = new_vector(16);
  result->globalVariables = new_vector(16);
  result->stringLiterals = new_vector(16);
  stringLiterals = result->stringLiterals;

  ParseContext *context = new_scope_context(NULL);

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
        analyze_global_declaration(declarationResult, result->globalVariables,
                                   context);
        continue;
      }
      token = head; //ロールバック
    }

    ERROR_AT(token->string->head, "認識できない構文です");
  }

  return result;
}

ListNode *switchStatementNest; //引数に押し込みたい

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

  // switch文のネスト情報の初期化
  switchStatementNest = new_list_node(NULL);

  //
  //関数には引数があるので複文オブジェクトの生成を特別扱いしている
  //
  CompoundStatement *body = calloc(1, sizeof(CompoundStatement));

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
  //
  //
  //

  {
    //型検査
    TypeCheckContext *context = calloc(1, sizeof(TypeCheckContext));
    context->returnType = functionVariable->type->returnType;
    context->variableContainer = localContext->scope->variableContainer;
    tag_type_to_statement(new_statement_union_compound(body), context);
  }

  definition->body = body;
  definition->stackSize = localContext->function->currentStackOffset;
  return functionVariable;
}

//文をパースする
StatementUnion *statement(ParseContext *context) {
  ReturnStatement *returnPattern = return_statement(context);
  if (returnPattern) {
    return new_statement_union_return(returnPattern);
  }

  IfStatement *ifPattern = if_statement(context);
  if (ifPattern) {
    return new_statement_union_if(ifPattern);
  }

  SwitchStatement *switchPattern = switch_statement(context);
  if (switchPattern) {
    return new_statement_union_switch(switchPattern);
  }

  LabeledStatement *labeledPattern = labeled_statement(context);
  if (labeledPattern) {
    return new_statement_union_labeled(labeledPattern);
  }

  WhileStatement *whilePattern = while_statement(context);
  if (whilePattern) {
    return new_statement_union_while(whilePattern);
  }

  DoWhileStatement *doWhilePattern = do_while_statement(context);
  if (doWhilePattern) {
    return new_statement_union_do_while(doWhilePattern);
  }

  ForStatement *forPattern = for_statement(context);
  if (forPattern) {
    return new_statement_union_for(forPattern);
  }

  CompoundStatement *compoundPattern = compound_statement(context);
  if (compoundPattern) {
    return new_statement_union_compound(compoundPattern);
  }

  BreakStatement *breakPattern = break_statement(context);
  if (breakPattern) {
    return new_statement_union_break(breakPattern);
  }

  ContinueStatement *continuePattern = continue_statement(context);
  if (continuePattern) {
    return new_statement_union_continue(continuePattern);
  }

  NullStatement *nullPattern = null_statement(context);
  if (nullPattern) {
    return new_statement_union_null(nullPattern);
  }

  ExpressionStatement *expressionPattern = expression_statement(context);
  return new_statement_union_expression(expressionPattern);
}

NullStatement *null_statement(ParseContext *context) {
  if (!consume(";"))
    return NULL;

  return calloc(1, sizeof(NullStatement));
}

// 式の文をパースする
ExpressionStatement *expression_statement(ParseContext *context) {
  ExpressionStatement *result = calloc(1, sizeof(ExpressionStatement));
  result->node = expression(context);
  expect(";");
  return result;
}

// return文をパースする
ReturnStatement *return_statement(ParseContext *context) {
  if (!consume("return")) {
    return NULL;
  }

  ReturnStatement *result = calloc(1, sizeof(ReturnStatement));

  if (!consume(";")) {
    result->node = expression(context);
    expect(";");
  }
  return result;
}

// if文をパースする
IfStatement *if_statement(ParseContext *context) {
  if (!consume("if") || !consume("(")) {
    return NULL;
  }

  IfStatement *result = calloc(1, sizeof(IfStatement));
  result->condition = expression(context);

  expect(")");

  result->thenStatement = statement(context);
  if (consume("else")) {
    result->elseStatement = statement(context);
  }

  return result;
}

// switch文をパースする
SwitchStatement *switch_statement(ParseContext *context) {
  if (!consume("switch") || !consume("(")) {
    return NULL;
  }

  SwitchStatement *result = calloc(1, sizeof(SwitchStatement));
  result->condition = expression(context);
  result->labeledStatements = new_vector(16);

  expect(")");

  switchStatementNest = list_push_front(switchStatementNest, result);
  result->statement = statement(context);
  switchStatementNest = switchStatementNest->next;

  return result;
}

// label付の文をパースする
LabeledStatement *labeled_statement(ParseContext *context) {
  Token *tokenHead = token;

  LabeledStatement *result = calloc(1, sizeof(LabeledStatement));
  bool isCaseLabel = consume("case");
  if (isCaseLabel) {
    result->constantExpression = expression(context);
  }
  if (isCaseLabel || consume("default")) {
    expect(":");

    //最も新しいswitch文に自身を登録
    SwitchStatement *switchStatement = switchStatementNest->body;
    if (switchStatement)
      vector_push_back(switchStatement->labeledStatements, result);
    else
      ERROR_AT(tokenHead->string->head,
               "switch文の外でcaseまたはdefaultラベルが定義されました");

    result->statement = statement(context);
    return result;
  }

  Token *identifier = consume_identifier();
  if (identifier && consume(":")) {
    result->name = identifier->string;
    result->statement = statement(context);
    return result;
  }

  token = tokenHead;
  return NULL;
}

// while文をパースする
WhileStatement *while_statement(ParseContext *context) {
  if (!consume("while") || !consume("(")) {
    return NULL;
  }

  WhileStatement *result = calloc(1, sizeof(WhileStatement));
  result->condition = expression(context);

  expect(")");

  result->statement = statement(context);

  return result;
}

// do-while文をパースする
DoWhileStatement *do_while_statement(ParseContext *context) {
  Token *tokenHead = token;

  if (!consume("do"))
    return NULL;

  DoWhileStatement *result = calloc(1, sizeof(DoWhileStatement));
  result->statement = statement(context);

  if (!consume("while") || !consume("(")) {
    token = tokenHead;
    return NULL;
  }

  result->condition = expression(context);

  expect(")");
  expect(";");
  return result;
}

// for文をパースする
ForStatement *for_statement(ParseContext *context) {
  if (!consume("for") || !consume("(")) {
    return NULL;
  }

  //スコープの生成
  context = new_scope_context(context);

  ForStatement *result = calloc(1, sizeof(ForStatement));

  // initialization
  if (!consume(";")) {
    Declaration *initialDeclaration =
        declaration(declaration_specifier(context), context);
    if (initialDeclaration) {
      //結果がNULLならNULLが代入されてよい
      result->initialization =
          analyze_local_declaration(initialDeclaration, context);
    } else {
      result->initialization = expression(context);
      expect(";");
    }
  }

  // condition
  if (!consume(";")) {
    result->condition = expression(context);
    expect(";");
  }

  // afterthought
  if (!consume(")")) {
    result->afterthought = expression(context);
    expect(")");
  }

  result->statement = statement(context);

  return result;
}

// 複文をパースする
CompoundStatement *compound_statement(ParseContext *context) {
  if (!consume("{")) {
    return NULL;
  }

  CompoundStatement *result = calloc(1, sizeof(CompoundStatement));

  //新しいスコープなので先頭に新しい変数テーブルを追加
  context = new_scope_context(context);

  //ブロック内の文をパ-ス
  result->blockItemList = new_vector(32);
  while (!consume("}")) {
    BlockItem *blockItem = calloc(1, sizeof(BlockItem));
    blockItem->declaration =
        declaration(declaration_specifier(context), context);
    if (blockItem->declaration) {
      blockItem->statement =
          analyze_blockItem_declaration(blockItem->declaration, context);
      blockItem->declaration = NULL;
      if (!blockItem->statement)
        continue;
    } else {
      blockItem->statement = statement(context);
    }
    vector_push_back(result->blockItemList, blockItem);
  }

  return result;
}

// break文をパースする
BreakStatement *break_statement(ParseContext *context) {
  if (!consume("break")) {
    return NULL;
  }
  expect(";");
  BreakStatement *result = calloc(1, sizeof(BreakStatement));
  return result;
}

// continue文をパースする
ContinueStatement *continue_statement(ParseContext *context) {
  if (!consume("continue")) {
    return NULL;
  }
  expect(";");
  ContinueStatement *result = calloc(1, sizeof(ContinueStatement));
  return result;
}

Program *parse(Token *head) {
  token = head;
  return program();
}
