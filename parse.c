#include "9cc.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Token *token;
char *user_input;

//次のトークンが期待している記号のときには、トークンを1つ読み進めて真を返す
//それ以外の場合には偽を返す
bool consume(char *op) {
  if (token->kind != TOKEN_RESERVED || strlen(op) != token->length ||
      memcmp(token->string, op, token->length))
    return false;
  token = token->next;
  return true;
}

//次のトークンが期待している記号のときには、トークンを1つ読み進める
//それ以外の場合にはエラーを報告する
void expect(char *op) {
  if (token->kind != TOKEN_RESERVED || strlen(op) != token->length ||
      memcmp(token->string, op, token->length))
    error("'%c'ではありません", op);
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

Node *expression();
Node *equality();
Node *relational();
Node *add();
Node *multiply();
Node *unary();
Node *primary();

// expression = equality
// equality   = relational ("==" relational | "!=" relational)*
// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
// add        = mul ("+" mul | "-" mul)*
// mul        = unary ("*" unary | "/" unary)*
// unary      = ("+" | "-")? primary
// primary    = num | "(" expr ")"

//式をパースする
Node *expression() { return equality(); }

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

  //そうでなければ整数
  return new_node_num(expect_number());
}
