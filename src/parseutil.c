#include "parseutil.h"
#include "container.h"
#include "declaration.h"
#include "membercontainer.h"
#include "node.h"
#include "parse.h"
#include "statement.h"
#include "tokenize.h"
#include "type.h"
#include "typecontainer.h"
#include "util.h"
#include "variable.h"
#include "variablecontainer.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Token *token;
Vector *stringLiterals; // String vector

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
        result->scope->tagContainer = type_container_push_table(
            parent->scope->tagContainer, localTagTable);
      else
        result->scope->tagContainer =
            new_type_container(new_list_node(localTagTable));
    }
    {
      HashTable *localTypedefTable = new_hash_table();
      if (parent)
        result->scope->typedefContainer = type_container_push_table(
            parent->scope->typedefContainer, localTypedefTable);
      else
        result->scope->typedefContainer =
            new_type_container(new_list_node(localTypedefTable));
    }
  }

  {
    //スコープが変わっても関数のコンテキストは不変
    if (parent) {
      result->function = parent->function;
      result->translationUnit = parent->translationUnit;
    }
  }

  return result;
}

bool at_eof(void) { return token->kind == TOKEN_EOF; }

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
Token *consume_string(void) {
  if (token->kind != TOKEN_STRING)
    return NULL;
  Token *current = token;
  token = token->next;
  return current;
}

//次のトークンが文字のときには、トークンを1つ読み進めてそのトークンを返す
//それ以外の場合にはNULLを返す
Token *consume_character(void) {
  if (token->kind != TOKEN_CHAR)
    return NULL;
  Token *current = token;
  token = token->next;
  return current;
}

//次のトークンが識別子のときには、トークンを1つ読み進めてそのトークンを返す
//それ以外の場合にはNULLを返す
Token *consume_identifier(void) {
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
int expect_number(void) {
  if (token->kind != TOKEN_NUMBER)
    ERROR_AT(token->string->head, "数ではありません");
  const int value = token->value;
  token = token->next;
  return value;
}

//次のトークンが識別子のときには、トークンを1つ読み進めてそのトークンを返す
//それ以外の場合にはエラーを報告する
Token *expect_identifier(void) {
  if (token->kind != TOKEN_IDENTIFIER)
    ERROR_AT(token->string->head, "識別子ではありません");
  Token *current = token;
  token = token->next;
  return current;
}

Variable *new_variable(Type *type, const String *name) {
  Variable *variable = calloc(1, sizeof(Variable));
  variable->type = type;
  variable->name = name ? new_string(name->head, name->length) : NULL;
  return variable;
}

Variable *variable_to_auto(Variable *variable, ParseContext *context) {
  variable->kind = VARIABLE_LOCAL;
  context->function->currentStackOffset += type_to_stack_size(variable->type);
  variable->offset = context->function->currentStackOffset;
  return variable;
}

Variable *variable_to_local_static(Variable *variable) {
  variable->kind = VARIABLE_LOCAL;
  return variable;
}

Variable *variable_to_global(Variable *variable) {
  variable->kind = VARIABLE_GLOBAL;
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

FunctionCall *new_function_call(void) {
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

  Variable *localVariable = variable_to_auto(variable, context);
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

Vector *
parameter_declaration_vector_to_type_vector(Vector *parameterDeclarations) {
  Vector *types = new_vector(vector_length(parameterDeclarations));
  for (int i = 0; i < vector_length(parameterDeclarations); i++) {
    Declaration *declaration = vector_get(parameterDeclarations, i);
    Variable *variable = vector_get(declaration->declarators, 0);
    vector_push_back(types, variable->type);
  }
  return types;
}
