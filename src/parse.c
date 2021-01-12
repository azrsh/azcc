#include "parse.h"
#include "container.h"
#include "declaration.h"
#include "membercontainer.h"
#include "node.h"
#include "statement.h"
#include "tagcontainer.h"
#include "tokenize.h"
#include "type.h"
#include "typecheck.h"
#include "util.h"
#include "variable.h"
#include "variablecontainer.h"
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Token *token;
Vector *globalVariables; // Variable vector
Vector *stringLiterals;  // String vector
Vector *typedefs;        // Typedef vector

typedef struct Scope Scope;
struct Scope {
  TagContainer *tagContainer;
  VariableContainer *variableContainer;
};

typedef struct Function Function;
struct Function {
  int currentStackOffset;
};

typedef struct ParseContext ParseContext;
struct ParseContext {
  Scope *scope;
  Function *function;
};

ParseContext *new_scope_context(ParseContext *parent) {
  ParseContext *result = calloc(1, sizeof(ParseContext));

  {
    result->scope = calloc(1, sizeof(Scope));
    {
      HashTable *localVariableTable = new_hash_table();
      if (parent)
        result->scope->variableContainer = variable_container_push_table(
            parent->scope->variableContainer, localVariableTable);
      else
        result->scope->variableContainer =
            new_variable_container(new_list_node(localVariableTable));
    }
    {
      HashTable *localTagTable = new_hash_table();
      if (parent)
        result->scope->tagContainer = tag_container_push_table(
            parent->scope->tagContainer, localTagTable);
      else
        result->scope->tagContainer =
            new_tag_container(new_list_node(localTagTable));
    }
  }

  {
    //スコープが変わっても関数のコンテキストは不変
    if (parent)
      result->function = parent->function;
  }

  return result;
}

bool at_eof() { return token->kind == TOKEN_EOF; }

//次のトークンが期待している記号のときには、トークンを1つ読み進めて真を返す
//それ以外の場合には偽を返す
bool consume(const char *op) {
  const String *operator= new_string(op, strlen(op));
  if (token->kind != TOKEN_RESERVED || !string_compare(token->string, operator))
    return false;
  token = token->next;
  return true;
}

//次のトークンが文字列のときには、トークンを1つ読み進めてそのトークンを返す
//それ以外の場合にはNULLを返す
Token *consume_string() {
  if (token->kind != TOKEN_STRING)
    return NULL;
  Token *current = token;
  token = token->next;
  return current;
}

//次のトークンが文字のときには、トークンを1つ読み進めてそのトークンを返す
//それ以外の場合にはNULLを返す
Token *consume_character() {
  if (token->kind != TOKEN_CHAR)
    return NULL;
  Token *current = token;
  token = token->next;
  return current;
}

//次のトークンが識別子のときには、トークンを1つ読み進めてそのトークンを返す
//それ以外の場合にはNULLを返す
Token *consume_identifier() {
  if (token->kind != TOKEN_IDENTIFIER)
    return NULL;
  Token *current = token;
  token = token->next;
  return current;
}

//次のトークンが期待している記号のときには、トークンを1つ読み進める
//それ以外の場合にはエラーを報告する
void expect(char *op) {
  if (!consume(op))
    ERROR_AT(token->string->head, "'%s'ではありません", op);
}

//次のトークンが数値のときには、トークンを1つ読み進めてその数値を返す
//それ以外の場合にはエラーを報告する
int expect_number() {
  if (token->kind != TOKEN_NUMBER)
    ERROR_AT(token->string->head, "数ではありません");
  const int value = token->value;
  token = token->next;
  return value;
}

//次のトークンが識別子のときには、トークンを1つ読み進めてそのトークンを返す
//それ以外の場合にはエラーを報告する
Token *expect_identifier() {
  if (token->kind != TOKEN_IDENTIFIER)
    ERROR_AT(token->string->head, "識別子ではありません");
  Token *current = token;
  token = token->next;
  return current;
}

Variable *new_variable(Type *type, const String *name) {
  Variable *variable = calloc(1, sizeof(Variable));
  variable->type = type;
  variable->name = new_string(name->head, name->length);
  return variable;
}

Variable *variable_to_local(Variable *variable, ParseContext *context) {
  variable->kind = VARIABLE_LOCAL;
  context->function->currentStackOffset += type_to_stack_size(variable->type);
  variable->offset = context->function->currentStackOffset;
  return variable;
}

Variable *variable_to_global(Variable *variable, Node *initialization) {
  variable->kind = VARIABLE_GLOBAL;
  variable->initialization = initialization;
  return variable;
}

Variable *variable_to_function(Variable *variable,
                               FunctionDefinition *function) {
  variable->kind = VARIABLE_GLOBAL;
  variable->function = function;
  return variable;
}

Variable *variable_to_enumerator(Variable *variable, Node *initialization) {
  variable->kind = VARIABLE_ENUMERATOR;
  variable->initialization = initialization;
  return variable;
}

Variable *variable_to_member(Variable *variable) {
  variable->kind = VARIABLE_MEMBER;
  return variable;
}

FunctionCall *new_function_call() {
  FunctionCall *functionCall = calloc(1, sizeof(FunctionCall));
  return functionCall;
}

TypeKind map_token_to_kind(Token *token) {
  if (token->kind != TOKEN_RESERVED)
    ERROR_AT(token->string->head, "組み込み型ではありません");

  if (string_compare(token->string, char_to_string("int")))
    return TYPE_INT;

  if (string_compare(token->string, char_to_string("char")))
    return TYPE_CHAR;

  if (string_compare(token->string, char_to_string("void")))
    return TYPE_VOID;

  if (string_compare(token->string, char_to_string("_Bool")))
    return TYPE_BOOL;

  ERROR_AT(token->string->head, "組み込み型ではありません");
  return 0;
}

Type *wrap_by_pointer(Type *base, Token *token) {
  Type *current = base;
  const String *star = char_to_string("*");
  while (token && string_compare(token->string, star)) {
    Type *pointer = new_type(TYPE_PTR);
    pointer->base = current;
    current = pointer;

    token = token->next;
  }

  return current;
}

Type *new_type_from_token(Token *typeToken) {
  Type *base = new_type(map_token_to_kind(typeToken));
  base->base = NULL;
  typeToken = typeToken->next;
  return wrap_by_pointer(base, typeToken);
}

Type *new_type_array(Type *base, size_t size) {
  Type *array = new_type(TYPE_ARRAY);
  array->base = base;
  array->length = size;
  return array;
}

Type *new_type_function(Type *returnType, Vector *arguments) {
  Type *function = new_type(TYPE_FUNC);
  function->returnType = returnType;
  function->arguments = arguments;
  return function;
}

Tag *type_to_tag(Type *type) {
  Tag *result = calloc(1, sizeof(Tag));
  result->type = type;
  return result;
}

//抽象構文木の末端をパースする
//抽象構文木の数値以外のノードを新しく生成する
Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  node->source = token->string->head;
  return node;
}

//抽象構文木の数値のノードを新しく生成する
Node *new_node_num(int val) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = NODE_NUM;
  node->val = val;
  node->source = token->string->head;
  return node;
}

//抽象構文木のローカル変数定義のノードを新しく生成する
Node *new_node_variable_definition(Variable *variable, ParseContext *context,
                                   Token *source) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = NODE_VAR;

  Variable *localVariable = variable_to_local(variable, context);
  if (!variable_container_push(context->scope->variableContainer,
                               localVariable))
    ERROR_AT(variable->name->head, "同名の変数が既に定義されています");

  node->variable = localVariable;
  node->source = source->string->head;
  return node;
}

//抽象構文木の変数のノードを新しく生成する
Node *new_node_variable(Token *token, VariableContainer *container) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = NODE_VAR;

  const String *variableName = token->string;
  Variable *variable = variable_container_get(container, variableName);
  if (!variable) {
    ERROR_AT(variableName->head, "変数%sは定義されていません",
             string_to_char(variableName));
  }

  node->variable = variable;
  node->source = token->string->head;
  return node;
}

//抽象構文木のメンバ変数のノードを新しく生成する
Node *new_node_member(Token *token) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = NODE_VAR;

  Variable *variable = variable_to_member(new_variable(NULL, token->string));
  node->variable = variable;
  node->source = token->string->head;
  return node;
}

//抽象構文木の関数のノードを新しく生成する
Node *new_node_function_call(Node *lhs) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = NODE_FUNC;
  node->lhs = lhs;
  node->functionCall = new_function_call();
  node->source = lhs->source;
  return node;
}

FunctionDefinition *new_function_declaration(Type *type, const String *name,
                                             Vector *arguments) {
  FunctionDefinition *result = calloc(1, sizeof(FunctionDefinition));
  result->returnType = type;
  result->name = new_string(name->head, name->length);
  result->arguments = arguments;
  return result;
}

Vector *node_vector_to_type_vector(Vector *nodes) {
  Vector *types = new_vector(vector_length(nodes));
  for (int i = 0; i < vector_length(nodes); i++) {
    Node *node = vector_get(nodes, i);
    Variable *variable = node->variable;
    vector_push_back(types, variable->type);
  }
  return types;
}

// EBNFパーサ
Program *program();
FunctionDefinition *function_definition(Type *returnType,
                                        ParseContext *context);
Vector *function_definition_argument(ParseContext *context);
Variable *global_variable_declaration(bool isExtern, Type *type,
                                      ParseContext *context);
Node *initializer();

Typedef *type_definition(ParseContext *context);
Type *type_specifier(ParseContext *context);
Type *enum_specifier(ParseContext *context);
Type *struct_specifier(ParseContext *context);
Variable *variable_declaration(Type *type, ParseContext *context);

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

Declaration *declaration(ParseContext *context);
Vector *init_declarator_list(Type *type, ParseContext *context);
Node *init_declarator(Type *type, ParseContext *context);

Node *expression(ParseContext *context);
Vector *argument_expression_list(ParseContext *context);

Node *assign(ParseContext *context);
Node *conditional(ParseContext *context);
Node *logical_or(ParseContext *context);
Node *logical_and(ParseContext *context);
Node *bitwise_inclusive_or(ParseContext *context);
Node *bitwise_exclusive_or(ParseContext *context);
Node *bitwise_and(ParseContext *context);
Node *equality(ParseContext *context);
Node *relational(ParseContext *context);
Node *add(ParseContext *context);
Node *shift(ParseContext *context);
Node *multiply(ParseContext *context);
Node *unary(ParseContext *context);
Node *postfix(ParseContext *context);
Node *primary(ParseContext *context);
Node *literal();

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
// struct_definition = "struct" identifier "{" (type_specifier identifier ";")*
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
// labeled_statement = (("case"? constant-expression) | "default") ":" statement
// while_statement = "while" "(" expression ")" statement
// do_while_statement = do statement "while" "(" expression ")" ";"
// for_statement = "for" "(" expression ";" expression ";" expression ")"
// statement
// compound_statement = "{" statement* "}"
// break_statement = "break" ";"
// continue_statement = "continue" ";"

// expression = (assign ("," assign)*) | variable_definition
// argument_expression_list = expression ("," expression)*

// variable_definition = type_specifier identity
// type_specifier = ("int" | "char" | "void" | "_Bool") "*"*
// assign = (unary (("=" | "+=" | "-=" | "*=" | "/=") assign)? | conditional)
// conditional = logic_or ("?" expression ":" conditional)?
// logical_or = logic_and ("||" logic_and)*
// logical_and = bitwise_inclusive_or ("&&" bitwise_inclusive_or)*
// bitwise_inclusive_or = bitwise_exclusive_or ("&" bitwise_exclusive_or)*
// bitwise_exclusive_xor = bitwise_and ("^" bitwise_and)*
// bitwise_and = equality ("&" equality)*
// equality = relational ("==" relational | "!=" relational)*
// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
// add = mul ("+" mul | "-" mul)*
// mul = unary ("*" unary | "/" unary)*
// unary = ("+" | "-" | "&" | "*" | "!" | "sizeof" | "_Alignof")? (primary | "("
// type_specifier ")" )
// postfix = primary ("(" argument_expression_list? ")" | "[" expression "]" |
// "." identifier)* primary = literal | identity ("(" argument_expression_list?
// ")" | "[" expression "]")? | "("expression")"
// literal = number | "\"" string"\""

//プログラムをパースする
Program *program() {
  Program *result = calloc(1, sizeof(Program));
  result->functionDefinitions = new_vector(16);
  result->globalVariables = new_vector(16);
  result->stringLiterals = new_vector(16);
  globalVariables = result->globalVariables;
  stringLiterals = result->stringLiterals;
  typedefs = new_vector(16);

  ParseContext *context = new_scope_context(NULL);

  while (!at_eof()) {
    //先頭に型指定子がつくもの
    bool isExtern = consume("extern");
    Type *type = type_specifier(context);
    if (type) {
      {
        FunctionDefinition *functionDefinition =
            function_definition(type, context);
        if (functionDefinition) {
          if (functionDefinition->body)
            vector_push_back(result->functionDefinitions, functionDefinition);
          continue;
        }
      }

      {
        if (global_variable_declaration(isExtern, type, context))
          continue;
      }

      {
        if (type && consume(";"))
          continue;
      }

      ERROR_AT(token->string->head, "型指定子のあとに続くトークンが不正です");
    }

    {
      Typedef *typeDefinition = type_definition(context);
      if (typeDefinition) {
        vector_push_back(typedefs, typeDefinition);
        continue;
      }
    }

    ERROR_AT(token->string->head, "認識できない構文です");
  }

  return result;
}

ListNode *switchStatementNest; //引数に押し込みたい

//関数の定義をパースする
FunctionDefinition *function_definition(Type *type, ParseContext *context) {
  Token *tokenHead = token;

  Token *identifier = consume_identifier();
  if (!identifier) {
    token = tokenHead;
    return NULL;
  }

  //引数のパース
  //新しいスコープなので先頭に新しい変数テーブルを追加
  ParseContext *localContext = new_scope_context(context);
  localContext->function = calloc(1, sizeof(Function));
  Vector *arguments = NULL;
  if (!consume("(")) {
    token = tokenHead;
    return NULL;
  }
  if (consume(")")) {
    arguments = NULL;
  } else {
    arguments = function_definition_argument(localContext);
    expect(")");
  }

  if (consume(";")) {
    FunctionDefinition *definition =
        new_function_declaration(type, identifier->string, arguments);
    Type *functionType = new_type_function(
        definition->returnType,
        definition->arguments
            ? node_vector_to_type_vector(definition->arguments)
            : NULL);
    Variable *variable = variable_to_function(
        new_variable(functionType, definition->name), definition);
    variable_container_push(context->scope->variableContainer, variable);
    return definition;
  }

  //ここまで到達したなら関数定義以外はありえない
  expect("{");

  if (!arguments)
    arguments = new_vector(0);

  //-----関数宣言との対応づけ-------
  //再帰関数に対応するためにここでやる
  FunctionDefinition *definition = NULL;
  {
    Variable *variable = variable_container_get(
        localContext->scope->variableContainer, identifier->string);
    if (variable && variable->type->kind != TYPE_FUNC)
      ERROR_AT(identifier->string->head,
               "同名のシンボルが既に定義されています");

    if (variable) {
      //宣言に引数がなければ引数チェックをスキップ
      if (variable->type->arguments) {
        if (!type_vector_compare(variable->type->arguments,
                                 node_vector_to_type_vector(arguments)))
          ERROR_AT(identifier->string->head,
                   "関数の定義と前方宣言の引数が一致しません");
      }
      definition = variable->function;
      definition->arguments = arguments; //一致が確認できたので上書き
    } else {
      definition =
          new_function_declaration(type, identifier->string, arguments);
      Type *functionType = new_type_function(
          definition->returnType,
          definition->arguments
              ? node_vector_to_type_vector(definition->arguments)
              : NULL);
      Variable *variable = variable_to_function(
          new_variable(functionType, definition->name), definition);
      variable_container_push(context->scope->variableContainer, variable);
    }
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
    blockItem->declaration = declaration(localContext);
    if (!blockItem->declaration)
      blockItem->statement = statement(localContext);
    vector_push_back(body->blockItemList, blockItem);
  }
  //
  //
  //

  {
    //型検査
    TypeCheckContext *context = calloc(1, sizeof(TypeCheckContext));
    context->returnType = type;
    context->variableContainer = localContext->scope->variableContainer;
    tag_type_to_statement(new_statement_union_compound(body), context);
  }

  definition->body = body;
  definition->stackSize = localContext->function->currentStackOffset;
  return definition;
}

// Local Variable Nodes
Vector *function_definition_argument(ParseContext *context) {
  Vector *arguments = new_vector(32);

  do {
    Token *source = token;
    Type *type = type_specifier(context);
    if (!type)
      ERROR_AT(source->string->head, "関数定義の引数の宣言が不正です");

    // func(void)への対応とvoid型の値の排除
    if (type->kind == TYPE_VOID) {
      if (vector_length(arguments) == 0 && !consume_identifier() &&
          !consume(","))
        return new_vector(0);
      else
        ERROR_AT(source->string->head, "関数定義の引数の宣言が不正です");
    }

    Variable *declaration = variable_declaration(type, context);
    if (!declaration) {
      char *disable = calloc(2, sizeof(char));
      disable[0] = '0' + vector_length(arguments);
      declaration = new_variable(type, char_to_string(disable));
    }

    //関数定義の引数のうち、配列はポインタに読み替え
    if (declaration->type->kind == TYPE_ARRAY) {
      declaration->type->kind = TYPE_PTR;
      declaration->type->length = 0;
    }

    Node *node = new_node_variable_definition(declaration, context, source);
    node->type =
        node->variable->type; // tag_type_to_node(type)できないので手動型付け

    vector_push_back(arguments, node);
  } while (consume(","));
  return arguments;
}

Variable *global_variable_declaration(bool isExtern, Type *type,
                                      ParseContext *context) {
  Token *tokenHead = token;

  Variable *declaration = variable_declaration(type, context);
  if (!declaration)
    return NULL;

  Node *initialization = NULL;
  if (!isExtern && consume("=")) {
    initialization = initializer();
  }

  if (!consume(";")) {
    token = tokenHead;
    return NULL;
  }

  bool isDefined = false;
  for (int i = 0; i < vector_length(globalVariables); i++) {
    Variable *variable = vector_get(globalVariables, i);
    if (string_compare(variable->name, declaration->name)) {
      isDefined = true;
      break;
    }
  }

  if (!isExtern && isDefined)
    ERROR_AT(token->string->head, "同名の変数が既に定義されています");

  Variable *globalVariable = variable_to_global(declaration, initialization);
  bool isDeclared = !variable_container_push(context->scope->variableContainer,
                                             globalVariable);
  if (isExtern && isDeclared)
    ERROR_AT(token->string->head, "同名の変数が既に宣言されています");

  if (!isExtern)
    vector_push_back(globalVariables, globalVariable);

  return globalVariable;
}

Typedef *type_definition(ParseContext *context) {
  if (!consume("typedef")) {
    return NULL;
  }

  Type *type = type_specifier(context);
  if (!type) {
    ERROR_AT(token->string->head, "型指定子ではありません");
  }

  Token *identifier = expect_identifier();
  const String *name = identifier->string;

  expect(";");

  Typedef *result = calloc(1, sizeof(Typedef));
  result->name = name;
  result->type = type;
  return result;
}

Variable *variable_declaration(Type *type, ParseContext *context) {
  Token *tokenHead = token;

  Token *identifier = consume_identifier();
  if (!identifier) {
    token = tokenHead;
    return NULL;
  }

  while (consume("[")) {
    int size = expect_number();
    type = new_type_array(type, size);
    expect("]");
  }

  return new_variable(type, identifier->string);
}

Node *initializer() {
  if (consume("{")) {
    Vector *elements = new_vector(16);
    do {
      vector_push_back(elements, literal());
    } while (consume(","));
    expect("}");

    Node *result = new_node(NODE_ARRAY, NULL, NULL);
    result->elements = elements;
    return result;
  }

  return literal();
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
    Declaration *initialDeclaration = declaration(context);
    if (initialDeclaration) {
      //宣言ならカンマ演算子に読み替え
      Node *node = NULL;
      for (int i = 0; i < vector_length(initialDeclaration->declarators); i++) {
        Node *declarator = vector_get(initialDeclaration->declarators, i);
        if (node)
          node = new_node(NODE_COMMA, node, declarator);
        else
          node = declarator;
      }
      result->initialization = node;
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
    blockItem->declaration = declaration(context);
    if (!blockItem->declaration)
      blockItem->statement = statement(context);
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

Declaration *declaration(ParseContext *context) {
  Declaration *result = calloc(1, sizeof(Declaration));

  Type *type = type_specifier(context); // declaration_specifier
  if (type) {
    if (consume(";")) {
      result->type = type;
      result->declarators = new_vector(0);
      return result;
    }

    Vector *declarators = init_declarator_list(type, context);
    expect(";");

    result->declarators = declarators;
    return result;
  }

  // static assert

  return NULL;
}

Vector *init_declarator_list(Type *type, ParseContext *context) {
  Vector *declarators = new_vector(8);
  do {
    vector_push_back(declarators, init_declarator(type, context));
  } while (consume(","));
  return declarators;
}

// 変数宣言をパースする
Node *init_declarator(Type *type, ParseContext *context) {
  Token *tokenHead = token;

  // declarator
  Variable *variable = variable_declaration(type, context);
  if (!variable)
    return NULL;

  Node *node = new_node_variable_definition(variable, context, tokenHead);

  // initalizer
  if (consume("="))
    node = new_node(NODE_ASSIGN, node, assign(context));

  return node;
}

//式をパースする
Node *expression(ParseContext *context) {
  //カンマ演算子
  Node *node = assign(context);
  while (consume(",")) {
    node = new_node(NODE_COMMA, node, assign(context));
  }

  return node;
}

// Node Vector
Vector *argument_expression_list(ParseContext *context) {
  Vector *arguments = new_vector(32);
  do {
    vector_push_back(arguments, assign(context));
  } while (consume(","));
  return arguments;
}

//型指定子をパースする
Type *type_specifier(ParseContext *context) {
  Token *tokenHead = token;

  // 型修飾子のスキップ
  // constの検証はgccに任せる
  consume("const");
  consume("restrict");
  consume("volatile");
  consume("_Atomic");

  Token *current = token;

  //プリミティブ
  const char *types[4];
  types[0] = "int";
  types[1] = "char";
  types[2] = "void";
  types[3] = "_Bool";

  for (int i = 0; i < sizeof(types) / sizeof(char *); i++) {
    if (!consume(types[i]))
      continue;

    while (consume("*"))
      ;

    return new_type_from_token(current);
  }

  // typedefされた型指定子の探索
  Token *identifier = consume_identifier();
  if (identifier) {
    for (int i = 0; i < vector_length(typedefs); i++) {
      Typedef *typeDefinition = vector_get(typedefs, i);
      Type *type = typeDefinition->type;
      if (string_compare(identifier->string, typeDefinition->name)) {
        while (consume("*")) {
          Type *pointer = new_type(TYPE_PTR);
          pointer->base = type;
          type = pointer;
        }
        return type;
      }
    }
    token = tokenHead;
    return NULL;
  }

  Type *structType = struct_specifier(context);
  if (structType) {
    Token *head = token;
    while (consume("*"))
      ;
    return wrap_by_pointer(structType, head);
  }

  Type *enumType = enum_specifier(context);
  if (enumType) {
    Token *head = token;
    while (consume("*"))
      ;
    return wrap_by_pointer(enumType, head);
  }

  token = tokenHead;
  return NULL;
}

Type *enum_specifier(ParseContext *context) {
  Token *tokenHead = token;

  if (!consume("enum"))
    return NULL;

  Token *identifier = consume_identifier();

  Type *result = NULL;

  // 宣言済み列挙体の解決
  if (identifier) {
    Tag *tag =
        tag_container_get(context->scope->tagContainer, identifier->string);
    if (tag) {
      if (tag->type->kind == TYPE_ENUM) {
        result = tag->type;
      } else {
        ERROR_AT(identifier->string->head,
                 "%sは違う種類のタグとして定義されています",
                 identifier->string->head);
      }
    }
  }

  // 解決できなければ生成
  if (!result) {
    result = new_type(TYPE_ENUM);
    if (identifier) {
      result->name = identifier->string;

      //この時点でresultは宣言済みでないことが保証されているので場合分けは不要
      tag_container_push(context->scope->tagContainer, type_to_tag(result));
    }
  }

  if (consume("{")) {
    if (!result->isDefined) {
      result->isDefined = true;
    } else {
      ERROR_AT(identifier->string->head, "列挙体が多重に定義されています")
    }

    int count = 0;
    do {
      Token *enumeratorIdentifier = consume_identifier();
      if (!enumeratorIdentifier)
        break;

      Node *constantExpression = NULL;
      if (consume("="))
        constantExpression = literal(); //定数式としてリテラルのみを許可する
      else {
        constantExpression = new_node_num(count);
        count++;
      }

      Variable *variable =
          new_variable(new_type(TYPE_INT), enumeratorIdentifier->string);
      Variable *enumeratorVariable =
          variable_to_enumerator(variable, constantExpression);
      if (!variable_container_push(context->scope->variableContainer,
                                   enumeratorVariable))
        ERROR_AT(enumeratorIdentifier->string->head,
                 "列挙子%sと同名の識別子が既に定義されています",
                 string_to_char(enumeratorIdentifier->string));
    } while (consume(","));
    expect("}");
  } else if (!identifier) {
    ERROR_AT(tokenHead->string->head,
             "列挙体の名称または列挙子を指定してください");
  }

  return result;
}

Type *struct_specifier(ParseContext *context) {
  if (!consume("struct"))
    return NULL;

  Type *result = NULL;

  Token *identifier = consume_identifier();

  // 宣言済み構造体の解決
  if (identifier) {
    Tag *tag =
        tag_container_get(context->scope->tagContainer, identifier->string);
    if (tag) {
      if (tag->type->kind == TYPE_STRUCT) {
        result = tag->type;
      } else {
        ERROR_AT(identifier->string->head,
                 "%sは違う種類のタグとして定義されています",
                 identifier->string->head);
      }
    }
  }

  // 解決できなければ生成
  if (!result) {
    result = new_type(TYPE_STRUCT);
    if (identifier) {
      result->name = identifier->string;

      //この時点でresultは宣言済みでないことが保証されているので場合分けは不要
      tag_container_push(context->scope->tagContainer, type_to_tag(result));
    }
  }

  if (consume("{")) {
    if (!result->isDefined) {
      result->isDefined = true;
      result->members = new_member_container();
    } else {
      ERROR_AT(identifier->string->head, "構造体が多重に定義されています")
    }

    int memberOffset = 0;
    while (!consume("}")) {
      Type *type = type_specifier(context);
      if (!type)
        ERROR_AT(token->string->head, "構造体のメンバの定義が不正です");
      Variable *member = variable_declaration(type, context);
      if (!member)
        ERROR_AT(token->string->head, "構造体のメンバの定義が不正です");

      member->kind = VARIABLE_LOCAL;
      size_t memberAlignment = type_to_align(member->type);
      memberOffset +=
          (memberAlignment - memberOffset % memberAlignment) % memberAlignment;
      member->offset = memberOffset;
      memberOffset += type_to_size(member->type);

      if (!member_container_push(result->members, member))
        ERROR_AT(member->name->head, "同名のメンバが既に定義されています");
      expect(";");
    }
  } else if (!identifier) {
    ERROR_AT(token->string->head,
             "無名構造体を定義なしで使用することはできません");
  }

  return result;
}

//代入をパースする
Node *assign(ParseContext *context) {
  //本来代入の左辺にとれるのはunaryだけなのでフィルタリングする必要があるがやっていない
  Node *node = conditional(context);

  if (consume("=")) {
    return new_node(NODE_ASSIGN, node, assign(context));
  } else if (consume("+=")) {
    Node *addNode = new_node(NODE_ADD, node, assign(context));
    return new_node(NODE_ASSIGN, node, addNode);
  } else if (consume("-=")) {
    Node *subNode = new_node(NODE_SUB, node, assign(context));
    return new_node(NODE_ASSIGN, node, subNode);
  } else if (consume("*=")) {
    Node *mulNode = new_node(NODE_MUL, node, assign(context));
    return new_node(NODE_ASSIGN, node, mulNode);
  } else if (consume("/=")) {
    Node *divNode = new_node(NODE_DIV, node, assign(context));
    return new_node(NODE_ASSIGN, node, divNode);
  } else if (consume("%=")) {
    Node *modNode = new_node(NODE_MOD, node, assign(context));
    return new_node(NODE_ASSIGN, node, modNode);
  } else if (consume("&=")) {
    Node *bandNode = new_node(NODE_BAND, node, assign(context));
    return new_node(NODE_ASSIGN, node, bandNode);
  } else if (consume("^=")) {
    Node *bxorNode = new_node(NODE_BXOR, node, assign(context));
    return new_node(NODE_ASSIGN, node, bxorNode);
  } else if (consume("|=")) {
    Node *borNode = new_node(NODE_BOR, node, assign(context));
    return new_node(NODE_ASSIGN, node, borNode);
  } else if (consume("<<=")) {
    Node *lshftNode = new_node(NODE_LSHIFT, node, assign(context));
    return new_node(NODE_ASSIGN, node, lshftNode);
  } else if (consume(">>=")) {
    Node *rshiftNode = new_node(NODE_RSHIFT, node, assign(context));
    return new_node(NODE_ASSIGN, node, rshiftNode);
  } else {
    return node;
  }
}

Node *conditional(ParseContext *context) {
  Node *node = logical_or(context);

  if (consume("?")) {
    Node *lhs = expression(context);
    expect(":");
    Node *rhs = conditional(context);

    Node *conditionalNode = new_node(NODE_COND, lhs, rhs);
    conditionalNode->condition = node;
    node = conditionalNode;
  }

  return node;
}

Node *logical_or(ParseContext *context) {
  Node *node = logical_and(context);

  for (;;) {
    if (consume("||"))
      node = new_node(NODE_LOR, node, logical_and(context));
    else
      return node;
  }
}

Node *logical_and(ParseContext *context) {
  Node *node = bitwise_inclusive_or(context);

  for (;;) {
    if (consume("&&"))
      node = new_node(NODE_LAND, node, bitwise_inclusive_or(context));
    else
      return node;
  }
}

Node *bitwise_inclusive_or(ParseContext *context) {
  Node *node = bitwise_exclusive_or(context);

  for (;;) {
    if (consume("|"))
      node = new_node(NODE_BOR, node, bitwise_exclusive_or(context));
    else
      return node;
  }
}

Node *bitwise_exclusive_or(ParseContext *context) {
  Node *node = bitwise_and(context);

  for (;;) {
    if (consume("^"))
      node = new_node(NODE_BXOR, node, bitwise_and(context));
    else
      return node;
  }
}

Node *bitwise_and(ParseContext *context) {
  Node *node = equality(context);

  for (;;) {
    if (consume("&"))
      node = new_node(NODE_BAND, node, equality(context));
    else
      return node;
  }
}

//等式をパースする
Node *equality(ParseContext *context) {
  Node *node = relational(context);

  for (;;) {
    if (consume("=="))
      node = new_node(NODE_EQ, node, relational(context));
    else if (consume("!="))
      node = new_node(NODE_NE, node, relational(context));
    else
      return node;
  }
}

//不等式をパースする
Node *relational(ParseContext *context) {
  Node *node = shift(context);

  for (;;) {
    if (consume("<"))
      node = new_node(NODE_LT, node, shift(context));
    else if (consume("<="))
      node = new_node(NODE_LE, node, shift(context));
    else if (consume(">"))
      node = new_node(NODE_LT, shift(context), node);
    else if (consume(">="))
      node = new_node(NODE_LE, shift(context), node);
    else
      return node;
  }
}

Node *shift(ParseContext *context) {
  Node *node = add(context);

  for (;;) {
    if (consume("<<"))
      node = new_node(NODE_LSHIFT, node, add(context));
    else if (consume(">>"))
      node = new_node(NODE_RSHIFT, node, add(context));
    else
      return node;
  }
}

Node *add(ParseContext *context) {
  Node *node = multiply(context);

  for (;;) {
    if (consume("+"))
      node = new_node(NODE_ADD, node, multiply(context));
    else if (consume("-"))
      node = new_node(NODE_SUB, node, multiply(context));
    else
      return node;
  }
}

//乗除算をパースする
Node *multiply(ParseContext *context) {
  Node *node = unary(context);

  for (;;) {
    if (consume("*"))
      node = new_node(NODE_MUL, node, unary(context));
    else if (consume("/"))
      node = new_node(NODE_DIV, node, unary(context));
    else if (consume("%"))
      node = new_node(NODE_MOD, node, unary(context));
    else
      return node;
  }
}

//単項演算子をパースする
Node *unary(ParseContext *context) {
  if (consume("+"))
    return postfix(context);
  if (consume("-"))
    return new_node(NODE_SUB, new_node_num(0), postfix(context));
  if (consume("!"))
    return new_node(NODE_LNOT, unary(context), NULL);
  if (consume("~"))
    return new_node(NODE_BNOT, unary(context), NULL);
  if (consume("&"))
    return new_node(NODE_REF, unary(context), NULL);
  if (consume("*"))
    return new_node(NODE_DEREF, unary(context), NULL);
  if (consume("++")) {
    Node *source = unary(context);
    Node *addNode = new_node(NODE_ADD, source, new_node_num(1));
    return new_node(NODE_ASSIGN, source, addNode);
  }
  if (consume("--")) {
    Node *source = unary(context);
    Node *addNode = new_node(NODE_SUB, source, new_node_num(1));
    return new_node(NODE_ASSIGN, source, addNode);
  }
  if (consume("sizeof")) {
    Token *head = token;
    if (consume("(")) {
      Type *type = type_specifier(context);
      if (type) {
        expect(")");
        return new_node_num(type_to_size(type));
      }
    }
    token = head;

    bool parentheses = consume("(");
    //識別子に対するsizeofのみを特別に許可する
    Token *identifier = consume_identifier();
    // postfix(context);
    if (!identifier) {
      ERROR_AT(identifier->string->head, "式に対するsizeof演算は未実装です");
    }
    if (parentheses)
      expect(")");

    Type *type =
        new_node_variable(identifier, context->scope->variableContainer)
            ->variable->type;
    if (!type)
      ERROR_AT(token->string->head, "sizeof演算子のオペランドが不正です");
    return new_node_num(type_to_size(type));
  }
  if (consume("_Alignof")) {
    expect("(");

    Type *type = type_specifier(context);
    if (!type)
      ERROR_AT(token->string->head, "型指定子ではありません");

    expect(")");
    return new_node_num(type_to_align(type));
  }
  return postfix(context);
}

//変数、関数呼び出し、添字付の配列
Node *postfix(ParseContext *context) {
  //----postfixが連続するときの先頭は関数呼び出しかprimary--------
  Node *node = primary(context);

  for (;;) {
    if (consume("(")) {
      Vector *arguments; // Node vector
      if (consume(")")) {
        arguments = new_vector(0);
      } else {
        arguments = argument_expression_list(context);
        expect(")");
      }

      node = new_node_function_call(node);
      node->functionCall->arguments = arguments;
    } else if (consume("[")) {
      //配列の添字をポインタ演算に置き換え
      //ポインタ演算の構文木を生成
      Node *addNode = new_node(NODE_ADD, node, expression(context));
      node = new_node(NODE_DEREF, addNode, NULL);
      expect("]");
    } else if (consume(".")) {
      Token *memberToken = expect_identifier();
      Node *memberNode = new_node_member(memberToken);
      node = new_node(NODE_DOT, node, memberNode);
    } else if (consume("->")) {
      Token *memberToken = expect_identifier();
      Node *memberNode = new_node_member(memberToken);
      node = new_node(NODE_DEREF, node, NULL);
      node = new_node(NODE_DOT, node, memberNode);
    } else if (consume("++")) {
      Node *addNode = new_node(NODE_ADD, node, new_node_num(1));
      Node *assignNode = new_node(NODE_ASSIGN, node, addNode);
      node = new_node(NODE_SUB, assignNode,
                      new_node_num(1)); //戻り値を変えるためにやっているが、最悪
    } else if (consume("--")) {
      Node *addNode = new_node(NODE_SUB, node, new_node_num(1));
      Node *assignNode = new_node(NODE_ASSIGN, node, addNode);
      node = new_node(NODE_ADD, assignNode,
                      new_node_num(1)); //戻り値を変えるためにやっているが、最悪
    } else {
      return node;
    }
  }
}

Node *primary(ParseContext *context) {
  //次のトークンが(なら入れ子になった式
  if (consume("(")) {
    Node *node = expression(context);
    expect(")");
    return node;
  }

  Token *identifier = consume_identifier();
  if (identifier) {
    Node *node =
        new_node_variable(identifier, context->scope->variableContainer);
    //列挙子の解決 ここでやるべきではない
    if (node->variable->kind == VARIABLE_ENUMERATOR)
      return node->variable->initialization;
    else
      return node;
  }

  //そうでなければリテラル
  return literal();
}

Node *literal() {
  Token *character = consume_character();
  if (character) {
    Node *node = new_node(NODE_CHAR, NULL, NULL);
    if (character->string->length == 1)
      node->val = *character->string->head;
    else if (character->string->head[0] == '\\') {
      switch (character->string->head[1]) {
      case '0':
        node->val = '\0';
        break;
      case 'a':
        node->val = '\a';
        break;
      case 'b':
        node->val = '\b';
        break;
      case 'f':
        node->val = '\f';
        break;
      case 'n':
        node->val = '\n';
        break;
      case 'r':
        node->val = '\r';
        break;
      case 't':
        node->val = '\t';
        break;
      case 'v':
        node->val = '\v';
        break;
      case '\\':
      case '\'':
      case '\"':
      case '\?':
        node->val = character->string->head[1];
        break;
      default:
        ERROR_AT(character->string->head,
                 "予期しない文字のエスケープシーケンスです");
        break;
      }
    } else {
      ERROR_AT(character->string->head, "文字リテラルではありません");
    }
    return node;
  }

  Token *string = consume_string();
  if (string) {
    const String *content = string->string;
    for (;;) {
      Token *next = consume_string();
      if (next)
        content = string_concat(content, next->string);
      else
        break;
    }

    Node *node = new_node(NODE_STRING, NULL, NULL);
    node->val = vector_length(stringLiterals);
    //コンテナはポインタしか受け入れられないので
    vector_push_back(stringLiterals, string_to_char(content));
    return node;
  }

  //そうでなければ整数
  return new_node_num(expect_number());
}

Program *parse(Token *head) {
  token = head;
  return program();
}
