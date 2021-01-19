/*
 * パーサのためのユーティリティ関数、データ構造、変数群
 * パーサのみでインクルードされる
 */

#ifndef PARSEUTIL_H
#define PARSEUTIL_H

#include "container.h"
#include "declaration.h"
#include "node.h"
#include "statement.h"
#include "tokenize.h"
#include "type.h"
#include "typecontainer.h"
#include "variable.h"
#include "variablecontainer.h"

extern Token *token;
extern Vector *stringLiterals; // String vector

typedef struct Scope Scope;
struct Scope {
  TypeContainer *tagContainer;
  VariableContainer *variableContainer;
  TypeContainer *typedefContainer;
};

typedef struct FunctionContext FunctionContext;
struct FunctionContext {
  int currentStackOffset;
};

typedef struct ParseContext ParseContext;
struct ParseContext {
  Scope *scope;
  FunctionContext *function;
};

ParseContext *new_scope_context(ParseContext *parent);

bool at_eof();

//次のトークンが期待している記号のときには、トークンを1つ読み進めて真を返す
//それ以外の場合には偽を返す
bool consume(const char *op);

//次のトークンが文字列のときには、トークンを1つ読み進めてそのトークンを返す
//それ以外の場合にはNULLを返す
Token *consume_string();

//次のトークンが文字のときには、トークンを1つ読み進めてそのトークンを返す
//それ以外の場合にはNULLを返す
Token *consume_character();

//次のトークンが識別子のときには、トークンを1つ読み進めてそのトークンを返す
//それ以外の場合にはNULLを返す
Token *consume_identifier();

//次のトークンが期待している記号のときには、トークンを1つ読み進める
//それ以外の場合にはエラーを報告する
void expect(char *op);

//次のトークンが数値のときには、トークンを1つ読み進めてその数値を返す
//それ以外の場合にはエラーを報告する
int expect_number();

//次のトークンが識別子のときには、トークンを1つ読み進めてそのトークンを返す
//それ以外の場合にはエラーを報告する
Token *expect_identifier();

Variable *new_variable(Type *type, const String *name);
Variable *variable_to_local(Variable *variable, ParseContext *context);
Variable *variable_to_global(Variable *variable);
Variable *variable_to_function(Variable *variable,
                               FunctionDefinition *function);
Variable *variable_to_enumerator(Variable *variable, Node *initialization);
Variable *variable_to_member(Variable *variable);

FunctionCall *new_function_call();

TypeKind map_token_to_kind(Token *token);
Type *wrap_by_pointer(Type *base, Token *token);
Type *new_type_from_token(Token *typeToken);
Type *new_type_array(Type *base, size_t size);
Type *new_type_function(Type *returnType, Vector *arguments);

//抽象構文木の末端をパースする
//抽象構文木の数値以外のノードを新しく生成する
Node *new_node(NodeKind kind, Node *lhs, Node *rhs);

//抽象構文木の数値のノードを新しく生成する
Node *new_node_num(int val);

//抽象構文木のローカル変数定義のノードを新しく生成する
Node *new_node_variable_definition(Variable *variable, ParseContext *context,
                                   Token *source);

//抽象構文木の変数のノードを新しく生成する
Node *new_node_variable(Token *token, VariableContainer *container);

//抽象構文木のメンバ変数のノードを新しく生成する
Node *new_node_member(Token *token);

//抽象構文木の関数のノードを新しく生成する
Node *new_node_function_call(Node *lhs);

FunctionDefinition *new_function_declaration(Type *type, const String *name,
                                             Vector *arguments);
Vector *node_vector_to_type_vector(Vector *nodes);
Vector *
parameter_declaration_vector_to_type_vector(Vector *parameterDeclarations);

#endif
