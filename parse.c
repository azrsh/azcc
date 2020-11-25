#include "parse.h"
#include "container.h"
#include "membercontainer.h"
#include "node.h"
#include "statement.h"
#include "tokenize.h"
#include "type.h"
#include "typecheck.h"
#include "util.h"
#include "variable.h"
#include "variablecontainer.h"
#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Token *token;
int currentOffset;
Vector *stringLiterals;   // String vector
Vector *userDefinedTypes; // Type vector

bool at_eof() { return token->kind == TOKEN_EOF; }

//次のトークンが期待している記号のときには、トークンを1つ読み進めて真を返す
//それ以外の場合には偽を返す
bool consume(const char *op) {
  const String operator= new_string(op, strlen(op));
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
    error_at(token->string.head, "'%s'ではありません", op);
}

//次のトークンが数値のときには、トークンを1つ読み進めてその数値を返す
//それ以外の場合にはエラーを報告する
int expect_number() {
  if (token->kind != TOKEN_NUMBER)
    error_at(token->string.head, "数ではありません");
  const int value = token->value;
  token = token->next;
  return value;
}

//次のトークンが識別子のときには、トークンを1つ読み進めてそのトークンを返す
//それ以外の場合にはエラーを報告する
Token *expect_identifier() {
  if (token->kind != TOKEN_IDENTIFIER)
    error_at(token->string.head, "識別子ではありません");
  Token *current = token;
  token = token->next;
  return current;
}

Variable *new_variable_local(Type *type, String name) {
  Variable *localVariable = calloc(1, sizeof(Variable));
  localVariable->type = type;
  localVariable->name = name;
  localVariable->kind = VARIABLE_LOCAL;
  currentOffset += type_to_stack_size(type);
  localVariable->offset = currentOffset;
  return localVariable;
}

Variable *new_variable_global(Type *type, String name, Node *initialization) {
  Variable *globalVariable = calloc(1, sizeof(Variable));
  globalVariable->type = type;
  globalVariable->name = name;
  globalVariable->kind = VARIABLE_GLOBAL;
  globalVariable->initialization = initialization;
  return globalVariable;
}

FunctionCall *new_function_call(Token *token) {
  FunctionCall *functionCall = calloc(1, sizeof(FunctionCall));
  functionCall->name = token->string;

  //関数呼び出しの戻り値は常にintであるト仮定する
  functionCall->type = new_type(TYPE_INT);

  return functionCall;
}

TypeKind map_token_to_kind(Token *token) {
  if (token->kind != TOKEN_RESERVED)
    error_at(token->string.head, "組み込み型ではありません");

  if (string_compare(token->string, new_string("int", 3)))
    return TYPE_INT;

  if (string_compare(token->string, new_string("char", 4)))
    return TYPE_CHAR;

  error_at(token->string.head, "組み込み型ではありません");
  return 0;
}

Type *new_type_from_token(Token *typeToken) {
  Type *base = new_type(map_token_to_kind(typeToken));
  base->base = NULL;
  typeToken = typeToken->next;

  Type *current = base;
  const String star = new_string("*", 1);
  while (typeToken && string_compare(typeToken->string, star)) {
    Type *pointer = new_type(TYPE_PTR);
    pointer->base = current;
    current = pointer;

    typeToken = typeToken->next;
  }

  return current;
}

Type *new_type_array(Type *base, size_t size) {
  Type *array = new_type(TYPE_ARRAY);
  array->base = base;
  array->length = size;
  return array;
}

//抽象構文木の末端をパースする
//抽象構文木の数値以外のノードを新しく生成する
Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

//抽象構文木の数値のノードを新しく生成する
Node *new_node_num(int val) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = NODE_NUM;
  node->val = val;
  return node;
}

//抽象構文木のローカル変数定義のノードを新しく生成する
Node *new_node_variable_definition(Type *type, Token *identifier,
                                   VariableContainer *variableContainer) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = NODE_VAR;

  String variableName = identifier->string;
  Variable *localVariable = new_variable_local(type, variableName);
  if (!variable_container_push(variableContainer, localVariable))
    error_at(variableName.head, "同名の変数が既に定義されています");

  node->variable = localVariable;
  return node;
}

//抽象構文木の変数のノードを新しく生成する
Node *new_node_variable(Token *token, VariableContainer *container) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = NODE_VAR;

  String variableName = token->string;
  Variable *variable = variable_container_get(container, variableName);
  if (!variable) {
    error_at(variableName.head, "変数%sは定義されていません",
             string_to_char(variableName));
  }

  node->type = variable->type; //本当は消したい
  node->variable = variable;
  return node;
}

//抽象構文木のメンバ変数のノードを新しく生成する
Node *new_node_member(Token *token, MemberContainer *container) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = NODE_VAR;

  String variableName = token->string;
  Variable *variable = member_container_get(container, variableName);
  if (!variable) {
    error_at(variableName.head, "メンバ変数%sは定義されていません",
             string_to_char(variableName));
  }

  node->variable = variable;
  return node;
}

//抽象構文木の関数のノードを新しく生成する
Node *new_node_function_call(Token *token) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = NODE_FUNC;
  node->functionCall = new_function_call(token);
  return node;
}

FunctionDefinition *new_function_definition(Token *identifier) {
  FunctionDefinition *definition = calloc(1, sizeof(FunctionDefinition));
  definition->name = identifier->string;
  return definition;
}

// EBNFパーサ
Program *program();
FunctionDefinition *function_definition(VariableContainer *variableContainer);
Vector *function_definition_argument(VariableContainer *variableContainer);
Variable *global_variable_definition(VariableContainer *variableContainer);
Type *struct_definition(VariableContainer *variableContainer);
StatementUnion *statement(VariableContainer *variableContainer);
ExpressionStatement *expression_statement(VariableContainer *variableContainer);
ReturnStatement *return_statement(VariableContainer *variableContainer);
IfStatement *if_statement(VariableContainer *variableContainer);
WhileStatement *while_statement(VariableContainer *variableContainer);
ForStatement *for_statement(VariableContainer *variableContainer);
CompoundStatement *compound_statement(VariableContainer *variableContainer);
BreakStatement *break_statement(VariableContainer *variableContainer);
ContinueStatement *continue_statement(VariableContainer *variableContainer);

Node *expression(VariableContainer *variableContainer);
Node *variable_definition(VariableContainer *variableContainer);
Type *type_specifier();
Node *assign(VariableContainer *variableContainer);
Node *equality(VariableContainer *variableContainer);
Node *relational(VariableContainer *variableContainer);
Node *add(VariableContainer *variableContainer);
Node *multiply(VariableContainer *variableContainer);
Node *unary(VariableContainer *variableContainer);
Node *primary(VariableContainer *variableContainer);
Vector *function_call_argument(VariableContainer *variableContainer);
Node *literal();

// program = (function_definition | global_variable_definition |
// struct_definition)*
// function_definition = type_specifier identity "("
// function_definition_argument? ")" compound_statement
// function_definition_argument = type_specifier identity ("," type_specifier
// identity)*
// global_variable_definition = variable_definition ";"
// statement = expression_statement | return_statement | if_statement
// | while_statementa | break_statement | continue_statement
// expression_statement = " expression ";"
// return_statement = "return" expression ";"
// if_statement = "if" "(" expression ")" statement ("else" statement)?
// while_statement = "while" "(" expression ")" statement
// for_statement = "for" "(" expression ";" expression ";" expression ")"
// statement
// compound_statement = "{" statement* "}"
// break_statement = "break" ";"
// continue_statement = "continue" ";"

// expression = assign | variable_definition
// variable_definition = type_specifier identity
// type_specifier = ("int" | "char") "*"*
// assign = equality ("=" assign)?
// equality = relational ("==" relational | "!=" relational)*
// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
// add = mul ("+" mul | "-" mul)*
// mul = unary ("*" unary | "/" unary)*
// unary = ("+" | "-" | "&" | "*")? primary
// primary = literal | identity ("("
// function_call_argument? ")" | "[" expression "]")? | "("expression")" |
// "sizeof" "(" (expression | type_specifier) ")" | "_Alignof" "("
// type_specifier ")"
// function_call_argument = expression ("," expression)* literal = number |
// "\"" string"\""

//プログラムをパースする
Program *program() {
  HashTable *globalVariableTable = new_hash_table();
  ListNode *listHead = new_list_node(globalVariableTable);
  VariableContainer *variableContainer = new_variable_container(listHead);

  Program *result = calloc(1, sizeof(Program));
  result->structs = new_vector(16);
  result->functions = new_vector(16);
  result->globalVariables = new_vector(16);
  result->stringLiterals = new_vector(16);
  userDefinedTypes = result->structs;
  stringLiterals = result->stringLiterals;

  while (!at_eof()) {
    FunctionDefinition *functionDefinition =
        function_definition(variableContainer);
    if (functionDefinition) {
      vector_push_back(result->functions, functionDefinition);
      continue;
    }

    Variable *globalVariableDefinition =
        global_variable_definition(variableContainer);
    if (globalVariableDefinition) {
      vector_push_back(result->globalVariables, globalVariableDefinition);
      continue;
    }

    Type *structDefinition = struct_definition(variableContainer);
    if (structDefinition) {
      vector_push_back(result->structs, structDefinition);
      continue;
    }

    error_at(token->string.head, "認識できない構文です");
  }

  return result;
}

//関数の定義をパースする
FunctionDefinition *function_definition(VariableContainer *variableContainer) {
  Token *tokenHead = token;

  Type *type = type_specifier();
  if (!type) {
    return NULL;
  }

  Token *identifier = consume_identifier();
  if (!identifier) {
    token = tokenHead;
    return NULL;
  }

  FunctionDefinition *definition = new_function_definition(identifier);

  //新しいスコープなので先頭に新しい変数テーブルを追加
  currentOffset = 0;
  HashTable *localVariableTable = new_hash_table();
  VariableContainer *mergedContainer =
      variable_container_push_table(variableContainer, localVariableTable);

  if (!consume("(")) {
    token = tokenHead;
    return NULL;
  }

  if (consume(")")) {
    definition->arguments = new_vector(0);
  } else {
    definition->arguments = function_definition_argument(mergedContainer);
    expect(")");
  }

  if (!consume("{")) {
    token = tokenHead;
    return NULL;
  }

  //
  //関数には引数があるので複文オブジェクトの生成を特別扱いしている
  //
  CompoundStatement *body = calloc(1, sizeof(CompoundStatement));

  //新しいスコープの追加を外へ移動

  //ブロック内の文をパ-ス
  ListNode head;
  head.next = NULL;
  ListNode *node = &head;
  while (!consume("}")) {
    StatementUnion *statementUnion = statement(mergedContainer);
    tag_type_to_statement(statementUnion, type);
    node = list_push_back(node, statementUnion);
  }
  body->statementHead = head.next;
  //
  //
  //

  definition->body = body;
  definition->stackSize = currentOffset;

  return definition;
}

// Local Variable Nodes
Vector *function_definition_argument(VariableContainer *variableContainer) {
  Vector *arguments = new_vector(32);

  do {
    Type *type = type_specifier();
    if (!type)
      error_at(token->string.head, "型指定子ではありません");

    Token *identifier = expect_identifier();
    Node *node =
        new_node_variable_definition(type, identifier, variableContainer);
    node->type =
        node->variable->type; // tag_type_to_node(type)できないので手動型付け

    vector_push_back(arguments, node);
  } while (consume(","));
  return arguments;
}

Variable *global_variable_definition(VariableContainer *variableContainer) {
  Token *tokenHead = token;

  Type *type = type_specifier();
  if (!type) {
    return NULL;
  }

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

  Node *initialization = NULL;
  if (consume("=")) {
    initialization = literal();
  }

  if (!consume(";")) {
    token = tokenHead;
    return NULL;
  }

  String variableName = identifier->string;
  Variable *globalVariable =
      new_variable_global(type, variableName, initialization);
  if (!variable_container_push(variableContainer, globalVariable))
    error_at(token->string.head, "同名の変数が既に定義されています");

  return globalVariable;
}

Type *struct_definition(VariableContainer *variavbelContainer) {
  Token *tokenHead = token;

  if (!consume("struct")) {
    return NULL;
  }

  Token *identifier = consume_identifier();
  if (!identifier) {
    token = tokenHead;
    return NULL;
  }

  Type *result = new_type(TYPE_STRUCT);
  result->kind = TYPE_STRUCT;
  result->name = identifier->string;
  result->members = new_member_container();
  int memberOffset = 0;
  if (consume("{")) {
    while (!consume("}")) {
      Variable *member = calloc(1, sizeof(Variable));
      member->kind = VARIABLE_LOCAL;
      member->type = type_specifier();
      if (!member->type) {
        error_at(token->string.head, "構造体のメンバの型を指定して下さい");
      }
      Token *memberName = consume_identifier();
      if (!memberName) {
        error_at(token->string.head, "構造体のメンバの名前を指定して下さい");
      }
      member->name = memberName->string;
      size_t memberSize = type_to_size(member->type);
      memberOffset += (memberSize - memberOffset % memberSize) % memberSize;
      member->offset = memberOffset;
      memberOffset += type_to_size(member->type);

      if (!member_container_push(result->members, member))
        error(memberName->string.head, "同名のメンバが既に定義されています");
      expect(";");
    }
  }

  vector_push_back(userDefinedTypes, result);

  expect(";");

  return result;
}

//文をパースする
StatementUnion *statement(VariableContainer *variableContainer) {
  ReturnStatement *returnPattern = return_statement(variableContainer);
  if (returnPattern) {
    return new_statement_union_return(returnPattern);
  }

  IfStatement *ifPattern = if_statement(variableContainer);
  if (ifPattern) {
    return new_statement_union_if(ifPattern);
  }

  WhileStatement *whilePattern = while_statement(variableContainer);
  if (whilePattern) {
    return new_statement_union_while(whilePattern);
  }

  ForStatement *forPattern = for_statement(variableContainer);
  if (forPattern) {
    return new_statement_union_for(forPattern);
  }

  CompoundStatement *compoundPattern = compound_statement(variableContainer);
  if (compoundPattern) {
    return new_statement_union_compound(compoundPattern);
  }

  BreakStatement *breakPattern = break_statement(variableContainer);
  if (breakPattern) {
    return new_statement_union_break(breakPattern);
  }

  ContinueStatement *continuePattern = continue_statement(variableContainer);
  if (continuePattern) {
    return new_statement_union_continue(continuePattern);
  }

  ExpressionStatement *expressionPattern =
      expression_statement(variableContainer);
  return new_statement_union_expression(expressionPattern);
}

// 式の文をパースする
ExpressionStatement *
expression_statement(VariableContainer *variableContainer) {
  ExpressionStatement *result = calloc(1, sizeof(ExpressionStatement));
  result->node = expression(variableContainer);
  expect(";");
  return result;
}

// return文をパースする
ReturnStatement *return_statement(VariableContainer *variableContainer) {
  if (!consume("return")) {
    return NULL;
  }

  ReturnStatement *result = calloc(1, sizeof(ReturnStatement));
  result->node = expression(variableContainer);
  expect(";");
  return result;
}

// if文をパースする
IfStatement *if_statement(VariableContainer *variableContainer) {
  if (!consume("if") || !consume("(")) {
    return NULL;
  }

  IfStatement *result = calloc(1, sizeof(IfStatement));
  result->condition = expression(variableContainer);

  expect(")");

  result->thenStatement = statement(variableContainer);
  if (consume("else")) {
    result->elseStatement = statement(variableContainer);
  }

  return result;
}

// while文をパースする
WhileStatement *while_statement(VariableContainer *variableContainer) {
  if (!consume("while") || !consume("(")) {
    return NULL;
  }

  WhileStatement *result = calloc(1, sizeof(WhileStatement));
  result->condition = expression(variableContainer);

  expect(")");

  result->statement = statement(variableContainer);

  return result;
}

// for文をパースする
ForStatement *for_statement(VariableContainer *variableContainer) {
  if (!consume("for") || !consume("(")) {
    return NULL;
  }

  ForStatement *result = calloc(1, sizeof(ForStatement));
  result->initialization = expression(variableContainer);
  expect(";");
  result->condition = expression(variableContainer);
  expect(";");
  result->afterthought = expression(variableContainer);

  expect(")");

  result->statement = statement(variableContainer);

  return result;
}

// 複文をパースする
CompoundStatement *compound_statement(VariableContainer *variableContainer) {
  if (!consume("{")) {
    return NULL;
  }

  CompoundStatement *result = calloc(1, sizeof(CompoundStatement));

  //新しいスコープなので先頭に新しい変数テーブルを追加
  HashTable *localVariableTable = new_hash_table();
  VariableContainer *mergedContainer =
      variable_container_push_table(variableContainer, localVariableTable);

  //ブロック内の文をパ-ス
  ListNode head;
  head.next = NULL;
  ListNode *node = &head;
  while (!consume("}")) {
    node = list_push_back(node, statement(mergedContainer));
  }
  result->statementHead = head.next;

  return result;
}

// break文をパースする
BreakStatement *break_statement(VariableContainer *variableContainer) {
  if (!consume("break")) {
    return NULL;
  }
  expect(";");
  BreakStatement *result = calloc(1, sizeof(BreakStatement));
  return result;
}

// continue文をパースする
ContinueStatement *continue_statement(VariableContainer *variableContainer) {
  if (!consume("continue")) {
    return NULL;
  }
  expect(";");
  ContinueStatement *result = calloc(1, sizeof(ContinueStatement));
  return result;
}

//式をパースする
Node *expression(VariableContainer *variableContainer) {
  Node *node = variable_definition(variableContainer);
  if (node) {
    return node;
  }

  node = assign(variableContainer);
  return node;
}

// 変数定義をパースする
Node *variable_definition(VariableContainer *variableContainer) {
  Token *current = token;
  Type *type = type_specifier();
  if (!type) {
    return NULL;
  }

  Token *identifier = consume_identifier();
  if (!identifier) {
    token = current;
    return NULL;
  }

  while (consume("[")) {
    int size = expect_number();
    type = new_type_array(type, size);
    expect("]");
  }

  Node *node =
      new_node_variable_definition(type, identifier, variableContainer);

  for (;;) {
    if (consume("=")) {
      node = new_node(NODE_ASSIGN, node, equality(variableContainer));
    } else {
      return node;
    }
  }

  return node;
}

//型指定子をパースする
Type *type_specifier() {
  Token *current = token;

  //プリミティブ
  const char *types[] = {"int", "char"};

  for (int i = 0; i < 2; i++) {
    if (!consume(types[i]))
      continue;

    while (consume("*"))
      ;

    return new_type_from_token(current);
  }

  if (consume("struct")) {
    Token *identifier = consume_identifier();
    if (!identifier) {
      token = current;
      return NULL;
    }

    for (int i = 0; i < vector_length(userDefinedTypes); i++) {
      Type *type = vector_get(userDefinedTypes, i);
      if (string_compare(identifier->string, type->name)) {
        return type;
      }
    }
  }

  token = current;
  return NULL;
}

//代入をパースする
Node *assign(VariableContainer *variableContainer) {
  Node *node = equality(variableContainer);

  for (;;) {
    if (consume("=")) {
      node = new_node(NODE_ASSIGN, node, equality(variableContainer));
    } else {
      return node;
    }
  }
}

//等式をパースする
Node *equality(VariableContainer *variableContainer) {
  Node *node = relational(variableContainer);

  for (;;) {
    if (consume("=="))
      node = new_node(NODE_EQ, node, relational(variableContainer));
    else if (consume("!="))
      node = new_node(NODE_NE, node, relational(variableContainer));
    else
      return node;
  }
}

//不等式をパースする
Node *relational(VariableContainer *variableContainer) {
  Node *node = add(variableContainer);

  for (;;) {
    if (consume("<"))
      node = new_node(NODE_LT, node, add(variableContainer));
    else if (consume("<="))
      node = new_node(NODE_LE, node, add(variableContainer));
    else if (consume(">"))
      node = new_node(NODE_LT, add(variableContainer), node);
    else if (consume(">="))
      node = new_node(NODE_LE, add(variableContainer), node);
    else
      return node;
  }
}

Node *add(VariableContainer *variableContainer) {
  Node *node = multiply(variableContainer);

  for (;;) {
    if (consume("+"))
      node = new_node(NODE_ADD, node, multiply(variableContainer));
    else if (consume("-"))
      node = new_node(NODE_SUB, node, multiply(variableContainer));
    else
      return node;
  }
}

//乗除算をパースする
Node *multiply(VariableContainer *variableContainer) {
  Node *node = unary(variableContainer);

  for (;;) {
    if (consume("*"))
      node = new_node(NODE_MUL, node, unary(variableContainer));
    else if (consume("/"))
      node = new_node(NODE_DIV, node, unary(variableContainer));
    else
      return node;
  }
}

//単項演算子をパースする
Node *unary(VariableContainer *variableContainer) {
  if (consume("+"))
    return primary(variableContainer);
  if (consume("-"))
    return new_node(NODE_SUB, new_node_num(0), primary(variableContainer));
  if (consume("&"))
    return new_node(NODE_REF, primary(variableContainer), NULL);
  if (consume("*"))
    return new_node(NODE_DEREF, primary(variableContainer), NULL);
  return primary(variableContainer);
}

Node *primary(VariableContainer *variableContainer) {
  //次のトークンが(なら入れ子になった式
  if (consume("(")) {
    Node *node = expression(variableContainer);
    expect(")");
    return node;
  }

  // sizeof演算子
  if (consume("sizeof")) {
    expect("(");

    Type *type = type_specifier();
    if (!type) {
      //識別子に対するsizeofのみを特別に許可する
      Token *identifier = consume_identifier();
      if (identifier) {
        type = new_node_variable(identifier, variableContainer)->variable->type;
      } else {
        // expression(variableContainer);
        error("式に対するsizeof演算は未実装です");
      }
    }
    if (!type)
      error_at(token->string.head, "sizeof演算子のオペランドが不正です");

    expect(")");
    return new_node_num(type_to_size(type));
  }

  // _Alignof演算子
  if (consume("_Alignof")) {
    expect("(");

    Type *type = type_specifier();
    if (!type)
      error_at(token->string.head, "型指定子ではありません");

    expect(")");
    return new_node_num(type_to_align(type));
  }

  //変数、関数呼び出し、添字付の配列
  Token *identifier = consume_identifier();
  if (identifier) {
    if (consume("(")) {
      Node *function = new_node_function_call(identifier);
      if (consume(")")) {
        function->functionCall->arguments = new_vector(0);
      } else {
        function->functionCall->arguments =
            function_call_argument(variableContainer);
        expect(")");
      }
      return function;
    } else if (consume("[")) {
      //配列の添字をポインタ演算に置き換え
      //ポインタ演算の構文木を生成
      Node *variableNode = new_node_variable(identifier, variableContainer);
      Node *addNode =
          new_node(NODE_ADD, variableNode, expression(variableContainer));
      Node *result = new_node(NODE_DEREF, addNode, NULL);

      expect("]");

      return result;
    } else if (consume(".")) {
      Node *structVariable = new_node_variable(identifier, variableContainer);
      if (structVariable->type->kind != TYPE_STRUCT) {
        error(identifier->string.head, "構造体ではありません");
      }

      Token *memberToken = expect_identifier();
      Node *memberNode =
          new_node_member(memberToken, structVariable->type->members);
      return new_node(NODE_DOT, structVariable, memberNode);
    } else {
      Node *node = new_node_variable(identifier, variableContainer);
      return node;
    }
  }

  //そうでなければリテラル
  return literal();
}

// Node Vector
Vector *function_call_argument(VariableContainer *variableContainer) {
  Vector *arguments = new_vector(32);
  do {
    vector_push_back(arguments, expression(variableContainer));
  } while (consume(","));
  return arguments;
}

Node *literal() {
  Token *string = consume_string();
  if (string) {
    Node *node = new_node(NODE_STRING, NULL, NULL);
    node->val = vector_length(stringLiterals);
    //コンテナはポインタしか受け入れられないので
    vector_push_back(stringLiterals, string_to_char(string->string));
    return node;
  }

  //そうでなければ整数
  return new_node_num(expect_number());
}

Program *parse(Token *head) {
  token = head;
  return program();
}
