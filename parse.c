#include "9cc.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Token *token;
HashTable *localVariableTable;
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

LocalVariable *new_local_variable(Token *token) {
  LocalVariable *localVariable = calloc(1, sizeof(LocalVariable));
  localVariable->name = new_string(token->string, token->length);
  currentOffset += 8;
  localVariable->offset = currentOffset;
  hash_table_store(localVariableTable, localVariable->name, localVariable);
  return localVariable;
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
Node *new_node_lvar(Token *token) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = NODE_LVAR;

  LocalVariable *localVariable = hash_table_find(
      localVariableTable, new_string(token->string, token->length));
  if (localVariable == NULL) {
    localVariable = new_local_variable(token);
  }
  node->offset = localVariable->offset;
  return node;
}

// Statement Structure
typedef struct StatementUnion StatementUnion;

typedef enum {
  STATEMENT_EXPRESSION,
  STATEMENT_IF,
  STATEMENT_RETURN,
} StatementKind;

struct StatementUnion {
  StatementKind tag;
  union {
    ExpressionStatement *expressionStatement;
    ReturnStatement *returnStatement;
    IfStatement *ifStatement;
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

ListNode *program();
StatementUnion *statement();
ExpressionStatement *expression_statement();
ReturnStatement *return_statement();
IfStatement *if_statement();

Node *expression();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *multiply();
Node *unary();
Node *primary();

// program              = statement*
// statement            = expression_statement | return_statement | if_statement
// expression_statement = " expression ";"
// return_statement     = ""return" expression ";"
// if_statement         = "if" "(" expression ")" statement ("else" statement)?

// expression           = assign
// assign               = equality ("=" assign)?
// equality             = relational ("==" relational | "!=" relational)*
// relational           = add ("<" add | "<=" add | ">" add | ">=" add)*
// add                  = mul ("+" mul | "-" mul)*
// mul                  = unary ("*" unary | "/" unary)*
// unary                = ("+" | "-")? primary
// primary              = num | ident | "(" expr ")"

//プログラムをパースする
ListNode *program() {
  localVariableTable = new_hash_table();
  currentOffset = 0;

  ListNode head;
  head.next = NULL;
  ListNode *current = &head;

  while (!at_eof()) {
    current = new_list_node(statement(), current);
  }

  return head.next;
}

//文をパースする
StatementUnion *statement() {
  ReturnStatement *returnPattern = return_statement();
  if (returnPattern) {
    StatementUnion *result = calloc(1, sizeof(StatementUnion));
    result->returnStatement = returnPattern;
    result->tag = STATEMENT_RETURN;
    return result;
  }

  IfStatement *ifPattern = if_statement();
  if (ifPattern) {
    StatementUnion *result = calloc(1, sizeof(StatementUnion));
    result->ifStatement = ifPattern;
    result->tag = STATEMENT_IF;
    return result;
  }

  ExpressionStatement *expressionPattern = expression_statement();
  StatementUnion *result = calloc(1, sizeof(StatementUnion));
  result->expressionStatement = expressionPattern;
  result->tag = STATEMENT_EXPRESSION;
  return result;
}

// 式の文をパースする
ExpressionStatement *expression_statement() {
  ExpressionStatement *result = calloc(1, sizeof(ExpressionStatement));
  result->node = expression();
  expect(";");
  return result;
}

// return文をパースする
ReturnStatement *return_statement() {
  if (!consume("return")) {
    return NULL;
  }

  ReturnStatement *result = calloc(1, sizeof(ReturnStatement));
  result->node = expression();
  expect(";");
  return result;
}

// if文をパースする
IfStatement *if_statement() {
  if (!consume("if") || !consume("(")) {
    return NULL;
  }

  IfStatement *result = calloc(1, sizeof(IfStatement));
  result->conditionExpression = expression();

  expect(")");

  result->thenStatement = statement();
  if (consume("else")) {
    result->elseStatement = statement();
  }

  return result;
}

//式をパースする
Node *expression() { return assign(); }

//式をパースする
Node *assign() {
  Node *node = equality();

  for (;;) {
    if (consume("=")) {
      node = new_node(NODE_ASSIGN, node, equality());
    } else {
      return node;
    }
  }
}

//等式をパースする
Node *equality() {
  Node *node = relational();

  for (;;) {
    if (consume("=="))
      node = new_node(NODE_EQ, node, relational());
    else if (consume("!="))
      node = new_node(NODE_NE, node, relational());
    else
      return node;
  }
}

//不等式をパースする
Node *relational() {
  Node *node = add();

  for (;;) {
    if (consume("<"))
      node = new_node(NODE_LT, node, add());
    else if (consume("<="))
      node = new_node(NODE_LE, node, add());
    else if (consume(">"))
      node = new_node(NODE_LT, add(), node);
    else if (consume(">="))
      node = new_node(NODE_LE, add(), node);
    else
      return node;
  }
}

Node *add() {
  Node *node = multiply();

  for (;;) {
    if (consume("+"))
      node = new_node(NODE_ADD, node, multiply());
    else if (consume("-"))
      node = new_node(NODE_SUB, node, multiply());
    else
      return node;
  }
}

//乗除算をパースする
Node *multiply() {
  Node *node = unary();

  for (;;) {
    if (consume("*"))
      node = new_node(NODE_MUL, node, unary());
    else if (consume("/"))
      node = new_node(NODE_DIV, node, unary());
    else
      return node;
  }
}

//単項演算子をパースする
Node *unary() {
  if (consume("+"))
    return primary();
  if (consume("-"))
    return new_node(NODE_SUB, new_node_num(0), primary());
  return primary();
}

//抽象構文木の末端をパースする
Node *primary() {
  //次のトークンが(なら入れ子になった式
  if (consume("(")) {
    Node *node = expression();
    expect(")");
    return node;
  }

  Token *identifier = consume_identifier();
  if (identifier) {
    return new_node_lvar(identifier);
  }

  //そうでなければ整数
  return new_node_num(expect_number());
}

ListNode *parse(Token *head) {
  token = head;
  return program();
}
