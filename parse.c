#include "9cc.h"
#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Token *token;
int currentOffset;

bool at_eof() { return token->kind == TOKEN_EOF; }

//次のトークンが期待している記号のときには、トークンを1つ読み進めて真を返す
//それ以外の場合には偽を返す
bool consume(char *op) {
  if (token->kind != TOKEN_RESERVED || strlen(op) != token->length ||
      memcmp(token->string, op, token->length))
    return false;
  token = token->next;
  return true;
}

//次のトークンが識別子のときには、トークンを1つ読み進めてそのトークンを返す
//それ以外の場合には偽を返す
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
  if (token->kind != TOKEN_RESERVED || strlen(op) != token->length ||
      memcmp(token->string, op, token->length))
    error_at(token->string, "'%s'ではありません", op);
  token = token->next;
}

//次のトークンが数値のときには、トークンを1つ読み進めてその数値を返す
//それ以外の場合にはエラーを報告する
int expect_number() {
  if (token->kind != TOKEN_NUMBER)
    error_at(token->string, "数ではありません");
  int value = token->value;
  token = token->next;
  return value;
}

//次のトークンが識別子のときには、トークンを1つ読み進めてそのトークンを返す
//それ以外の場合には偽を返す
Token *expect_identifier() {
  if (token->kind != TOKEN_IDENTIFIER)
    error_at(token->string, "識別子ではありません");
  Token *current = token;
  token = token->next;
  return current;
}

LocalVariable *new_local_variable(String name, VariableContainer *container) {
  LocalVariable *localVariable = calloc(1, sizeof(LocalVariable));
  localVariable->name = name;
  currentOffset += 8;
  localVariable->offset = currentOffset;
  return localVariable;
}

FunctionCall *new_function_call(Token *token) {
  FunctionCall *functionCall = calloc(1, sizeof(FunctionCall));
  functionCall->name = new_string(token->string, token->length);
  return functionCall;
}

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

//抽象構文木のローカル変数のノードを新しく生成する
Node *new_node_lvar(Token *token, VariableContainer *container) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = NODE_LVAR;

  LocalVariable *localVariable = variable_container_get(
      container, new_string(token->string, token->length));
  if (localVariable == NULL) {
    String variableName = new_string(token->string, token->length);
    localVariable = new_local_variable(variableName, container);
    if (!variable_container_push(container, localVariable))
      error_at(token->string, "同名の変数が既に定義されています");
  }

  node->offset = localVariable->offset;
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
  definition->name = new_string(identifier->string, identifier->length);
  return definition;
}

// Statement Structure
typedef struct StatementUnion StatementUnion;

typedef enum {
  STATEMENT_EXPRESSION,
  STATEMENT_IF,
  STATEMENT_WHILE,
  STATEMENT_FOR,
  STATEMENT_COMPOUND,
  STATEMENT_RETURN,
} StatementKind;

struct StatementUnion {
  StatementKind tag;
  union {
    ExpressionStatement *expressionStatement;
    ReturnStatement *returnStatement;
    IfStatement *ifStatement;
    WhileStatement *whileStatement;
    ForStatement *forStatement;
    CompoundStatement *compoundStatement;
  };
};

ExpressionStatement *
statement_union_take_expression(StatementUnion *statementUnion) {
  if (statementUnion->tag == STATEMENT_EXPRESSION)
    return statementUnion->expressionStatement;
  return NULL;
}

ReturnStatement *statement_union_take_return(StatementUnion *statementUnion) {
  if (statementUnion->tag == STATEMENT_RETURN)
    return statementUnion->returnStatement;
  return NULL;
}

IfStatement *statement_union_take_if(StatementUnion *statementUnion) {
  if (statementUnion->tag == STATEMENT_IF)
    return statementUnion->ifStatement;
  return NULL;
}

WhileStatement *statement_union_take_while(StatementUnion *statementUnion) {
  if (statementUnion->tag == STATEMENT_WHILE)
    return statementUnion->whileStatement;
  return NULL;
}

ForStatement *statement_union_take_for(StatementUnion *statementUnion) {
  if (statementUnion->tag == STATEMENT_FOR)
    return statementUnion->forStatement;
  return NULL;
}

CompoundStatement *
statement_union_take_compound(StatementUnion *statementUnion) {
  if (statementUnion->tag == STATEMENT_COMPOUND)
    return statementUnion->compoundStatement;
  return NULL;
}

//
// VariableContainer
//
struct VariableContainer {
  ListNode *tableHead;
};

VariableContainer *new_variable_container(ListNode *tableHead) {
  VariableContainer *container = calloc(1, sizeof(VariableContainer));
  container->tableHead = tableHead;
  return container;
}

LocalVariable *variable_container_get(VariableContainer *container,
                                      String name) {
  ListNode *list = container->tableHead;
  while (list) {
    LocalVariable *variable = hash_table_find(list->body, name);
    if (variable)
      return variable;

    list = list->next;
  }

  return NULL;
}

bool variable_container_push(VariableContainer *container,
                             LocalVariable *variable) {
  HashTable *localTable = container->tableHead->body;
  bool isExist = hash_table_contain(localTable, variable->name);
  if (isExist)
    return false;

  hash_table_store(localTable, variable->name, variable);
  return true;
}

bool variable_container_update(VariableContainer *container,
                               LocalVariable *variable) {
  if (variable_container_get(container, variable->name)) {
    HashTable *localTable = container->tableHead->body;
    hash_table_store(localTable, variable->name, variable);
    return true;
  }

  return false;
}

VariableContainer *variable_container_push_table(VariableContainer *container,
                                                 HashTable *table) {
  ListNode *newHead = list_push_front(container->tableHead, table);
  return new_variable_container(newHead);
}

// EBNFパーサ
ListNode *program();
FunctionDefinition *function_definition(VariableContainer *variableContainer);
Vector *function_definition_argument(VariableContainer *variableContainer);
StatementUnion *statement(VariableContainer *variableContainer);
ExpressionStatement *expression_statement(VariableContainer *variableContainer);
ReturnStatement *return_statement(VariableContainer *variableContainer);
IfStatement *if_statement(VariableContainer *variableContainer);
WhileStatement *while_statement(VariableContainer *variableContainer);
ForStatement *for_statement(VariableContainer *variableContainer);
CompoundStatement *compound_statement(VariableContainer *variableContainer);

Node *expression(VariableContainer *variableContainer);
Node *assign(VariableContainer *variableContainer);
Node *equality(VariableContainer *variableContainer);
Node *relational(VariableContainer *variableContainer);
Node *add(VariableContainer *variableContainer);
Node *multiply(VariableContainer *variableContainer);
Node *unary(VariableContainer *variableContainer);
Node *primary(VariableContainer *variableContainer);
Vector *function_call_argument(VariableContainer *variableContainer);

// program              = statement*
// function_definition  = identity "(" function_definition_argument? ")"
// compound_statement
// function_definition_argument = identity ("," identity)*
// statement            = expression_statement | return_statement | if_statement
// | while_statement
// expression_statement = " expression ";"
// return_statement     = "return" expression ";"
// if_statement         = "if" "(" expression ")" statement ("else" statement)?
// while_statement      = "while" "(" expression ")" statement
// for_statement        = "for" "(" expression ";" expression ";" expression ")"
// statement
// compound_statement   = "{" statement* "}"

// expression           = assign
// assign               = equality ("=" assign)?
// equality             = relational ("==" relational | "!=" relational)*
// relational           = add ("<" add | "<=" add | ">" add | ">=" add)*
// add                  = mul ("+" mul | "-" mul)*
// mul                  = unary ("*" unary | "/" unary)*
// unary                = ("+" | "-")? primary
// primary              = number | identity ("(" function_call_argument? ")")? |
// "("expression")"
// function_call_argument = expression ("," expression)*

//プログラムをパースする
ListNode *program() {
  HashTable *globalVariableTable = new_hash_table();
  ListNode *listHead = new_list_node(globalVariableTable);
  VariableContainer *variableContainer = new_variable_container(listHead);

  currentOffset = 0;

  ListNode head;
  ListNode *current = &head;

  while (!at_eof()) {
    FunctionDefinition *definition = function_definition(variableContainer);
    if (!definition)
      break;

    current = list_push_back(current, definition);
  }

  return head.next;
}

//関数の定義をパースする
FunctionDefinition *function_definition(VariableContainer *variableContainer) {
  Token *identifier = consume_identifier();
  if (!identifier) {
    return NULL;
  }

  FunctionDefinition *definition = new_function_definition(identifier);

  //新しいスコープなので先頭に新しい変数テーブルを追加
  HashTable *localVariableTable = new_hash_table();
  VariableContainer *mergedContainer =
      variable_container_push_table(variableContainer, localVariableTable);

  expect("(");
  if (consume(")")) {
    definition->arguments = new_vector(0);
  } else {
    definition->arguments = function_definition_argument(mergedContainer);
    expect(")");
  }

  if (!consume("{")) {
    return NULL;
  }

  //
  //関数には引数があるので複文オブジェクトの生成を特別扱いしている
  //
  CompoundStatement *body = calloc(1, sizeof(CompoundStatement));

  //新しいスコープの追加を外へ移動

  //ブロック内の文をパ-ス
  ListNode head;
  ListNode *node = &head;
  while (!consume("}")) {
    node = list_push_back(node, statement(mergedContainer));
  }
  body->statementHead = head.next;
  //
  //
  //

  definition->body = body;

  return definition;
}

// Local Variable Nodes
Vector *function_definition_argument(VariableContainer *variableContainer) {
  Vector *arguments = new_vector(32);

  do {
    Token *identifier = expect_identifier();
    Node *node = new_node_lvar(identifier, variableContainer);
    vector_push_back(arguments, node);
  } while (consume(","));
  return arguments;
}

//文をパースする
StatementUnion *statement(VariableContainer *variableContainer) {
  ReturnStatement *returnPattern = return_statement(variableContainer);
  if (returnPattern) {
    StatementUnion *result = calloc(1, sizeof(StatementUnion));
    result->returnStatement = returnPattern;
    result->tag = STATEMENT_RETURN;
    return result;
  }

  IfStatement *ifPattern = if_statement(variableContainer);
  if (ifPattern) {
    StatementUnion *result = calloc(1, sizeof(StatementUnion));
    result->ifStatement = ifPattern;
    result->tag = STATEMENT_IF;
    return result;
  }

  WhileStatement *whilePattern = while_statement(variableContainer);
  if (whilePattern) {
    StatementUnion *result = calloc(1, sizeof(StatementUnion));
    result->whileStatement = whilePattern;
    result->tag = STATEMENT_WHILE;
    return result;
  }

  ForStatement *forPattern = for_statement(variableContainer);
  if (forPattern) {
    StatementUnion *result = calloc(1, sizeof(StatementUnion));
    result->forStatement = forPattern;
    result->tag = STATEMENT_FOR;
    return result;
  }

  CompoundStatement *compoundPattern = compound_statement(variableContainer);
  if (compoundPattern) {
    StatementUnion *result = calloc(1, sizeof(StatementUnion));
    result->compoundStatement = compoundPattern;
    result->tag = STATEMENT_COMPOUND;
    return result;
  }

  ExpressionStatement *expressionPattern =
      expression_statement(variableContainer);
  StatementUnion *result = calloc(1, sizeof(StatementUnion));
  result->expressionStatement = expressionPattern;
  result->tag = STATEMENT_EXPRESSION;
  return result;
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
  ListNode *node = &head;
  while (!consume("}")) {
    node = list_push_back(node, statement(mergedContainer));
  }
  result->statementHead = head.next;

  return result;
}

//式をパースする
Node *expression(VariableContainer *variableContainer) {
  return assign(variableContainer);
}

//式をパースする
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
  return primary(variableContainer);
}

//抽象構文木の末端をパースする
Node *primary(VariableContainer *variableContainer) {
  //次のトークンが(なら入れ子になった式
  if (consume("(")) {
    Node *node = expression(variableContainer);
    expect(")");
    return node;
  }

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
    } else {
      return new_node_lvar(identifier, variableContainer);
    }
  }

  //そうでなければ整数
  return new_node_num(expect_number());
}

// Node Vector
Vector *function_call_argument(VariableContainer *variableContainer) {
  Vector *arguments = new_vector(32);
  do {
    vector_push_back(arguments, expression(variableContainer));
  } while (consume(","));
  return arguments;
}

ListNode *parse(Token *head) {
  token = head;
  return program();
}
